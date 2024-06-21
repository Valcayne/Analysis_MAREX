#ifndef DETECTOR_H
#define DETECTOR_H

#include <TCanvas.h>

#include <map>
#include <string>
#include <vector>

class Detector {
 public:
  const std::string name;
  const std::vector<int> ids;
  const std::pair<double, double> neutron_energy_range;
  const std::pair<double, double> deposited_energy_range;

  Detector(const std::string name, const std::vector<int> ids,
           const std::pair<double, double> neutron_energy_range,
           const std::pair<double, double> deposited_energy_range);
  ~Detector();

  void AppendCounts(const int id, const double c);
  std::map<int, std::vector<double>> GetCounts();

  void AppendGraph(TCanvas *graph);
  std::vector<TCanvas *> GetGraphs();

 private:
  std::map<int, std::vector<double>> counts;

  std::vector<TCanvas *> graphs;
};
#endif
