#ifndef CREATEHISTOFUNCTIONS_HH
#define CREATEHISTOFUNCTIONS_HH 1

#include "CalibrationManager.hh"

//================================================================================================
void CreateTH1DHistoPulseIntesity(TH1D** h1, int N_PULSETYPE, TFile* fout);

void CreateTH2DHisto(HistoInfo* theHistoInfo, int N_PULSETYPE, TH2D**** h2,
                     TFile* fout);
//================================================================================================

void CreateTH1DHistoPulseIntesity(TH1D** h1, int N_PULSETYPE, TFile* fout) {
  char hname[100];
  char htitle[300];

  for (int ptype = 0; ptype < N_PULSETYPE; ptype++) {
    sprintf(hname, "hPulIntens_%d", ptype);
    sprintf(htitle,
            "Pulse intensities for  PulseType %d "
            "[NProtons,NPulses,PKUPamp,PKUParea]",
            ptype);
    h1[ptype] = new TH1D(hname, htitle, 4, 0.5, 4.5);
    h1[ptype]->SetDirectory(fout);
  }
  for (int ptype = 0; ptype < N_PULSETYPE; ptype++) {
    sprintf(hname, "hProtonsIntensity_%d", ptype);
    sprintf(htitle, "Histogram of Proton intensity pulses %d ", ptype);
    h1[N_PULSETYPE + ptype] = new TH1D(hname, htitle, 1e4, 0, 1e13);
    h1[N_PULSETYPE + ptype]->SetDirectory(fout);
  }
}

void CreateTH2DHisto(bool CreateHistograms, HistoInfo* theHistoInfo,
                     int N_PULSETYPE, TH2D**** h2, TFile* fout) {
  string hName, hTitle;
  if (!CreateHistograms) {
    return;
  }
  double* xaxis;

  for (int ptype = 0; ptype < N_PULSETYPE; ptype++) {
    h2[ptype] = new TH2D**[theHistoInfo->HistoType.size() + 1];

    for (int HistoType = 0; HistoType < (int)theHistoInfo->HistoType.size();
         HistoType++) {
      h2[ptype][HistoType] = new TH2D*[theHistoInfo->DetectorNumber.size()];
      xaxis = new double[(int)theHistoInfo->Xaxis[HistoType][0] + 1];
      if (theHistoInfo->HistoType[HistoType] == 1) {
        for (int i = 0; i <= theHistoInfo->Xaxis[HistoType][0]; i++) {
          // if()
          xaxis[i] = theHistoInfo->Xaxis[HistoType][1] *
                     pow(theHistoInfo->Xaxis[HistoType][2] /
                             theHistoInfo->Xaxis[HistoType][1],
                         i / (double)theHistoInfo->Xaxis[HistoType][0]);
        }
      } else if (theHistoInfo->HistoType[HistoType] == 2 ||
                 theHistoInfo->HistoType[HistoType] == 3 ||
                 theHistoInfo->HistoType[HistoType] == 4) {
        for (int i = 0; i <= theHistoInfo->Xaxis[HistoType][0]; i++) {
          // if()
          xaxis[i] = theHistoInfo->Xaxis[HistoType][1] +
                     (i *
                      (theHistoInfo->Xaxis[HistoType][2] -
                       theHistoInfo->Xaxis[HistoType][1]) /
                      (double)theHistoInfo->Xaxis[HistoType][0]);
          // cout << i << " " << xaxis[i] << endl;
        }
      }

      else {
        cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
             << " ###########" << endl;
        exit(1);
      }
      for (int det = 0; det < ((int)theHistoInfo->DetectorNumber.size());
           det++) {
        hName = theHistoInfo->HistoName[HistoType] + "_" +
                theHistoInfo->DetectorName + "_" +
                to_string(theHistoInfo->DetectorNumber[det]) + "_PType_" +
                to_string(ptype);
        hTitle = theHistoInfo->HistoTitle[HistoType] + "_" +
                 theHistoInfo->DetectorName + "_" +
                 to_string(theHistoInfo->DetectorNumber[det]) + "_PType_" +
                 to_string(ptype);

        h2[ptype][HistoType][det] = new TH2D(
            hName.c_str(), hTitle.c_str(),
	    theHistoInfo->Xaxis[HistoType][0], xaxis,
	    theHistoInfo->Yaxis[HistoType][0],
            theHistoInfo->Yaxis[HistoType][1],
            theHistoInfo->Yaxis[HistoType][2]);
	
        h2[ptype][HistoType][det]->GetYaxis()->SetTitle("E_{dep}(MeV)");
        if (theHistoInfo->HistoType[HistoType] == 1) {
          h2[ptype][HistoType][det]->GetXaxis()->SetTitle(
              "Neutron energy (eV)");
        } else if (theHistoInfo->HistoType[HistoType] == 2 ||
                   theHistoInfo->HistoType[HistoType] == 3 ||
                   theHistoInfo->HistoType[HistoType] == 4) {
          h2[ptype][HistoType][det]->GetXaxis()->SetTitle("TOF (ns)");
        }
        h2[ptype][HistoType][det]->SetDirectory(fout);
      }
    }
  }
}
#endif
