#include <Pulse.h>

std::vector<double> Pulse::runs = {};

Pulse::Pulse(const int type, const std::string name,
             const std::vector<Detector> detectors)
    : type(type), name(name), detectors(detectors) {}

Pulse::~Pulse() {}

void Pulse::AppendNumbers(const double n) { numbers.push_back(n); }

void Pulse::AppendProtons(const double p) { protons.push_back(p); }

void Pulse::AppendPkupAreas(const double pa) { pkup_areas.push_back(pa); }

bool Pulse::HasProtons() { return !protons.empty(); }

void Pulse::CreateGraphs() {
  std::string result;
  std::vector<double> ratios;
  std::vector<double> uncertainties;

  result = "Protons per pulse";
  ratios.clear();
  uncertainties.clear();
  for (auto i = 0; i < numbers.size(); i++) {
    ratios.push_back(protons.at(i) * 8e12 / numbers.at(i));
    uncertainties.push_back(0);
  }
  graphs.push_back(CreateGraph(result, ratios, uncertainties,
                               "Protons / 8*10^{12} / pulse"));

  result = "Protons per PKUP area";
  ratios.clear();
  uncertainties.clear();
  for (auto i = 0; i < pkup_areas.size(); i++)
    ratios.push_back(protons.at(i) / pkup_areas.at(i));
  graphs.push_back(CreateGraph(result, ratios, uncertainties,
                               "Protons / 8*10^{12} / PKUP area"));

  for (auto &detector : detectors) {
    for (const auto [id, counts] : detector.GetCounts()) {
      result = detector.name + " " + std::to_string(id) + " counts per proton";
      ratios.clear();
      uncertainties.clear();
      for (auto i = 0; i < protons.size(); i++) {
        const auto c = counts.at(i);
        const auto cpp = c / protons.at(i);
        ratios.push_back(cpp);
        uncertainties.push_back(cpp * TMath::Sqrt(c) / c);
      }
      detector.AppendGraph(CreateGraph(result, ratios, uncertainties,
                                       "Counts / proton / 8*10^{12}"));
    }
  }
}

void Pulse::WriteGraphsToFile(const std::string filename) {
  const auto file =
      std::unique_ptr<TFile>(new TFile(filename.data(), "UPDATE"));

  const auto directory = file->mkdir(name.c_str());
  for (const auto graph : graphs) directory->Append(graph);

  for (auto &detector : detectors) {
    const auto detector_directory = directory->mkdir(detector.name.c_str());
    for (const auto graph : detector.GetGraphs())
      detector_directory->Append(graph);
  }

  file->Write();
  file->Close();
}

TCanvas *Pulse::CreateGraph(const std::string name,
                            const std::vector<double> ratios,
                            const std::vector<double> uncertainties,
                            const std::string ratios_title) {
  const auto c = new TCanvas((name + " (" + std::to_string(type) + ")").c_str(),
                             name.c_str());
  c->SetGrid();

  const auto g = new TGraphErrors(runs.size(), runs.data(), ratios.data(),
                                  nullptr, uncertainties.data());
  g->SetName(c->GetName());
  g->SetTitle(c->GetTitle());

  const auto y_axis = g->GetYaxis();
  y_axis->SetTitle(ratios_title.c_str());

  const auto x_axis = g->GetXaxis();
  x_axis->SetTitle("Run");
  x_axis->SetNoExponent();

  g->Draw("AP");

  return c;
}
