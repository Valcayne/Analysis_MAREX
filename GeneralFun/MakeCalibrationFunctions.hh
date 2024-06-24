#ifndef MAKECALIBRATIONFUNCTIONS_HH
#define MAKECALIBRATIONFUNCTIONS_hh 1

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


*/

TH1D* FillTH1DFromMC(TH1D* h1, const char* MCFile, int detn);
void FillTH1DFromMC(TH1D* hmc, const char* MCFile, int detn, double CalibFactor,
                    double a_factor, int EnergyAdjustCalibrationinKeV = 0);

TH1D* FillTH1DFromMC(TH1D* h1, const char* MCFile, int detn) {
  cout << " numero bins" << h1->GetNbinsX() << endl;
  TH1D* hmc = new TH1D(*h1);
  // hmc->SetDirectory(0);

  hmc->Reset();

  TFile* f = new TFile(MCFile, "READ");
  TTree* TTreeMC = (TTree*)f->Get("C6D6_All");

  char condition[100];
  sprintf(condition, "Det==%d", detn);
  TTreeMC->Project("hmc", "Edep", condition);

  TH1D* hNumberOfEvents = (TH1D*)f->Get("hNumberOfEvents");
  double NumberEvents = hNumberOfEvents->GetBinContent(1);

  hmc->Scale(1.0 / NumberEvents / hmc->GetBinWidth(1));
  cout << "Scale " << 1.0 / NumberEvents / hmc->GetBinWidth(1) << endl;
  hmc->GetYaxis()->SetTitle("counts/event/BinWidth");
  f->Close();
  delete f;
  return hmc;
}

void FillTH1DFromMC(TH1D* hmc, const char* MCFile, int detn, double CalibFactor,
                    double a_factor, int EnergyAdjustCalibrationinKeV) {
  TFile* f = new TFile(MCFile, "READ");
  //  cout<<"open "<<MCFile<<endl;
  char nameTREE[100];
  sprintf(nameTREE, "C6D6_%d", detn);
  TTree* TTreeMC = (TTree*)f->Get(nameTREE);

  Double_t Edep;
  TTreeMC->SetBranchAddress("Edep", &Edep);

  // read all entries and fill the histograms
  Int_t nentries = TTreeMC->GetEntries();
  // cout<<"Read "<<nameTREE<<" nentries "<<nentries<<  endl;

  for (Int_t i = 0; i < nentries; i++) {
    TTreeMC->GetEntry(i);
    //    cout<<i<<" Edep-0.020 "<<Edep<<endl;
    Edep = Edep - ((double)EnergyAdjustCalibrationinKeV / 1000);
    //  cout<<i<<" Edep-"<<(double)EnergyAdjustCalibrationinKeV/1000<<endl;

    hmc->Fill(
        gRandom->Gaus(Edep / CalibFactor, a_factor * sqrt(Edep) / CalibFactor));
    //   cout<<"Fill
    //   "<<gRandom->Gaus(Edep/CalibFactor,a_factor*sqrt(Edep)/CalibFactor)<<endl;
  }
  TH1D* hNumberOfEvents = (TH1D*)f->Get("hNumberOfEvents");
  double NumberEvents = hNumberOfEvents->GetBinContent(1);

  hmc->Scale(1.0 / NumberEvents / hmc->GetBinWidth(0));
  hmc->GetYaxis()->SetTitle("counts/event/BinWidth");
  f->Close();
  delete f;
}

double ResFun(double* x, double* par) {
  return 2.35 * sqrt(par[0] * x[0] + par[1] * x[0] * x[0]) / x[0];
}
double ResFunOnlyCuadratic(double* x, double* par) {
  return 2.35 * sqrt(par[0] / x[0]);
  // sigma=sqrt(bo*E)=E*R/2.35->R=2.35*sqrt(bo*E)/E=2.35*sqrt(bo/E)
}
double ResFunNormal(double x, double par0, double par1) {
  return 2.35 * sqrt(par0 * x + par1 * x * x) / x;
}

bool FileExists(string filename) {
  ifstream file(filename);
  bool FileExist;
  if (file.is_open()) {
    FileExist = 1;
    file.close();
  } else {
    FileExist = 0;
  }
  return FileExist;
}
//=======================================================================

#endif
