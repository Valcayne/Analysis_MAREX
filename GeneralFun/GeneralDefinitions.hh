#ifndef GENERALDEFINITIONS_HH
#define GENERALDEFINITIONS_HH 1

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

// =====================================================================
//   ------------------------------ PKUP -------------------------------
/*
Las siguientes variables sirven para decidir qué pulsos son dedicados y qué
pulsos son parásitos. Sólo los pulsos que cumplen determinadas condiciones se
tienen en cuenta. Lo de DEDICATED_PULSES_ID y PARASITIC_PULSES_ID es qué flag se
usa para cada pulso, dedicado o parásito. Ha cambiado con el tiempo, así que las
variables que llevan *_ROOTFILE hacen referencia al valor de la flag del
treefile: PSpulse. Las otras "DEDICATED_PULSES_ID" son para que lleven aquí
siempre el mismo valor. Puede faltar alguna condición más en los silicios, que
no se ha tenido en cuenta.
*/

#define ALLOW_MORE_THAN_ONE_PKUP_SIGNAL 1

#define DEDICATED_PULSES_ID_ROOTFILE 2  // 1
#define PARASITIC_PULSES_ID_ROOTFILE 3  // 2
#define DEDICATED_PULSES_ID 2
#define PARASITIC_PULSES_ID 3

#define DEDICATED_PULSES_PKUPAMPMIN 1
#define DEDICATED_PULSES_PKUPAMPMAX 1.e10
#define PARASITIC_PULSES_PKUPAMPMIN 1
#define PARASITIC_PULSES_PKUPAMPMAX 1.e10

#define DEDICATED_PULSES_INTMIN 6.e12   // 5.e12
#define DEDICATED_PULSES_INTMAX 1.e20   // 8.e12
#define PARASITIC_PULSES_INTMIN 1.e12   // 2.2e12
#define PARASITIC_PULSES_INTMAX 4.5e12  // 3.8e12
// =====================================================================


#define NDetectorMAX 15  // this is the maximum number of detectors of one typw


// =====================================================================
//   ------------------------------ TAC --------------------------------

#define TFLASH_BAF2_MIN_NS 11000
#define TFLASH_BAF2_MAX_NS 11500

// =====================================================================
//   ---------------------------- Other --------------------------------
#define MAXNBUNCHESINFILE 100000
#define MAXNSIGNALSINPULSE 1000000
// #define MAXNEVENTSINPULSE 1000000
//  =====================================================================

struct HistoInfo {
  string DetectorName;
  std::vector<int> DetectorNumber;
  std::vector<string> HistoName;
  std::vector<string> HistoTitle;
  std::vector<int> HistoType;
  std::vector<vector<double>> Xaxis;
  std::vector<vector<double>> Yaxis;
};

struct Signal {
  int RunNumber, BunchNumber, date, detn, time, PSpulse, isAlpha, movie;
  double edep, tof, tflash, eNeutron, tofPreviousSignal, edepPreviousSignal;
  float amp, area, fwhm, fwtm, area_0, amp_0, area2, PulseIntensity, afast,
      aslow, Tau;
  string DetName;
};

struct PKUPInfo {
  int npulses;
  int np1, np2, np3;                    // dedicated,parasitic,other
  double PulsInt1, PulsInt2, PulsInt3;  // dedicated,parasitic,other
  double PKUPamp1, PKUPamp2, PKUPamp3, PKUParea1, PKUParea2,
      PKUParea3;  // dedicated,parasitic,other
  int pulseType[MAXNBUNCHESINFILE];
  int BunchNumber[MAXNBUNCHESINFILE];
  double PulseIntensity[MAXNBUNCHESINFILE];
  double PKUPAmp[MAXNBUNCHESINFILE];
  double PKUPArea[MAXNBUNCHESINFILE];
  double PKUPTflash[MAXNBUNCHESINFILE];
};

#endif
