
#include "GainMonitoring01.hh"

/*
Programa para monitorizar ganancias:
Compara las ganancias de los detectores en Run1 con las de Run2
El resultado son los shift-factors por los que hay que multiplicar Run2 para tener la ganancia de Run1 
*/


int main(int argc, char** argv) {

  if(argc!=4 && argc!=5 && argc!=6){
    cout<<" ########## Input has to be like: GainMonitoring01 [RunDescriptor] [RefRun] [inputfile] ([RunMin=0]) ([RunMax=1.e20]) ########## "<<endl;
    return 1;
  }

  //========================================================================
  char RunListFname[500]="../RunLists/RunList2024_01.txt";
  char outdir[500]="outputs/GainMon01";
  double RunMin=0,RunMax=1.e20;
  int const MAXNRUNS=100000;
  //========================================================================

  if(argc>4){RunMin=std::atof(argv[4]);}
  if(argc>5){RunMax=std::atof(argv[5]);}

  //------------------------------------------------------------------------
  //Read input file:
  vector<GainMonInfo> vMonInfo=ReadGainMonInput(argv[3]);
  cout<<" Detectors to be processed: "<<endl;
  cout<<"---------------------------------------------------------------------------------------------------------------------------------------------"<<endl;
  cout<<"detName  DetID  PulseType  rebin  EnMin  EnMax  ampMin  ampMax  ShiftMin  ShiftMax  npShift  norm MinNCounts HalfNPToFitParabolic ExtraShift"<<endl;
  for(auto mInf = vMonInfo.begin(); mInf != vMonInfo.end(); ++mInf){
    cout<<mInf->detName<<"  "<<mInf->DetID<<"  "<<mInf->PulseType<<"  "<<mInf->rebin<<"  "<<mInf->EnMin<<"  "<<mInf->EnMax<<"  "<<mInf->ampMin<<"  "<<mInf->ampMax<<"  "<<mInf->ShiftMin<<"  "<<mInf->ShiftMax<<"  "<<mInf->npShift<<"  "<<mInf->norm<<"  "<<mInf->MinNCounts<<"  "<<mInf->HalfNPToFitParabolic<<"  "<<mInf->ExtraShift<<endl;
  }
  cout<<"---------------------------------------------------------------------------------------------------------------------------------------------"<<endl;
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  //Get the runs to be pocessed and the reference run:
  int RefRun=std::atoi(argv[2]);
  int RunList[10000];
  int nRuns=TakeRunList(RunListFname,argv[1],RunList,0);
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  //Here we go:
  for(int i=0;i<nRuns;i++){
    if(RunList[i]>=RunMin && RunList[i]<=RunMax){
      char outfname[200];
      sprintf(outfname,"%s/GShift_%s_%d.out",outdir,argv[1],RunList[i]);
      ofstream out(outfname);
      out<<RefRun<<"  "<<RunList[i]<<endl;
      for(int j=0;j<vMonInfo.size();j++){
	double GainChange=GetGainChange(RefRun,RunList[i],vMonInfo[j].detName,vMonInfo[j].DetID,vMonInfo[j].PulseType,vMonInfo[j].rebin,vMonInfo[j].EnMin,vMonInfo[j].EnMax,vMonInfo[j].ampMin,vMonInfo[j].ampMax,false,vMonInfo[j].ShiftMin,vMonInfo[j].ShiftMax,vMonInfo[j].npShift,vMonInfo[j].norm,vMonInfo[j].MinNCounts,vMonInfo[j].HalfNPToFitParabolic);
	out<<vMonInfo[j].detName<<"  "<<std::setw(5)<<vMonInfo[j].DetID<<"  "<<std::setw(20)<<GainChange*vMonInfo[j].ExtraShift<<endl;
      }
      out.close();
    }
  }
  //------------------------------------------------------------------------

  return 0;
}

