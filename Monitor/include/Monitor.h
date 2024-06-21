#ifndef MONITOR_H
#define MONITOR_H

#include <Pulse.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TROOT.h>

#include <fstream>
#include <iostream>

class Monitor {
 public:
  Monitor(const char *ini_filename);
  ~Monitor();

  void Start();

 private:
  std::string root_files_dir;
  std::string run_list_filename;
  std::string run_type;

  std::vector<int> GetRuns();

  std::vector<double> monitored_runs;
  std::vector<Pulse *> pulses;
  std::vector<int> silis;
  std::vector<int> c6d6s;

  std::string output_filename;
};
#endif
