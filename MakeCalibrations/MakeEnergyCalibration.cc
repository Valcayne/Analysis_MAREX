
#include "../GeneralFun/MakeCalibrationFunctions.hh"

void CalibrateWithChi(string MCfname, string Expfname, string outFolder,
                      int DetN, string Source, double GammaEnergy, int rebin,
                      double Percen, int npRes, double Res, double PERRes,
                      int npCalib, double Calib, double PERCalib);

void CreateExpfname(string DataFolder, string RunList, string FolderRootFiles,
                    string Expfname, int DetectorsCalibrated, string SourceType,
                    int NumberBinsExpfname, double EMaxExpfname);
void CreateExpfnameRemoveBackground(string DataFolder, string RunList,
                                    string FolderRootFiles, string Expfname,
                                    string ExpfnameBackground,
                                    int DetectorsCalibrated, string SourceType,
                                    int NumberBinsExpfname,
                                    double EMaxExpfname);
void MakeEnergyCalibration(string DataFolder, string RunList,
                           string FolderRootFiles, string Expfname,
                           string ExpfnameBackground, int DetectorsCalibrated,
                           string SourceType, string Source,
                           string SourceTypeBakground, int NumberBinsExpfname,
                           double EMaxExpfname, string MCfname,
                           string outFolder, double GammaEnergy, int rebin,
                           double Percen, int npRes, double Res, double PERRes,
                           int npCalib, double Calib, double PERCalib);
void FitEnergy(string outfolder, int detn);

void FitRes(string outfolder, int detn);

int main(int argc, char** argv) {
  // int Plot(){
  if (argc == 3) {
    int detn = atoi(argv[1]);
    string outfolder = argv[2];
    FitEnergy(outfolder, detn);
    // FitRes(outfolder, detn);

  }

  else if (argc == 23) {
    string Source = argv[1];
    int DetectorsCalibrated = atoi(argv[2]);
    string SourceType = argv[3];

    string DataFolder = argv[4];          //
    string RunList = argv[5];             //
    string FolderRootFiles = argv[6];     //
    string Expfname = argv[7];            //
    string ExpfnameBackground = argv[8];  //
    string SourceTypeBakground = argv[9];
    int NumberBinsExpfname = atoi(argv[10]);
    double EMaxExpfname = atof(argv[11]);
    string MCfname = argv[12];
    string outFolder = argv[13];
    double GammaEnergy = atof(argv[14]);
    int rebin = atoi(argv[15]);
    double Percen = atof(argv[16]);
    int npRes = atoi(argv[17]);
    double Res = atof(argv[18]);
    double PERRes = atof(argv[19]);
    int npCalib = atoi(argv[20]);
    double Calib = atof(argv[21]);
    double PERCalib = atof(argv[22]);

    cout << "Percen " << Percen << endl;
    cout << "npRes " << npRes << endl;
    cout << "Res " << Res << endl;
    cout << "PERRes " << PERRes << endl;
    cout << "npCalib " << npCalib << endl;
    cout << "Calib " << Calib << endl;
    cout << "PERCalib " << PERCalib << endl;

    MakeEnergyCalibration(DataFolder, RunList, FolderRootFiles, Expfname,
                          ExpfnameBackground, DetectorsCalibrated, SourceType,
                          Source, SourceTypeBakground, NumberBinsExpfname,
                          EMaxExpfname, MCfname, outFolder, GammaEnergy, rebin,
                          Percen, npRes, Res, PERRes, npCalib, Calib, PERCalib);
  } else {
    cout << " ******* Input has to be like 22 or 2 and has  " << argc - 1
         << "  *************** entries=  " << endl;
    exit(1);
  }
}

void MakeEnergyCalibration(string DataFolder, string RunList,
                           string FolderRootFiles, string Expfname,
                           string ExpfnameBackground, int DetectorsCalibrated,
                           string SourceType, string Source,
                           string SourceTypeBakground, int NumberBinsExpfname,
                           double EMaxExpfname, string MCfname,
                           string outFolder, double GammaEnergy, int rebin,
                           double Percen, int npRes, double Res, double PERRes,
                           int npCalib, double Calib, double PERCalib) {
  TFile* f = new TFile(ExpfnameBackground.c_str(), "READ");
  if (f->IsZombie()) {
    cout << " ######   file " << ExpfnameBackground.c_str()
         << " does not exist we will created #######" << endl;
    CreateExpfname(DataFolder, RunList, FolderRootFiles, ExpfnameBackground,
                   DetectorsCalibrated, SourceTypeBakground, NumberBinsExpfname,
                   EMaxExpfname);
  }

  f = new TFile(Expfname.c_str(), "READ");
  if (f->IsZombie()) {
    cout << " ######   file " << Expfname.c_str()
         << " does not exist we will created #######" << endl;

    CreateExpfnameRemoveBackground(
        DataFolder, RunList, FolderRootFiles, Expfname, ExpfnameBackground,
        DetectorsCalibrated, SourceType, NumberBinsExpfname, EMaxExpfname);
  }
  CalibrateWithChi(MCfname, Expfname, outFolder, DetectorsCalibrated, Source,
                   GammaEnergy, rebin, Percen, npRes, Res, PERRes, npCalib,
                   Calib, PERCalib);
}

void CreateExpfnameRemoveBackground(string DataFolder, string RunList,
                                    string FolderRootFiles, string Expfname,
                                    string ExpfnameBackground,
                                    int DetectorsCalibrated, string SourceType,
                                    int NumberBinsExpfname,
                                    double EMaxExpfname) {
  string SRunList[100000];
  cout << "start take runlist" << endl;

  int nRuns = TakeRunList(RunList.c_str(), SourceType, FolderRootFiles.c_str(),
                          SRunList);
  cout << "end take runlist" << endl;
  ///-------------------------------------------------
  //-------------------------------------------------

  TH1D* h1 = new TH1D("h1", "h1", NumberBinsExpfname, 0, EMaxExpfname);
  double TotalBunchNumber = 0;

  for (int i = 0; i < nRuns; i++) {
    cout << " open " << SRunList[i].c_str() << " Detname "
         << DetectorsCalibrated << " detN " << DetectorsCalibrated << endl;
    ifstream in(SRunList[i].c_str());
    in.close();
    if (!in.good()) {
      cout << " ######  WARNING, file " << SRunList[i]
           << " does not exist #######" << endl;
    } else {
      TFile* f = new TFile(SRunList[i].c_str(), "READ");
      TTree* TTreeExp = (TTree*)f->Get("C6D6");
      Float_t amp;
      Int_t detn;

      TTreeExp->SetBranchAddress("amp", &amp);
      TTreeExp->SetBranchAddress("detn", &detn);

      Int_t nentries = TTreeExp->GetEntries();
      for (Int_t i = 0; i < nentries; i++) {
        TTreeExp->GetEntry(i);
        if (detn == DetectorsCalibrated) {
          h1->Fill(amp);
        }
      }

      TotalBunchNumber += TTreeExp->GetMaximum("BunchNumber");
    }
  }
  cout << "TotalBunchNumber= " << TotalBunchNumber << endl;
  h1->Scale(1.0 / (double)TotalBunchNumber);

  TFile* f = new TFile(ExpfnameBackground.c_str(), "READ");
  if (f->IsZombie()) {
    cout << " ######   file " << ExpfnameBackground.c_str()
         << " does not exist  #######" << endl;
    exit(1);
  }
  TH1D* hBack = (TH1D*)f->Get("h1");
  h1->Add(hBack, -1);

  TFile* outfnameFile = new TFile(Expfname.c_str(), "RECREATE");
  h1->SetDirectory(outfnameFile);
  outfnameFile->Write();

  f->Close();
  outfnameFile->Close();
}

void CreateExpfname(string DataFolder, string RunList, string FolderRootFiles,
                    string Expfname, int DetectorsCalibrated, string SourceType,
                    int NumberBinsExpfname, double EMaxExpfname) {
  string SRunList[100000];
  cout << "start take runlist" << endl;

  int nRuns = TakeRunList(RunList.c_str(), SourceType, FolderRootFiles.c_str(),
                          SRunList);
  cout << "end take runlist" << endl;
  ///-------------------------------------------------
  //-------------------------------------------------

  TH1D* h1 = new TH1D("h1", "h1", NumberBinsExpfname, 0, EMaxExpfname);
  double TotalBunchNumber = 0;

  for (int i = 0; i < nRuns; i++) {
    cout << " open " << SRunList[i].c_str() << " Detname "
         << DetectorsCalibrated << " detN " << DetectorsCalibrated << endl;
    ifstream in(SRunList[i].c_str());
    in.close();
    if (!in.good()) {
      cout << " ######  WARNING, file " << SRunList[i]
           << " does not exist #######" << endl;
    } else {
      TFile* f = new TFile(SRunList[i].c_str(), "READ");
      TTree* TTreeExp = (TTree*)f->Get("C6D6");
      Float_t amp;
      Int_t detn;

      TTreeExp->SetBranchAddress("amp", &amp);
      TTreeExp->SetBranchAddress("detn", &detn);

      Int_t nentries = TTreeExp->GetEntries();
      for (Int_t i = 0; i < nentries; i++) {
        TTreeExp->GetEntry(i);
        if (detn == DetectorsCalibrated) {
          h1->Fill(amp);
        }
      }

      TotalBunchNumber += TTreeExp->GetMaximum("BunchNumber");
    }
  }
  cout << "TotalBunchNumber= " << TotalBunchNumber << endl;
  h1->Scale(1.0 / (double)TotalBunchNumber);

  TFile* outfnameFile = new TFile(Expfname.c_str(), "RECREATE");
  h1->SetDirectory(outfnameFile);
  outfnameFile->Write();

  outfnameFile->Close();
}

void CalibrateWithChi(string MCfname, string Expfname, string outFolder,
                      int detN, string Source, double GammaEnergy, int rebin,
                      double Percen, int NRESOLUTIONS, double Res,
                      double PERRes, int NCALIBENE, double Calib,
                      double PERCalib) {
  cout << "starting" << endl;
  time_t starttime = time(0);
  int EnergyAdjustCalibrationinKeV = 0;  // it is a variable used to add a small
                                         // shift in the energy calibration
  int const MAXNGAMMAS =
      100000000;  // maximo numero de gammas a coger del Monte Carlo
  char rootoutfname[10000], rootoutfname1[10000];
  ;
  //=======================================================================================l

  double ResMin = 0.01 * Res * (100 - PERRes);
  double ResMax = 0.01 * Res * (100 + PERRes);

  double CalibMin = 0.01 * Calib * (100 - PERCalib);
  double CalibMax = 0.01 * Calib * (100 + PERCalib);

  //--------------------------------------------
  cout << "starting" << endl;

  //--------------------------------------------
  //======================================================================================

  cout << "------------------------------------------------------" << endl;
  cout << "------------------------------------------------------" << endl;
  cout << " Making calibration of detector " << detN << endl;
  cout << "    " << NRESOLUTIONS << " points for resolution between " << ResMin
       << " and " << ResMax << endl;
  cout << "    " << NCALIBENE << " points for calibration between " << CalibMin
       << " and " << CalibMax << endl;
  cout << " Rebin " << rebin << endl;
  cout << " GammaEnergy " << GammaEnergy << endl;

  cout << " Expfname " << Expfname.c_str() << endl;
  cout << " MCfname " << MCfname.c_str() << endl;

  //-----------------------------------------------------------------------------------
  // Take the experimental histogram:
  double ComptonEdgeEnergy =
      GammaEnergy * (1 - 1 / (1 + 2 * GammaEnergy / 0.511));
  cout << " ComptonEdgeEnergy  " << ComptonEdgeEnergy << endl;
  cout << "------------------------------------------------------" << endl;

  cout << " Taking experimental histogram from ..." << Expfname << endl;

  TFile* f = new TFile(Expfname.c_str(), "READ");
  TH1D* hExp = (TH1D*)f->Get("h1");
  hExp->SetDirectory(0);
  f->Close();
  delete f;
  hExp->Rebin(rebin);
  cout << " numero bins" << hExp->GetNbinsX() << endl;

  //-----------------------------------------------------------------------------------

  //-----------------------------------------------------------------------------------

  cout << " Reading Monte Carlo data ..." << endl;

  TH1D* hMC = FillTH1DFromMC(hExp, MCfname.c_str(), detN);

  cout << " Reading Monte Carlo data done" << endl;
  //-----------------------------------------------------------------------------------

  //-----------------------------------------------------------------------------------
  cout << " Comparing MC with the experimental data ..." << endl;
  double DeltaRes = 0;
  double DeltaCalib = 0;
  if (NRESOLUTIONS > 1) {
    DeltaRes = (ResMax - ResMin) / (NRESOLUTIONS - 1.) / 2.;
  }
  if (NCALIBENE > 1) {
    DeltaCalib = (CalibMax - CalibMin) / (NCALIBENE - 1.) / 2.;
  }
  TH2D* h2 =
      new TH2D("h2", "", NRESOLUTIONS, ResMin - DeltaRes, ResMax + DeltaRes,
               NCALIBENE, CalibMin - DeltaCalib, CalibMax + DeltaCalib);
  cout << " Testing " << NRESOLUTIONS << " resolution values:" << endl;
  for (int i = 1; i <= NRESOLUTIONS; i++) {
    cout << "   " << h2->GetXaxis()->GetBinCenter(i);
  }
  cout << endl;
  cout << " Testing " << NCALIBENE << " calibration values:" << endl;
  for (int i = 1; i <= NCALIBENE; i++) {
    cout << "   " << h2->GetYaxis()->GetBinCenter(i);
  }
  cout << endl;

  TH1D* h1 = new TH1D(*hExp);
  TH1D* hRES0 = new TH1D(*hExp);

  h1->SetName("hMC");
  double CalibFactor, ResAtComptonEdge, ResFinal, sigma, amp, Chi2, a_factor;
  int i_min = 0, j_min = 0;
  double MinChi2 = 1.e20;
  for (int i = 1; i <= NRESOLUTIONS; i++) {
    for (int j = 1; j <= NCALIBENE; j++) {
      h1->Reset();
      ResAtComptonEdge = h2->GetXaxis()->GetBinCenter(i);
      CalibFactor = h2->GetYaxis()->GetBinCenter(j);
      // sigma=a*sqrt(E); ---> metemos una dependencia de este tipo en la
      // resolución con la energía
      // Res=2.35*sigma/E=2.35*a*sqrt(E)/E=2.35*a/sqrt(E);
      // a=Res*sqrt(E)/2.35;  --> ese es el valor de a, que es una constante,
      // y se fija con el valor del Compton Alpha is a*a
      // sigma^2=alpha*E+Beta*E*E
      a_factor = ResAtComptonEdge * sqrt(ComptonEdgeEnergy) / 2.35;
      // Llenamos el histograma:
      FillTH1DFromMC(h1, MCfname.c_str(), detN, CalibFactor, a_factor,
                     EnergyAdjustCalibrationinKeV);
      // cout << "CalibFactor " << CalibFactor << " "      <<
      // "ResAtComptonEdge " << ResAtComptonEdge << " "       << "Percen  " <<
      // Percen << " " << "a_factor  " << a_factor << endl << endl;
      Chi2 = CompareHistograms(
          hExp, h1, ComptonEdgeEnergy * (100 - Percen) / (100 * CalibFactor),
          ComptonEdgeEnergy * (100 + Percen) / (100 * CalibFactor));
      h2->SetBinContent(i, j, Chi2);
      if (Chi2 == 0) {
        cout << ResAtComptonEdge << " " << CalibFactor << " " << Chi2 << endl;
      }
      // cout << "Chi2 " << Chi2 << endl;
      if (MinChi2 > Chi2 && Chi2 != 0) {
        MinChi2 = Chi2;
        i_min = i;
        j_min = j;
      }
    }
    cout << "   ....... " << i * 100 / NRESOLUTIONS << " % done" << endl;
  }
  // Creamos histograma con el mejor fit:
  ResAtComptonEdge = h2->GetXaxis()->GetBinCenter(i_min);
  a_factor = ResAtComptonEdge * sqrt(ComptonEdgeEnergy) / 2.35;
  CalibFactor = h2->GetYaxis()->GetBinCenter(j_min);
  h1->Reset();
  // Llenamos el histograma:
  FillTH1DFromMC(h1, MCfname.c_str(), detN, CalibFactor, a_factor,
                 EnergyAdjustCalibrationinKeV);

  cout << " ...... done" << endl;
  //-----------------------------------------------------------------------------------

  //-----------------------------------------------------------------------------------
  // Guardamos los resultados:
  cout << " Saving results ..." << endl;
  ResFinal = h2->GetXaxis()->GetBinCenter(i_min);
  CalibFactor = h2->GetYaxis()->GetBinCenter(j_min);
  cout << " MinChi=" << MinChi2 << endl;
  int firstbin =
      hExp->FindBin(ComptonEdgeEnergy * (100 - Percen) / 100 / CalibFactor);
  int lastbin =
      hExp->FindBin(ComptonEdgeEnergy * (100 + Percen) / 100 / CalibFactor);
  double area1 = hExp->Integral(firstbin, lastbin);
  double area2 = h1->Integral(firstbin, lastbin);
  double area3 = hRES0->Integral(firstbin, lastbin);
  cout << "firstbin" << firstbin << "lastbin" << lastbin << "area1 " << area1
       << " area2 " << area2 << " area3 " << area3 << endl;
  h1->Scale(area1 / area2);
  hRES0->Scale(area1 / area3);

  TH1D* h3 = new TH1D(*h1);
  h3->SetName("hFit");
  for (int i = 1; i <= h3->GetNbinsX(); i++) {
    if (i < firstbin || i > lastbin) {
      h3->SetBinContent(i, 0);
    }
  }
  hExp->SetLineColor(2);
  h1->SetLineColor(8);
  h3->SetLineColor(4);
  //  hRES0->SetLineColor(kCyan);

  TCanvas* c1 = new TCanvas("c1", "", 1000, 600);
  gStyle->SetOptStat(0);

  c1->SetGrid();
  hExp->Draw("histo");
  h1->Draw("histo same");
  h3->Draw("histo same");
  // hRES0->Draw("histo same");
  hExp->GetYaxis()->SetTitle("counts/ms");
  hExp->SetTitle("red:exp -- green:MC -- blue:fit region");

  hExp->GetXaxis()->SetRangeUser(0, ComptonEdgeEnergy / CalibFactor * 1.4);
  hExp->GetYaxis()->SetRangeUser(
      0, 2.0 * h1->GetBinContent(
                   h1->FindBin(0.8 * ComptonEdgeEnergy / CalibFactor)));
  cout << "Channel used to plot " << 0.8 * ComptonEdgeEnergy / CalibFactor
       << endl;
  TLegend* legend = new TLegend(0.5, 0.65, 0.85, 0.85, "");  // tamaño
  string legendentry = "Chi " + to_string(MinChi2);
  legend->AddEntry((TObject*)0, legendentry.c_str(), "");
  legendentry = "Res " + to_string(ResFinal);
  legend->AddEntry((TObject*)0, legendentry.c_str(), "");
  legendentry = "CalibFactor " + to_string(CalibFactor);
  legend->AddEntry((TObject*)0, legendentry.c_str(), "");
  legend->SetFillColor(0);
  legend->SetFillStyle(0);
  legend->Draw();

  // sprintf(rootoutfname,
  //       "%s/Calibration%s_Det%dEner%0.4fChi%0.5fRes%0.4fCalib%0.6f_%d",
  //     outFolder.c_str(), Source.c_str(), detN, ComptonEdgeEnergy, MinChi2,
  //   ResFinal, CalibFactor, EnergyAdjustCalibrationinKeV);
  sprintf(rootoutfname, "%s/Calibration_Det%d_%s", outFolder.c_str(), detN,
          Source.c_str());
  SaveRootEpsPngTxtFunction(c1, rootoutfname);

  //-----------------------------------------------------------------------------------

  cout << "Time since beginning: " << difftime(time(0), starttime) << endl;

  //-----------------------------------------------------------------------------------

  cout << " Line to paste in the file (if converged):" << endl;
  cout << ComptonEdgeEnergy << "  " << ResFinal << "  " << CalibFactor << endl;
  cout << "Alpha parameter  "
       << (ResAtComptonEdge * sqrt(ComptonEdgeEnergy) / 2.35) *
              (ResAtComptonEdge * sqrt(ComptonEdgeEnergy) / 2.35)
       << "  " << endl;

  //-----------------------------------------------------------------------------------
  char namefile[10000];
  sprintf(namefile, "%s/DataDet%d.txt", outFolder.c_str(), detN);
  cout << "Writting results  in file " << namefile << endl;
  ofstream outdata(namefile, ios::out | ios::app | ios::binary);
  outdata << ComptonEdgeEnergy << "  " << ResFinal << "  " << CalibFactor
          << endl;
  outdata.close();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////7
void FitEnergy(string outfolder, int detn) {
  gStyle->SetOptStat(0);

  string OutNameDet = outfolder + "/DataDet" + to_string(detn) + ".out";
  string OutName = outfolder + "/DataCalibration.out";
  cout << "FitEnergy file: " << OutNameDet.c_str() << endl;
  ofstream out(OutNameDet.c_str(), ios::out | ios::app | ios::binary);

  out << endl
      << endl
      << "********************NameFile************** " << endl
      << OutNameDet << endl;
  double maximumAmp = 0;
  string inputFile = outfolder + "/DataDet" + to_string(detn) + ".txt";
  ifstream in(inputFile.c_str());
  double E[100], Res[100], CalibFactor[100], Amp[100];
  int np = 0;
  while (in >> E[np] >> Res[np] >> CalibFactor[np]) {
    Amp[np] = E[np] / CalibFactor[np];
    cout << " reading Value E " << E[np] << " Amp " << Amp[np] << endl;
    out << " reading Value E " << E[np] << " Amp " << Amp[np] << endl;
    if (maximumAmp < Amp[np]) {
      maximumAmp = Amp[np];
    }
    np++;
  }

  TGraph* gr = new TGraph(np, Amp, E);
  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  TH2D* h2 = new TH2D("", "", 1000, -10, 40000, 1000, -0.1, 7);
  TPad* pad1;
  TPad* pad2;
  pad1 = new TPad("pad1", "Pad", 0.0, 0.33, 1., 1., 10);
  pad2 = new TPad("pad2", "Pad", 0.0, 0.02, 1., 0.33, 10);

  pad1->Draw();
  pad2->Draw();
  pad1->SetGridx();
  pad1->SetGridy();
  pad1->SetTopMargin(0.11);      // para que no se vea el eje
  pad1->SetBottomMargin(0.033);  // para que no se vea el eje
  // pad1_6->SetBottomMargin(0.2);//

  pad2->SetTopMargin(0.07);    // para que no se vea el eje
  pad2->SetBottomMargin(0.4);  // para que se vea el eje

  pad1->SetLeftMargin(0.15);  // para que no se vea el eje
  pad2->SetLeftMargin(0.15);  // para que no se vea el eje
  pad2->SetGridy();

  pad1->cd();
  h2->Draw();

  cout << "Fitting pol1 " << endl;
  TF1* myfitpol1 = new TF1("myfitpol1", "[0] + [1]*x", 0.1, 30000);  // pol1
  myfitpol1->SetParLimits(0, 0.001, 0.01);

  gr->Fit("myfitpol1", "R0");
  myfitpol1->SetLineColor(kBlack);
  myfitpol1->Draw("same");
  // myfitpol1->SetLineStyle(2);
  Double_t par1[2];
  myfitpol1->GetParameters(&par1[0]);
  c1->Update();
  double p0pol1 = par1[0];
  double p1pol1 = par1[1];

  cout << "  p0pol1 " << p0pol1 << "  p1pol1 " << p1pol1 << endl;

  cout << "Fitting pol2 " << endl;

  TF1* myfitpol2 =
      new TF1("myfitpol2", "[0] + [1]*x + [2]*x*x", 0.1, 30000);  // pol2
  myfitpol2->SetParLimits(0, 0.001, 0.01);

  gr->Fit("myfitpol2", "R0");
  myfitpol2->Draw("same");
  myfitpol2->SetLineColor(kRed);
  myfitpol2->SetLineStyle(2);

  Double_t par2[3];
  myfitpol2->GetParameters(&par2[0]);
  c1->Update();
  double p0pol2 = par2[0];
  double p1pol2 = par2[1];
  double p2pol2 = par2[2];

  cout << "Fitting pol3 " << endl;

  TF1* myfitpol3 = new TF1("myfitpol3", FunPol2andPol1, 0.1, 3000, 4);  // pol3
  myfitpol3->SetParLimits(0, 0.0001, 0.001);
  // myfitpol3->SetParLimits(1, p1pol2 * 0.99, p1pol2 * 1.01);
  //  myfitpol3->SetParLimits(2, 3000, 10000);
  myfitpol3->SetParLimits(3, 2000, 10000);

  gr->Fit("myfitpol3", "R0");
  myfitpol3->SetParLimits(1, p1pol2 * 0.99, p1pol2 * 1.01);
  Double_t par3[4];
  myfitpol3->GetParameters(&par3[0]);
  c1->Update();
  double p0pol12 = par3[0];
  double p1pol12 = par3[1];
  double p2pol12 = par3[2];
  double p3pol12 = par3[3];
  myfitpol3->SetParLimits(1, p1pol12 * 0.99, p1pol12 * 1.01);

  myfitpol3->SetRange(0.1, 30000);
  gr->Fit("myfitpol3", "R0");

  myfitpol3->Draw("same");
  myfitpol3->SetLineColor(8);
  myfitpol3->SetLineStyle(2);

  myfitpol3->GetParameters(&par3[0]);
  c1->Update();
  p0pol12 = par3[0];
  p1pol12 = par3[1];
  p2pol12 = par3[2];
  p3pol12 = par3[3];

  double x, y;
  gr->GetPoint(np - 1, x, y);

  double Average = 0;
  for (int j = 0; j < np; j++) {
    Average += E[j];
    cout << E[j] << endl;
  }
  Average = Average / np;
  cout << "Average " << Average << endl;

  /// Calculation R2 from Wikipedia
  /// https://en.wikipedia.org/wiki/Coefficient_of_determination
  double SStot = 0;

  double SSresLin = 0;
  double SSresPol = 0;
  double SSresPol12 = 0;

  for (int j = 0; j < np; j++) {
    SStot += (E[j] - Average) * (E[j] - Average);

    SSresLin +=
        (p0pol1 + Amp[j] * p1pol1 - E[j]) * (p0pol1 + Amp[j] * p1pol1 - E[j]);

    SSresPol += (p0pol2 + Amp[j] * p1pol2 + Amp[j] * Amp[j] * p2pol2 - E[j]) *
                (p0pol2 + Amp[j] * p1pol2 + Amp[j] * Amp[j] * p2pol2 - E[j]);
    SSresPol12 +=
        (myfitpol3->Eval(Amp[j]) - E[j]) * (myfitpol3->Eval(Amp[j]) - E[j]);
  }

  double R2Lin = 1 - SSresLin / SStot;
  double R2Pol = 1 - SSresPol / SStot;
  double R2Pol12 = 1 - SSresPol12 / SStot;

  cout << "SSresLin= " << SSresLin << " SStotLin= " << SStot << " R2Lin "
       << R2Lin << endl;
  cout << "SSresPol= " << SSresPol << " SStotLin= " << SStot << " R2Pol "
       << R2Pol << endl;
  cout << "SSresPol12= " << SSresPol12 << " SStotLin= " << SStot << " R2Pol "
       << R2Pol12 << endl;

  double PointCheckLinear[100], PointCheckParabola[100], PointCheckPol12[100];

  /// Checking Results
  for (int j = 0; j < np; j++) {
    PointCheckLinear[j] = (p0pol1 + Amp[j] * p1pol1) / E[j];
    PointCheckParabola[j] =
        (p0pol2 + Amp[j] * p1pol2 + Amp[j] * Amp[j] * p2pol2) / E[j];
    PointCheckPol12[j] = myfitpol3->Eval(Amp[j]) / E[j];
    cout << "TEST " << j << " " << E[j] << " "
         << (p0pol1 + Amp[j] * p1pol1) / E[j] << endl;

    cout << "Pol1 energy Real E " << E[j] << " using "
         << p0pol1 + Amp[j] * p1pol1 << " Diferance % "
         << 100 * (p0pol1 + Amp[j] * p1pol1 - E[j]) / E[j] << endl;
    cout << "Pol2 energy Real E " << E[j] << " using "
         << p0pol2 + Amp[j] * p1pol2 + Amp[j] * Amp[j] * p2pol2
         << " Diferance % "
         << 100 * (p0pol2 + Amp[j] * p1pol2 + Amp[j] * Amp[j] * p2pol2 - E[j]) /
                E[j]
         << endl;

    cout << "Pol12 energy Real E " << E[j] << " using "
         << myfitpol3->Eval(Amp[j]) << " Diferance % "
         << 100 * (myfitpol3->Eval(Amp[j]) - E[j]) / E[j] << endl;
    cout << endl << endl;

    out << "Pol1 energy Real E " << E[j] << " using "
        << p0pol1 + Amp[j] * p1pol1 << " Diferance % "
        << 100 * (p0pol1 + Amp[j] * p1pol1 - E[j]) / E[j] << endl;
    out << "Pol2 energy Real E " << E[j] << " using "
        << p0pol2 + Amp[j] * p1pol2 + Amp[j] * Amp[j] * p2pol2
        << " Diferance % "
        << 100 * (p0pol2 + Amp[j] * p1pol2 + Amp[j] * Amp[j] * p2pol2 - E[j]) /
               E[j]
        << endl;
    out << "Pol12 energy Real E " << E[j] << " using "
        << myfitpol3->Eval(Amp[j]) << " Diferance % "
        << 100 * (myfitpol3->Eval(Amp[j]) - E[j]) / E[j] << endl;
    cout << endl << endl;
    out << endl << endl;
  }

  h2->GetXaxis()->SetTitleSize(0.090);
  h2->GetXaxis()->SetLabelSize(0.090);
  h2->GetXaxis()->SetLabelOffset(0.02);

  h2->GetYaxis()->SetTitleSize(0.090);
  h2->GetYaxis()->SetLabelSize(0.090);
  h2->GetYaxis()->SetTitleOffset(0.85);
  gr->SetMarkerStyle(20);
  gr->SetMarkerColor(4);

  h2->GetXaxis()->SetTitle("Amp(channels)");
  h2->GetYaxis()->SetTitle("Energy (MeV)");
  gr->Draw("P same");
  h2->GetXaxis()->SetRangeUser(0, x * 1.1);
  h2->GetYaxis()->SetRangeUser(0, y * 1.1);

  h2->GetXaxis()->SetRangeUser(0, maximumAmp * 1.1);
  h2->GetYaxis()->SetRangeUser(0, 6);

  TLegend* legend1 = new TLegend(0.15, 0.6, 0.6, 0.85, "");  // tamaño
  legend1->AddEntry(gr, "Exp points", "p");
  legend1->AddEntry(myfitpol1, "Pol 1st degree", "l");
  legend1->AddEntry(myfitpol2, "Pol 2nd degree", "l");
  legend1->AddEntry(myfitpol3, "Pol 2nd+1st degree", "l");

  legend1->SetBorderSize(1);

  legend1->SetFillColor(0);
  legend1->Draw();

  TLine* l3 = new TLine(p3pol12, 0, p3pol12, 6);

  l3->SetLineColor(8);
  l3->SetLineWidth(2);
  l3->Draw();

  pad2->cd();
  TH2D* h3 = new TH2D("", "", 1000, -10, 40000, 100, 0, 2);
  TGraph* CheckParabola = new TGraph(np, Amp, PointCheckParabola);
  TGraph* CheckLinear = new TGraph(np, Amp, PointCheckLinear);
  TGraph* CheckPol12 = new TGraph(np, Amp, PointCheckPol12);

  legend1->AddEntry(l3, "Line to separate pols ", "l");

  TLine* l4 = new TLine(0, 1, maximumAmp * 1.1, 1);
  h3->GetXaxis()->SetRangeUser(0, maximumAmp * 1.1);

  l4->SetLineColor(kBlack);
  l4->SetLineWidth(2);

  h3->SetMarkerStyle(20);
  h3->SetMarkerColor(2);
  h3->SetMarkerSize(0.7);
  h3->GetYaxis()->SetTitle("Ratio");
  h3->GetYaxis()->CenterTitle();
  h3->GetYaxis()->SetTitleOffset(0.35);
  h3->GetYaxis()->SetTitleSize(0.14);
  h3->GetYaxis()->SetLabelSize(0.2);
  h3->GetXaxis()->SetTitleSize(0.18);
  h3->GetYaxis()->SetTitleSize(0.18);
  h3->GetXaxis()->SetLabelSize(0.2);
  h3->GetYaxis()->SetNdivisions(5, 5, 0);
  h3->GetXaxis()->SetNdivisions(8, 5, 0);
  h3->GetXaxis()->SetLabelOffset(0.006);
  h3->GetXaxis()->SetTitle("Amp(channels)");

  CheckParabola->SetMarkerStyle(20);
  CheckParabola->SetMarkerColor(kRed);
  CheckLinear->SetMarkerStyle(20);
  CheckLinear->SetMarkerColor(kBlack);
  CheckPol12->SetMarkerStyle(20);
  CheckPol12->SetMarkerColor(8);
  h3->GetYaxis()->SetRangeUser(0.7, 1.3);
  h3->Draw();
  l4->Draw();

  CheckParabola->Draw("P same");
  CheckLinear->Draw("P same");
  CheckPol12->Draw("P same");

  cout << "FisrtDegree" << endl << p0pol1 << "  " << p1pol1 << endl << endl;
  cout << "SecondDegree" << endl
       << p0pol2 << "  " << p1pol2 << " " << p2pol2 << endl;
  cout << "Pol12" << endl
       << p0pol12 << "  " << p1pol12 << " " << p2pol12 << " " << p3pol12
       << endl;

  out << "FisrtDegree" << endl << p0pol1 << "  " << p1pol1 << endl << endl;
  out << "SecondDegree" << endl
      << p0pol2 << "  " << p1pol2 << " " << p2pol2 << endl;

  string OutNameDetRoot = outfolder + "/EnergyFitDet" + to_string(detn);
  SaveRootEpsPngTxtFunction(c1, OutNameDetRoot.c_str());

  ofstream outAllDet(OutName, ios::out | ios::app | ios::binary);

  outAllDet << detn << " " << p0pol2 << "  " << p1pol2 << " " << p2pol2 << endl;
  outAllDet.close();

  out.close();
}
////////////////

/////////////////

// Energy resolution:
void FitRes(string outfolder, int detn) {
  gStyle->SetOptStat(0);

  string OutNameDet = outfolder + "/DataDet" + to_string(detn) + ".out";
  string OutName = outfolder + "/DataRes.out";

  ofstream out(OutNameDet.c_str(), ios::out | ios::app | ios::binary);

  out << endl
      << endl
      << "********************NameFile************** " << endl
      << OutNameDet << endl;
  string inputFile = outfolder + "/DataDet" + to_string(detn) + ".txt";

  ifstream in(inputFile.c_str());

  double E[100], Res[100], CalibFactor[100];
  int np = 0;
  while (in >> E[np] >> Res[np] >> CalibFactor[np]) {
    np++;
    //  if(E[np-1]==4.19641){np=np-1; cout<<endl<<"
    //  ################################ "<<endl<<"The AmBe point is not
    //  considered for the
    //  resolution"<<endl<<"################################
    //  "<< endl;   }
  }
  cout << np << " points read in " << outfolder << endl;

  TGraph* gr = new TGraph(np, E, Res);
  gr->SetMarkerStyle(20);
  gr->SetMarkerColor(2);
  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);

  TH2D* h2 =
      new TH2D("", "Energy resolution", 1000, 0, E[np - 1] * 50, 1000, 0, 0.6);
  TPad* pad1;
  TPad* pad2;
  pad1 = new TPad("pad1", "Pad", 0.0, 0.33, 1., 1., 10);
  pad2 = new TPad("pad2", "Pad", 0.0, 0.02, 1., 0.33, 10);

  pad1->Draw();
  pad2->Draw();
  pad1->SetGridx();
  pad1->SetGridy();
  pad1->SetTopMargin(0.11);      // para que no se vea el eje
  pad1->SetBottomMargin(0.033);  // para que no se vea el eje
  // pad1_6->SetBottomMargin(0.2);//

  pad2->SetTopMargin(0.07);    // para que no se vea el eje
  pad2->SetBottomMargin(0.4);  // para que se vea el eje

  pad1->SetLeftMargin(0.15);  // para que no se vea el eje
  pad2->SetLeftMargin(0.15);  // para que no se vea el eje
  pad2->SetGridy();

  pad1->cd();
  h2->Draw();
  h2->GetXaxis()->SetTitle("Energy (MeV)");
  h2->GetYaxis()->SetTitle("Resolution");
  h2->GetYaxis()->SetTitleOffset(1.2);

  h2->GetXaxis()->SetRangeUser(0, 6);
  h2->GetYaxis()->SetRangeUser(0, 0.5);

  gStyle->SetOptStat(0);

  h2->Draw();
  gr->Draw("P same");

  // sig=sqrt(a*E+b*E*E)
  // Res=2.35*sigma/E=2.35*sqrt(a*E+b*E*E)/E;
  TF1* f1 = new TF1("ResFun", ResFun, 0.1, 10, 2);
  f1->SetParameter(0, 0.005);
  f1->SetParameter(1, 0.0005);
  // f1->FixParameter(0, 0.005);
  // f1->FixParameter(1, 0.0005);

  gr->Fit(f1);

  Double_t par[2], par1[2];
  f1->GetParameters(&par[0]);
  // c1->Update();
  double par0v = par[0];
  double par1v = par[1];
  // f1->Draw("L same");
  double PointCheck[100];
  for (int j = 0; j < np; j++) {
    PointCheck[j] = ResFunNormal(E[j], par0v, par1v) / Res[j];

    cout << "energy Real E " << E[j] << " Res " << Res[j] << " using resfun "
         << ResFunNormal(E[j], par0v, par1v) << " Diferance % "
         << 100 * (ResFunNormal(E[j], par0v, par1v) - Res[j]) / Res[j] << endl;
    out << "energy Real E " << E[j] << " Res " << Res[j] << " using resfun "
        << ResFunNormal(E[j], par0v, par1v) << " Diferance % "
        << 100 * (ResFunNormal(E[j], par0v, par1v) - Res[j]) / Res[j] << endl;
  }
  h2->GetXaxis()->SetTitleSize(0.090);
  h2->GetXaxis()->SetLabelSize(0.090);
  h2->GetXaxis()->SetLabelOffset(0.02);

  h2->GetYaxis()->SetTitleSize(0.090);
  h2->GetYaxis()->SetLabelSize(0.090);
  h2->GetYaxis()->SetTitleOffset(0.85);
  gr->SetMarkerStyle(20);
  gr->SetMarkerColor(4);

  h2->GetXaxis()->SetTitle("Amp(channels)");
  h2->GetYaxis()->SetTitle("Energy (MeV)");
  gr->Draw("P same");

  // sig=sqrt(a*E+b*E*E)
  // Res=2.35*sigma/E=2.35*sqrt(a*E+b*E*E)/E;
  pad2->cd();
  TH2D* h3 = new TH2D("", "", 1000, -0, 6, 100, 0, 2);
  TGraph* Check = new TGraph(np, E, PointCheck);
  TLine* l4 = new TLine(0, 1, 6, 1);

  l4->SetLineColor(kBlack);
  l4->SetLineWidth(2);

  h3->SetMarkerStyle(20);
  h3->SetMarkerColor(2);
  h3->SetMarkerSize(0.7);
  h3->GetYaxis()->SetTitle("Ratio");
  h3->GetYaxis()->CenterTitle();
  h3->GetYaxis()->SetTitleOffset(0.35);
  h3->GetYaxis()->SetTitleSize(0.14);
  h3->GetYaxis()->SetLabelSize(0.2);
  h3->GetXaxis()->SetTitleSize(0.18);
  h3->GetYaxis()->SetTitleSize(0.18);
  h3->GetXaxis()->SetLabelSize(0.2);
  h3->GetYaxis()->SetNdivisions(5, 5, 0);
  h3->GetXaxis()->SetNdivisions(8, 5, 0);
  h3->GetXaxis()->SetLabelOffset(0.006);
  h3->GetXaxis()->SetTitle("Amp(channels)");

  Check->SetMarkerStyle(20);
  Check->SetMarkerColor(kRed);

  h3->GetYaxis()->SetRangeUser(0.5, 1.5);
  h3->Draw();
  l4->Draw();

  Check->Draw("P same");
  cout << "Energy resolution " << endl << par0v << " " << par1v << endl;
  out << "Energy resolution " << endl << par0v << " " << par1v << endl;

  string OutNameroot = outfolder + "/ResolFitDet" + to_string(detn);

  SaveRootEpsPngTxtFunction(c1, OutNameroot.c_str());

  ofstream outAllDet(OutName, ios::out | ios::app | ios::binary);

  outAllDet << detn << " " << par0v << "  " << par1v << endl;
  outAllDet.close();

  out.close();
}

///////////////////////    ///////////////////////

///////////////////////    ///////////////////////

///////////////////////    ///////////////////////

///////////////////////    ///////////////////////

//
