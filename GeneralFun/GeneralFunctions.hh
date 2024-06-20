#ifndef GENERALFUNCTIONS_HH
#define GENERALFUNCTIONS_HH 1

#include "GeneralDefinitions.hh"

//--------------------------------------------------------------------------------------
void AttachStruct(Signal* theS, TTree* tr);
int GetPulseType(Signal* theS);  // 2 -> Dedicated, 3-> Parasitic, 0->Other
//---------------------------------------------------------------------------------------
// Retrieves the number of pulses, together with the
// pulseType,PulseIntensity,PKUPAmp and PKUPArea for each pulse:
void TakePKUPInfo(const char* fname, PKUPInfo* thePKUPInfo);
//---------------------------------------------------------------------------------------
int CreateBunchesIndex(TTree* tr, Long64_t* bunchIndex, int* BunchNumber);
void ReCalibrateBunchesIndexVector(int& NBunches, Long64_t* bunchIndex,
                                   int* BunchNumber, int NBunches_ref,
                                   Long64_t* bunchIndex_ref,
                                   int* BunchNumber_ref);
//---------------------------------------------------------------------------------------
double TOFtoE(double tof, double tofd);
double EtoTOF(double nene, double tofd);
double DeltaEnergyToTime(double E1, double E2, double tofd);
void DivideByBinTime(TH1D* h1, double TOFD);
void DivideByBinWidth(TH1D* h1);
void AverageNoBeamHisto(TH1D* h1, double TOFD);
double IntegrateBest(TGraph* gr, double emin, double emax);
//---------------------------------------------------------------------------------------
int TakeRunList(const char* fname, string RunType, int* RunList,
                int vervose = 1);
int TakeRunList(const char* fname, string RunType, const char* roorfilesdirname,
                string* fnameRunList, int* RunList = 0);
int TakeRunList(const char* fname, string* fnameRunList);
void GetRunMinMax(const char* fname, string RunType, int& RunMin, int& RunMax);
//---------------------------------------------------------------------------------------
void ScaleGraphErrors(TGraphErrors* gr, double ScaleFactor);
void ScaleGraph(TGraph* gr, double ScaleFactor);
TGraphErrors* TakeEffFunction(const char* fname, double EsumMin, double EsumMax,
                              int multmin, int multmax, int Opt);
//---------------------------------------------------------------------------------------
void DivideByNeutronFluence(TH1D* h1, const char* fluxfname,
                            const char* hfluxname, double beamFactor = 1);
void SubtractYieldFromLibrary(TH1D* h1, TH1D* h_yield);
double TakeNumberOfNeutrons(TH1F* n_flux, double ene1, double ene2);
//---------------------------------------------------------------------------------------
void SetStyle01();
//---------------------------------------------------------------------------------------
int FindDetectorPosition(const std::vector<int>& vector, double numero);
int FindPositionString(const std::vector<string>& vector, string name);
int FindPositionString(const std::vector<int>& vector, int name);
string doubleToDecimalString(double num, int decimales);
void SaveRootEpsPngTxtFunction(TCanvas* c1, const char* name);
TH1D* Transform(TH1D* histo1, double m);
//---------------------------------------------------------------------------------------

//**********************************************************************************************************************
//**********************************************************************************************************************


void SetStyle01(){

  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.10);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleY(0.97);
  gStyle->SetTitleAlign(23);

  gStyle->SetLabelSize(0.05,"xy");
  gStyle->SetTitleSize(0.05,"xy");
  gStyle->SetTitleOffset(1.3,"xy");
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  gROOT->ForceStyle();
}




double CompareHistogramsNoConsideringErrors(TH1D* histo1, TH1D* histo2,
                                            double x1Min, double x1Max);

double CompareHistograms(TH1D* histo1, TH1D* histo2, double x1Min,
                         double x1Max);

double CompareHistograms(TH1D* histo1, TH1D* histo2, double x1Min,
                         double x1Max) {
  // cout << " x1Min" << x1Min << " x1Max " << x1Max << endl;
  if (histo1->GetNbinsX() != histo2->GetNbinsX()) {
    cout << " ######## Error in " << __FILE__ << " line " << __LINE__
         << " ########" << endl;
    exit(1);
  }
  if (std::fabs(histo1->GetBinLowEdge(1) - histo2->GetBinLowEdge(1)) > 1.e-10) {
    cout << " ######## Error in " << __FILE__ << " line " << __LINE__
         << " ########" << endl;
    exit(1);
  }
  if (std::fabs(histo1->GetBinLowEdge(histo1->GetNbinsX()) -
                histo2->GetBinLowEdge(histo2->GetNbinsX())) > 1.e-10) {
    cout << " ######## Error in " << __FILE__ << " line " << __LINE__
         << " ########" << endl;
    exit(1);
  }

  // cout<<" Start Compare Xmin "<< x1Min<<" xma "<<x1Max<<endl;

  int firstbin = 1, lastbin = histo1->GetNbinsX();
  // Range condition:
  // firstbin=100;
  // lastbin=400;

  if (x1Min > 0) {
    firstbin = histo1->FindBin(x1Min);
  }
  if (x1Max > 0) {
    lastbin = histo1->FindBin(x1Max);
  }
  if (x1Min > x1Max) {
    cout << " ######## Error in " << __FILE__ << " line " << __LINE__
         << " ########" << endl;
    exit(1);
  }
  // Set same areas:
  double area1 = histo1->Integral(firstbin, lastbin);
  double area2 = histo2->Integral(firstbin, lastbin);

  histo2->Scale(area1 / area2);
  // cout << "scale " << area1 / area2 << endl;
  // cout << "firstbin" << firstbin << "lastbin" << lastbin << endl;
  //  Comparison:
  double result = 0;
  double val1, val2, sig1, sig2, sig;
  for (int i = firstbin; i <= lastbin; i++) {
    val1 = histo1->GetBinContent(i);
    val2 = histo2->GetBinContent(i);
    sig1 = histo1->GetBinError(i);
    sig2 = histo2->GetBinError(i);
    sig = sqrt(sig1 * sig1 + sig2 * sig2);
    // cout << " vasl1" << histo1->GetBinContent(i) << "        vasl2 "
    //    << histo2->GetBinContent(i) << endl;
    // cout << " sig[" << i << "] = " << sig << " " << sig1 << " " << sig2 <<
    // endl;
    if (sig > 0) {
      result += ((val1 - val2) / sig) * ((val1 - val2) / sig);
    }
  }

  result /= (lastbin - firstbin + 1);
  // cout << "ChisSquare " << result << endl;
  return result;
}

double CompareHistogramsNoConsideringErrors(TH1D* histo1, TH1D* histo2,
                                            double x1Min, double x1Max) {
  if (histo1->GetNbinsX() != histo2->GetNbinsX()) {
    cout << " ######## Error in " << __FILE__ << " line " << __LINE__
         << " ########" << endl;
    exit(1);
  }
  if (std::fabs(histo1->GetBinLowEdge(1) - histo2->GetBinLowEdge(1)) > 1.e-10) {
    cout << " ######## Error in " << __FILE__ << " line " << __LINE__
         << " ########" << endl;
    exit(1);
  }
  if (std::fabs(histo1->GetBinLowEdge(histo1->GetNbinsX()) -
                histo2->GetBinLowEdge(histo2->GetNbinsX())) > 1.e-10) {
    cout << " ######## Error in " << __FILE__ << " line " << __LINE__
         << " ########" << endl;
    exit(1);
  }

  // cout<<" Start Compare Xmin "<< x1Min<<" xma "<<x1Max<<endl;

  int firstbin = 1, lastbin = histo1->GetNbinsX();
  // Range condition:
  // firstbin=100;
  // lastbin=400;

  if (x1Min > 0) {
    firstbin = histo1->FindBin(x1Min);
  }
  if (x1Max > 0) {
    lastbin = histo1->FindBin(x1Max);
  }

  // Set same areas:
  double area1 = histo1->Integral(firstbin, lastbin);
  double area2 = histo2->Integral(firstbin, lastbin);

  histo2->Scale(area1 / area2);

  // Comparison:
  double result = 0;
  double val1, val2, sig1, sig2, sig;
  for (int i = firstbin; i <= lastbin; i++) {
    val1 = histo1->GetBinContent(i);
    val2 = histo2->GetBinContent(i);
    sig1 = histo1->GetBinError(i);
    sig2 = histo2->GetBinError(i);
    sig = sqrt(sig1 * sig1 + sig2 * sig2);
    // cout<<" vasl1"<<histo1->GetBinContent(i)<<"
    // vasl2"<<histo2->GetBinContent(i)<<endl; cout<<"sig["<<i<<"]="<<sig<<"
    // "<<sig1<<"  "<<sig2<<endl;
    if (sig > 0) {
      //      result+=((val1-val2)/sig)*((val1-val2)/sig);
      result += ((val1 - val2)) * ((val1 - val2));
    }
  }

  result /= (lastbin - firstbin + 1);
  // cout<<"ChisSquare "<<result<<endl;
  return result;
}

TH1D* Transform(TH1D* histo1, double m) {
  TH1D* histo2 = new TH1D(*histo1);
  histo2->Reset("ICEM");
  int NBins = histo1->GetNbinsX();
  double* xvals = new double[NBins + 1];
  for (int i = 0; i <= NBins; i++) {
    xvals[i] = histo1->GetBinLowEdge(i + 1) * m;
  }
  double val, width, fraction;
  double lowlim, uplim;
  for (int i = 1; i <= NBins; i++) {
    val = histo1->GetBinContent(i);
    width = xvals[i] - xvals[i - 1];
    for (int j = 1; j <= NBins; j++) {
      fraction = 0;
      lowlim = histo1->GetBinLowEdge(j);
      uplim = histo1->GetBinLowEdge(j + 1);
      if (lowlim <= xvals[i - 1] && uplim > xvals[i - 1] && uplim <= xvals[i]) {
        fraction = (uplim - xvals[i - 1]) / width;
      } else if (lowlim <= xvals[i - 1] && uplim > xvals[i]) {
        fraction = 1;
      } else if (lowlim > xvals[i - 1] && lowlim <= xvals[i] &&
                 uplim <= xvals[i]) {
        fraction = (uplim - lowlim) / width;
      } else if (lowlim > xvals[i - 1] && lowlim <= xvals[i] &&
                 uplim > xvals[i]) {
        fraction = (xvals[i] - lowlim) / width;
      }
      histo2->AddBinContent(j, val * fraction);
      if (lowlim > xvals[i]) {
        break;
      }
    }
  }
  delete[] xvals;

  // Setting errors: --> está mal, pero de momento ...
  for (int i = 1; i < NBins; i++) {
    histo2->SetBinError(i, histo1->GetBinError(i));
  }
  //  delete histo1;
  return histo2;
}

void DivideByBinWidth(TH1D* h1) {
  int nbins = h1->GetNbinsX();
  double val, err, ene1, ene2, tof;
  for (int i = 1; i <= nbins; i++) {
    val = h1->GetBinContent(i);
    err = h1->GetBinError(i);
    ene1 = h1->GetBinLowEdge(i);
    ene2 = h1->GetBinLowEdge(i + 1);

    h1->SetBinContent(i, val / (ene2 - ene1));
    h1->SetBinError(i, err / (ene2 - ene1));
  }
}

void SaveRootEpsPngTxtFunction(TCanvas* c1, const char* name) {
  char EPS[1000], ROOT[1000], PNG[1000], TXT[1000];
  sprintf(ROOT, "%s.root", name);
  sprintf(EPS, "%s.eps", name);
  sprintf(PNG, "%s.png", name);
  sprintf(TXT, "%s.info", name);

  /// Create file with info
  char Command[10000];
  int test = 0;
  sprintf(Command,
          "echo File with info>>%s;date>>%s;echo>>%s;echo Plot created in:>>%s",
          TXT, TXT, TXT, TXT);
  test = system(Command);
  sprintf(Command, "pwd>>%s;echo>>%s;echo Last Root commands>>%s", TXT, TXT,
          TXT);
  test = system(Command);
  sprintf(Command, "tail ~/.root_hist>>%s;echo>>%s", TXT, TXT);
  test = system(Command);
  // sprintf(Command," more %s",TXT);
  // system(Command);
  cout << "Creating: " << TXT << endl;
  c1->SaveAs(ROOT);
  c1->SaveAs(EPS);
  c1->SaveAs(PNG);
}

int FindPositionString(const std::vector<string>& vector, string name) {
  int DetectorNumber = 0;
  for (size_t i = 0; i < vector.size(); ++i) {
    if (vector[i] == name) {
      DetectorNumber = i;
    }
  }
  return DetectorNumber;
}

int FindPositionString(const std::vector<int>& vector, int name) {
  int DetectorNumber = 0;
  for (size_t i = 0; i < vector.size(); ++i) {
    if (vector[i] == name) {
      DetectorNumber = i;
    }
  }
  return DetectorNumber;
}

string doubleToDecimalString(double num, int decimales) {
  // Convertimos la parte entera a string
  string parteEntera = to_string(static_cast<int>(num));

  // Calculamos la parte decimal
  double parteDecimal = abs(num - static_cast<int>(num));

  // Convertimos la parte decimal a string con el número correcto de decimales
  string parteDecimalStr = to_string(parteDecimal);
  // Si la longitud excede el número de decimales solicitados, truncamos
  if ((int)parteDecimalStr.length() >
      (int)(decimales + 2)) {  // +2 para tener en cuenta el punto decimal y el
                               // dígito anterior
    parteDecimalStr = parteDecimalStr.substr(0, decimales + 2);
  }

  // Concatenamos la parte entera y la parte decimal
  return parteEntera + "." +
         parteDecimalStr.substr(2);  // omitimos el "0." de la parte decimal
}

int FindDetectorPosition(const std::vector<int>& vector, double numero) {
  int DetectorNumber = -1;
  for (size_t i = 0; i < vector.size(); ++i) {
    if (vector[i] == numero) {
      DetectorNumber = i;
    }
  }
  return DetectorNumber;
}
int FindPositionString(const std::vector<int>& vector, string name) {
  int DetectorNumber = -1;
  for (size_t i = 0; i < vector.size(); ++i) {
    if (vector[i] == name) {
      DetectorNumber = i;
    }
  }
  return DetectorNumber;
}

void DivideByNeutronFluence(TH1D* h1, const char* fluxfname,
                            const char* hfluxname, double beamFactor) {
  TFile* f1 = new TFile(fluxfname, "READ");
  if (f1->IsZombie()) {
    cout << " ######## Error opening " << fluxfname << " ###########" << endl;
    exit(1);
  }
  TH1F* n_flux = (TH1F*)f1->Get(hfluxname);
  if (n_flux == 0) {
    cout << " ######## Error getting " << hfluxname << " from " << fluxfname
         << " ###########" << endl;
    exit(1);
  }
  double val, err, ene1, ene2, Nneutrons;

  for (int i = 1; i <= h1->GetNbinsX(); i++) {
    val = h1->GetBinContent(i);
    err = h1->GetBinError(i);
    ene1 = h1->GetBinLowEdge(i);
    ene2 = h1->GetBinLowEdge(i + 1);
    Nneutrons = beamFactor * TakeNumberOfNeutrons(n_flux, ene1, ene2);
    if (Nneutrons > 0) {
      h1->SetBinContent(i, val / Nneutrons);
      h1->SetBinError(i, err / Nneutrons);
    } else {
      h1->SetBinContent(i, 0);
      h1->SetBinError(i, 0);
    }
  }

  f1->Close();
  delete f1;
}

void SubtractYieldFromLibrary(TH1D* h1, TH1D* h_yield) {
  int Nbins1 = h1->GetNbinsX();
  int Nbins2 = h_yield->GetNbinsX();
  cout << Nbins1 << " bins for h1 and " << Nbins2 << " bins for the histo_yield"
       << endl;
  double val, val_yield;
  double x;
  for (int i = 1; i <= Nbins1; i++) {
    val = h1->GetBinContent(i);
    x = h1->GetBinCenter(i);
    val_yield = h_yield->Interpolate(x);
    h1->SetBinContent(i, val - val_yield);
  }
}

double TakeNumberOfNeutrons(TH1F* n_flux, double ene1, double ene2) {
  double Emin = n_flux->GetBinLowEdge(1);
  double Emax = n_flux->GetBinLowEdge(n_flux->GetNbinsX() + 1);
  if (ene1 < Emin) {
    ene1 = Emin;
  }
  if (ene1 > Emax) {
    ene1 = Emax;
  }
  if (ene2 < Emin) {
    ene2 = Emin;
  }
  if (ene2 > Emax) {
    ene2 = Emax;
  }

  int bin1 = n_flux->FindBin(ene1);
  int bin2 = n_flux->FindBin(ene2);

  if (bin1 == bin2) {
    return n_flux->GetBinContent(bin1) * log(ene2 / ene1);
  }

  // else:
  double TotalNeutrons = 0, emin, emax;
  if (ene1 == 0 || n_flux->GetBinLowEdge(bin2) == 0) {
    cout << " ******* Error in " << __FILE__ << ", line " << __LINE__ << " ****"
         << endl;
    exit(1);
  }
  TotalNeutrons +=
      n_flux->GetBinContent(bin1) * log(n_flux->GetBinLowEdge(bin1 + 1) / ene1);
  TotalNeutrons +=
      n_flux->GetBinContent(bin2) * log(ene2 / n_flux->GetBinLowEdge(bin2));
  for (int i = 1; i < bin2 - bin1; i++) {
    emin = n_flux->GetBinLowEdge(bin1 + i);
    emax = n_flux->GetBinLowEdge(bin1 + i + 1);
    if (n_flux->GetBinLowEdge(bin1 + i + 1) == 0) {
      cout << " ******* Error in " << __FILE__ << ", line " << __LINE__
           << " ****" << endl;
      exit(1);
    }
    TotalNeutrons += n_flux->GetBinContent(bin1 + i) * log(emax / emin);
  }

  return TotalNeutrons;
}

// multmin=1,2,3,...
// If multmax<0 no cuts in EsumMax
// Opt=1,2 --> Eff as a function of the reaction rate (Opt=1), as a function of
// the counting rate (Opt==2)
TGraphErrors* TakeEffFunction(const char* fname, double EsumMin, double EsumMax,
                              int multmin, int multmax, int Opt) {
  char grname[100];
  if (Opt == 1) {
    if (multmax < 0) {
      sprintf(grname, "Eff1_%.2f_%.2f_mg%d", EsumMin, EsumMax, multmin - 1);
    } else {
      sprintf(grname, "Eff1_%.2f_%.2f_m_%d_%d", EsumMin, EsumMax, multmin,
              multmax);
    }
  } else if (Opt == 2) {
    if (multmax < 0) {
      sprintf(grname, "Eff2_%.2f_%.2f_mg%d", EsumMin, EsumMax, multmin - 1);
    } else {
      sprintf(grname, "Eff2_%.2f_%.2f_m_%d_%d", EsumMin, EsumMax, multmin,
              multmax);
    }
  } else {
    cout << " Opt = " << Opt << " not allowed in " << fname << endl;
    cout << " ###### Error in " << __FILE__ << ", line " << __LINE__
         << " ######" << endl;
    exit(1);
  }

  TFile* f1 = new TFile(fname, "READ");
  TGraphErrors* gr1 = (TGraphErrors*)f1->Get(grname);
  if (gr1 == 0) {
    cout << " Error: graph with name " << grname << " does not exist in "
         << fname << endl;
    cout << " ###### Error in " << __FILE__ << ", line " << __LINE__
         << " ######" << endl;
    exit(1);
  }
  f1->Close();
  delete f1;

  return gr1;
}

// Coje una lista de runes de un fichero
int TakeRunList(const char* fname, string* fnameRunList) {
  int nruns = 0;
  ifstream in(fname);
  while (in >> fnameRunList[nruns]) {
    nruns++;
  }
  in.close();
  return nruns;
}

// Coje una lista de runes de una medida
int TakeRunList(const char* fname, string RunType, const char* roorfilesdirname,
                string* fnameRunList, int* RunList) {
  int aRunList[100000];
  int nruns = TakeRunList(fname, RunType, aRunList);
  char afname[100];
  for (int i = 0; i < nruns; i++) {
    sprintf(afname, "%s/run%d.root", roorfilesdirname, aRunList[i]);
    fnameRunList[i] = string(afname);
    if (RunList) {
      RunList[i] = aRunList[i];
    }
  }
  return nruns;
}

void GetRunMinMax(const char* fname, string RunType, int& RunMin, int& RunMax) {
  int RunList[100000];
  int nruns = TakeRunList(fname, RunType, RunList, 0);
  RunMin = RunList[0];
  RunMax = RunList[0];
  for (int i = 0; i < nruns; i++) {
    if (RunList[i] < RunMin) {
      RunMin = RunList[i];
    }
    if (RunList[i] > RunMax) {
      RunMax = RunList[i];
    }
  }
}

// Coje una lista de runes de una medida
int TakeRunList(const char* fname, string RunType, int* RunList, int vervose) {
  string word;
  ifstream in(fname);
  while (in >> word) {
    if (word == string("RUNLIST")) {
      break;
    }
  }
  if (!in.good()) {
    cout << " ###### Error in " << __FILE__ << ", line " << __LINE__
         << " ###### " << fname << endl;
    exit(1);
  }

  int runmin, runmax, oldrunmax = -1;
  int nRuns = 0;
  while (in >> runmin >> runmax >> word) {
    if (word == RunType) {
      if (runmax < runmin || (oldrunmax > runmin && oldrunmax > 0)) {
        cout << runmin << "  " << runmax << "  " << oldrunmax << endl;
        cout << " ###### Error in " << __FILE__ << ", line " << __LINE__
             << " ######" << endl;
        exit(1);
      }
      oldrunmax = runmax;
      for (int i = runmin; i <= runmax; i++) {
        RunList[nRuns] = i;
        nRuns++;
      }
    }
  }
  in.close();

  if (vervose != 0) {
    cout << " Taken run list from " << fname << " of run type " << RunType
         << ". The list is:" << endl;
    for (int i = 0; i < nRuns; i++) {
      cout << RunList[i] << endl;
    }
  }

  if (nRuns == 0) {
    cout << " ####### Error in " << __FILE__ << ", line " << __LINE__
         << " ###### " << fname << endl;
    cout << " ####### No runs found for RunType = " << RunType
         << " in the RunList" << endl;
    exit(1);
  }
  return nRuns;
}

void TakePKUPInfo(const char* fname, PKUPInfo* thePKUPInfo) {
  TFile* f1 = new TFile(fname, "READ");
  TTree* tr = (TTree*)f1->Get("PKUP");

  if (tr == 0) {
    cout << " ######## Error getting PKUP from so we would use C6D6 as PKUP"
         << fname << " ###########" << endl;
    tr = (TTree*)f1->Get("C6D6");
    thePKUPInfo->npulses = tr->GetMaximum("BunchNumber");
    thePKUPInfo->np1 = 0;
    thePKUPInfo->np2 = 0;
    thePKUPInfo->np3 = thePKUPInfo->npulses;
    thePKUPInfo->PulsInt1 = 0;
    thePKUPInfo->PulsInt2 = 0;
    thePKUPInfo->PulsInt3 = 0;
    thePKUPInfo->PKUPamp1 = 0;
    thePKUPInfo->PKUPamp2 = 0;
    thePKUPInfo->PKUPamp3 = 0;
    thePKUPInfo->PKUParea1 = 0;
    thePKUPInfo->PKUParea2 = 0;

    for (int i = 0; i < thePKUPInfo->npulses; i++) {
      thePKUPInfo->pulseType[i] = 0;
      thePKUPInfo->pulseType[i] = 0;
      thePKUPInfo->BunchNumber[i] = 0;
      thePKUPInfo->PulseIntensity[i] = 0;
      thePKUPInfo->PKUPAmp[i] = 0;
      thePKUPInfo->PKUPArea[i] = 0;
      thePKUPInfo->PKUPTflash[i] = 0;
      thePKUPInfo->pulseType[i] = 0;
      thePKUPInfo->pulseType[i] = 0;
    }
  } else {
    //
    Signal theS;
    AttachStruct(&theS, tr);
    int nentries = tr->GetEntries();
    int thisBunch = -1;
    int npulses = -1;
    thePKUPInfo->npulses = -1;
    bool PKUPsignalInPulse = false;
    bool AllowMoreThanOnePKUPSignal = false;
#ifdef ALLOW_MORE_THAN_ONE_PKUP_SIGNAL
    AllowMoreThanOnePKUPSignal = true;
#endif
    for (int i = 0; i < nentries; i++) {
      tr->GetEntry(i);
      if (theS.BunchNumber != thisBunch) {  // new pulse
        thisBunch = theS.BunchNumber;
        thePKUPInfo->npulses++;
        npulses = thePKUPInfo->npulses;
        PKUPsignalInPulse = false;
      }
      if (GetPulseType(&theS) == DEDICATED_PULSES_ID &&
          theS.amp > DEDICATED_PULSES_PKUPAMPMIN &&
          theS.amp < DEDICATED_PULSES_PKUPAMPMAX) {
        if (!PKUPsignalInPulse || theS.amp > thePKUPInfo->PKUPAmp[npulses]) {
          thePKUPInfo->pulseType[npulses] = DEDICATED_PULSES_ID;
          thePKUPInfo->BunchNumber[npulses] = theS.BunchNumber;
          thePKUPInfo->PulseIntensity[npulses] = theS.PulseIntensity;
          thePKUPInfo->PKUPAmp[npulses] = theS.amp;
          thePKUPInfo->PKUPArea[npulses] = theS.area;
          thePKUPInfo->PKUPTflash[npulses] = theS.tflash;
        }
        if (PKUPsignalInPulse && !AllowMoreThanOnePKUPSignal) {
          cout << " ############ Warning, in " << fname
               << " there are two PKUP pulses in bunch " << theS.BunchNumber
               << " - rejecting pulse ... ############" << endl;
          cout << theS.PSpulse << "  Amp1= " << thePKUPInfo->PKUPAmp[npulses]
               << "  Amp2=" << theS.amp << endl;
          thePKUPInfo->pulseType[npulses] = 0;
        }
        PKUPsignalInPulse = true;
      } else if (GetPulseType(&theS) == PARASITIC_PULSES_ID &&
                 theS.amp > PARASITIC_PULSES_PKUPAMPMIN &&
                 theS.amp < PARASITIC_PULSES_PKUPAMPMAX) {
        if (!PKUPsignalInPulse || theS.amp > thePKUPInfo->PKUPAmp[npulses]) {
          thePKUPInfo->pulseType[npulses] = PARASITIC_PULSES_ID;
          thePKUPInfo->BunchNumber[npulses] = theS.BunchNumber;
          thePKUPInfo->PulseIntensity[npulses] = theS.PulseIntensity;
          thePKUPInfo->PKUPAmp[npulses] = theS.amp;
          thePKUPInfo->PKUPArea[npulses] = theS.area;
          thePKUPInfo->PKUPTflash[npulses] = theS.tflash;
        }
        if (PKUPsignalInPulse && !AllowMoreThanOnePKUPSignal) {
          cout << " ############ Warning, in " << fname
               << " there are two PKUP pulses in bunch " << theS.BunchNumber
               << " - rejecting pulse ... ############" << endl;
          cout << theS.PSpulse << "  Amp1= " << thePKUPInfo->PKUPAmp[npulses]
               << "  Amp2=" << theS.amp << endl;
          thePKUPInfo->pulseType[npulses] = 0;
        }
        PKUPsignalInPulse = true;
      } else {
        // cout<<theS.PSpulse<<"  "<<theS.PulseIntensity<<"  "<<theS.area<<endl;
        if (!PKUPsignalInPulse || theS.amp > thePKUPInfo->PKUPAmp[npulses]) {
          thePKUPInfo->pulseType[npulses] = 0;
          thePKUPInfo->BunchNumber[npulses] = theS.BunchNumber;
          thePKUPInfo->PulseIntensity[npulses] = theS.PulseIntensity;
          thePKUPInfo->PKUPAmp[npulses] = theS.amp;
          thePKUPInfo->PKUPArea[npulses] = theS.area;
          thePKUPInfo->PKUPTflash[npulses] = theS.tflash;
        }
      }
    }

    thePKUPInfo->npulses++;

    thePKUPInfo->np1 = 0;
    thePKUPInfo->np2 = 0;
    thePKUPInfo->np3 = 0;
    thePKUPInfo->PulsInt1 = 0;
    thePKUPInfo->PulsInt2 = 0;
    thePKUPInfo->PulsInt3 = 0;
    thePKUPInfo->PKUPamp1 = 0;
    thePKUPInfo->PKUPamp2 = 0;
    thePKUPInfo->PKUPamp3 = 0;
    thePKUPInfo->PKUParea1 = 0;
    thePKUPInfo->PKUParea2 = 0;
    thePKUPInfo->PKUParea3 = 0;

    for (int i = 0; i < thePKUPInfo->npulses; i++) {
      if (thePKUPInfo->pulseType[i] == DEDICATED_PULSES_ID) {
        thePKUPInfo->np1++;
        thePKUPInfo->PulsInt1 += thePKUPInfo->PulseIntensity[i];
        thePKUPInfo->PKUPamp1 += thePKUPInfo->PKUPAmp[i];
        thePKUPInfo->PKUParea1 += thePKUPInfo->PKUPArea[i];
      } else if (thePKUPInfo->pulseType[i] == PARASITIC_PULSES_ID) {
        thePKUPInfo->np2++;
        thePKUPInfo->PulsInt2 += thePKUPInfo->PulseIntensity[i];
        thePKUPInfo->PKUPamp2 += thePKUPInfo->PKUPAmp[i];
        thePKUPInfo->PKUParea2 += thePKUPInfo->PKUPArea[i];
      } else if (thePKUPInfo->pulseType[i] == 0) {
        thePKUPInfo->np3++;
        thePKUPInfo->PulsInt3 += thePKUPInfo->PulseIntensity[i];
        thePKUPInfo->PKUPamp3 += thePKUPInfo->PKUPAmp[i];
        thePKUPInfo->PKUParea3 += thePKUPInfo->PKUPArea[i];
      } else {
        cout << " ######## Error in " << __FILE__ << ", line " << __LINE__
             << " ########" << endl;
        exit(1);
      }
    }
  }
}
// Returns the number of bunches
int CreateBunchesIndex(TTree* tr, Long64_t* bunchIndex, int* BunchNumber) {
  Signal theS;
  AttachStruct(&theS, tr);
  Long64_t nentries = tr->GetEntries();
  int thisBunch = -1;
  int npul = -1;
  for (Long64_t i = 0; i < nentries; i++) {
    int check = tr->GetEntry(i);
    if (check <= 0) {
      cout << " ############ Error in " << __FILE__ << ", line " << __LINE__
           << " ############" << endl;
      exit(1);
    }
    if (theS.BunchNumber != thisBunch) {  // new pulse
      thisBunch = theS.BunchNumber;
      npul++;
      if (npul >= MAXNBUNCHESINFILE) {
        cout << " ############ Error in " << __FILE__ << ", line " << __LINE__
             << " ############" << endl;
        exit(1);
      }
      bunchIndex[npul] = i;
      if (BunchNumber != 0) {
        BunchNumber[npul] = theS.BunchNumber;
      }
    }
  }
  bunchIndex[npul + 1] =
      nentries;  // one more than the high limit, as the others
  cout << "Finish CreateBunchesIndex   Number trigger " << bunchIndex[npul + 1]
       << endl;

  return npul + 1;
}

// we change bunchIndex and BunchNumber to acomodate them to bunchIndex_ref and
// BunchNumber_ref
void ReCalibrateBunchesIndexVector(int& NBunches, Long64_t* bunchIndex,
                                   int* BunchNumber, int NBunches_ref,
                                   Long64_t* bunchIndex_ref,
                                   int* BunchNumber_ref) {
  Long64_t bunchIndex_new[100000];
  for (int i = 0; i <= NBunches_ref; i++) {
    bunchIndex_new[i] = -1;
  }

  for (int i = 0; i < NBunches; i++) {
    for (int j = 0; j < NBunches_ref; j++) {
      if (BunchNumber[i] == BunchNumber_ref[j]) {
        bunchIndex_new[j] = bunchIndex[i];
        break;
      }
    }
  }
  bunchIndex_new[NBunches_ref] = bunchIndex[NBunches];

  for (int i = 0; i <= NBunches_ref; i++) {
    if (bunchIndex_new[i] < 0) {
      for (int j = i + 1; j <= NBunches_ref; j++) {
        if (bunchIndex_new[j] > 0) {
          bunchIndex_new[i] = bunchIndex_new[j];
          break;
        }
      }
    }
    if (i < NBunches_ref) {
      BunchNumber[i] = BunchNumber_ref[i];
    }
    bunchIndex[i] = bunchIndex_new[i];
  }
  NBunches = NBunches_ref;
}

void AttachStruct(Signal* P, TTree* tr) {
  tr->SetBranchAddress("RunNumber", &(P->RunNumber));
  tr->SetBranchAddress("BunchNumber", &(P->BunchNumber));
  tr->SetBranchAddress("date", &(P->date));
  tr->SetBranchAddress("detn", &(P->detn));
  tr->SetBranchAddress("time", &(P->time));
  tr->SetBranchAddress("amp", &(P->amp));
  tr->SetBranchAddress("tof", &(P->tof));
  tr->SetBranchAddress("tflash", &(P->tflash));
  tr->SetBranchAddress("area", &(P->area));
  tr->SetBranchAddress("fwhm", &(P->fwhm));
  tr->SetBranchAddress("fwtm", &(P->fwtm));
  tr->SetBranchAddress("area_0", &(P->area_0));
  tr->SetBranchAddress("amp_0", &(P->amp_0));
  tr->SetBranchAddress("area2", &(P->area2));
  tr->SetBranchAddress("PSpulse", &(P->PSpulse));
  tr->SetBranchAddress("PulseIntensity", &(P->PulseIntensity));
  tr->SetBranchAddress("afast", &(P->afast));
  tr->SetBranchAddress("aslow", &(P->aslow));
  tr->SetBranchAddress("isAlpha", &(P->isAlpha));
  tr->SetBranchAddress("risetime", &(P->Tau));

  tr->SetBranchAddress("movie", &(P->movie));  // NEW
}

// 2 -> Dedicated, 3-> Parasitic, 0->Other
int GetPulseType(Signal* theS) {
  if (theS->PSpulse == DEDICATED_PULSES_ID_ROOTFILE &&
      theS->PulseIntensity > DEDICATED_PULSES_INTMIN &&
      theS->PulseIntensity < DEDICATED_PULSES_INTMAX) {
    return DEDICATED_PULSES_ID;
  } else if ((theS->PSpulse == PARASITIC_PULSES_ID_ROOTFILE ||
              theS->PSpulse == DEDICATED_PULSES_ID_ROOTFILE) &&
             theS->PulseIntensity > PARASITIC_PULSES_INTMIN &&
             theS->PulseIntensity < PARASITIC_PULSES_INTMAX) {
    // Allow low intensity dedicated pulses to be treated as regular parasitic
    // pulses
    return PARASITIC_PULSES_ID;
  }
  // else if(theS->PSpulse==PARASITIC_PULSES_ID_ROOTFILE &&
  // theS->PulseIntensity>PARASITIC_PULSES_INTMIN &&
  // theS->PulseIntensity<PARASITIC_PULSES_INTMAX){
  //   return PARASITIC_PULSES_ID;
  // }

  return 0;
}

double TOFtoE(double tof, double tofd) {
  double beta = 0.;
  double gamma = 0.;
  double n_energy = 0.;
  double cx = 0.;
  double tflash = 0.;

  tflash = tofd / 299792458. * 1.e9;
  beta = (double)tofd * 1.e9 / (tof + tflash) / 299792458.;
  cx = sqrt((1. - beta * beta));
  gamma = (double)1. / cx;
  n_energy = (gamma - 1.) * 939.57 * 1.e6; /* e in eV */

  if (n_energy != n_energy) {
    return 0;
  }

  return n_energy;
}

double EtoTOF(double nene, double tofd) {
  double tflash = tofd / 299792458. * 1.e9;

  double gamma = 1. + nene / (939.57 * 1.e6);
  double cx = 1. / gamma;
  double beta = sqrt(1. - cx * cx);
  double tof = ((tofd * 1.e9) / (beta * 299792458.)) - tflash;

  if (tof != tof) {
    return 0;
  }

  return tof;
}

double DeltaEnergyToTime(double E1, double E2, double tofd) {  // eV

  double tof1 = EtoTOF(E1, tofd);
  double tof2 = EtoTOF(E2, tofd);

  return (tof1 - tof2);
}

void DivideByBinTime(TH1D* h1, double TOFD) {
  int nbins = h1->GetNbinsX();
  double val, err, ene1, ene2, tof;
  for (int i = 1; i <= nbins; i++) {
    val = h1->GetBinContent(i);
    err = h1->GetBinError(i);
    ene1 = h1->GetBinLowEdge(i);
    ene2 = h1->GetBinLowEdge(i + 1);
    tof = DeltaEnergyToTime(ene1, ene2, TOFD) / 1.e3;  // mus
    h1->SetBinContent(i, val / tof);
    h1->SetBinError(i, err / tof);
  }
}

// Funcion para promediar el En de NoBeam y suavizarlo.
void AverageNoBeamHisto(TH1D* h1, double TOFD) {
  int nbins = h1->GetNbinsX();
  double integral = h1->Integral(1, nbins);
  double total_time =
      DeltaEnergyToTime(h1->GetBinLowEdge(1), h1->GetBinLowEdge(nbins), TOFD) /
      1.e3;                                // mus
  double average = integral / total_time;  // counts per microsecond
  double val, err, ene1, ene2, tof;
  for (int i = 1; i <= nbins; i++) {
    val = h1->GetBinContent(i);
    err = h1->GetBinError(i);
    ene1 = h1->GetBinLowEdge(i);
    ene2 = h1->GetBinLowEdge(i + 1);
    tof = DeltaEnergyToTime(ene1, ene2, TOFD) / 1.e3;  // mus
    h1->SetBinContent(i, average * tof);
    h1->SetBinError(i, 1. / sqrt(average)); /** PUEDE QUE ESTE MAL */
  }
}

double IntegrateBest(TGraph* gr, double emin, double emax) {
  int const MAXSIZE = 200000;
  int np = gr->GetN();
  if (np > MAXSIZE) {
    cout << " *** ERROR IN " << __FILE__ << " , line " << __LINE__ << endl;
    exit(0);
  }
  double x[MAXSIZE];
  double y[MAXSIZE];
  int np2 = 1;
  x[0] = emin;
  y[0] = gr->Eval(emin);
  for (int i = 0; i < np; i++) {
    gr->GetPoint(i, x[np2], y[np2]);
    if (x[np2] > emin && x[np2] < emax) {
      np2++;
    }
  }
  x[np2] = emax;
  y[np2] = gr->Eval(emax);
  np2++;
  double Sum = 0;
  for (int i = 0; i < np2 - 1; i++) {
    Sum += (y[i] + y[i + 1]) / 2. * (x[i + 1] - x[i]);
    if (x[i + 1] - x[i] < 0) {
      cout << "x[" << i + 1 << "]=" << x[i + 1] << "  x[" << i << "]=" << x[i]
           << "  np=" << np2 << endl;
    }
  }
  if (Sum < 1.e-20) {
    cout << " #### Warning, Integral(" << emin << "," << emax << ")=" << Sum
         << "  , np=" << np2 << "  ####" << endl;
  }
  return Sum;
}

void ScaleGraphErrors(TGraphErrors* gr, double ScaleFactor) {
  int np = gr->GetN();
  for (int i = 0; i < np; i++) {
    gr->GetY()[i] *= ScaleFactor;
    gr->GetEY()[i] *= ScaleFactor;
  }
}
void ScaleGraph(TGraph* gr, double ScaleFactor) {
  int np = gr->GetN();
  for (int i = 0; i < np; i++) {
    gr->GetY()[i] *= ScaleFactor;
  }
}

#endif
