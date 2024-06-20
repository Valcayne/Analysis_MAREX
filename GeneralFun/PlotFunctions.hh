#ifndef PLOTFUNCTIONS_HH
#define PLOTFUNCTIONS_HH 1

#include "GeneralFunctions.hh"

TH1D* TH1DFromMC_Resol_v01(TH1D* h1, string MCFile, int detn, double Resa,
                           double Resb);

TH1D* TH1DFromMC_Resol_v01(TH1D* h1, string MCFile, int detn, double Resa,
                           double Resb) {
  TH1D* hmc = new TH1D(*h1);
  cout << " numero bins mc" << hmc->GetNbinsX() << endl;

  // hmc->SetDirectory(0);

  hmc->Reset();

  TFile* f = new TFile(MCFile.c_str(), "READ");
  cout << "openMC " << MCFile << endl;
  cout << "Resa " << Resa << " Resb " << Resb << endl;

  char nameTREE[100];
  sprintf(nameTREE, "C6D6_%d", detn);
  TTree* TTreeMC = (TTree*)f->Get(nameTREE);

  Double_t Edep;
  Double_t Edep_Gaus;

  TTreeMC->SetBranchAddress("Edep", &Edep);

  // read all entries and fill the histograms
  Int_t nentries = TTreeMC->GetEntries();
  // cout<<"Read "<<nameTREE<<" nentries "<<nentries<<  endl;
  for (Int_t i = 0; i < nentries; i++) {
    TTreeMC->GetEntry(i);
    Edep_Gaus = gRandom->Gaus(Edep, sqrt(Resa * Edep + Resb * Edep * Edep));

    // cout<<i<<" Edep "<<Edep<<" Edep_Gaus "<<Edep_Gaus<<" Sigma"<<sqrt(
    // Resa*Edep+Resb*Edep*Edep)<<endl;
    hmc->Fill(Edep_Gaus);
  }
  TH1D* hNumberOfEvents = (TH1D*)f->Get("hNumberOfEvents");
  double NumberEvents = hNumberOfEvents->GetBinContent(1);

  hmc->Scale(1.0 / NumberEvents / hmc->GetBinWidth(10));
  hmc->GetYaxis()->SetTitle("counts/Event/BinWidth");
  cout << " nEvents" << NumberEvents << "hmc->GetBinWidth(1) "
       << hmc->GetBinWidth(10) << endl;

  f->Close();
  delete f;
  return hmc;
}

void DivideByNeutronFluence_2023(TH1D* h1) {
  // cout << " Dividing by neutron fluence "
  //         "...data/h_EAR1_Flux_FLUKA4.3_L184.4m_r4.5cm"
  //      << endl;
  // // TFile* f1=new TFile("data2/flux_ear2.root","READ");
  // TFile* f1 =
  //     new TFile("data/h_EAR1_Flux_FLUKA4.3_L184.4m_r4.5cm.root", "READ");

  cout << " Dividing by neutron fluence "
          "...DataPlot/evalFlux_prelim.root"
       << endl;
  TFile* f1 = new TFile("DataPlot/evalFlux_prelim.root", "READ");

  if (f1->IsZombie()) {
    cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
         << " ###########" << endl;
    exit(1);
  }
  //  TH1F* n_flux = (TH1F*)f1->Get("h_EAR2_Flux");
  TH1F* n_flux = (TH1F*)f1->Get("hEval_Abs");

  int nbins = h1->GetNbinsX();
  double ene1, ene2, ene, Nneutrons;
  for (int i = 1; i <= nbins; i++) {
    ene1 = h1->GetBinLowEdge(i);
    ene2 = h1->GetBinLowEdge(i + 1);
    ene = h1->GetBinCenter(i);
    Nneutrons = n_flux->Interpolate(ene) * log(ene2 / ene1);

    if (Nneutrons == 0) {
      //  cout << " ########### warning in " << __FILE__ << ", line " <<
      //  __LINE__<< " the numbers of neutrons at energy" << ene1 << " is zero
      //  ###########"      << endl;
      Nneutrons = 1e10;
    }
    h1->SetBinContent(i, h1->GetBinContent(i) / Nneutrons);
    h1->SetBinError(i, h1->GetBinError(i) / Nneutrons);
  }
  h1->GetYaxis()->SetTitle("Yield");

  f1->Close();
  delete f1;
}

TH1D* Project2DHisto(string fname, string NameDetector, string NameHisto,
                     string NameMeasurement, int PulseType, int NumberDetector,
                     bool IfEnOrEdep, double& NPulses, double& NProtons,
                     double TOFD, double EnOrEdepMin, double EnOrEdepMax,
                     double TimeMeasurement_ns);

TH1D* Project2DHistoSubtractBackground(
    string fname, string NameDetector, string NameHisto, string NameMeasurement,
    int PulseType, int NumberDetector, bool IfEnOrEdep, double& NPulses,
    double& NProtons, double TOFD, double EnOrEdepMin, double EnOrEdepMax,
    double TimeMeasurement_ns, std::vector<string>& MeasPredefined,
    std::vector<string>& BackgroundMeasPredefined,
    std::vector<string>& DummyPredefined,
    std::vector<string>& BackgroundDummyPredefined, TCanvas* c2) {
  string fnameMeas;

  fnameMeas = fname + NameMeasurement + ".root";
  double npulses[4];
  double nprotons[4];
  int PositionBackgrounds = FindPositionString(MeasPredefined, NameMeasurement);
  double rebinFactor = 1;

  TH1D* hMeas = Project2DHisto(fnameMeas, NameDetector, NameHisto,
                               NameMeasurement, PulseType, NumberDetector,
                               IfEnOrEdep, npulses[0], nprotons[0], TOFD,
                               EnOrEdepMin, EnOrEdepMax, TimeMeasurement_ns);
  TH1D* hFinal = (TH1D*)hMeas->Clone();

  if ((PulseType == 3) && (NameHisto == "Edep")) {
    cout << "##################" << endl;
    cout << "hMeas " << NameMeasurement << endl;
    cout << "hDummy " << DummyPredefined[PositionBackgrounds] << endl;
    cout << "##################" << endl;
    fnameMeas = fname + DummyPredefined[PositionBackgrounds] + ".root";

    TH1D* hDummy =
        Project2DHisto(fnameMeas, NameDetector, NameHisto,
                       DummyPredefined[PositionBackgrounds], PulseType,
                       NumberDetector, IfEnOrEdep, npulses[1], nprotons[1],
                       TOFD, EnOrEdepMin, EnOrEdepMax, TimeMeasurement_ns);

    hFinal->Add(hDummy,
                -npulses[0] / npulses[1]);  // Quitamos el no beam

    c2->SetTickx();
    c2->SetTicky();
    c2->SetLogy();

    hDummy->Scale(npulses[0] / npulses[1]);

    hMeas->Draw("histo E");
    hDummy->Draw("histo E same");
    hFinal->Draw("histo E same");

    hMeas->SetLineColor(1);
    hDummy->SetLineColor(4);
    hFinal->SetLineColor(8);

    c2->ToggleEditor();
    TLegend* legend = new TLegend(0.6, 0.62, 0.96, 0.86, "");
    legend->AddEntry(hMeas, "hMeas", "l");
    legend->AddEntry(hDummy, "hDummy", "l");
    legend->AddEntry(hFinal, "hFinal", "l");

    legend->SetTextSize(0.06);
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetFillStyle(0);
    legend->Draw();
    c2->cd();
    NPulses = npulses[0];
    NProtons = nprotons[0];
  }

  else {
    rebinFactor = 10000;
    double PulseTypForNoBeam = 3;  // can also be 0
    if (NameHisto == "TOF") {
      PulseTypForNoBeam = 0;
    }
    cout << "##################" << endl;
    cout << "hMeas " << NameMeasurement << endl;
    cout << "hDummy " << DummyPredefined[PositionBackgrounds] << endl;
    cout << "hBackMeas " << BackgroundMeasPredefined[PositionBackgrounds]
         << endl;
    cout << "hBackDummy " << BackgroundDummyPredefined[PositionBackgrounds]
         << endl;
    cout << "##################" << endl;

    fnameMeas = fname + DummyPredefined[PositionBackgrounds] + ".root";
    TH1D* hDummy =
        Project2DHisto(fnameMeas, NameDetector, NameHisto,
                       DummyPredefined[PositionBackgrounds], PulseType,
                       NumberDetector, IfEnOrEdep, npulses[1], nprotons[1],
                       TOFD, EnOrEdepMin, EnOrEdepMax, TimeMeasurement_ns);

    fnameMeas = fname + BackgroundMeasPredefined[PositionBackgrounds] + ".root";
    TH1D* hBackMeas = Project2DHisto(
        fnameMeas, NameDetector, NameHisto,
        BackgroundMeasPredefined[PositionBackgrounds], PulseTypForNoBeam,
        NumberDetector, IfEnOrEdep, npulses[2], nprotons[2], TOFD, EnOrEdepMin,
        EnOrEdepMax, TimeMeasurement_ns);

    fnameMeas =
        fname + BackgroundDummyPredefined[PositionBackgrounds] + ".root";
    TH1D* hBackDummy = Project2DHisto(
        fnameMeas, NameDetector, NameHisto,
        BackgroundDummyPredefined[PositionBackgrounds], PulseTypForNoBeam,
        NumberDetector, IfEnOrEdep, npulses[3], nprotons[3], TOFD, EnOrEdepMin,
        EnOrEdepMax, TimeMeasurement_ns);

    // E
    TH1D* hAllBack = (TH1D*)hDummy->Clone();

    hAllBack->Add(hBackDummy,
                  -npulses[1] / npulses[3]);  // quitamos el no beam del dummy
    hAllBack->Scale(nprotons[0] /
                    nprotons[1]);  // normalizamos a protones del Meas
    hAllBack->Add(hBackMeas,
                  npulses[0] / npulses[2]);  // añadimos el no beam del Meas
    hFinal->Add(hAllBack, -1);               // sustraemos todos los fondos.

    //
    hDummy->Scale(nprotons[0] /
                  nprotons[1]);  // normalizamos a protones del Meas
    hBackMeas->Scale(npulses[0] / npulses[2]);   // normalizamos pulsos del Meas
    hBackDummy->Scale(npulses[0] / npulses[3]);  // normalizamos pulsos del Meas

    NPulses = npulses[0];
    NProtons = nprotons[0];

    c2->SetTickx();
    c2->SetTicky();
    c2->SetLogy();

    if (IfEnOrEdep) {
      rebinFactor = 100;
      c2->SetLogx();
    }
    hMeas->Draw("histo E");
    hAllBack->Draw("histo E same");
    hBackMeas->Draw("histo E same");
    hBackDummy->Draw("histo E same");
    hDummy->Draw("histo E same");
    hFinal->Draw("histo E same");

    hMeas->SetLineColor(1);
    hBackMeas->SetLineColor(2);
    hBackDummy->SetLineColor(6);
    hDummy->SetLineColor(4);
    hAllBack->SetLineColor(5);
    hFinal->SetLineColor(8);

    c2->ToggleEditor();
    TLegend* legend = new TLegend(0.6, 0.62, 0.96, 0.86, "");
    legend->AddEntry(hMeas, "hMeas", "l");
    legend->AddEntry(hBackMeas, "hBackMeas", "l");
    legend->AddEntry(hBackDummy, "hBackDummy", "l");
    legend->AddEntry(hDummy, "hDummy", "l");
    legend->AddEntry(hAllBack, "hAllBack", "l");
    legend->AddEntry(hFinal, "hFinal", "l");

    legend->SetTextSize(0.06);
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetFillStyle(0);
    legend->Draw();
    c2->cd();

    hMeas->Rebin(rebinFactor);
    hBackMeas->Rebin(rebinFactor);
    hBackDummy->Rebin(rebinFactor);
    hDummy->Rebin(rebinFactor);
    hAllBack->Rebin(rebinFactor);
  }

  TH1D* hFinalReturn = (TH1D*)hFinal->Clone();

  hFinal->Rebin(rebinFactor);

  return hFinalReturn;
}

TH1D* Project2DHisto(string fname, string NameDetector, string NameHisto,
                     string NameMeasurement, int PulseType, int NumberDetector,
                     bool IfEnOrEdep, double& NPulses, double& NProtons,
                     double TOFD, double EnOrEdepMin, double EnOrEdepMax,
                     double TimeMeasurement_ns) {
  // gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  TFile* f = new TFile(fname.c_str(), "READ");
  // f->ls();
  cout << "Project2DHisto from " << fname.c_str() << endl;
  if (f->IsZombie()) {
    cout << " The rootfile " << fname.c_str() << " does not exist" << endl
         << " ########### in " << __FILE__ << ", line " << __LINE__
         << " ###########" << endl;
    // TH1D* hFalse = new TH1D("false", "false", 100, 1, 1e4);
    // NProtons = 1;
    // NPulses = 1;
    sleep(3);
    // exit(1);
    // return hFalse;
  }

  string NameHistoPulseIntensity = "hPulIntens_" + to_string(PulseType);
  cout << "Get Histo " << NameHistoPulseIntensity << endl;

  TH1D* h1 = (TH1D*)f->Get(NameHistoPulseIntensity.c_str());
  if (h1->IsZombie()) {
    cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
         << " ###########" << endl;
    exit(1);
  }

  NProtons = (double)h1->GetBinContent(1);
  NPulses = (double)h1->GetBinContent(2);

  TH2D* h2;

  string NameHisto2D = NameHisto + "_" + NameDetector + "_" +
                       to_string(NumberDetector) + "_PType_" +
                       to_string(PulseType);
  cout << "Get Histo " << NameHisto2D << endl;

  h2 = (TH2D*)f->Get(NameHisto2D.c_str());
  h2->Sumw2();

  if (h2->IsZombie()) {
    cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
         << " ###########" << endl;
    exit(1);
  }

  //
  string NameHisto1D = "1D" + NameHisto2D;
  TH1D* h3;
  if (PulseType == 3) {
    // Checks to see if the h2 Histo for Y axis include a time similar to
    // TimeMeasurement_ns
    double TimeHighLimit = EtoTOF(h2->GetYaxis()->GetBinLowEdge(1), TOFD);
    double TimeLowLimit =
        EtoTOF(h2->GetYaxis()->GetBinUpEdge(h2->GetNbinsY()), TOFD);
    if (TimeHighLimit < TimeMeasurement_ns) {
      cout << "TimeHighLimit (" << TimeHighLimit
           << ") is smaller than TimeMeasurement_ns  (" << TimeMeasurement_ns
           << ")" << endl;
      // The time measured high limit has to be higher
      cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
           << " ###########" << endl;
      exit(1);
    }
    if (TimeLowLimit > (TimeMeasurement_ns / 10000)) {
      // The time measured low limit has to be smaller than 0.01%  of the time
      cout << "TimeLowLimit (" << TimeLowLimit
           << ") is bigger than TimeMeasurement_ns / 10000 ("
           << TimeMeasurement_ns / 10000 << ")" << endl;
      cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
           << " ###########" << endl;
      exit(1);
    }
    cout << "Type 3 pulse lowlimit= " << TimeLowLimit
         << "  TimeHighLimit= " << TimeHighLimit << endl;
    if (IfEnOrEdep) {
      int minbin = h2->GetXaxis()->FindBin(EnOrEdepMin);
      int maxbin = h2->GetXaxis()->FindBin(EnOrEdepMax);
      cout << endl
           << " Taking En type 3 spectrum  from " << fname << " between "
           << h2->GetYaxis()->GetBinLowEdge(minbin) << "(" << EnOrEdepMin
           << ") and " << h2->GetYaxis()->GetBinLowEdge(maxbin + 1) << "("
           << EnOrEdepMax << ")   ......" << endl;

      h3 = h2->ProjectionY(NameHisto1D.c_str(), minbin, maxbin, "e");
      // h3->Sumw2();

      double Emin, Emax, ErrorTotalCounts;

      double TotalCounts =
          h3->IntegralAndError(0, h3->GetNbinsX(), ErrorTotalCounts);
      for (int i = 1; i <= h3->GetNbinsX(); i++) {
        Emin = h3->GetBinLowEdge(i);
        Emax = h3->GetBinLowEdge(i + 1);

        h3->SetBinContent(i, TotalCounts * DeltaEnergyToTime(Emin, Emax, TOFD) /
                                 TimeMeasurement_ns);
        h3->SetBinError(i, ErrorTotalCounts *
                               DeltaEnergyToTime(Emin, Emax, TOFD) /
                               TimeMeasurement_ns);
      }
    } else {
      int minbin = h2->GetYaxis()->FindBin(EnOrEdepMin);
      int maxbin = h2->GetYaxis()->FindBin(EnOrEdepMax);
      cout << endl
           << " Taking Edep type 3 for spectrum from " << fname << " between "
           << h2->GetYaxis()->GetBinLowEdge(minbin) << "(" << EnOrEdepMin
           << ") and " << h2->GetYaxis()->GetBinLowEdge(maxbin + 1) << "("
           << EnOrEdepMax << ")   ......" << endl;
      h3 = h2->ProjectionX(NameHisto1D.c_str(), 0, h2->GetNbinsY(), "e");
      // h3->Sumw2();

      double scalefactor = DeltaEnergyToTime(EnOrEdepMin, EnOrEdepMax, TOFD) /
                           TimeMeasurement_ns;
      cout << " DeltaEnergyToTime  "
           << DeltaEnergyToTime(EnOrEdepMin, EnOrEdepMax, TOFD) << endl;

      cout << "scalefactor  " << scalefactor << endl;
      h3->Scale(1.0 * scalefactor);
    };

  }
  ////No pulse 3
  else {
    if (IfEnOrEdep) {
      int minbin = h2->GetXaxis()->FindBin(EnOrEdepMin);
      int maxbin = h2->GetXaxis()->FindBin(EnOrEdepMax);
      cout << endl
           << " Taking En spectrum from " << fname << " between "
           << h2->GetXaxis()->GetBinLowEdge(minbin) << "(" << EnOrEdepMin
           << ") and " << h2->GetXaxis()->GetBinLowEdge(maxbin + 1) << "("
           << EnOrEdepMax << ")   ......" << endl;
      h3 = h2->ProjectionY(NameHisto1D.c_str(), minbin, maxbin, "e");
      // h3->Sumw2();

    } else {
      int minbin = h2->GetYaxis()->FindBin(EnOrEdepMin);
      int maxbin = h2->GetYaxis()->FindBin(EnOrEdepMax);
      cout << endl
           << " Taking Edep spectrum from " << fname << " between "
           << h2->GetYaxis()->GetBinLowEdge(minbin) << "(" << EnOrEdepMin
           << ") and " << h2->GetYaxis()->GetBinLowEdge(maxbin + 1) << "("
           << EnOrEdepMax << ")   ......" << endl;
      h3 = h2->ProjectionX(NameHisto1D.c_str(), minbin, maxbin, "e");
      // h3->Sumw2();
    }
  }

  // Check that the cuts in energy have a diference less than 5% with respect to
  // the ones given in the program
  double MaximunDiferenceAllowedInPercent = 3;

  double EnOrEdepMinUsed =
      h2->GetXaxis()->GetBinLowEdge(h2->GetXaxis()->FindBin(EnOrEdepMin));
  double EnOrEdepMaxUsed =
      h2->GetXaxis()->GetBinLowEdge(h2->GetXaxis()->FindBin(EnOrEdepMax) + 1);

  if (!IfEnOrEdep) {
    EnOrEdepMinUsed =
        h2->GetYaxis()->GetBinLowEdge(h2->GetYaxis()->FindBin(EnOrEdepMin));
    EnOrEdepMaxUsed =
        h2->GetYaxis()->GetBinLowEdge(h2->GetYaxis()->FindBin(EnOrEdepMax) + 1);
  }
  if ((EnOrEdepMinUsed <
       (EnOrEdepMin * (100 - MaximunDiferenceAllowedInPercent) / 100)) ||
      (EnOrEdepMinUsed >
       (EnOrEdepMin * (100 + MaximunDiferenceAllowedInPercent) / 100))) {
    cout << endl
         << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl
         << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl
         << "There is more than " << MaximunDiferenceAllowedInPercent
         << " \% diference used=" << EnOrEdepMinUsed << " should be "
         << EnOrEdepMin << endl;
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl
         << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl
         << endl;
  }

  if ((EnOrEdepMaxUsed <
       (EnOrEdepMax * (100 - MaximunDiferenceAllowedInPercent) / 100)) ||
      (EnOrEdepMaxUsed >
       (EnOrEdepMax * (100 + MaximunDiferenceAllowedInPercent) / 100))) {
    cout << endl
         << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl
         << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl
         << "There is more than " << MaximunDiferenceAllowedInPercent
         << " \% diference used=" << EnOrEdepMaxUsed << " should be "
         << EnOrEdepMax << endl;
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl
         << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl
         << endl;
  }

  cout << " Npulses " << NPulses << " Nprotons " << NProtons
       << " Nprotons/NPulses " << NProtons / NPulses << endl;
  string NameForHisto2D = NameMeasurement + "_" + NameHisto + "_" +
                          NameDetector + "_" + to_string(NumberDetector) +
                          "_PType_" + to_string(PulseType);
  h3->SetName(NameForHisto2D.c_str());
  h3->SetDirectory(0);
  f->Close();
  delete f;

  h3->SetLineWidth(3);

  h3->GetXaxis()->SetLabelSize(0.06);
  h3->GetXaxis()->SetTitleSize(0.06);

  h3->GetXaxis()->SetTitleOffset(1.1);
  h3->GetXaxis()->SetLabelOffset(0.015);

  h3->GetYaxis()->SetLabelSize(0.06);
  h3->GetYaxis()->SetTitleSize(0.06);

  return h3;
}

TH1D* GetSimulMC(TH1D* h1, int detn, string MeasurementType,
                 std::vector<string>& NameSimulArray,
                 std::vector<string>& NameSimulRootfile,
                 std::vector<double>& Activity_kBq,
                 std::vector<int>& ResolNumberDetector,
                 std::vector<double>& ResolParameter1,
                 std::vector<double>& ResolParameter2, string TypeOfPlot) {
  int PositionNameSimul = FindPositionString(NameSimulArray, MeasurementType);
  string fname = NameSimulRootfile[PositionNameSimul];
  double Activity = Activity_kBq[PositionNameSimul];

  int PositionDetector = FindPositionString(ResolNumberDetector, detn);
  double Resol1 = ResolParameter1[PositionDetector];
  double Resol2 = ResolParameter2[PositionDetector];

  TH1D* h2 = TH1DFromMC_Resol_v01(h1, fname, detn, Resol1, Resol2);
  h2->Scale(Activity * 1000);
  cout << "Activity " << Activity << " kBq" << endl;
  return h2;
}

TH1D* GetSimul(TH1D* h1, string MeasurementType,
               std::vector<string>& NameSimulArray,
               std::vector<string>& NameSimulRootfile, string TypeOfPlot) {
  int PositionNameSimul = FindPositionString(NameSimulArray, MeasurementType);
  string fname = NameSimulRootfile[PositionNameSimul] + ".root";

  TFile* f1 = new TFile(fname.c_str(), "READ");
  cout << endl << "GetSimul " << fname << endl;
  if (f1->IsZombie()) {
    cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
         << " ###########" << endl;
    exit(1);
  }

  TH1D* h2 = (TH1D*)f1->Get("h1");

  TH1D* h3 = (TH1D*)h1->Clone();
  for (int i = 1; i <= h3->GetNbinsX(); i++) {
    h3->SetBinContent(i, h2->GetBinContent(h2->FindBin(h3->GetBinCenter(i))));
    h3->SetBinError(i, 0);
  }

  return h3;
}

void Normalize(TH1D** h1, int const nHisto, TH1D* hToNormalize, double Emin,
               double Emax) {
  double Integrals[nHisto];
  double Errors[nHisto];
  double IntegralsToNormalize;
  double ErrorsToNormalize;
  IntegralsToNormalize = hToNormalize->IntegralAndError(
      hToNormalize->FindBin(Emin), hToNormalize->FindBin(Emax),
      ErrorsToNormalize);
  cout << "#####################" << endl;

  cout << "Normalize betwen" << Emin << " " << Emax << endl;
  for (auto i = 0; i < nHisto; i++) {
    Integrals[i] = h1[i]->IntegralAndError(h1[i]->FindBin(Emin),
                                           h1[i]->FindBin(Emax), Errors[i]);
    h1[i]->Scale(IntegralsToNormalize / Integrals[i]);
    cout << "Scale" << h1[i]->GetTitle() << " by "
         << IntegralsToNormalize / Integrals[i] << endl;
  }
  cout << "#####################" << endl;
}

TH1D* CalculateRatioHisto(TH1D* h1, TH1D* h2) {
  TH1D* h3 = (TH1D*)h1->Clone();
  double y1, y2, e1;
  if (h1->GetNbinsX() != h2->GetNbinsX()) {
    cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
         << " ###########" << endl;
    exit(1);
  }
  for (int i = 0; i < h1->GetNbinsX(); i++) {
    y1 = h1->GetBinContent(i);
    e1 = h1->GetBinError(i);
    y2 = h2->GetBinContent(i);
    if (y2 == 0) {
      h3->SetBinContent(i, 0);
      h3->SetBinError(i, 0);
    } else {
      h3->SetBinContent(i, y1 / y2);
      h3->SetBinError(i, e1 / y2);
    }
  }
  return h3;
}

TH1D* CalculateBestShift(double& shift, TH1D* h1, TH1D* h2, double EminFit,
                         double EmaxFit, int Iterations, double MinShift,
                         double MaxShift) {
  TH1D* h3 = (TH1D*)h1->Clone();
  double BestFit = 1e8, BestFitk = 0;
  double Chi2 = 1e8;
  for (int k = 0; k < Iterations; k++) {
    h3 = Transform(h1, MinShift + (k * (MaxShift - MinShift) / Iterations));

    Chi2 = CompareHistograms(h2, h3, EminFit, EmaxFit);

    // cout << k << " " << MinShift + (k * (MaxShift - MinShift) / Iterations)
    //    << " Chi2 " << Chi2 << endl;

    if (Chi2 < BestFit) {
      BestFit = Chi2, BestFitk = k;
    }
  }

  h3 =
      Transform(h1, MinShift + (BestFitk * (MaxShift - MinShift) / Iterations));

  // cout << "BestFit"
  //    << MinShift + (BestFitk * (MaxShift - MinShift) / Iterations) << endl;
  shift = MinShift + (BestFitk * (MaxShift - MinShift) / Iterations);

  return h3;
}

#endif
