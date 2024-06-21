
#include "../GeneralFun/PlotFunctions02.hh"

#define HISTOS2D_DIR "/eos/home-e/emendoza/data/Er-2024/Histo2D-01/"

//===============================================================================
TH1D* GetEdepSpectrum(int run,string detName,int DetID,int PulseType,double EnMin,double EnMax,double &npulses,double& nprotons);
double GetGainChange(int run1,int run2,string detName,int DetID,int PulseType,int rebin,double EnMin,double EnMax,double ampMin,double ampMax,bool PLOT=true,double ShiftMin=0.8,double ShiftMax=1.2,int npShift=200,bool NORM=true);
TGraph* GetGainMonitoring(const char* DetName,int DetID,const char* outdir,const char* RunDescriptor);

//===============================================================================

//Retrieves the gain change in run2 with respect to run1
double GetGainChange(int run1,int run2,string detName,int DetID,int PulseType,int rebin,double EnMin,double EnMax,double ampMin,double ampMax,bool PLOT,double ShiftMin,double ShiftMax,int npShift,bool NORM){

  cout<<detName<<"  "<<DetID<<"  "<<PulseType<<"  "<<rebin<<"  "<<EnMin<<"  "<<EnMax<<"  "<<ampMin<<"  "<<ampMax<<"  "<<PLOT<<"  "<<ShiftMin<<"  "<<ShiftMax<<"  "<<npShift<<"  "<<NORM<<endl;
  SetStyle01();
  double MinNCounts=1.e3;
  double npulses1,npulses2;
  double nprotons1,nprotons2;
  TH1D* h1=GetEdepSpectrum(run1,detName,DetID,PulseType,EnMin,EnMax,npulses1,nprotons1);
  TH1D* h2=GetEdepSpectrum(run2,detName,DetID,PulseType,EnMin,EnMax,npulses2,nprotons2);
  if(h1==0 || h2==0){return -1;}
  double err;
  double NCounts1=IntegrateHisto(h1,ampMin,ampMax,err,true,false);
  double NCounts2=IntegrateHisto(h2,ampMin,ampMax,err,true,false);
  //cout<<NCounts1<<"  "<<NCounts2<<endl;
  if(NCounts1<MinNCounts || NCounts2<MinNCounts){
    return -2;
  }
  h1->Scale(8.e12/nprotons1);
  h2->Scale(8.e12/nprotons2);
  h1->Rebin(rebin);
  h2->Rebin(rebin);
  TH1D* hComp=new TH1D("hComp","",npShift,ShiftMin,ShiftMax);
  double ShiftFactor=TakeBestShiftFactor(h2,h1,ampMin,ampMax,hComp,NORM);
  cout<<" Shift factor is --> "<<ShiftFactor<<endl;
  
  TCanvas* c1=0;
  TCanvas* c2=0;
  if(PLOT){
    h1->GetYaxis()->SetTitle("counts/(8e12 protons)");
    h1->GetXaxis()->SetNdivisions(507);
    TH1D* h3=ShiftHisto(h1,ShiftFactor);
    h1->SetLineColor(1);
    h2->SetLineColor(2);
    h3->SetLineColor(4);
    h3->SetLineStyle(7);
    c1=new TCanvas("c1","c1",600,600);
    c1->SetLogy();
    h1->Draw("histo");
    h2->Draw("histo same");
    h3->Draw("histo same");
    c2=new TCanvas("c2","c2",600,600);
    hComp->SetLineWidth(3);
    hComp->GetXaxis()->SetNdivisions(507);
    hComp->SetLineColor(2);
    hComp->GetXaxis()->SetTitle("Shift");
    hComp->GetYaxis()->SetTitle("Chi2");
    hComp->Draw("histo");
  }
  else{
    delete hComp;
  }
  return ShiftFactor;
}


TH1D* GetEdepSpectrum(int run,string detName,int DetID,int PulseType,double EnMin,double EnMax,double &npulses,double& nprotons){

  char fname[300],basehname[100];
  sprintf(fname,"%s/Histos01_%d.root",HISTOS2D_DIR,run);
  sprintf(basehname,"Edep_%s",detName.c_str());
  
  TH1D* h1=GetEdepFrom2DHisto(fname,basehname,DetID,PulseType,EnMin,EnMax,npulses,nprotons);
  return h1;
}


TGraph* GetGainMonitoring(const char* DetName,int DetID,const char* outdir,const char* RunDescriptor){

  char command[1000];
  sprintf(command,"ls %s/GShift_%s_*.out > borrar.txt",outdir,RunDescriptor);
  int check=system(command);

  double RunNumber[100000];
  double GainShift[100000];

  char fname[200],dum[100],detname[100];
  int detID;
  double Shift;
  int np=0;
  ifstream in1("borrar.txt");
  while(in1>>fname){
    ifstream in2(fname);
    in2>>dum>>RunNumber[np];
    while(in2>>detname>>detID>>Shift){
      GainShift[np]=Shift;
      if(detID==DetID && string(detname)==string(DetName) && Shift>0){
	np++;
	break;
      }
    }
    in2.close();
  }
  in1.close();
  
  sprintf(command,"rm borrar.txt");
  check=system(command);

  TGraph* gr=new TGraph(np,RunNumber,GainShift);
  char title[100];
  sprintf(title,"%s - %.2d",DetName,DetID);
  gr->SetTitle(title);
  gr->GetXaxis()->SetTitle("Run number");
  gr->GetYaxis()->SetTitle("Gain variation");

  gr->SetLineWidth(2);
  gr->SetMarkerStyle(20);
  
  return gr;
}
