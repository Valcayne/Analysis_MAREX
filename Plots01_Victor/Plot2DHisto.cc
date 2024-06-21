
#include "../GeneralFun/PlotFunctions_Victor.hh"
#include "DefPlot2DHisto_2024_Er_Cu_U_v01.hh"
// #include "DefPlot2DHisto_2024_Test_L6D6.hh"

void plot01(string NameHisto, std::vector<string> MeasurementType,
            std::vector<int> detN, std::vector<double> EnOrEdepMin,
            std::vector<double> EnOrEdepMax, std::vector<int> PType,
            string TypeOfPlot, int rebin, bool UseBackgroundSubtracted = false,
            bool IfNormalize = false, bool CompareWithSimul = false,
            bool CalculateRatio = false);

void Plot2DHisto() {
  // plot01("Edep", {"Au_YDetO"}, {1}, {50, 6, 1, 0.03}, {200, 50, 6, 1}, {1},
  // "Counts", 1, true, true); plot01("En", {"Au"}, {1, 2}, {0.1}, {10}, {1},
  // "Yield", 100, true, true, true,         true); plot01("Edep", {"Y"}, {1, 2,
  // 3, 4}, {0.001}, {10}, {3}, "CR", 1, false);

  // plot01("En", {"Au", "Empty"}, {1, 2}, {0.12}, {6}, {1}, "Counts", 1,
  // false);
}

void plot01(string NameHisto, std::vector<string> MeasurementType,
            std::vector<int> detN, std::vector<double> EnOrEdepMin,
            std::vector<double> EnOrEdepMax, std::vector<int> PType,
            string TypeOfPlot, int rebin, bool UseBackgroundSubtracted,
            bool IfNormalize, bool CompareWithSimul, bool CalculateRatio) {
  // -------------------------------------------------

  int MeasurementTypeSize = MeasurementType.size();
  int detNSize = detN.size();
  int EnOrEdepMinSize = EnOrEdepMin.size();
  int EnOrEdepMaxSize = EnOrEdepMax.size();
  int PTypeSize = PType.size();
  int NumberOfHisto = MeasurementTypeSize * detNSize * EnOrEdepMinSize *
                      EnOrEdepMaxSize * PTypeSize;

  double npulses[NumberOfHisto];
  double nprotons[NumberOfHisto];
  TH1D* h1[NumberOfHisto];
  TH1D* h1GainShift[NumberOfHisto];

  TCanvas* c2[NumberOfHisto];
  string TitleLegend[NumberOfHisto];
  string TitleHisto;
  string SaveName = DirectionSave2DHisto + NameDetector + "_" + NameHisto;
  double Returnpulses, Returnprotons;
  int n = 0;
  string fname;
  TH1D* hSimul;
  bool SubtractBackground = false;
  double Used_PType[NumberOfHisto];
  double Used_EnOrEdepMin[NumberOfHisto];
  double Used_EnOrEdepMax[NumberOfHisto];
  double ifEnOrEdepMaxAndEnOrEdepMinCouple = false;
  bool IfEnOrEdep = true;
  if (NameHisto == "Edep") {
    IfEnOrEdep = false;
  }

  if (EnOrEdepMinSize > 1 && (EnOrEdepMinSize == EnOrEdepMaxSize)) {
    EnOrEdepMaxSize = 1;
    ifEnOrEdepMaxAndEnOrEdepMinCouple = true;
    NumberOfHisto =
        MeasurementTypeSize * detNSize * EnOrEdepMinSize * PTypeSize;
  }

  for (auto i = 0; i < MeasurementTypeSize; i++) {
    fname = Direction2DHisto + MeasurementType[i] + ".root";
    SaveName = SaveName + "_" + MeasurementType[i];
    for (auto j = 0; j < detNSize; j++) {
      if (i == 0) {
        SaveName = SaveName + "_Det" + to_string(detN[j]);
      }
      for (auto k = 0; k < EnOrEdepMinSize; k++) {
        if (ifEnOrEdepMaxAndEnOrEdepMinCouple) {
          EnOrEdepMaxSize = 1;
        }

        for (auto l = 0; l < EnOrEdepMaxSize; l++) {
          if (ifEnOrEdepMaxAndEnOrEdepMinCouple) {
            l = k;
          }
          for (auto m = 0; m < PTypeSize; m++) {
            if (i == 0 && j == 0) {
              SaveName = SaveName + "_PType" + to_string(PType[m]);
            }

            if (MeasurementTypeSize != 1) {
              TitleLegend[n] = TitleLegend[n] + MeasurementType[i];
            }
            if (detNSize != 1) {
              TitleLegend[n] = TitleLegend[n] + " Det " + to_string(detN[j]);
            }
            if (EnOrEdepMinSize != 1 || EnOrEdepMaxSize != 1) {
              if (IfEnOrEdep) {
                TitleLegend[n] =
                    TitleLegend[n] + " (" +
                    doubleToDecimalString(EnOrEdepMin[k], 2) + "," +
                    doubleToDecimalString(EnOrEdepMax[l], 1) + ") MeV";
              } else {
                if (EnOrEdepMin[k] > 99) {
                  TitleLegend[n] =
                      TitleLegend[n] + " (" +
                      doubleToDecimalString(EnOrEdepMin[k] / 1000, 2) + "," +
                      doubleToDecimalString(EnOrEdepMax[l] / 1000, 1) + ") keV";
                } else {
                  TitleLegend[n] =
                      TitleLegend[n] + " (" +
                      doubleToDecimalString(EnOrEdepMin[k], 2) + "," +
                      doubleToDecimalString(EnOrEdepMax[l], 1) + ") eV";
                }
              }
            }

            if (PTypeSize != 1) {
              TitleLegend[n] = TitleLegend[n] + " PType " + to_string(PType[m]);
            }
            cout << "fname " << fname << endl;

            if (UseBackgroundSubtracted) {
              c2[n] = new TCanvas(TitleLegend[n].c_str(),
                                  TitleLegend[n].c_str(), 800, 600);

              h1[n] = Project2DHistoSubtractBackground(
                  Direction2DHisto, NameDetector, NameHisto, MeasurementType[i],
                  PType[m], detN[j], IfEnOrEdep, Returnpulses, Returnprotons,
                  TOFD, EnOrEdepMin[k], EnOrEdepMax[l], TimeMeasurement_ns,
                  MeasPredefined, BackgroundMeasPredefined, DummyPredefined,
                  BackgroundDummyPredefined, c2[n]);

            } else {
              h1[n] = Project2DHisto(
                  fname, NameDetector, NameHisto, MeasurementType[i], PType[m],
                  detN[j], IfEnOrEdep, Returnpulses, Returnprotons, TOFD,
                  EnOrEdepMin[k], EnOrEdepMax[l], TimeMeasurement_ns);
            }
            // ChangeTOFD(h1[n], TOFD, 184);  // Change TOF in case of needed
            npulses[n] = Returnpulses;
            nprotons[n] = Returnprotons;
            Used_PType[n] = PType[m];
            Used_EnOrEdepMin[n] = EnOrEdepMin[k];
            Used_EnOrEdepMax[n] = EnOrEdepMax[l];
            // cout << i << " " << TitleLegend[i] << " pulses " << npulses[n]
            //    << " nprotons " << nprotons[n] << endl;
            h1[n]->SetLineColor(n + 10001);
            n++;
          }
        }
      }
    }
  }

  hSimul = (TH1D*)h1[0]->Clone();
  if (CompareWithSimul) {
    if (MeasurementTypeSize != 1) {
      cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
           << " ###########" << endl;
      exit(1);
    } else if (NameHisto == "Edep" && PType[0] == 3) {
      cout << "Start with GetSimulMC" << endl;
      hSimul = GetSimulMC(h1[0], detN[0], MeasurementType[0], NameSimulArray,
                          NameSimulRootfile, Activity_kBq, ResolNumberDetector,
                          ResolParameter1, ResolParameter2, TypeOfPlot);
    } else {
      cout << "···············3" << MeasurementType[0] << " " << PType[0]
           << endl;
      hSimul = GetSimul(h1[0], MeasurementType[0], NameSimulArray,
                        NameSimulRootfile, TypeOfPlot);
    }
    hSimul->SetLineColor(10009);
  }
  hSimul->Rebin(rebin);
  hSimul->Scale(1.0 / (double)rebin);

  /// We define  the title of the histo
  TitleHisto = TitleHisto + NameDetector;
  if (MeasurementTypeSize == 1) {
    TitleHisto = TitleHisto + " " + MeasurementType[0];
    if (UseBackgroundSubtracted) {
      TitleHisto = TitleHisto + "-Bkgs";
    }
    if (IfNormalize && IfEnOrEdep) {
      TitleHisto = TitleHisto + " Nor (" +
                   doubleToDecimalString(NormalizeMinEn, 0) + "," +
                   doubleToDecimalString(NormalizeMaxEn, 0) + ") eV";
    }
    if (IfNormalize && (!IfEnOrEdep)) {
      TitleHisto = TitleHisto + " Nor (" +
                   doubleToDecimalString(NormalizeMinEdep, 0) + "," +
                   doubleToDecimalString(NormalizeMaxEdep, 0) + ") eV";
    }
  }

  if (detNSize == 1) {
    TitleHisto = TitleHisto + " Det" + to_string(detN[0]);
  }
  if (PTypeSize == 1) {
    TitleHisto = TitleHisto + " PType " + to_string(PType[0]);
  }
  if (EnOrEdepMinSize == 1 && EnOrEdepMaxSize == 1) {
    if (IfEnOrEdep) {
      TitleHisto = TitleHisto + " (" +
                   doubleToDecimalString(EnOrEdepMin[0], 2) + "," +
                   doubleToDecimalString(EnOrEdepMax[0], 1) + ") MeV";
    } else {
      TitleHisto = TitleHisto + " (" +
                   doubleToDecimalString(EnOrEdepMin[0], 1) + "," +
                   doubleToDecimalString(EnOrEdepMax[0], 1) + ") eV";
    }
  }

  cout << "TitleHisto " << TitleHisto << endl;

  /// We define Y axis for TypeOfPlot
  for (auto i = 0; i < NumberOfHisto; i++) {
    h1[i]->Rebin(rebin);
    // cout << "rebin " << rebin << " i " << i << endl;
    if (TypeOfPlot == "Counts") {
      cout << "Counts" << endl;
      h1[i]->GetYaxis()->SetTitle("counts");
    } else if (TypeOfPlot == "CP") {
      h1[i]->Scale(8.e12 / nprotons[i]);
      h1[i]->GetYaxis()->SetTitle("counts/8#upoint10^{12} protons  ");
    } else if (TypeOfPlot == "CR") {
      if (!IfEnOrEdep) {
        double BinWidth = h1[i]->GetBinWidth(1);
        // The idea is to have the number of counts per second.
        double scalefactor =
            DeltaEnergyToTime(Used_EnOrEdepMin[i], Used_EnOrEdepMax[i], TOFD) /
            TimeMeasurement_ns * (double)npulses[i];
        h1[i]->Scale(10.0 / BinWidth / scalefactor);
        h1[i]->GetYaxis()->SetTitle("counts/s/BinWidth");
        double CRDet =
            BinWidth / 1e6 *
            h1[i]->Integral(h1[i]->FindBin(0.15), h1[i]->FindBin(20));
        cout << "CR" << TitleLegend[i] << " threshold 0.15 MeV Det" << detN[i]
             << " " << CRDet << endl;

        TitleLegend[i] = TitleLegend[i] +
                         " CR=" + doubleToDecimalString(CRDet, 3) + " c/#mus";

      } else {
        h1[i]->Scale(1.0 / npulses[i]);
        DivideByBinTime(h1[i], TOFD);
        h1[i]->GetYaxis()->SetTitle("counts/#mus");
      }
    }

    else if (TypeOfPlot == "Yield") {
      h1[i]->Scale(7.e12 / nprotons[i]);
      DivideByNeutronFluence_2023(h1[i]);

      h1[i]->GetYaxis()->SetTitle("Yield* ");
    }

    else if (TypeOfPlot ==
             "CountsTXT") {  /// counts per 7e12 protons per bin width
      cout << " Do CountsTXT" << endl;
      h1[i]->Scale(7.e12 / nprotons[i]);
      DivideByBinWidth(h1[i]);

      int nbins = h1[i]->GetNbinsX();
      double val, err, ene1;
      for (int j = 1; j <= nbins; j++) {
        val = h1[i]->GetBinContent(j);
        err = h1[i]->GetBinError(j);
        ene1 = h1[i]->GetBinCenter(j);
        cout << ene1 << " " << val << endl;
      }

      h1[i]->GetYaxis()->SetTitle("counts/7#upoint10^{12} protons/BinWidth");

    } else {
      cout << " ########### Error in " << __FILE__ << ", line " << __LINE__
           << " ###########" << endl;
      exit(1);
    }
  }

  cout << "after counts" << endl;

  if (!CompareWithSimul) {
    hSimul = (TH1D*)h1[0]->Clone();
  }
  /// We Normalize
  if (IfNormalize && IfEnOrEdep) {
    Normalize(h1, NumberOfHisto, hSimul, NormalizeMinEn, NormalizeMaxEn);
  }
  if (IfNormalize && (!IfEnOrEdep)) {
    cout << "Beforenoramlize" << endl;

    Normalize(h1, n, hSimul, NormalizeMinEdep, NormalizeMaxEdep);

    cout << "StartGAinsShift" << endl;

    double shiftArray[NumberOfHisto];
    double shift;
    for (auto i = 0; i < NumberOfHisto; i++) {
      h1GainShift[i] = CalculateBestShift(shift, h1[0], h1[i], NormalizeMinEdep,
                                          NormalizeMaxEdep, NumberIterations,
                                          MinShift, MaxShift);
      shiftArray[i] = shift;
      cout << TitleLegend[i] << " Shift= " << shiftArray[i] << endl;
    }
  }
  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  TGaxis::SetMaxDigits(3);
  TPad* pad1;
  TPad* pad2 = new TPad("pad2", "Pad", 0.0, 0.02, 1., 0.33, 10);
  c1->ToggleEditor();

  if (CalculateRatio) {
    pad1 = new TPad("pad1", "Pad", 0.0, 0.33, 1., 1., 10);
    pad2 = new TPad("pad2", "Pad", 0.0, 0.02, 1., 0.33, 10);

    pad1->Draw();
    pad2->Draw();
    pad1->SetGridx();
    pad1->SetGridy();
    if (IfEnOrEdep) {
      pad1->SetLogy();
      pad1->SetLogx();
      pad2->SetLogx();
    }
    pad1->SetTopMargin(0.11);      // para que no se vea el eje
    pad1->SetBottomMargin(0.033);  // para que no se vea el eje
    // pad1_6->SetBottomMargin(0.2);//

    pad2->SetTopMargin(0.07);    // para que no se vea el eje
    pad2->SetBottomMargin(0.4);  // para que se vea el eje

    pad1->SetLeftMargin(0.15);  // para que no se vea el eje
    pad2->SetLeftMargin(0.15);  // para que no se vea el eje
    pad2->SetGridy();
    pad1->cd();
    // pad1->SetLogx();
    // pad2->SetLogx();
  }
  c1->SetTickx();
  c1->SetTicky();
  if (IfEnOrEdep) {
    c1->SetLogy();
    c1->SetLogx();
  }
  if (TypeOfPlot == "CR") {
    // c1->SetLogy(0);
    c1->SetGridx();
    c1->SetGridy();
  }
  if (TypeOfPlot == "CP") {
    c1->SetGridx();
    c1->SetGridy();
  }
  if (NameHisto == "TOF") {
    c1->SetLogy(0);
    c1->SetLogx(0);
    c1->SetGridx();
    c1->SetGridy();
  }
  TLegend* legend = new TLegend(0.6, 0.62, 0.96, 0.86, "");
  TH1D* h1Axis = (TH1D*)h1[0]->Clone();
  h1Axis->SetName("h1Axis");
  h1Axis->SetTitle(TitleHisto.c_str());
  if (IfEnOrEdep) {
    // h1Axis->GetXaxis()->SetRangeUser(ForPlotEmin, ForPlotEmax);
    h1Axis->SetAxisRange(ForPlotEminEn, ForPlotEmaxEn, "x");
  } else {
    h1Axis->SetAxisRange(ForPlotEminEdep, ForPlotEmaxEdep, "x");
  }
  h1Axis->Draw("histo E");

  for (auto i = 0; i < NumberOfHisto; i++) {
    h1[i]->Draw("histo E same");
    legend->AddEntry(h1[i], TitleLegend[i].c_str(), "l");
    if (IfEnOrEdep) {
      // h1Axis->GetXaxis()->SetRangeUser(ForPlotEmin, ForPlotEmax);
      h1[i]->SetAxisRange(ForPlotEminEn, ForPlotEmaxEn, "x");
    } else {
      h1[i]->SetAxisRange(ForPlotEminEdep, ForPlotEmaxEdep, "x");
    }
  }
  if (CompareWithSimul) {
    hSimul->Draw("histo E same");
    legend->AddEntry(hSimul, "Eval", "l");
  }
  legend->SetTextSize(0.06);
  legend->SetBorderSize(0);
  legend->SetFillColor(0);
  legend->SetFillStyle(0);
  legend->Draw();

  if (CalculateRatio) {
    h1Axis->GetXaxis()->SetTitleSize(0.090);
    h1Axis->GetXaxis()->SetLabelSize(0.090);
    h1Axis->GetXaxis()->SetLabelOffset(0.005);

    h1Axis->GetYaxis()->SetTitleSize(0.090);
    h1Axis->GetYaxis()->SetLabelSize(0.090);
    h1Axis->GetYaxis()->SetTitleOffset(0.85);

    pad2->cd();
    TH1D* h1Ratio[NumberOfHisto];

    for (auto i = 0; i < NumberOfHisto; i++) {
      h1Ratio[i] = (TH1D*)h1[i]->Clone();
      h1Ratio[i] = CalculateRatioHisto(h1[i], hSimul);
      h1Ratio[i]->SetTitle("");
      if (IfEnOrEdep) {
        h1Ratio[i]->SetAxisRange(ForPlotEminEn, ForPlotEmaxEn, "x");
        h1Ratio[i]->SetAxisRange(MinYForCompare, MaxYForCompare, "y");
      } else {
        h1Ratio[i]->SetAxisRange(ForPlotEminEdep, ForPlotEmaxEdep, "x");
        h1Ratio[i]->SetAxisRange(MinYForCompare, MaxYForCompare, "y");
      }
    }

    h1Ratio[0]->SetMarkerStyle(20);
    h1Ratio[0]->SetMarkerColor(2);
    h1Ratio[0]->SetMarkerSize(0.7);
    h1Ratio[0]->GetYaxis()->SetTitle("Ratio");

    h1Ratio[0]->GetYaxis()->CenterTitle();

    h1Ratio[0]->GetYaxis()->SetTitleOffset(0.35);
    h1Ratio[0]->GetYaxis()->SetTitleSize(0.14);
    h1Ratio[0]->GetYaxis()->SetLabelSize(0.2);
    h1Ratio[0]->GetXaxis()->SetTitleSize(0.18);
    h1Ratio[0]->GetYaxis()->SetTitleSize(0.18);

    h1Ratio[0]->GetXaxis()->SetLabelSize(0.2);
    h1Ratio[0]->GetYaxis()->SetNdivisions(5, 5, 0);
    h1Ratio[0]->GetXaxis()->SetNdivisions(9, 5, 0);
    h1Ratio[0]->GetXaxis()->SetLabelOffset(0.006);

    h1Ratio[0]->Draw("histo E");
    for (auto i = 1; i < NumberOfHisto; i++) {
      h1Ratio[i]->Draw("histo E same");
    }

    // h1Ratio[0]->GetYaxis()->SetRangeUser(-2.01, 2.01);
  }

  SaveRootEpsPngTxtFunction(c1, SaveName.c_str());
}
