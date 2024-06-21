#ifndef PULSE_H
#define PULSE_H

#include <Detector.h>
#include <TAxis.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TMath.h>

#include <memory>
#include <string>
#include <vector>

class Pulse {
 public:
  const int type;
  const std::string name;
  std::vector<Detector> detectors;

  static std::vector<double> runs;

  Pulse(const int type, const std::string name,
        const std::vector<Detector> detectors);
  ~Pulse();

  void AppendNumbers(const double n);
  void AppendProtons(const double p);
  void AppendPkupAreas(const double pa);

  void CreateGraphs();
  void WriteGraphsToFile(const std::string filename);

 private:
  std::vector<double> numbers;
  std::vector<double> protons;
  std::vector<double> pkup_areas;

  std::vector<TCanvas *> graphs;

  TCanvas *CreateGraph(const std::string name, const std::vector<double> ratios,
                       const std::vector<double> uncertainties,
                       const std::string ratios_title);
};
#endif
