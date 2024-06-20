
#include "../GeneralFun/FillHistoFunctions.hh"

int main(int argc, char** argv) {
  if (argc != 2) {
    cout << " ******* Input has to be like: Make2DHisto [RunNumber] "
            "***************"
         << endl;
    return 1;
  }
  // HistoType
  //  1 Normal 2D (En/Edep)
  //  2 Similar to 1 but with TOF (TOF/Edep)
  //  3 Save the TFlash values
  //  4 Save the DeltaTime; The difference in time with the preovious detector.
  //  (Delta/TOF )

  //==================================================================================
  char outdir[200] =
      "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/2DHistos/v03";

  char EOSPATH[200] =
      "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/RootFiles/v02/"
      "done/";

  char CalibDirName[200] =
      "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/Calibration/"
      "CalibrationRunByRun";
  //==================================================================================

  HistoInfo* HistoInfoC6D6 = new HistoInfo();

  HistoInfoC6D6->DetectorName = "C6D6";
  HistoInfoC6D6->DetectorNumber = {0, 1, 2, 3, 4};

  HistoInfoC6D6->HistoName.push_back("Edep");
  HistoInfoC6D6->HistoTitle.push_back("Edep");
  HistoInfoC6D6->HistoType.push_back(1);
  HistoInfoC6D6->Xaxis.push_back({1000, 1.e-3, 1.e7});
  HistoInfoC6D6->Yaxis.push_back({2000, 0, 20});

  HistoInfoC6D6->HistoName.push_back("En");
  HistoInfoC6D6->HistoTitle.push_back("En");
  HistoInfoC6D6->HistoType.push_back(1);
  HistoInfoC6D6->Xaxis.push_back({5e4, 1.e-3, 1.e7});
  HistoInfoC6D6->Yaxis.push_back({400, 0, 20});

  // HistoInfoC6D6->HistoName.push_back("TOF");
  // HistoInfoC6D6->HistoTitle.push_back("TOF");
  // HistoInfoC6D6->HistoType.push_back(2);
  // HistoInfoC6D6->Xaxis.push_back({1e5, 0, 1.e8});
  // HistoInfoC6D6->Yaxis.push_back({400, 0, 20});

  HistoInfoC6D6->HistoName.push_back("TFlash");
  HistoInfoC6D6->HistoTitle.push_back("TFlash");
  HistoInfoC6D6->HistoType.push_back(3);
  HistoInfoC6D6->Xaxis.push_back({1e6, 0, 1.e6});
  HistoInfoC6D6->Yaxis.push_back({1, 0, 2});

  HistoInfoC6D6->HistoName.push_back("DeltaTOF");
  HistoInfoC6D6->HistoTitle.push_back("DeltaTOF");
  HistoInfoC6D6->HistoType.push_back(4);
  HistoInfoC6D6->Xaxis.push_back({1e4, 0, 1.e4});
  HistoInfoC6D6->Yaxis.push_back({400, 0, 20});

  HistoInfo* HistoInfoSILI = new HistoInfo();

  HistoInfoSILI->DetectorName = "SILI";
  HistoInfoSILI->DetectorNumber = {0, 1, 2, 3, 4};

  HistoInfoSILI->HistoName.push_back("Edep");
  HistoInfoSILI->HistoTitle.push_back("Edep");
  HistoInfoSILI->HistoType.push_back(1);
  HistoInfoSILI->Xaxis.push_back({100, 1.e-3, 1.e7});
  HistoInfoSILI->Yaxis.push_back({2000, 0, 1e5});

  HistoInfoSILI->HistoName.push_back("En");
  HistoInfoSILI->HistoTitle.push_back("En");
  HistoInfoSILI->HistoType.push_back(1);
  HistoInfoSILI->Xaxis.push_back({10000, 1.e-3, 1.e7});
  HistoInfoSILI->Yaxis.push_back({200, 0, 1e5});

  HistoInfo* HistoInfoFIMG = new HistoInfo();

  HistoInfoFIMG->DetectorName = "FIMG";
  HistoInfoFIMG->DetectorNumber = {0, 1, 2};

  HistoInfoFIMG->HistoName.push_back("Edep");
  HistoInfoFIMG->HistoTitle.push_back("Edep");
  HistoInfoFIMG->HistoType.push_back(1);
  HistoInfoFIMG->Xaxis.push_back({100, 1.e-3, 1.e7});
  HistoInfoFIMG->Yaxis.push_back({1000, 0, 1e4});

  HistoInfoFIMG->HistoName.push_back("En");
  HistoInfoFIMG->HistoTitle.push_back("En");
  HistoInfoFIMG->HistoType.push_back(1);
  HistoInfoFIMG->Xaxis.push_back({10000, 1.e-3, 1.e7});
  HistoInfoFIMG->Yaxis.push_back({200, 0, 1e4});

  HistoInfo* HistoInfoLIGL = new HistoInfo();

  HistoInfoLIGL->DetectorName = "LIGL";
  HistoInfoLIGL->DetectorNumber = {1};

  HistoInfoLIGL->HistoName.push_back("Edep");
  HistoInfoLIGL->HistoTitle.push_back("Edep");
  HistoInfoLIGL->HistoType.push_back(1);
  HistoInfoLIGL->Xaxis.push_back({100, 1.e-3, 1.e7});
  HistoInfoLIGL->Yaxis.push_back({1000, 0, 1e4});

  HistoInfoLIGL->HistoName.push_back("En");
  HistoInfoLIGL->HistoTitle.push_back("En");
  HistoInfoLIGL->HistoType.push_back(1);
  HistoInfoLIGL->Xaxis.push_back({10000, 1.e-3, 1.e7});
  HistoInfoLIGL->Yaxis.push_back({200, 0, 1e4});

  int const N_PULSETYPE = 4;  // All, Dedicated, Parasitic, Other
  //==============================================================================
  //==============================================================================

  int RunNumber = atoi(argv[1]);

  //==============================================================================
  //==============================================================================

  // Get data file:
  char infname[1000];
  sprintf(infname, "%s/run%d.root", EOSPATH, RunNumber);
  TFile* f1 = new TFile(infname, "READ");
  if (f1->IsZombie()) {
    cout << " ################# ERROR: FILE " << infname
         << " does not exist  #################" << endl;
    return 1;
  }
  cout << "RootFile= " << infname << endl;
  //==============================================================================
  //==============================================================================

  // Output files:
  cout << " Creating output files ..." << endl;
  char outrootfname[1000];
  // char outtxtfname[1000];
  sprintf(outrootfname, "%s/Histos01_%d.root", outdir, RunNumber);
  // sprintf(outtxtfname, "%s/Histo01_%d.txt", outdir, RunNumber);
  TFile* fout = new TFile(outrootfname, "RECREATE");
  // ofstream out(outtxtfname);
  // out << " RUN " << RunNumber << endl;

  //==============================================================================
  //==============================================================================

  // Init PKUP objects:
  cout << "Start PKUP" << endl;
  //-----------------------------------------------
  PKUPInfo* thePKUPInfo = new PKUPInfo();
  TakePKUPInfo(infname, thePKUPInfo);
  int nbunches = thePKUPInfo->npulses;  ///
  //-----------------------------------------------

  // Calibrate all detectors:
  CalibrationManager* theCalibM =
      new CalibrationManager(CalibDirName, RunNumber);
  // theCalibM->PrintData(outtxtfname);
  //==============================================================================
  //==============================================================================

  // Create tree and signal for each detector
  TTree* trC6D6 = (TTree*)f1->Get(HistoInfoC6D6->DetectorName.c_str());
  bool ThereIsC6D6Detector = false;
  int nbunchesC6D6 = 0;
  Long64_t bunchesIndexC6D6[MAXNBUNCHESINFILE];
  int BunchNumberC6D6[MAXNBUNCHESINFILE];
  Signal theC6D6S;
  int NC6D6Signals = 0;
  Signal* theC6D6Signals;
  theC6D6Signals = new Signal[MAXNSIGNALSINPULSE];

  if (!(trC6D6 == 0)) {
    cout << "There is C6D6" << endl;

    ThereIsC6D6Detector = true;
    nbunchesC6D6 =
        CreateBunchesIndex(trC6D6, bunchesIndexC6D6, BunchNumberC6D6);
    AttachStruct(&theC6D6S, trC6D6);
  }
  //==============================================================================
  //===============
  //===============================================================

  TTree* trSILI = (TTree*)f1->Get(HistoInfoSILI->DetectorName.c_str());
  bool ThereIsSILIDetector = false;
  Signal theSILIS;
  int nbunchesSILI = 0;
  Long64_t bunchesIndexSILI[MAXNBUNCHESINFILE];
  int BunchNumberSILI[MAXNBUNCHESINFILE];
  int NSILISignals = 0;
  Signal* theSILISignals;
  if (!trSILI == 0) {
    cout << "There is SILI" << endl;
    ThereIsSILIDetector = true;
    nbunchesSILI =
        CreateBunchesIndex(trSILI, bunchesIndexSILI, BunchNumberSILI);
    AttachStruct(&theSILIS, trSILI);
    theSILISignals = new Signal[MAXNSIGNALSINPULSE];
  }
  //==============================================================================
  //==============================================================================

  TTree* trFIMG = (TTree*)f1->Get(HistoInfoFIMG->DetectorName.c_str());
  bool ThereIsFIMGDetector = false;
  Signal theFIMGS;
  int nbunchesFIMG = 0;
  Long64_t bunchesIndexFIMG[MAXNBUNCHESINFILE];
  int BunchNumberFIMG[MAXNBUNCHESINFILE];
  int NFIMGSignals = 0;
  Signal* theFIMGSignals;
  if (!trFIMG == 0) {
    cout << "There is FIMG" << endl;
    ThereIsFIMGDetector = true;
    nbunchesFIMG =
        CreateBunchesIndex(trFIMG, bunchesIndexFIMG, BunchNumberFIMG);
    AttachStruct(&theFIMGS, trFIMG);
    theFIMGSignals = new Signal[MAXNSIGNALSINPULSE];
  }
  // //==============================================================================
  //==============================================================================

  TTree* trLIGL = (TTree*)f1->Get(HistoInfoLIGL->DetectorName.c_str());
  bool ThereIsLIGLDetector = false;
  Signal theLIGLS;
  int nbunchesLIGL = 0;
  Long64_t bunchesIndexLIGL[MAXNBUNCHESINFILE];
  int BunchNumberLIGL[MAXNBUNCHESINFILE];
  int NLIGLSignals = 0;
  Signal* theLIGLSignals;
  if (!trLIGL == 0) {
    cout << "There is LIGL" << endl;
    ThereIsLIGLDetector = true;
    nbunchesLIGL =
        CreateBunchesIndex(trLIGL, bunchesIndexLIGL, BunchNumberLIGL);
    AttachStruct(&theLIGLS, trLIGL);
    theLIGLSignals = new Signal[MAXNSIGNALSINPULSE];
  }
  //==============================================================================
  //==============================================================================

  cout << " Creating histograms ..." << endl;

  // Pulse intensity histograms:
  TH1D* hPulIntens[N_PULSETYPE * 2];
  CreateTH1DHistoPulseIntesity(hPulIntens, N_PULSETYPE, fout);

  // C6D6 histos

  TH2D*** h2D_C6D6[N_PULSETYPE];
  CreateTH2DHisto(ThereIsC6D6Detector, HistoInfoC6D6, N_PULSETYPE, h2D_C6D6,
                  fout);

  // SILI histos

  TH2D*** h2D_SILI[N_PULSETYPE];
  CreateTH2DHisto(ThereIsSILIDetector, HistoInfoSILI, N_PULSETYPE, h2D_SILI,
                  fout);

  // FIMG histos

  TH2D*** h2D_FIMG[N_PULSETYPE];
  CreateTH2DHisto(ThereIsFIMGDetector, HistoInfoFIMG, N_PULSETYPE, h2D_FIMG,
                  fout);

  // LIGL histos

  TH2D*** h2D_LIGL[N_PULSETYPE];
  CreateTH2DHisto(ThereIsLIGLDetector, HistoInfoLIGL, N_PULSETYPE, h2D_LIGL,
                  fout);

  /************************************************************************
   *************************************************************************
   ************************************************************************/

  // Main loop:
  cout << " Starting main loop ..." << endl;
  for (int npul = 0; npul < nbunches; npul++) {
    // cout << "Pulse " << npul << "Type pulse" <<
    // thePKUPInfo->pulseType[npul]<< endl;
    int PulseType = thePKUPInfo->pulseType[npul];
    int thisPulseType = 3;  // Other
    if (thePKUPInfo->pulseType[npul] == DEDICATED_PULSES_ID) {
      thisPulseType = 1;
    }
    if (thePKUPInfo->pulseType[npul] == PARASITIC_PULSES_ID) {
      thisPulseType = 2;
    }
    double C6D6TFlash = -1;
    double SILITFlash = -1;
    double FIMGTFlash = -1;
    // double LIGLTFlash = -1;

    // cout << "Inte " << thePKUPInfo->PulseIntensity[npul] << endl;
    if (thePKUPInfo->PulseIntensity[npul] < 1.e12) {
      // There is not beam in this run so I force the Tflash to Zero for
      // security
      C6D6TFlash = 0;
      SILITFlash = 0;
      FIMGTFlash = 0;
      // LIGLTFlash = 0;
    }

    FillPulseIntensity(
        N_PULSETYPE, thisPulseType, thePKUPInfo->PulseIntensity[npul],
        thePKUPInfo->PKUPAmp[npul], thePKUPInfo->PKUPArea[npul], hPulIntens);

    if (ThereIsC6D6Detector) {
      cout << "There isThereIsC6D6Detector " << endl;
      NC6D6Signals =
          GetSignals(HistoInfoC6D6->DetectorName, trC6D6, &theC6D6S,
                     bunchesIndexC6D6[npul], bunchesIndexC6D6[npul + 1] - 1,
                     theC6D6Signals, theCalibM, 1, C6D6TFlash);  //

      FillType1(thisPulseType, NC6D6Signals, theC6D6Signals, h2D_C6D6, -1.e20,
                1.e20, HistoInfoC6D6);

      FillType2(thisPulseType, NC6D6Signals, theC6D6Signals, h2D_C6D6, -1.e20,
                1.e20, HistoInfoC6D6);

      FillType3(thisPulseType, NC6D6Signals, theC6D6Signals, h2D_C6D6, -1.e20,
                1.e20, HistoInfoC6D6);

      FillType4(thisPulseType, NC6D6Signals, theC6D6Signals, h2D_C6D6, -1.e20,
                1.e20, HistoInfoC6D6);
    }
    if (ThereIsSILIDetector) {
      cout << "There isThereIsSILIDetector " << endl;

      NSILISignals =
          GetSignals(HistoInfoSILI->DetectorName, trSILI, &theSILIS,
                     bunchesIndexSILI[npul], bunchesIndexSILI[npul + 1] - 1,
                     theSILISignals, theCalibM, 1, SILITFlash);  //

      FillType1(thisPulseType, NSILISignals, theSILISignals, h2D_SILI, -1.e20,
                1.e20, HistoInfoSILI);
    }
    if (ThereIsFIMGDetector) {
      cout << "There isThereIsFIMGDetector " << endl;

      NFIMGSignals =
          GetSignals(HistoInfoFIMG->DetectorName, trFIMG, &theFIMGS,
                     bunchesIndexFIMG[npul], bunchesIndexFIMG[npul + 1] - 1,
                     theFIMGSignals, theCalibM, 1, FIMGTFlash);  //
      cout << "There isThereIsFIMGDetector " << endl;

      FillType1(thisPulseType, NFIMGSignals, theFIMGSignals, h2D_FIMG, -1.e20,
                1.e20, HistoInfoFIMG);
      //   cout << "There isThereIsFIMGDetector " << endl;
    }

    if (ThereIsLIGLDetector) {
      cout << "There isThereIsLIGLDetector " << endl;

      //  NLIGLSignals =
      // GetSignals(HistoInfoLIGL->DetectorName, trLIGL, &theLIGLS,
      //            bunchesIndexLIGL[npul], bunchesIndexLIGL[npul + 1] - 1,
      //            theLIGLSignals, theCalibM, 1, LIGLTFlash);  //

      // FillType1(thisPulseType, NLIGLSignals, theLIGLSignals, h2D_LIGL,
      // -1.e20,
      //           1.e20, HistoInfoLIGL);
    }
  }
  cout << " ... end main loop" << endl;

  //==============================================================================
  //==============================================================================

  // Write results:
  cout << " Writting results ..." << endl;
  fout->Write();
  fout->Close();
  delete fout;
  cout << " ... done" << endl;

  //==============================================================================
  //==============================================================================

  cout << " File " << outrootfname << " has been created " << endl;

  return 0;
}
