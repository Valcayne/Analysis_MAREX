

#include "../GeneralFun/GeneralFunctions.hh"

// Program used to merge several Monitoring files

std::ifstream::pos_type GetFileSize(const char* filename);

int main(int argc, char** argv) {
  if (argc != 2) {
    cout << " ******* Input has to be like: Merge01 [RunType]  ***************"
         << endl;
    return 1;
  }

  //========================================================================
  char DirName[200] =
      "/eos/home-v/valcayne/nTOFDataProcessing/2024_Er_Cu_U/2DHistos/"
      "v03GainCorrected";
  char RunListFname[200] =
      "/afs/cern.ch/work/v/valcayne/2024_Er_U_ProgramsC6D6/Analysis_nTOF_v01/"
      "RunLists/RunList2024_02.txt";
  char StartFname[100] = "Histos01";
  //========================================================================

  string RunType = string(argv[1]);
  // char* outfnamebase = argv[2];
  char outfn_root[400];
  char outfn_txt[400];
  sprintf(outfn_root, "%s/%s_%s.root", DirName, StartFname, RunType.c_str());
  sprintf(outfn_txt, "%s/%s.txt", DirName, RunType.c_str());

  int RunList[10000];
  bool DoRun[10000];
  int nRuns = TakeRunList(RunListFname, RunType, RunList);

  char fname[400];
  bool txtdone = true;
  int NFilesToMerge = 0;
  for (int i = 0; i < nRuns; i++) {
    sprintf(fname, "%s/%s_%d.root", DirName, StartFname, RunList[i]);
    if (GetFileSize(fname) < 1000) {
      cout << " ########### WARNING: file " << fname
           << " does not exist ################" << endl;
      DoRun[i] = false;
    } else {
      DoRun[i] = true;
      NFilesToMerge++;
      if (!txtdone) {
        char tmpcpmmand[1000];
        sprintf(tmpcpmmand, "cp %s/%s_%d.txt %s", DirName, StartFname,
                RunList[i], outfn_txt);
        int check = system(tmpcpmmand);
      }
    }
  }
  cout << NFilesToMerge << " files to be merged" << endl;

  string command = "hadd -f " + string(outfn_root);
  for (int i = 0; i < nRuns; i++) {
    sprintf(fname, " %s/%s_%d.root", DirName, StartFname, RunList[i]);
    if (DoRun[i]) {
      command += string(fname);
    }
  }
  cout << " Command is: " << command << endl;

  int check = system(command.c_str());
  cout << " check = " << check << endl;

  return 0;
}

std::ifstream::pos_type GetFileSize(const char* filename) {
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  if (!in.good()) {
    return 0;
  }
  return in.tellg();
}
