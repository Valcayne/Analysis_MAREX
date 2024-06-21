
#include "GainMonitoring01.hh"
#include <iomanip>

/*
Programa para monitorizar ganancias:
Compara las ganancias de los detectores en Run1 con las de Run2
El resultado son los shift-factors por los que hay que multiplicar Run2 para tener la ganancia de Run1 
*/

struct GainMonInfo{
  string detName="none";
  int DetID=1,PulseType=1,rebin=1,npShift=200;
  double EnMin=1.,EnMax=1.e4,ampMin=0,ampMax=1.e5,ShiftMin=0.8,ShiftMax=1.2,ExtraShift=1.0;
  bool norm=false;
};

int main(int argc, char** argv) {

  if(argc!=4){
    cout<<" ########## Input has to be like: GainMonitoring01 [RunDescriptor] [RefRun] [inputfile] ########## "<<endl;
    return 1;
  }

  //========================================================================
  char RunListFname[500]="../RunLists/RunList2024_01.txt";
  char outdir[500]="outputs/GainMon01";
  int const MAXNRUNS=100000;
  //========================================================================

  //------------------------------------------------------------------------
  //Read input file:
  ifstream in(argv[3]);
  if(!in.good()){cout<<" ######## ERROR in "<<__FILE__<<" , line "<<__LINE__<<" ########"<<endl; exit(1);}
  string line,word;
  vector<GainMonInfo> vMonInfo;
  while(getline(in,line)){
    if(!line[0]!='#'){
      istringstream iss(line);
      GainMonInfo theMonInfo;
      iss>>theMonInfo.detName>>theMonInfo.DetID>>theMonInfo.PulseType>>theMonInfo.rebin>>theMonInfo.EnMin>>theMonInfo.EnMax;
      if(iss.good()){
	iss>>theMonInfo.ampMin>>theMonInfo.ampMax;
	int cont=0;
	while(iss>>word){
	  if(cont==0){theMonInfo.ShiftMin=stod(word);}
	  else if(cont==1){theMonInfo.ShiftMax=stod(word);}
	  else if(cont==2){theMonInfo.ShiftMax=stod(word);}
	  else if(cont==3){theMonInfo.npShift=stoi(word);}
	  else if(cont==4){theMonInfo.norm=stoi(word);}
	  else if(cont==5){theMonInfo.ExtraShift=stod(word);}
	  cont++;
	}
	vMonInfo.push_back(theMonInfo);
      }
    }
  }
  in.close();
  cout<<" Detectors to be processed: "<<endl;
  cout<<"------------------------------------------------------------------------------------------------------------"<<endl;
  cout<<"detName  DetID  PulseType  rebin  EnMin  EnMax  ampMin  ampMax  ShiftMin  ShiftMax  npShift  norm ExtraShift"<<endl;
  for(auto mInf = vMonInfo.begin(); mInf != vMonInfo.end(); ++mInf){
    cout<<mInf->detName<<"  "<<mInf->DetID<<"  "<<mInf->PulseType<<"  "<<mInf->rebin<<"  "<<mInf->EnMin<<"  "<<mInf->EnMax<<"  "<<mInf->ampMin<<"  "<<mInf->ampMax<<"  "<<mInf->ShiftMin<<"  "<<mInf->ShiftMax<<"  "<<mInf->npShift<<"  "<<mInf->norm<<"  "<<mInf->ExtraShift<<endl;
  }
  cout<<"------------------------------------------------------------------------------------------------------------"<<endl;
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
    char outfname[200];
    sprintf(outfname,"%s/GShift_%s_%d.out",outdir,argv[1],RunList[i]);
    ofstream out(outfname);
    for(int j=0;j<vMonInfo.size();j++){
      double GainChange=GetGainChange(RefRun,RunList[i],vMonInfo[j].detName,vMonInfo[j].DetID,vMonInfo[j].PulseType,vMonInfo[j].rebin,vMonInfo[j].EnMin,vMonInfo[j].EnMax,vMonInfo[j].ampMin,vMonInfo[j].ampMax,false,vMonInfo[j].ShiftMin,vMonInfo[j].ShiftMax,vMonInfo[j].npShift,vMonInfo[j].norm);
      out<<vMonInfo[j].detName<<"  "<<std::setw(5)<<vMonInfo[j].DetID<<"  "<<std::setw(20)<<GainChange*vMonInfo[j].ExtraShift<<endl;
    }
    out.close();
  }
  //------------------------------------------------------------------------

  return 0;
}

