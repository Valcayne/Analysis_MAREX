#ifndef ANALYSISFUNCTIONS01_HH
#define ANALYSISFUNCTIONS01_HH 1

#ifndef FILES_OUT_DIRNAME
#define FILES_OUT_DIRNAME "../outputs01"
#endif

#ifndef RUNLISTFNAME
#define RUNLISTFNAME "../RunLists/RunList_Pu239_Adrian_02.txt"
#endif

#include "GeneralFunctions.hh"


//====================================================================================
int ChangeTOFD(TH1D* histo,double oldTOFD,double newTOFD);
TH1D* GetMonHistogram(const char* hname,const char* fname);
TH1D* GetMonHistogram(const char* hname,int RunNumber,string FnameBase="Monitoring01");
TH1D* GetMonHistogram(const char* hname,int NRuns,int* RunList,string FnameBase="Monitoring01");
TH1D* GetMonHistogram(const char* hname,string RunType,string FnameBase="Monitoring01");
TGraph* GetResiduals(TH1D* h1,TH1D* h2,double Xmin,double Xmax); //h1-> exp, h2->MC
void ReCalculateUncertainties(TH1D* h1);
void UpdateMinimumAndMaximum(TH1D* h1,double xmin,double xmax,double& MinVal,double& MaxVal);
void UpdateMinimumAndMaximum(TGraph* gr1,double xmin,double xmax,double& MinVal,double& MaxVal);
double IntegrateHisto(TH1D* h1,double xmin,double xmax,double &err,bool DONOTMULTBYX,int CORRELATEDERRORS);
TH1D* ShiftHisto(TH1D* h1,double shift); //Shift h1 by a factor "shift"
TH1D* GetIntegratedHisto(TH1D* h1,int nbins,double Xmin,double Xmax,bool IsLog,bool DONOTMULTBYX=false,int CORRELATEDERRORS=false,bool DivideByBinWidth=false);
TH1D* GetIntegratedHisto(TH1D* h1,TH1D* h2_base,bool DONOTMULTBYX,int CORRELATEDERRORS,bool DivideByBinWidth);
TH1D* GetIntegratedHisto(TGraph* gr1,int nbins,double Xmin,double Xmax,bool IsLog);
int GetEnIntEmaxEmin01(const char* fname,int EnInt,double &Emin,double& Emax);
int GetEnIntEmaxEmin(int RunNumber,int EnInt,double &Emin,double& Emax,string FnameBase="Monitoring01");
int GetEnIntEmaxEmin(string RunType,int EnInt,double &Emin,double& Emax,string FnameBase="Monitoring01");
int GetEnTOFCuts(const char* fnamebase,double& Emin,double& Emax);
TH1D* MakeRatio(TH1D* h1,TH1D* h2,int Opt);
double TakeBestShiftFactor(TH1D* h1,TH1D* h2,double Xmin,double Xmax,double ShiftMin,double ShiftMax,int NSteps,bool NORM=true,int HalfNPToFitParabolic=25);
double TakeBestShiftFactor(TH1D* h1,TH1D* h2,double Xmin,double Xmax,TH1D* hComp,bool NORM=true,int HalfNPToFitParabolic=25);
double GetChi2(TH1D* h1,TH1D* h2,double Xmin,double Xmax,bool NORM=true);
//====================================================================================

double GetChi2(TH1D* h1,TH1D* h2,double Xmin,double Xmax,bool NORM){

  int BinMin=h1->FindBin(Xmin);
  int BinMax=h1->FindBin(Xmax);
  double area1=h1->Integral(BinMin,BinMax);
  double area2=h2->Integral(BinMin,BinMax);

  double val1,val2,err1,err2,chi2=0;
  int np=0;
  double NormFactor=1;
  if(NORM){NormFactor=area1/area2;}
  for(int i=BinMin;i<=BinMax;i++){
    val1=h1->GetBinContent(i);
    err1=h1->GetBinError(i);
    val2=h2->GetBinContent(i)*NormFactor;
    err2=h2->GetBinError(i)*NormFactor;
    if(val1!=0 && val2!=0){
      chi2+=(val1-val2)*(val1-val2)/(err1*err1+err2*err2);
      np++;
    }
  }
  if(np==0){return 1.e20;}
  
  return chi2/np;
}

//Returns the best shift-factor so that h2 transformed with shift-factor mathches h1 between Xmin and Xmax
double TakeBestShiftFactor(TH1D* h1,TH1D* h2,double Xmin,double Xmax,double ShiftMin,double ShiftMax,int NSteps,bool NORM,int HalfNPToFitParabolic){

  TH1D* hComp=new TH1D("hComp","hComp",NSteps,ShiftMin,ShiftMax);
  double ShifFactor=TakeBestShiftFactor(h1,h2,Xmin,Xmax,hComp,NORM,HalfNPToFitParabolic);
  delete hComp;
  return ShifFactor;
  
}

//Returns the best shift-factor so that h2 transformed with shift-factor mathches h1 between Xmin and Xmax
double TakeBestShiftFactor(TH1D* h1,TH1D* h2,double Xmin,double Xmax,TH1D* hComp,bool NORM,int HalfNPToFitParabolic){


  int NSteps=hComp->GetNbinsX();
  for(int i=1;i<=NSteps;i++){
    TH1D* h3=ShiftHisto(h2,hComp->GetBinCenter(i));
    hComp->SetBinContent(i,GetChi2(h1,h3,Xmin,Xmax,NORM));
    delete h3;
  }

  //The result is the minimum of hComp:
  int minbin=hComp->GetMinimumBin();
  if(minbin<2 || minbin>NSteps-1){
    cout<<" ########## WARNING calculating best ShiftFactor: minbin is "<<minbin<<" ##########"<<endl;
    delete hComp;
    return -1;
  }
  
  //--------------------------------------------------------
  //Take minimum value:
  //option 1 (just take the min. val):
  //double result=hComp->GetBinCenter(minbin);
  //option 2 (parabolic fit):
  //int HalfNPToFitParabolic=25;
  int fitstart=minbin-HalfNPToFitParabolic;if(fitstart<1){fitstart=1;}
  int fitend=minbin+HalfNPToFitParabolic;if(fitend>NSteps){fitend=NSteps;}
  int fitnp=fitend-fitstart+1;
  double* data_x=new double[fitnp];
  double* data_y=new double[fitnp];
  for(int i=0;i<fitnp;i++){
    data_x[i]=hComp->GetBinCenter(fitstart+i);
    data_y[i]=hComp->GetBinContent(fitstart+i);
  }
  //Translation to Sum(xi)=0:
  double Sum=0;
  for(int i=0;i<fitnp;i++){
    Sum+=data_x[i];
  }
  double trval=-Sum/fitnp;
  for(int i=0;i<fitnp;i++){
    data_x[i]+=trval;
  }

  double p1=0,p2=0,p3=0,p4=0,p5=0;
  for(int i=0;i<fitnp;i++){
    p1+=data_y[i];
    p2+=data_x[i]*data_x[i];
    p3+=data_x[i]*data_x[i]*data_y[i];
    p4+=data_x[i]*data_y[i];
    p5+=data_x[i]*data_x[i]*data_x[i]*data_x[i];
  }
  double a,b,c;//ax2+bx+c
  if((fitnp*p5-p2*p2)==0){
    cout<<" **** Error in parabolic fit ****"<<endl;
    exit(1);
  }
  a=(fitnp*p3-p2*p1)/(fitnp*p5-p2*p2);
  b=p4/p2;
  c=(p1-a*p2)/fitnp;
  delete [] data_x;
  delete [] data_y;

  double result=-b/(2.*a)-trval;
  cout<<"min(chi2)= "<<a*(result+trval)*(result+trval)+b*(result+trval)+c<<" at MF= "<<result<<" --> shift = "<<1./result<<endl;
  //--------------------------------------------------------

  return result;
  
}



//returns h1/h2
//Opt=1, use interpolations in h2, Opt=2: use integrals
//Errors of h2 are omitted
TH1D* MakeRatio(TH1D* h1,TH1D* h2,int Opt){

  TH1D* h3=new TH1D(*h1);
  h3->Reset();
  int nbins=h1->GetNbinsX();
  double val1,err1,val2,dum;
  for(int i=1;i<=nbins;i++){
    val1=h1->GetBinContent(i);
    err1=h1->GetBinError(i);
    if(Opt==1){
      val2=h2->Interpolate(h1->GetBinCenter(i));
    }
    else{
      val2=IntegrateHisto(h2,h1->GetBinLowEdge(i),h1->GetBinLowEdge(i+1),dum,false,0)/(h1->GetBinLowEdge(i+1)-h1->GetBinLowEdge(i));
    }
    if(val2>0){
      h3->SetBinContent(i,val1/val2);
      h3->SetBinError(i,err1/val2);
    }
  }

  return h3;
}


int GetEnTOFCuts(const char* fnamebase,double& Emin,double& Emax){

  char fname[1000];
  sprintf(fname,"%s.txt",fnamebase);
  Emin=0; Emax=0;
  string word;
  ifstream in(fname);
  while(in>>word){
    if(word=="TACEventTOFCuts"){
      in>>Emin>>Emax;
      break;
    }
  }
  in.close();
  return 0;
}

int GetEnIntEmaxEmin01(const char* fname,int EnInt,double &Emin,double& Emax){

  ifstream in(fname);
  if(!in.good()){
    return 0;
  }
  char word[100];
  int NEneIntervals=0;

  in>>word>>word>>word>>NEneIntervals>>word>>word;
  for(int i=0;i<NEneIntervals;i++){
    if(i==EnInt){
      in>>Emin>>Emax;
    }
    else{
      in>>word>>word;
    }
  }

  in.close();

  return NEneIntervals;
}

int GetEnIntEmaxEmin(int RunNumber,int EnInt,double &Emin,double& Emax,string FnameBase){

  Emin=0; Emax=0;
  char fname[300];
  sprintf(fname,"%s/%s_%d.txt",FILES_OUT_DIRNAME,FnameBase.c_str(),RunNumber);

  int NEneIntervals=GetEnIntEmaxEmin01(fname,EnInt,Emin,Emax);
  
  return NEneIntervals;
}


int GetEnIntEmaxEmin(string RunType,int EnInt,double &Emin,double& Emax,string FnameBase){

  int RunList[10000];
  int nRuns=TakeRunList(RUNLISTFNAME,RunType,RunList);
  int NEneIntervals=GetEnIntEmaxEmin(RunList[0],EnInt,Emin,Emax,FnameBase);

  return NEneIntervals;
}


TH1D* GetIntegratedHisto(TH1D* h1,int nbins,double Xmin,double Xmax,bool IsLog,bool DONOTMULTBYX,int CORRELATEDERRORS,bool DivideByBinWidth){

  double*  xlims=new double[nbins+1];
  for(int i=0;i<=nbins;i++){
    if(IsLog){xlims[i]=Xmin*pow(Xmax/Xmin,i/(double)(nbins));}
    else{xlims[i]=Xmin+i/(double)(nbins)*(Xmax-Xmin);}
  }

  TH1D* h2=new TH1D("","",nbins,xlims);
  delete [] xlims;

  double x1,x2,val,err;
  for(int i=1;i<=nbins;i++){
    x1=h2->GetBinLowEdge(i);
    x2=h2->GetBinLowEdge(i+1);
    val=IntegrateHisto(h1,x1,x2,err,DONOTMULTBYX,CORRELATEDERRORS);
    if(DivideByBinWidth){
      val/=(x2-x1);
      err/=(x2-x1);
    }
    h2->SetBinContent(i,val);
    h2->SetBinError(i,err);
  }

  h2->SetLineWidth(3);
  h2->SetLineColor(h1->GetLineColor());

  return h2;
}


TH1D* GetIntegratedHisto(TH1D* h1,TH1D* h2_base,bool DONOTMULTBYX,int CORRELATEDERRORS,bool DivideByBinWidth){

  TH1D* h2=new TH1D(*h2_base);
  h2->Reset();
  int nbins=h2->GetNbinsX();
  
  double x1,x2,val,err;
  for(int i=1;i<=nbins;i++){
    x1=h2->GetBinLowEdge(i);
    x2=h2->GetBinLowEdge(i+1);
    val=IntegrateHisto(h1,x1,x2,err,DONOTMULTBYX,CORRELATEDERRORS);
    if(DivideByBinWidth){
      val/=(x2-x1);
      err/=(x2-x1);
    }
    h2->SetBinContent(i,val);
    h2->SetBinError(i,err);
  }

  h2->SetLineWidth(3);
  h2->SetLineColor(h1->GetLineColor());

  return h2;
}



TH1D* GetIntegratedHisto(TGraph* gr1,int nbins,double Xmin,double Xmax,bool IsLog){

  double*  xlims=new double[nbins+1];
  for(int i=0;i<=nbins;i++){
    if(IsLog){xlims[i]=Xmin*pow(Xmax/Xmin,i/(double)(nbins));}
    else{xlims[i]=Xmin+i/(double)(nbins)*(Xmax-Xmin);}
  }

  TH1D* h2=new TH1D("","",nbins,xlims);
  delete [] xlims;

  double x1,x2,val,err;
  for(int i=1;i<=nbins;i++){
    x1=h2->GetBinLowEdge(i);
    x2=h2->GetBinLowEdge(i+1);
    val=IntegrateBest(gr1,x1,x2)/(x2-x1);
    err=0;
    h2->SetBinContent(i,val);
    h2->SetBinError(i,err);
  }

  h2->SetLineWidth(3);
  h2->SetLineColor(gr1->GetLineColor());

  return h2;
}


//Shift h1 by a factor "shift"
TH1D* ShiftHisto(TH1D* h1,double shift){

  TH1D* h2=new TH1D(*h1);
  h2->Reset();

  int nbins=h1->GetNbinsX();
  double LowLim=h1->GetBinLowEdge(1);
  double HighLim=h1->GetBinLowEdge(nbins+1);
  double emin,emax,val,err,IntegEmin,IntegEmax;
  for(int i=1;i<=nbins;i++){
    emin=h2->GetBinLowEdge(i);
    emax=h2->GetBinLowEdge(i+1);
    IntegEmin=emin/shift; if(IntegEmin<LowLim){IntegEmin=LowLim;}
    IntegEmax=emax/shift; if(IntegEmax>HighLim){IntegEmax=HighLim;}
    val=IntegrateHisto(h1,IntegEmin,IntegEmax,err,true,0);
    h2->SetBinContent(i,val);
    h2->SetBinError(i,err);
  }

  return h2;
}


//CORRELATEDERRORS=0,-1,1 --> no correlated,correlated +1 when positive and -1 when negative, correlated +1 
double IntegrateHisto(TH1D* h1,double xmin,double xmax,double &err,bool DONOTMULTBYX,int CORRELATEDERRORS){

  int nb=h1->GetNbinsX();
  double Sum=0;
  double SumErr=0;
  double SumErr2=0;
  double minval,maxval,val,valerr;
  for(int i=1;i<=nb;i++){
    minval=h1->GetBinLowEdge(i);
    maxval=h1->GetBinLowEdge(i+1);
    val=h1->GetBinContent(i);
    valerr=h1->GetBinError(i);
    double fraction=0;
    if(maxval<=xmin || minval>xmax){
      fraction=-1;
    }
    else if(minval<=xmin && maxval>xmin && maxval<=xmax){
      fraction=(maxval-xmin)/(maxval-minval);
    }
    else if(minval<=xmin && maxval>xmin && maxval>xmax){
      fraction=(xmax-xmin)/(maxval-minval);
    }
    else if(minval>xmin && minval<=xmax && maxval<=xmax){
      fraction=1;
    }
    else if(minval>xmin && minval<=xmax && maxval>xmax){
      fraction=(xmax-minval)/(maxval-minval);
    }
    else{
      cout<<" **** Error in IntegrateHisto ****"<<endl;
      exit(1);
    }
    if(fraction>0){
      if(DONOTMULTBYX){
	Sum+=val*fraction;
	//
	if(CORRELATEDERRORS==-1){if(val>0){SumErr+=valerr*fraction;}else{SumErr-=valerr*fraction;}}
	else if(CORRELATEDERRORS==+1){SumErr+=valerr*fraction;}
	//
	SumErr2+=valerr*valerr*fraction;
      }
      else{
	Sum+=val*(maxval-minval)*fraction;
	//
	if(CORRELATEDERRORS==-1){if(val>0){SumErr+=valerr*(maxval-minval)*fraction;}else{SumErr-=valerr*(maxval-minval)*fraction;}}
	else if(CORRELATEDERRORS==+1){SumErr+=valerr*(maxval-minval)*fraction;}
	//	
	SumErr2+=valerr*(maxval-minval)*valerr*(maxval-minval)*fraction;
      }
    }
  }

  if(CORRELATEDERRORS==-1 || CORRELATEDERRORS==+1){
    err=SumErr;
  }
  else{
    err=sqrt(SumErr2);
  }

  return Sum;

}



void UpdateMinimumAndMaximum(TH1D* h1,double xmin,double xmax,double& MinVal,double& MaxVal){

  int binMin=h1->GetXaxis()->FindBin(xmin);
  int binMax=h1->GetXaxis()->FindBin(xmax);
  double val;
  for(int i=binMin;i<=binMax;i++){
    val=h1->GetBinContent(i);
    if(val<MinVal){MinVal=val;}
    if(val>MaxVal){MaxVal=val;}
  }

}

void UpdateMinimumAndMaximum(TGraph* gr1,double xmin,double xmax,double& MinVal,double& MaxVal){

  int np=gr1->GetN();
  double xval,yval;
  for(int i=0;i<np;i++){
    gr1->GetPoint(i,xval,yval);
    if(xval>xmin && xval<xmax && yval<MinVal){MinVal=yval;}
    if(xval>xmin && xval<xmax && yval>MaxVal){MaxVal=yval;}
 }
  
}


//Calcula de nuevo los errores como la raíz cuadrada del contenido de los bines:
void ReCalculateUncertainties(TH1D* h1){

  int nbins=h1->GetNbinsX();
  for(int i=0;i<=nbins+1;i++){
    h1->SetBinError(i,sqrt(h1->GetBinContent(i)));
  }

}

//h1-> exp, h2->MC
TGraph* GetResiduals(TH1D* h1,TH1D* h2,double Xmin,double Xmax){

  int nbins=h1->GetNbinsX();
  double* xvals=new double[nbins];
  double* yvals=new double[nbins];
  double xval,residual;
  int np=0;
  for(int i=1;i<=nbins;i++){
    xval=h1->GetBinCenter(i);
    if((xval>Xmin || Xmin<0) && (xval<Xmax || Xmax<0) && h1->GetBinError(i)>0){
      residual=(h1->GetBinContent(i)-h2->Interpolate(xval))/h1->GetBinError(i);
      xvals[np]=xval;
      yvals[np]=residual;
      np++;
    }
  }
  
  TGraph* gr1=new TGraph(np,xvals,yvals);
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(2);
  
  delete [] xvals;
  delete [] yvals;
  
  return gr1;
}



TH1D* GetMonHistogram(const char* hname,string RunType,string FnameBase){
  int RunList[10000];
  int nRuns=TakeRunList(RUNLISTFNAME,RunType,RunList);
  TH1D* h1=GetMonHistogram(hname,nRuns,RunList,FnameBase);
  return h1;
}

TH1D* GetMonHistogram(const char* hname,int NRuns,int* RunList,string FnameBase){

  TH1D* h1=GetMonHistogram(hname,RunList[0],FnameBase);
  for(int i=1;i<NRuns;i++){
    TH1D* h_tmp=GetMonHistogram(hname,RunList[i],FnameBase);
    if(h_tmp && h1){
      h1->Add(h_tmp);
      delete h_tmp;
    }
  }
  return h1;
  
}


TH1D* GetMonHistogram(const char* hname,int RunNumber,string FnameBase){

  char fname[300];
  sprintf(fname,"%s/%s_%d.root",FILES_OUT_DIRNAME,FnameBase.c_str(),RunNumber);

  return GetMonHistogram(hname,fname);
}


TH1D* GetMonHistogram(const char* hname,const char* fname){

  TFile* f1=new TFile(fname,"READ");
  if(f1->IsZombie()){
    cout<<" ##### ERROR opening "<<fname<<" #####"<<endl; 
    return 0;
    exit(1);
  }
  TH1D* h1=(TH1D*)f1->Get(hname);
  if(!h1){
    cout<<" ############# WARNING: histogram "<<hname<<" does not exist in "<<fname<<" #############"<<endl;
    return 0;
  }
  h1->Sumw2();
  h1->SetDirectory(0);
  f1->Close();
  delete f1;

  h1->SetLineWidth(3);
  return h1;

}


int ChangeTOFD(TH1D* histo,double oldTOFD,double newTOFD){

  int NBins=histo->GetNbinsX();
  double* Xvals=new double[NBins+1];
  double ene,time;
  for(int i=0;i<=NBins;i++){
    ene=histo->GetBinLowEdge(i+1);
    time=EtoTOF(ene,oldTOFD);
    ene=TOFtoE(time,newTOFD);
    Xvals[i]=ene;
  }
  histo->GetXaxis()->Set(NBins,Xvals);
  delete [] Xvals;
  return 0;
}






#endif

