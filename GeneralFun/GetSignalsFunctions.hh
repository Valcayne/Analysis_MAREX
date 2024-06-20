#ifndef GETSIGNALSFUNCTIONS_HH
#define GETSIGNALSFUNCTIONS_HH 1

#include "CreateHistoFunctions.hh"
//================================================================================================
int GetSignals(string DetectorName, TTree* tr, Signal* theS, Long64_t EntryMin,
               Long64_t EntryMax, Signal* theSignals,
               CalibrationManager* theCalibM, int CondType, double TFlash);
double GetAvgTFlash(Signal* theSignals, int NSignals);
double GetTflash(Signal* theSignals, int NSignals, int DetN);
int FindDetectorPosition(const std::vector<double>& vector, double numero);
//================================================================================================

// CondType=0 --> se cogen todas las señales
// CondType==1 --> se cogen solo las señales que cumplen las condiciones del
// threshold If TFlash<0, se coge el TFlash de las señales
int GetSignals(string DetectorName, TTree* tr, Signal* theS, Long64_t EntryMin,
               Long64_t EntryMax, Signal* theSignals,
               CalibrationManager* theCalibM, int CondType, double TFlash) {
  double SignalEne, SignalTOF, SignalTFlash, TimeOfFlightDistance;
  double EdepPreviousSignal[NDetectorMAX], TOFPreviousSignal[NDetectorMAX];
  for (Long64_t i = 0; i < NDetectorMAX; i++) {
    EdepPreviousSignal[i] = -1;
    TOFPreviousSignal[i] = -1;
  }

  int NSignals = 0;

  for (Long64_t i = EntryMin; i <= EntryMax; i++) {
    tr->GetEntry(i);
    theS->DetName = DetectorName;
    SignalEne = theCalibM->TakeEnergy(theS);
    SignalTOF = theCalibM->TakeTOF(theS);
    SignalTFlash = TFlash;
    if (TFlash < 0) {
      SignalTFlash = theCalibM->TakeTFlash(theS);
    } else {
      // cout << "Tfalsh forced to" << TFlash << endl;
    }

    // if ((SignalTFlash < (11100 / 1.0e6)) && (DetectorName == "C6D6")) {
    //   cout << " Previous TFLash" << SignalTFlash  << "Forced to 11190"
    //        << endl;
    //   SignalTFlash = 11190.0 / 1e6;
    // }
    if (CondType == 0 ||
        (CondType == 1 && theCalibM->IsInsideThresholdLimits(theS))) {
      theSignals[NSignals].edep = SignalEne;
      theSignals[NSignals].tof = SignalTOF;
      theSignals[NSignals].tflash = SignalTFlash;
      TimeOfFlightDistance = theCalibM->TakeTOFD(theS);
      theSignals[NSignals].eNeutron =
          TOFtoE((SignalTOF - SignalTFlash) * 1.e6, TimeOfFlightDistance);
      theSignals[NSignals].detn = theS->detn;
      theSignals[NSignals].edepPreviousSignal = EdepPreviousSignal[theS->detn];
      theSignals[NSignals].tofPreviousSignal = TOFPreviousSignal[theS->detn];

      EdepPreviousSignal[theS->detn] = theSignals[NSignals].edep;
      TOFPreviousSignal[theS->detn] = theSignals[NSignals].tof;
      NSignals++;
    }
  }
  return NSignals;
}

// VERSION DE GetAvgTFlash DE ADRIAN
double GetAvgTFlash(Signal* theSignals, int NSignals) {
  double TFlash[NDetectorMAX + 1];
  for (int ndet = 1; ndet <= NDetectorMAX; ndet++) {
    TFlash[ndet] = GetTflash(theSignals, NSignals, ndet);
  }

  double Sum = 0;
  int count = 0;
  for (int i = 1; i <= NDetectorMAX; i++) {
    if (TFlash[i] * 1.e6 < TFLASH_BAF2_MAX_NS &&
        TFlash[i] * 1.e6 > TFLASH_BAF2_MIN_NS) {
      Sum += TFlash[i];
      count++;
    }
  }

  // return TFlash/(double)NSig;
  return Sum / (double)count;  // in ms
}

double GetTflash(Signal* theSignals, int NSignals, int DetN) {
  for (int i = 0; i < NSignals; i++) {
    if (theSignals[i].detn == DetN) {
      return theSignals[i].tflash;
    }
  }
  return 0;
}

#endif
