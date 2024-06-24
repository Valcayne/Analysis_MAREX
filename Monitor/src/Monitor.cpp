#include <Monitor.h>

Monitor::Monitor(const char* ini_filename) {
  root_files_dir =
      "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/2DHistos/v03";
  run_list_filename = "../RunLists/RunList2024_02.txt";
  run_type = "AllRuns";

  const auto detectors = {
      Detector("SILI", {0, 1, 2, 3, 4}, {0.02, 1e6}, {0, 6e4}),
      Detector("C6D6", {0, 1, 2, 3, 4}, {0.02, 1e6}, {0.150, 20}),
      Detector("FIMG", {0, 1, 2}, {0.02, 1e5}, {2900, 5000}),
      Detector("LIGL", {1}, {0.02, 3e4}, {1e3, 3e3})};
  pulses = {
      new Pulse(0, "All", detectors), new Pulse(1, "Dedicated", detectors),
      new Pulse(2, "Parasitic", detectors), new Pulse(3, "Others", detectors)};

  output_filename =
      "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/Monitoring/"
      "Monitoring.root";
}

Monitor::~Monitor() {
  for (const auto pulse : pulses) delete pulse;
}

void Monitor::Start() {
  gROOT->SetStyle("ATLAS");

  std::cout << std::endl
            << "Monitoring process started! 📈" << std::endl
            << std::endl;

  const auto output_file =
      std::unique_ptr<TFile>(new TFile(output_filename.data(), "RECREATE"));
  output_file->Close();

  const auto runs = GetRuns();

  std::cout << "Getting data from run " << runs.front() << " to run "
            << runs.back() << std::endl;

  // Get data for every run.
  for (const auto run : runs) {
    std::unique_ptr<TFile> run_file(new TFile(
        (root_files_dir + "/Histos01_" + std::to_string(run) + ".root").c_str(),
        "READ"));
    if (!run_file || !run_file->IsOpen() || run_file->IsZombie()) continue;

    std::string h_name;
    bool run_with_protons = false;
    for (const auto pulse : pulses) {
      const auto pulse_type = std::to_string(pulse->type);

      // Get pulse instensity's data for the current run.
      h_name = "hPulIntens_" + pulse_type;
      std::unique_ptr<TH1D> h_pulse_intensity(
          run_file->Get<TH1D>(h_name.c_str()));
      const auto protons = h_pulse_intensity->GetBinContent(1) / 8e12;
      if (!run_with_protons && protons > 0.1) run_with_protons = true;
      if (!run_with_protons) break;

      pulse->AppendNumbers(h_pulse_intensity->GetBinContent(2));
      pulse->AppendProtons(protons);
      pulse->AppendPkupAreas(h_pulse_intensity->GetBinContent(4));

      // Get detectors' data for the current run.
      for (auto& detector : pulse->detectors) {
        const auto detector_name = detector.name;
        const auto [min_neutron_energy, max_neutron_energy] =
            detector.neutron_energy_range;
        const auto [min_deposited_energy, max_deposited_energy] =
            detector.deposited_energy_range;
        for (const auto detector_id : detector.ids) {
          h_name = "En_" + detector_name + "_" + std::to_string(detector_id) +
                   "_PType_" + pulse_type;
          std::unique_ptr<TH2D> h2d(run_file->Get<TH2D>(h_name.c_str()));
          double counts = 0;
          if (h2d) {
            const auto x_axis = h2d->GetXaxis();
            std::unique_ptr<TH1D> h(
                h2d->ProjectionY("_py", x_axis->FindBin(min_neutron_energy),
                                 x_axis->FindBin(max_neutron_energy)));
            const auto axis = h->GetXaxis();
            counts = h->Integral(axis->FindBin(min_deposited_energy),
                                 axis->FindBin(max_deposited_energy));
          }
          detector.AppendCounts(detector_id, counts);
        }
      }
    }
    if (run_with_protons) monitored_runs.push_back(run);
  }

  Pulse::runs = monitored_runs;

  std::cout << std::endl
            << "Creating graphs for " << pulses.size() << " pulse types during "
            << monitored_runs.size() << " monitored runs" << std::endl;

  for (const auto pulse : pulses) pulse->CreateGraphs();

  std::cout << std::endl
            << "Writing the graphs to \"" << output_filename << "\""
            << std::endl;

  for (const auto pulse : pulses) pulse->WriteGraphsToFile(output_filename);

  std::cout << std::endl << "Monitoring ended! 👋" << std::endl << std::endl;
}

std::vector<int> Monitor::GetRuns() {
  std::ifstream in(run_list_filename);
  if (!in.good()) {
    std::cout << " ###### Error in " << __FILE__ << ", line " << __LINE__
              << " ###### " << run_list_filename << std::endl;
    std::exit(1);
  }

  std::string id;
  while (in >> id)
    if (id == std::string("RUNLIST")) break;

  int run_min, run_max, previous_run_max = -1;
  std::vector<int> runs;
  while (in >> run_min >> run_max >> id) {
    if (id == run_type) {
      if (run_max < run_min ||
          (previous_run_max > run_min && previous_run_max > 0)) {
        std::cout << run_min << "  " << run_max << "  " << previous_run_max
                  << std::endl;
        std::cout << " ###### Error in " << __FILE__ << ", line " << __LINE__
                  << " ######" << std::endl;
        std::exit(1);
      }
      previous_run_max = run_max;
      for (auto i = run_min; i <= run_max; i++) runs.push_back(i);
    }
  }
  in.close();

  if (runs.empty()) {
    std::cout << " ####### Error in " << __FILE__ << ", line " << __LINE__
              << " ###### " << run_list_filename << std::endl;
    std::cout << " ####### No runs found for run_type = " << run_type
              << " in the RunList" << std::endl;
    std::exit(1);
  }

  return runs;
}
