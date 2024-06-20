#ifndef CALIBRATIONMANAGER_HH
#define CALIBRATIONMANAGER_HH 1

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
#include <iomanip>
#include <iostream>

#include "GeneralFunctions.hh"

using namespace std;

/*

Copiado de TACCalibrationManager

*/

struct CalibParams {
  string DetectorName = "Empty";
  int DetID = -1;
  double TOFD = -1, p0 = -1, p1 = -1, p2 = -1, EneMin = -1, EneMax = 1.e20,
         dum1 = 0, dum2 = 0, t0 = 0, t1 = 0;
};

//=======================================================================
class CalibrationManager {
 public:
  CalibrationManager(const char* calibdirname, int run);
  ~CalibrationManager();
  double TakeTOF(Signal* P);     // ms
  double TakeTFlash(Signal* P);  // ms
  double TakeEnergy(Signal* P);  // MeV
  double TakeTOFD(Signal* P);    // m
  bool IsInsideThresholdLimits(Signal* P);
  int WriteCalibFile(const char* calibdirname, int run);

 private:
  vector<CalibParams> theCalibParams;
};

bool CalibrationManager::IsInsideThresholdLimits(Signal* P) {
  int DetN = P->detn;
  string DetName = P->DetName;
  double SignalEne = TakeEnergy(P);

  bool found = false;
  for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
    if (it->DetectorName == DetName && it->DetID == DetN) {
      if (SignalEne >= it->EneMin && SignalEne <= it->EneMax) {
        return true;
      }
      found = true;
      break;
    }
  }
  if (!found) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }
  return false;
}

double CalibrationManager::TakeTOF(Signal* P) {
  int DetN = P->detn;
  string DetName = P->DetName;
  double SignalEne = TakeEnergy(P);
  double TOF = -1;
  bool found = false;
  for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
    if (it->DetectorName == DetName && it->DetID == DetN) {
      TOF = (P->tof - (it->t0 + P->tof * it->t1)) * 1.e-6;
      found = true;
      break;
    }
  }
  if (!found) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }

  return TOF;
}

double CalibrationManager::TakeTFlash(Signal* P) {
  int DetN = P->detn;
  string DetName = P->DetName;
  double TFLASH = -1;

  bool found = false;
  for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
    if (it->DetectorName == DetName && it->DetID == DetN) {
      TFLASH = (P->tflash - (it->t0 + P->tflash * it->t1)) * 1.e-6;
      found = true;
      break;
    }
  }
  if (!found) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }

  return TFLASH;
}

double CalibrationManager::TakeEnergy(Signal* P) {
  int DetN = P->detn;
  string DetName = P->DetName;
  double Amplitude = P->amp;
  double ENERGY = -1;

  bool found = false;
  for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
    if (it->DetectorName == DetName && it->DetID == DetN) {
      ENERGY = it->p0 + it->p1 * Amplitude + it->p2 * Amplitude * Amplitude;
      found = true;
      break;
    }
  }
  if (!found) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }

  return ENERGY;
}

double CalibrationManager::TakeTOFD(Signal* P) {
  int DetN = P->detn;
  string DetName = P->DetName;

  double TOFDistance = -1;

  bool found = false;
  for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
    if (it->DetectorName == DetName && it->DetID == DetN) {
      TOFDistance = it->TOFD;
      found = true;
      break;
    }
  }
  if (!found) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }

  return TOFDistance;
}

int CalibrationManager::WriteCalibFile(const char* calibdirname, int run) {
  char fname[300];
  sprintf(fname, "%s/Calib_%d.dat", calibdirname, run);
  ofstream out(fname);
  string previousDetName = "none";
  for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
    if (it->DetectorName != previousDetName) {
      out << endl;
    }
    previousDetName = it->DetectorName;
    out << it->DetectorName << " " << std::setw(3) << it->DetID << " "
        << std::setw(8) << it->TOFD << " " << std::setw(12) << it->p0 << " "
        << std::setw(12) << it->p1 << " " << std::setw(12) << it->p2 << " "
        << std::setw(12) << it->EneMin << " " << std::setw(12) << it->EneMax
        << " " << std::setw(6) << it->dum1 << " " << std::setw(6) << it->dum2
        << " " << std::setw(12) << it->t0 << " " << std::setw(12) << it->t1
        << endl;
  }
  out << endl;

  out.close();
  return 0;
}

CalibrationManager::CalibrationManager(const char* calibdirname, int run) {
  cout << " Taking calibration data from " << calibdirname << " " << run
       << endl;

  //-----------------------------------------------------------------------------------------
  // Read and store info:
  char fname[300], word[100];
  sprintf(fname, "%s/Calib_%d.dat", calibdirname, run);
  ifstream in(fname);
  if (!in.good()) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }

  CalibParams theCP;
  while (in >> theCP.DetectorName) {
    if (theCP.DetectorName[0] == '#') {
      in.ignore(10000, '\n');
    } else {
      in >> theCP.DetID >> theCP.TOFD >> theCP.p0 >> theCP.p1 >> theCP.p2 >>
          theCP.EneMin >> theCP.EneMax >> theCP.dum1 >> theCP.dum2 >>
          theCP.t0 >> theCP.t1;
      theCalibParams.push_back(theCP);
    }
  }
  in.close();

  cout << " ... done" << endl;
  //-----------------------------------------------------------------------------------------
}

CalibrationManager::~CalibrationManager() {}

//=======================================================================

#endif
