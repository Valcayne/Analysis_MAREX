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

string Direction2DHisto =
    "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
    "2DHistos/v03/Histos01_";

string DirectionSave2DHisto =
    "/media/victor93/Elements/Valcayne/ExperimentData_Outputs/2024_Er_Cu_U/"
    "ResultsPlot2DHisto/v03/";

double TOFD = 184;
// For subtract backgroudnS
vector<string> MeasPredefined = {"Predefined", "Au_2_Si"};
vector<string> BackgroundMeasPredefined = {"Background", "Background_Si"};
vector<string> DummyPredefined = {"Empty_Er", "Empty_Si"};
vector<string> BackgroundDummyPredefined = {
    "Background",
    "Background_Si",
};

/// For GetSimul
vector<string> NameSimulArray = {"Au_13_100", "Au_40_200", "Er167_13"};
vector<string> NameSimulRootfile = {"DataPlot/Yield_Au_2_100",
                                    "DataPlot/Yield_Au_2_200",
                                    "DataPlot/Yield_TC_Er167_13mm_ENDF8"};

vector<double> Activity_kBq = {274, 274};

vector<int> ResolNumberDetector = {1, 2, 3};
// vector<double> ResolParameter1 = {0.00152035, 0.00134748, 0.00018827};
// vector<double> ResolParameter2 = {0.0100738, 0.0034685, 0.00467216};

vector<double> ResolParameter1 = {0.00193404, 0.00187058, 0.00256307};
vector<double> ResolParameter2 = {0.00838911, 0.00181082, 0.00139247};

// En for integrals normalize
// double NormalizeMinEn = 100;
// double NormalizeMaxEn = 500;

double NormalizeMinEn = 0.4;
double NormalizeMaxEn = 0.6;

// En or the xaxis plot
double ForPlotEminEn = 0.02;
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
