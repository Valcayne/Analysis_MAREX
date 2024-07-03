
#include "../GeneralFun/PlotFunctions02.hh"
#include <iomanip>

//#define HISTOS2D_DIR "/eos/home-e/emendoza/data/Er-2024/Histo2D-01/"
#define HISTOS2D_DIR "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/2DHistos/v03/"

struct GainMonInfo{
  string detName="none";
  int DetID=1,PulseType=1,rebin=1,npShift=200,HalfNPToFitParabolic=25;
  double EnMin=1.,EnMax=5.e3,ampMin=0,ampMax=1.e5,ShiftMin=0.8,ShiftMax=1.2,MinNCounts=1.e4,ExtraShift=1.0;
  bool norm=true;
};

//===============================================================================
TH1D* GetEdepSpectrum(int run,string detName,int DetID,int PulseType,double EnMin,double EnMax,double &npulses,double& nprotons);
double GetGainChange(int run1,int run2,string detName,int DetID,int PulseType,int rebin,double EnMin,double EnMax,double ampMin,double ampMax,bool PLOT=true,double ShiftMin=0.8,double ShiftMax=1.2,int npShift=200,bool NORM=true,double MinNCounts=1.e4,int HalfNPToFitParabolic=25);
double GetGainChange(int run1,int run2,string detName,int DetID,string inpfile);
std::vector<GainMonInfo> ReadGainMonInput(string fname);
TGraph* GetGainMonitoring(const char* DetName,int DetID,const char* outdir,int &FirstRefRun,const char* RunDescriptor=0,bool Recalculate=true);
int ReCalculateGainShifts(int nRuns,double* RunNumber,double* RefRun,double* GainShift,bool Recalculate=true);

//===============================================================================


double GetGainChange(int run1,int run2,string detName,int DetID,string inpfile){

  vector<GainMonInfo> vMonInfo=ReadGainMonInput(inpfile);
  auto theMonInfo=vMonInfo.begin();
  bool found=false;
  for( ; theMonInfo != vMonInfo.end(); ++theMonInfo){
    if(theMonInfo->detName==detName && theMonInfo->DetID==DetID){
      found=true; break;
    }
  }
  if(!found){cout<<" ######## ERROR in "<<__FILE__<<" , line "<<__LINE__<<" ########"<<endl; exit(1);}
  
  double GainChange=GetGainChange(run1,run2,theMonInfo->detName,theMonInfo->DetID,theMonInfo->PulseType,theMonInfo->rebin,theMonInfo->EnMin,theMonInfo->EnMax,theMonInfo->ampMin,theMonInfo->ampMax,true,theMonInfo->ShiftMin,theMonInfo->ShiftMax,theMonInfo->npShift,theMonInfo->norm,theMonInfo->MinNCounts,theMonInfo->HalfNPToFitParabolic);

  return GainChange;
}
  

//Retrieves the gain change in run2 with respect to run1
double GetGainChange(int run1,int run2,string detName,int DetID,int PulseType,int rebin,double EnMin,double EnMax,double ampMin,double ampMax,bool PLOT,double ShiftMin,double ShiftMax,int npShift,bool NORM,double MinNCounts,int HalfNPToFitParabolic){

  cout<<detName<<"  "<<DetID<<"  "<<PulseType<<"  "<<rebin<<"  "<<EnMin<<"  "<<EnMax<<"  "<<ampMin<<"  "<<ampMax<<"  "<<PLOT<<"  "<<ShiftMin<<"  "<<ShiftMax<<"  "<<npShift<<"  "<<NORM<<"  "<<MinNCounts<<"  "<<HalfNPToFitParabolic<<endl;
  SetStyle01();
  //double MinNCounts=1.e4;
  double npulses1,npulses2;
  double nprotons1,nprotons2;
  TH1D* h1=GetEdepSpectrum(run1,detName,DetID,PulseType,EnMin,EnMax,npulses1,nprotons1);
  TH1D* h2=GetEdepSpectrum(run2,detName,DetID,PulseType,EnMin,EnMax,npulses2,nprotons2);
  if(h1==0 || h2==0){return -1;}
  double err;
  double NCounts1=IntegrateHisto(h1,ampMin,ampMax,err,true,false);
  double NCounts2=IntegrateHisto(h2,ampMin,ampMax,err,true,false);
  cout<<" Number of counts in each histo --> "<<NCounts1<<"  "<<NCounts2<<" , nbins = "<<h1->GetNbinsX()/rebin<<endl;
  if(NCounts1<MinNCounts || NCounts2<MinNCounts){
    return -2;
  }
  h1->Scale(8.e12/nprotons1);
  h2->Scale(8.e12/nprotons2);
  h1->Rebin(rebin);
  h2->Rebin(rebin);
  TH1D* hComp=new TH1D("hComp","",npShift,ShiftMin,ShiftMax);
  double ShiftFactor=TakeBestShiftFactor(h2,h1,ampMin,ampMax,hComp,NORM,HalfNPToFitParabolic);
  cout<<" Shift factor is --> "<<ShiftFactor<<endl;
  
  TCanvas* c1=0;
  TCanvas* c2=0;
  if(PLOT){
    h1->GetYaxis()->SetTitle("counts/(8e12 protons)");
    h1->GetXaxis()->SetNdivisions(507);
    TH1D* h3=ShiftHisto(h1,ShiftFactor);
    if(NORM){h3->Scale(h2->Integral(h2->FindBin(ampMin),h2->FindBin(ampMax))/h3->Integral(h3->FindBin(ampMin),h3->FindBin(ampMax)));}
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


std::vector<GainMonInfo> ReadGainMonInput(string fname){

  ifstream in(fname);
  if(!in.good()){cout<<" ######## ERROR in "<<__FILE__<<" , line "<<__LINE__<<" ########"<<endl; exit(1);}
  string line,word;
  std::vector<GainMonInfo> vMonInfo;
  while(getline(in,line)){
    if(line[0]!='#'){
      istringstream iss(line);
      GainMonInfo theMonInfo;
      iss>>theMonInfo.detName>>theMonInfo.DetID>>theMonInfo.PulseType>>theMonInfo.rebin>>theMonInfo.EnMin>>theMonInfo.EnMax;
      if(iss.good()){
	iss>>theMonInfo.ampMin>>theMonInfo.ampMax;
	int cont=0;
	while(iss>>word){
	  if(cont==0){theMonInfo.ShiftMin=stod(word);}
	  else if(cont==1){theMonInfo.ShiftMax=stod(word);}
	  else if(cont==2){theMonInfo.npShift=stoi(word);}
	  else if(cont==3){theMonInfo.norm=stoi(word);}
	  else if(cont==4){theMonInfo.MinNCounts=stod(word);}
	  else if(cont==5){theMonInfo.HalfNPToFitParabolic=stoi(word);}
	  else if(cont==6){theMonInfo.ExtraShift=stod(word);}
	  cont++;
	}
	vMonInfo.push_back(theMonInfo);
      }
    }
  }
  in.close();
  
  return vMonInfo;

}


TGraph* GetGainMonitoring(const char* DetName,int DetID,const char* outdir,int &FirstRefRun,const char* RunDescriptor,bool Recalculate){

  char command[1000];
    sprintf(command,"ls %s/GShift_*.out > borrar.txt",outdir);
  if(RunDescriptor!=0){
    sprintf(command,"ls %s/GShift_%s_*.out > borrar.txt",outdir,RunDescriptor);
  }
  int check=system(command);

  double RunNumber[100000];
  double RefRun[100000];
  double GainShift[100000];
  
  char fname[200],dum[100],detname[100];
  int detID;
  double Shift;
  int np=0;
  ifstream in1("borrar.txt");
  while(in1>>fname){
    ifstream in2(fname);
    in2>>RefRun[np]>>RunNumber[np];
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
  
  ReCalculateGainShifts(np,RunNumber,RefRun,GainShift,Recalculate);
  FirstRefRun=RefRun[0];
  
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


//If we use different reference runs:
//If ReCalculate==true then all the GainShift are transformed to be respect to the same RefRun
int ReCalculateGainShifts(int nRuns,double* RunNumber,double* RefRun,double* GainShift,bool ReCalculate){

  struct GainShiftByRun{
    double RunNumber,RefRun,GainShift;
    bool operator() (GainShiftByRun i,GainShiftByRun j) { return (i.RunNumber<j.RunNumber);}
  }theGSbyRun;

  //We first order by RunNumber, just in case:
  vector<GainShiftByRun> vGSbyRun;
  for(int i=0;i<nRuns;i++){
    theGSbyRun.RunNumber=RunNumber[i];
    theGSbyRun.RefRun=RefRun[i];
    theGSbyRun.GainShift=GainShift[i];
    vGSbyRun.push_back(theGSbyRun);
  }
  std::sort(vGSbyRun.begin(), vGSbyRun.end(), theGSbyRun);

  //Now we get the number of reference runs:
  vector<int> RefRunList;
  for(auto it = vGSbyRun.begin(); it != vGSbyRun.end(); ++it){
    bool NewRefRun=true;
    for(auto it2=RefRunList.begin();it2!=RefRunList.end();++it2){
      if(*it2==it->RefRun){NewRefRun=false; break;}
    }
    /*
    for(unsigned int i=0;i<RefRunList.size();i++){
      if(RefRunList.at(i)==it->RefRun){NewRefRun=false; break;}
    }
    */
    if(NewRefRun){
      RefRunList.push_back(it->RefRun);
    }
  }
  std::sort(RefRunList.begin(),RefRunList.end());//Sorting the vector
  cout<<" We have "<<RefRunList.size()<<" reference runs: ";
  for(auto it2=RefRunList.begin();it2!=RefRunList.end();++it2){cout<<"  "<<*it2;}
  //for(int i=0;i<RefRunList.size();i++){cout<<"  "<<RefRunList.at(i);}
  cout<<endl;

  if(ReCalculate){
    cout<<" Refering all shifts to the same reference run ..."<<endl;
    //-----------------------------------------------------------------------------
    //Now we refer all gain shifts to the same RefRun
    for(unsigned int i=1;i<RefRunList.size();i++){
      double GShift=-1;
      int lastRefRun=RefRunList.at(i-1);
      int thisRefRun=RefRunList.at(i);
      for(auto it = vGSbyRun.begin(); it != vGSbyRun.end(); ++it){
	if(it->RunNumber==thisRefRun){
	  GShift=it->GainShift;
	  break;
	}
      }
      if(GShift<0){cout<<" ######## ERROR in "<<__FILE__<<" , line "<<__LINE__<<" ########"<<endl; exit(1);}
      for(auto it = vGSbyRun.begin(); it != vGSbyRun.end(); ++it){
	if(it->RefRun==thisRefRun){
	  it->RefRun=lastRefRun;
	  it->GainShift=it->GainShift*GShift;
	}
      }
    }
    //-----------------------------------------------------------------------------
  }
  
  //We "write" the results:
  for(int i=0;i<nRuns;i++){
    RunNumber[i]=vGSbyRun[i].RunNumber;
    RefRun[i]=vGSbyRun[i].RefRun;
    GainShift[i]=vGSbyRun[i].GainShift;
  }

  
  return 0;
}

