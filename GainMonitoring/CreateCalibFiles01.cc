
#include "../GeneralFun/CalibrationManager.hh"
#include "GainMonitoring01.hh"

/*
Programa que crea ficheros de calibración, teniendo en cuenta los cambios de
ganancia

*/

void ChangeRefRun(TGraph* gr1, int newRefRun);

int main(int argc, char** argv) {
  if (argc != 8 && argc != 9 && argc != 10) {
    cout
        << " ########## Input has to be like: CreateCalibFiles01 [InpCalibDir] "
           "[OutCalibDir] [GainShiftsDir] [DetName] [DetID] [RunDescriptor] "
           "[RefCalibRunNumber] ([RunMin=0]) ([RunMax=1.e20]) ##########"
        << endl;
    return 1;
  }

  char* InpCalibDir = argv[1];
  char* OutCalibDir = argv[2];
  char* GainShiftsDir = argv[3];
  char* DetName = argv[4];
  int DetID = std::atoi(argv[5]);
  char* RunDescriptor = argv[6];
  if (string(argv[6]) == "0") {
    RunDescriptor = 0;
  }
  int RefCalibRunNumber = std::atoi(argv[7]);
  double RunMin = 0, RunMax = 1.e20;
  if (argc > 8) {
    RunMin = std::atoi(argv[8]);
  }
  if (argc > 9) {
    RunMax = std::atoi(argv[9]);
  }

  //------------------------------------------------------------------
  // We get the gain shifts:
  int RefRun = 0;
  TGraph* gr1 =
      GetGainMonitoring(DetName, DetID, GainShiftsDir, RefRun, RunDescriptor);
  double MinRun_GS = 0, MaxRun_GS = 0, GS_min = 0, GS_max;
  gr1->GetPoint(0, MinRun_GS, GS_min);
  gr1->GetPoint(gr1->GetN() - 1, MaxRun_GS, GS_max);
  ChangeRefRun(gr1, RefCalibRunNumber);
  //------------------------------------------------------------------

  //------------------------------------------------------------------
  // We read the calibration file of the reference run:
  CalibrationManager* theRefRunCal =
      new CalibrationManager(InpCalibDir, RefCalibRunNumber);
  double p0_ref = 0, p1_ref = 0, p2_ref = 0, p3_ref = 0, p4_ref = 0, p5_ref = 0,
         p6_ref = 0;
  theRefRunCal->GetEneCalibParams(DetName, DetID, p0_ref, p1_ref, p2_ref,
                                  p3_ref, p4_ref, p5_ref, p6_ref);
  //------------------------------------------------------------------

  //------------------------------------------------------------------
  // Here we go:
  char command[2000];
  sprintf(command, "ls %s/Calib_*.dat > borrar.txt", InpCalibDir);
  int check = system(command);
  if (check != 0) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }
  char inCalibFname[500], outCalibFname[500];
  int thisRun = 0;
  ifstream in1("borrar.txt");
  char sscanfstring[200];
  sprintf(sscanfstring, "%s/Calib_%%d.dat", InpCalibDir);
  while (in1 >> inCalibFname) {
    sscanf(inCalibFname, sscanfstring, &thisRun);
    if (thisRun >= RunMin && thisRun <= RunMax) {
      CalibrationManager* theCalMan =
          new CalibrationManager(InpCalibDir, thisRun, 0);
      double GainShift = -1;
      if (thisRun < MinRun_GS) {
        GainShift = GS_min;
        cout << " Gain shift for run " << thisRun << " is " << GainShift
             << " take min" << endl;

      } else if (thisRun > MaxRun_GS) {
        GainShift = GS_max;
        cout << " Gain shift for run " << thisRun << " is " << GainShift
             << " take max" << endl;
      } else if (ContainsXValue(gr1, thisRun)) {
        cout << " Gain shift for run " << thisRun << " is " << GainShift
             << endl;

      } else {
        GainShift = gr1->Eval(thisRun);
        cout << " Gain shift for run " << thisRun << " is " << GainShift
             << " interpolate " << endl;
      }
      double p0 = p0_ref;
      double p1 = p1_ref / GainShift;
      double p2 = p2_ref / GainShift / GainShift;
      double p3 = p3_ref * GainShift;  // In order to keep having a continuos
                                       // and derivable function
      double p4 = p4_ref;
      double p5 = p5_ref / GainShift;
      double p6 = p6_ref / GainShift / GainShift;
      theCalMan->SetEneCalibParams(DetName, DetID, p0, p1, p2, p3, p4, p5, p6);
      theCalMan->WriteCalibFile(OutCalibDir, thisRun);
      delete theCalMan;
    }
  }
  in1.close();
  sprintf(command, "rm borrar.txt");
  check = system(command);
  if (check != 0) {
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }
  //------------------------------------------------------------------

  delete theRefRunCal;

  return 0;
}

void ChangeRefRun(TGraph* gr1, int newRefRun) {
  int np = gr1->GetN();

  double MinRun_GS = 0, MaxRun_GS = 0, GS_min = 0, GS_max;
  gr1->GetPoint(0, MinRun_GS, GS_min);
  gr1->GetPoint(np - 1, MaxRun_GS, GS_max);
  if (newRefRun < MinRun_GS || newRefRun > MaxRun_GS) {
    cout << "MinRun_GS " << MinRun_GS << " newRefRun " << newRefRun
         << " MaxRun_GS " << MaxRun_GS << endl;
    cout << " ############# ERROR in " << __FILE__ << ", line " << __LINE__
         << " #############" << endl;
    exit(1);
  }

  double xval, yval;

  double ScaleFac = 1. / gr1->Eval(newRefRun);
  for (int i = 0; i < np; i++) {
    gr1->GetPoint(i, xval, yval);
    gr1->SetPoint(i, xval, yval * ScaleFac);
  }
}
