
#include "../GeneralFun/MakeCalibrationFunctions.hh"
// int main(int argc, char** argv);
// void MakeEnergyCalibration_2024_Test_L6D6() {
int main(int argc, char** argv) {
  //  int Plot(){

  cout << "starting" << endl;

  // Sources and detectors calibrated
  std::vector<string> SourcesCalibrated = {"Cs", "Bi1", "Bi2", "AmBe", "Y1",
                                           "Y2", "Co",  "Ba",  "CmC"};
  std::vector<int> DetectorsCalibrated = {1, 2, 3, 4};

  // Information MC
  string MCFolder =
      "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/Simulations/"
      "2024_Er_Cu_U_v01_";
  string MCEndFile = "_1e7.root";
  string MCfname;

  // Information experimental data
  string DataFolder =
      "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/Calibration/"
      "HistoAmp_v01";
  string RunList =
      "/afs/cern.ch/work/v/valcayne/2024_Er_U_ProgramsC6D6/Analysis_nTOF_v01/"
      "RunLists/RunList2024_02.txt";
  string FolderRootFiles = "/eos/experiment/ntof/processing/official/done/";
  int PositionArraySourceThatMatch = -1;
  string Expfname;
  string ExpfnameBackground;
  int NumberBinsExpfname = 2e4;
  double EMaxExpfname = 1e5;

  // Information outputfolder
  string ArgumentsCondor = "Condor/Inputs/ArgumentsForCondor.txt";
  string outFolder =
      "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/Calibration/"
      "OutputMakeEnergyCalibration_v02";

  // Information calibration sources
  string BackgroundType = "Background_1";
  std::vector<string> Source = {"Cs", "Bi1", "Bi2", "AmBe", "Y1",
                                "Y2", "Co",  "Ba",  "CmC",  "Mn"};
  std::vector<string> SourceType = {"Cs_1", "Bi_1", "Bi_1", "AmBe_1", "Y_1",
                                    "Y_1",  "Co_1", "Ba_1", "CmC_1",  "Mn_1"};
  std::vector<string> SourceTypeSimulation = {"Cs", "Bi", "Bi", "AmBe", "Y",
                                              "Y",  "Co", "Ba", "CmC",  "Mn"};
  std::vector<double> Percen = {30, 25, 20, 13, 20, 20, 30, 30, 25, 30};
  std::vector<double> GammaEnergy = {0.661657, 0.569698, 1.063656, 4.438,
                                     0.89804,  1.836063, 1.173,    0.356012,
                                     6.130,    0.834848};
  std::vector<int> Rebin = {2, 2, 2, 8, 2, 4, 4, 1, 16, 4};

  bool LaunchFitPoints = true;
  // Parameters for fitting
  int GeneralRebin = 1;
  string NameoutputFile;
  int npRes = 10;      // Number points resolution
  double Res = 0.2;    // Resolution value
  double PERRes = 60;  // Percentage variation resolution. The range tested is
                       //   [Res-Res*PERRes, Res+Res*PERRes]
  int npCalib = 10;    // Number of points calibration

  double Calib[(int)DetectorsCalibrated.size()] = {
      0.00043,
      0.00042,
      0.0004,
      0.0004,
  };  // Calibration value
  double PERCalib = 20;  // Percentage variation calibratio. The range tested is
                         // [Calib-Calib*PERCalib, Calib+Calib*PERCalib]

  if (((int)Source.size() == (int)SourceType.size() ==
       (int)SourceTypeSimulation.size() == (int)Percen.size() ==
       (int)GammaEnergy.size())) {
    cout << (int)Source.size() << " " << (int)SourceType.size() << " "
         << (int)Percen.size() << " " << (int)GammaEnergy.size() << endl;
    cout << " ######## Error in " << __FILE__ << ", line " << __LINE__
         << " ########" << endl;
    exit(1);
  }

  cout << "start for" << endl;
  ofstream outdata(ArgumentsCondor);
  for (int j = 0; j < (int)DetectorsCalibrated.size(); j++) {
    for (int i = 0; i < (int)SourcesCalibrated.size(); i++) {
      for (size_t k = 0; k < Source.size(); ++k) {
        // cout << Source[k] << " " << SourcesCalibrated[i] << endl;
        if (Source[k] == SourcesCalibrated[i]) {
          PositionArraySourceThatMatch = k;
          break;
        }
      }
      if (PositionArraySourceThatMatch == -1) {
        cout << " ######## Error in " << __FILE__ << ", line " << __LINE__
             << " ########" << endl;
        exit(1);
      }
      NameoutputFile = outFolder + "/Calibration_Det" +
                       to_string(DetectorsCalibrated[j]) + "_" +
                       SourcesCalibrated[i] + ".root";
      if (!FileExists(NameoutputFile)) {
        // cout << NameoutputFile << " exists" << endl;
        LaunchFitPoints = false;
        MCfname = MCFolder +
                  SourceTypeSimulation[PositionArraySourceThatMatch] +
                  MCEndFile;
        Expfname = DataFolder + "/Amp_Det" + to_string(DetectorsCalibrated[j]) +
                   "_" + SourceType[PositionArraySourceThatMatch] + ".root";
        ExpfnameBackground = DataFolder + "/Amp_Det" +
                             to_string(DetectorsCalibrated[j]) + "_" +
                             BackgroundType + ".root";
        outdata << SourcesCalibrated[i] << " " << DetectorsCalibrated[j] << "  "
                << SourceType[PositionArraySourceThatMatch] << "  "
                << DataFolder << "  " << RunList << "  " << FolderRootFiles
                << "  " << Expfname << "  " << ExpfnameBackground << "  "
                << BackgroundType << "  " << NumberBinsExpfname << "  "
                << EMaxExpfname << "  " << MCfname << "  " << outFolder << "  "
                << GammaEnergy[PositionArraySourceThatMatch] << "  "
                << Rebin[PositionArraySourceThatMatch] << "  "
                << Percen[PositionArraySourceThatMatch] << "  " << npRes << "  "
                << Res << "  " << PERRes << "  " << npCalib << "  " << Calib[j]
                << "  " << PERCalib << endl;
        cout << endl
             << "Launch " << SourcesCalibrated[i] << " "
             << DetectorsCalibrated[j] << "  " << endl;

        cout << "MakeEnergyCalibration " << SourcesCalibrated[i] << " "
             << DetectorsCalibrated[j] << "  "
             << SourceType[PositionArraySourceThatMatch] << "  " << DataFolder
             << "  " << RunList << "  " << FolderRootFiles << "  " << Expfname
             << "  " << ExpfnameBackground << "  " << BackgroundType << "  "
             << NumberBinsExpfname << "  " << EMaxExpfname << "  " << MCfname
             << "  " << outFolder << "  "
             << GammaEnergy[PositionArraySourceThatMatch] << "  "
             << Rebin[PositionArraySourceThatMatch] << "  "
             << Percen[PositionArraySourceThatMatch] << "  " << npRes << "  "
             << Res << "  " << PERRes << "  " << npCalib << "  " << Calib[j]
             << "  " << PERCalib << endl;

        // cout << SourceType[PositionArraySourceThatMatch] << "  " <<
        // DataFolder
        //    << "  " << RunList << "  " << FolderRootFiles << "  " << Expfname
        //  << "  " << ExpfnameBackground << "  " << BackgroundType << "  "
        // << NumberBinsExpfname << "  " << EMaxExpfname << "  " << MCfname
        //<< "  " << outFolder << "  "
        //<< GammaEnergy[PositionArraySourceThatMatch] << "  "
        //<< Rebin[PositionArraySourceThatMatch] << "  "
        //<< Percen[PositionArraySourceThatMatch] << "  " << npRes << "  "
        //<< Res << "  " << PERRes << "  " << npCalib << "  " << Calib
        //<< "  " << PERCalib << endl;
      }
    }

    if (LaunchFitPoints) {
      NameoutputFile =
          outFolder + "/DataDet" + to_string(DetectorsCalibrated[j]) + ".out";
      if (!FileExists(NameoutputFile)) {
        cout << "FitPoints for " << DetectorsCalibrated[j] << endl;

        outdata << DetectorsCalibrated[j] << " " << outFolder << "  " << endl;
        string LaunchFits = "MakeEnergyCalibration " +
                            to_string(DetectorsCalibrated[j]) + " " + outFolder;
        cout << LaunchFits << endl;
        // system(LaunchFits.c_str());
      } else {
        cout << "File " << NameoutputFile
             << " exits so the calibration is finished" << endl;
      }
    }
  }
  outdata.close();

  system(
      "condor_submit "
      "Condor/ProgramsLaunch/LaunchCondorMakeEnergyCalibration.sh");
}
