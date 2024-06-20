#ifndef PLOTFUNCTIONS02_HH
#define PLOTFUNCTIONS02_HH 1

#include "AnalysisFunctions01.hh"

//===========================================================================================
TH1D* GetEnFrom2DHisto(const char* fname,const char* basehname,int DetN,int PulseType,double Edep_min,double Edep_max,double &npulses,double &nprotons);
TH1D* GetEdepFrom2DHisto(const char* fname,const char* basehname,int DetN,int PulseType,double En_min,double En_max,double &npulses,double &nprotons);

//===========================================================================================

TH1D* GetEnFrom2DHisto(const char* fname,const char* basehname,int DetN,int PulseType,double Edep_min,double Edep_max,double &npulses,double &nprotons){

  char hname[200];
  sprintf(hname,"%s_%d_PType_%d",basehname,DetN,PulseType); 
  TFile* f1=new TFile(fname,"READ");
  TH2D* h2=(TH2D*)f1->Get(hname);
  if(h2==0){return 0;}
  int binmin=h2->GetYaxis()->FindBin(Edep_min); if(binmin<1){binmin=1;}
  int binmax=h2->GetYaxis()->FindBin(Edep_max); if(binmax>h2->GetYaxis()->GetBinLowEdge(h2->GetYaxis()->GetNbins()+1)){binmax=h2->GetYaxis()->GetNbins();}
  TH1D* h1=h2->ProjectionX(fname,binmin,binmax,"e");
  cout<<" Getting histogram from "<<fname<<" with limits "<<h2->GetYaxis()->GetBinLowEdge(binmin)<<" and "<<h2->GetYaxis()->GetBinLowEdge(binmax+1)<<endl;
  h1->SetDirectory(0);
  f1->Close();
  delete f1;

  char hbeamname[100]; sprintf(hbeamname,"hPulIntens_%d",PulseType);
  TH1D* hBeam1=GetMonHistogram(hbeamname,fname);
  npulses=hBeam1->GetBinContent(2);
  nprotons=hBeam1->GetBinContent(1);
  delete hBeam1;
			       
  h1->SetLineWidth(3);
  return h1;
}


TH1D* GetEdepFrom2DHisto(const char* fname,const char* basehname,int DetN,int PulseType,double En_min,double En_max,double &npulses,double &nprotons){

  char hname[200];
  sprintf(hname,"%s_%d_PType_%d",basehname,DetN,PulseType); 
  TFile* f1=new TFile(fname,"READ");
  TH2D* h2=(TH2D*)f1->Get(hname);
  if(h2==0){return 0;}
  int binmin=h2->GetXaxis()->FindBin(En_min); if(binmin<1){binmin=1;}
  int binmax=h2->GetXaxis()->FindBin(En_max); if(binmax>h2->GetXaxis()->GetBinLowEdge(h2->GetXaxis()->GetNbins()+1)){binmax=h2->GetXaxis()->GetNbins();}
  TH1D* h1=h2->ProjectionY(fname,binmin,binmax,"e");
  cout<<" Getting histogram from "<<fname<<" with limits "<<h2->GetXaxis()->GetBinLowEdge(binmin)<<" and "<<h2->GetXaxis()->GetBinLowEdge(binmax+1)<<endl;
  h1->SetDirectory(0);
  f1->Close();
  delete f1;

  char hbeamname[100]; sprintf(hbeamname,"hPulIntens_%d",PulseType);
  TH1D* hBeam1=GetMonHistogram(hbeamname,fname);
  if(hBeam1){
    npulses=hBeam1->GetBinContent(2);
    nprotons=hBeam1->GetBinContent(1);
  }
  else{
    npulses=0;
    nprotons=0;
  }
  h1->SetLineWidth(3);
  delete hBeam1;
  
  return h1;
}


#endif
