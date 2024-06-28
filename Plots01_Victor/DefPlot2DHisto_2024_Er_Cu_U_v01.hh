#ifndef DEFPLOT2DHISTO_2024_TEST_L6D6_HH
#define DEFPLOT2DHISTO_2024_TEST_L6D6_HH 1

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TKey.h>
#include <TLegend.h>
#include <TMath.h>
#include <TPad.h>
#include <TPaletteAxis.h>
#include <TROOT.h>
#include <TRandom.h>
#include <TStyle.h>
#include <TTree.h>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

double TimeMeasurement_ns = 1e8;
string NameDetector = "C6D6";

// string Direction2DHisto =
//     "/afs/cern.ch/user/v/valcayne/LinkEOS/nTOFDataProcessing/2024_Test_L6D6/"
//     "2DHisto/v01/2DHisto_";
// string DirectionSave2DHisto =
//     "/afs/cern.ch/user/v/valcayne/LinkEOS/nTOFDataProcessing/2024_Test_L6D6/"
//     "ResultsPlot2DHisto/v01/";

// string Direction2DHisto =
//   "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Test_L6D6/"
//  "2DHisto/v07/2DHisto_";

// string Direction2DHisto =
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "2DHistos/v03/Histos01_";

string Direction2DHisto =
    " /home/victor93/ExperimentData_Outputs/2024_Er_U/2Dhistos/v03/Histos01_";

string DirectionSave2DHisto =
    "/home/victor93/ExperimentData_Outputs/2024_Er_U/ResultsPlot2DHisto/v03/";

double TOFD = 184;
// For subtract backgroudnS
vector<string> MeasPredefined = {"Predefined", "Au_2_Si"};
vector<string> BackgroundMeasPredefined = {"Background_1", "Background_Si"};
vector<string> DummyPredefined = {"Empty_Er", "Empty_Si"};
vector<string> BackgroundDummyPredefined = {
    "Background_1",
    "Background_Si",
};

/// For GetSimul
vector<string> NameSimulArray = {"Au_13_100", "Au_40_200", "Er167_13",
                                 "118969"};
vector<string> NameSimulRootfile = {
    "DataPlot/Yield_Au_2_100", "DataPlot/Yield_Au_2_200",
    "DataPlot/Yield_SAMMY_Er167_13mm", "DataPlot/Yield_SAMMY_Er166_13mm"};

vector<string> NameSimulRootfile2 = {"DataPlot/Yield_Au_2_100",
                                     "DataPlot/Yield_Au_2_200",
                                     "DataPlot/Yield_TC_Er167_13mm_ENDF8_BKG",
                                     "DataPlot/Yield_TC_Er166_13mm_ENDF8_BKG"};

// vector<string> NameSimulArray = {"Cs_1", "Co_1",   "Mn_1",  "Co57_1", "Bi_1",
//                                  "Ba_1", "AmBe_1", "CmC_1", "Eu_1",   "Y_1"};
// vector<string> NameSimulRootfile = {
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Cs_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Co_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Mn_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Co57_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Bi_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Ba_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_AmBe_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_CmC_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Eu_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Y_1e7.root"};
// vector<string> NameSimulRootfile2 = {
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Cs_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Co_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Mn_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Co57_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Bi_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Ba_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_AmBe_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_CmC_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Eu_1e7.root",
//     "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
//     "Simulations/v01/2024_Er_Cu_U_v01_Y_1e7.root"};

vector<double> Activity_kBq = {282,  17.4, 4.02, 9.27,  31.5,
                               16.4, 1,    1,    10.05, 237.38};

vector<int> ResolNumberDetector = {1, 2, 3, 4};
// vector<double> ResolParameter1 = {0.00152035, 0.00134748, 0.00018827};
// vector<double> ResolParameter2 = {0.0100738, 0.0034685, 0.00467216};

// vector<double> ResolParameter1 = {0.00193404, 0.00187058, 0.00256307};
// vector<double> ResolParameter2 = {0.00838911, 0.00181082, 0.00139247};

// vector<double> ResolParameter1 = {0.00329717, 0.00285433,
// 0.00136088,0.00211402}; vector<double> ResolParameter2 = {0.00215909,
// 0.0030931, 0.00293019, 0.0105527};

vector<double> ResolParameter1 = {0.00258232, 0.00227249, 0.00167079,
                                  0.00371062};
vector<double> ResolParameter2 = {0.00233583, 0.00225643, 0.00221157, 0.008693};

// En for integrals normalize
// double NormalizeMinEn = 100;
// double NormalizeMaxEn = 500;

double NormalizeMinEn = 14;
double NormalizeMaxEn = 17;

// En or the xaxis plot
double ForPlotEminEn = 0.1;
double ForPlotEmaxEn = 1.0e7;

// Edep for integrals normalize
double NormalizeMinEdep = 0.2;
double NormalizeMaxEdep = 2.5;

// Edep for the xaxis plot
double ForPlotEminEdep = 0.2;
double ForPlotEmaxEdep = 3;

// For the Y axis of compare
double MaxYForCompare = 2;
double MinYForCompare = 0.5;

// For calculating gain shifts

int NumberIterations = 200;
double MinShift = 0.9;
double MaxShift = 1.1;

#endif
