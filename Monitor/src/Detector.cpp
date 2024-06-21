#include <Detector.h>

Detector::Detector(const std::string name, const std::vector<int> ids,
                   const std::pair<double, double> neutron_energy_range,
                   const std::pair<double, double> deposited_energy_range)
    : name(name),
      ids(ids),
      neutron_energy_range(neutron_energy_range),
      deposited_energy_range(deposited_energy_range) {
  for (const auto id : ids) counts.insert({id, {}});
}

Detector::~Detector() {}

void Detector::AppendCounts(const int id, const double c) {
  counts.at(id).push_back(c);
}

std::map<int, std::vector<double>> Detector::GetCounts() { return counts; }

void Detector::AppendGraph(TGraphErrors* graph) { graphs.push_back(graph); }

std::vector<TGraphErrors*> Detector::GetGraphs() { return graphs; }
