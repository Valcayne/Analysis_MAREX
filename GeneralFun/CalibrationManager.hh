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
2024_07 Updated by Victor to include calibration with 2 parabolas
#  The energy calibration has 7 terms to use two parabolic curves a+bx+cxx and
e+fx+gxx. The point of separation is "d". The order for the input is a b c d e f
g. If d=0 only the second parabola is used.
*/

struct CalibParams {
  string DetectorName = "Empty";
  int DetID = -1;
  double TOFD = -1, p0 = -1, p1 = -1, p2 = -1, p3 = -1, p4 = -1, p5 = -1,
         p6 = -1, EneMin = -1, EneMax = 1.e20, dum1 = 0, dum2 = 0, t0 = 0,
         t1 = 0;
};

//=======================================================================
class CalibrationManager {
 public:
  CalibrationManager(const char* calibdirname, int run, int verbose = 1);
  ~CalibrationManager();
  double TakeTOF(Signal* P);     // ms
  double TakeTFlash(Signal* P);  // ms
  double TakeEnergy(Signal* P);  // MeV
  double TakeTOFD(Signal* P);    // m
  bool IsInsideThresholdLimits(Signal* P);
  int WriteCalibFile(const char* calibdirname, int run);
  // double GetEneCalibParams(string DetName, int DetN, double& p0, double& p1,
  //                          double& p2);
  // double SetEneCalibParams(string DetName, int DetN, double p0, double p1,
  //                         double p2);

  double GetEneCalibParams(string DetName, int DetN, double& p0, double& p1,
                           double& p2, double& p3, double& p4, double& p5,
                           double& p6);
  double SetEneCalibParams(string DetName, int DetN, double p0, double p1,
                           double p2, double p3, double p4, double p5,
                           double p6);

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
      if (Amplitude < it->p3) {
        ENERGY = it->p0 + it->p1 * Amplitude + it->p2 * Amplitude * Amplitude;
      } else {
        ENERGY = it->p4 + it->p5 * Amplitude + it->p6 * Amplitude * Amplitude;
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
        << std::setw(12) << it->p3 << " " << std::setw(12) << it->p4 << " "
        << std::setw(12) << it->p5 << " " << std::setw(12) << it->p6 << " "
        << std::setw(12) << it->EneMin << " " << std::setw(12) << it->EneMax
        << " " << std::setw(6) << it->dum1 << " " << std::setw(6) << it->dum2
        << " " << std::setw(12) << it->t0 << " " << std::setw(12) << it->t1
        << endl;
  }
  out << endl;

  out.close();
  return 0;
}

CalibrationManager::CalibrationManager(const char* calibdirname, int run,
                                       int verbose) {
  if (verbose > 0) {
    cout << " Taking calibration data from " << calibdirname << endl;
  }
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
          theCP.p3 >> theCP.p4 >> theCP.p5 >> theCP.p6 >> theCP.EneMin >>
          theCP.EneMax >> theCP.dum1 >> theCP.dum2 >> theCP.t0 >> theCP.t1;
      theCalibParams.push_back(theCP);
    }
  }
  in.close();

  // cout << " ... done" << endl;
  //-----------------------------------------------------------------------------------------
}

CalibrationManager::~CalibrationManager() {}

// double CalibrationManager::GetEneCalibParams(string DetName, int DetN,
//                                              double& p0, double& p1,
//                                              double& p2) {
//   bool found = false;
//   for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
//     if (it->DetectorName == DetName && it->DetID == DetN) {
//       p0 = it->p0;
//       p1 = it->p1;
//       p2 = it->p2;
//       found = true;
//       break;
//     }
//   }
//   if (!found) {
//     cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
//          << " #############" << endl;
//     exit(1);
//   }

//   return 0;
// }

double CalibrationManager::SetEneCalibParams(string DetName, int DetN,
                                             double p0, double p1, double p2,
                                             double p3, double p4, double p5,
                                             double p6) {
  bool found = false;
  for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
    if (it->DetectorName == DetName && it->DetID == DetN) {
      it->p0 = p0;
      it->p1 = p1;
      it->p2 = p2;
      it->p3 = p3;
      it->p4 = p4;
      it->p5 = p5;
      it->p6 = p6;

      found = true;
      break;
    }
  }
  if (!found) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }

  return 0;
}

double CalibrationManager::GetEneCalibParams(string DetName, int DetN,
                                             double& p0, double& p1, double& p2,
                                             double& p3, double& p4, double& p5,
                                             double& p6) {
  bool found = false;
  for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
    if (it->DetectorName == DetName && it->DetID == DetN) {
      p0 = it->p0;
      p1 = it->p1;
      p2 = it->p2;
      p3 = it->p3;
      p4 = it->p4;
      p5 = it->p5;
      p6 = it->p6;

      found = true;
      break;
    }
  }
  if (!found) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }

  return 0;
}

// double CalibrationManager::SetEneCalibParams(string DetName, int DetN,
//                                              double p0, double p1, double p2)
//                                              {
//   bool found = false;
//   for (auto it = theCalibParams.begin(); it != theCalibParams.end(); ++it) {
//     if (it->DetectorName == DetName && it->DetID == DetN) {
//       it->p0 = p0;
//       it->p1 = p1;
//       it->p2 = p2;
//       found = true;
//       break;
//     }
//   }
//   if (!found) {
//     cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
//          << " #############" << endl;
//     exit(1);
//   }

//   return 0;
// }

//=======================================================================

#endif
