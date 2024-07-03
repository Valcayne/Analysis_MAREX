#ifndef PLOTMONITORING01_HH
#define PLOTMONITORING01_HH 1

#define INPUTFILES_DIRNAME "/afs/cern.ch/user/v/valcayne/LinkEOS/nTOFDataProcessing/2024_Er_Cu_U/2DHisto/v01"
#define OUTPUTFILES_DIRNAME "pdfs01/"
#define RUNLISTFNAME "../RunLists/RunList2024_01.txt"


int GetIntegratedValues2D(string hname){


}


int GetIntegralValues(int ValueType,int DetN,int PulseType,int nRuns,int* RunList,double* IntegVals,double* IntegValsErr,char* ValueName,double Xmin,double Xmax,double Ymin,double Ymax){

  char hname[100]; int binMin=-1; int binMax=-1;
  double emin=0,emax=0; //Neutron energy 
  int NEnInt=GetEnIntEmaxEmin(RunType,EnInt,emin,emax);

  
  // 1-4 --> Pulsos
  if(ValueType==1){sprintf(hname,"hPulIntens_%d",PulseType); sprintf(ValueName,"NProtons");}
  if(ValueType==2){sprintf(hname,"hPulIntens_%d",PulseType); sprintf(ValueName,"NPulses");}
  if(ValueType==3){sprintf(hname,"hPulIntens_%d",PulseType); sprintf(ValueName,"PKUPAmp");}
  if(ValueType==4){sprintf(hname,"hPulIntens_%d",PulseType); sprintf(ValueName,"PKUPArea");}

  //5-XXX --> integrales de H2D:
  if(ValueType==5){sprintf(hname,"hEn_SILI_%d_PType_%.2d",DetN,PulseType); sprintf(ValueName,"SILI-%.2d  (%.2f<E_{n}/eV<%.2f)",DetN,EMin,EMax);}
  
  
  // 5-10 --> tiempo de vuelo
  if(ValueType==5){sprintf(hname,"hEn_SILI_%d_%.2d",PulseType,DetN); sprintf(ValueName,"SILI-%.2d  (%.2f<E_{n}/eV<%.2f)",DetN,EMin,EMax);}
  if(ValueType==6){sprintf(hname,"hEn_FICH_tot_%d_%.2d",PulseType,DetN); sprintf(ValueName,"FICH_tot-%.2d  (%.2f<E_{n}/eV<%.2f)",DetN,EMin,EMax);}
  if(ValueType==7){sprintf(hname,"hEn_FICH_coinc_%d_%.2d",PulseType,DetN); sprintf(ValueName,"FICH_coinc-%.2d  (%.2f<E_{n}/eV<%.2f)",DetN,EMin,EMax);}
  if(ValueType==8){sprintf(hname,"hEn_BAF2_tot_%d_mg%d",PulseType,DetN); sprintf(ValueName,"TAC_tot-%.2d  (%.2f<E_{n}/eV<%.2f)",DetN,EMin,EMax);}
  if(ValueType==9){sprintf(hname,"hEn_BAF2_coinc_%d_mg%d",PulseType,DetN); sprintf(ValueName,"TAC_coinc-%.2d  (%.2f<E_{n}/eV<%.2f)",DetN,EMin,EMax);}
  //if(ValueType==10){sprintf(hname,"hEn_BAF2_alphas_%d_mg%d",PulseType,DetN); sprintf(ValueName,"BAF2_alphas-%.2d",DetN);}

  //20-26 --> Edep
  if(ValueType==20){sprintf(hname,"hEdep_BAF2_gammas_%d_%d_%.2d",PulseType,EnInt,DetN); sprintf(ValueName,"BAF2_gammas-%.2d (%.2f<E_{n}/eV<%.2f)",DetN,emin,emax);}
  if(ValueType==21){sprintf(hname,"hEdep_BAF2_alphas_%d_%d_%.2d",PulseType,EnInt,DetN); sprintf(ValueName,"BAF2_alphas-%.2d (%.2f<E_{n}/eV<%.2f)",DetN,emin,emax);}
  if(ValueType==22){sprintf(hname,"hEdep_SILI_%d_%d_%.2d",PulseType,EnInt,DetN); sprintf(ValueName,"SILI-%.2d (%.2f<E_{n}/eV<%.2f)",DetN,emin,emax);}
  if(ValueType==23){sprintf(hname,"hEdep_FICH_tot_%d_%d_%.2d",PulseType,EnInt,DetN); sprintf(ValueName,"FICH_tot-%.2d (%.2f<E_{n}/eV<%.2f)",DetN,emin,emax);}
  if(ValueType==24){sprintf(hname,"hEdep_FICH_coinc_%d_%d_%.2d",PulseType,EnInt,DetN); sprintf(ValueName,"FICH_coinc-%.2d (%.2f<E_{n}/eV<%.2f)",DetN,emin,emax);}
  if(ValueType==25){sprintf(hname,"hESum_BAF2_tot_%d_%d_mg%d",PulseType,EnInt,DetN); sprintf(ValueName,"TAC_tot (m_{cr}>%d, %.2f<E_{n}/eV<%.2f)",DetN,emin,emax);}
  if(ValueType==26){sprintf(hname,"hESum_BAF2_coinc_%d_%d_mg%d",PulseType,EnInt,DetN); sprintf(ValueName,"TAC_coinc (m_{cr}>%d, %.2f<E_{n}/eV<%.2f)",DetN,emin,emax);}

  cout<<hname<<endl;
  
  int RunList[10000];
  int nRuns=TakeRunList(RUNLISTFNAME,RunType,RunList);
  int nvals=0;
  
  for(int i=0;i<nRuns;i++){
    char fname[300];
    sprintf(fname,"%s/%s_%d.root",FILES_OUT_DIRNAME,FnameBase.c_str(),RunList[i]);
    bool fileExists=true;
    TFile* f1=new TFile(fname,"READ");
    if(f1->IsZombie()){
      fileExists=false;
    }
    delete f1;
    if(fileExists){
      TH1D* h1=GetMonHistogram(hname,RunList[i]);
      IntegVals[nvals]=0;
      if(h1){
	RunVals[nvals]=RunList[i];
	if(ValueType>=1 && ValueType<=4){
	  IntegVals[nvals]=h1->GetBinContent(ValueType);
	  IntegValsErr[nvals]=0;
	}
	else if(ValueType>=5 && ValueType<=9){
	  int binMin=h1->FindBin(EMin);
	  int binMax=h1->FindBin(EMax);
	  IntegVals[nvals]=h1->IntegralAndError(binMin,binMax,IntegValsErr[nvals]);

	}
	else if(ValueType>=20 && ValueType<=26){
	  int binMin=h1->FindBin(EMin);
	  int binMax=h1->FindBin(EMax);
	  IntegVals[nvals]=h1->IntegralAndError(binMin,binMax,IntegValsErr[nvals]);
	}
	delete h1;
      }
      nvals++;
    }
  }
  
  return nvals;
}



#endif

