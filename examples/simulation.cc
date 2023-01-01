#include "hmm.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <unordered_map>

using namespace org::mcss;

void SaveTrace(const std::string &fpath, int alphabet_size, LabelTrace &trace) {
  std::ofstream ofs(fpath);
  if (ofs.good()) {
    ofs << alphabet_size << std::endl << trace.ToStr();
  }
}

void GenerateTrace(const std::string &fpath, const int &n_steps) {
  // Original model:
  // We don't need HMM here due to 1-1 map from state to labal
  int states_count = 4;
  auto init_p = Eigen::VectorXd(states_count);
  auto transition_p = Eigen::MatrixXd(states_count, states_count);
  init_p << 0.25, 0.25, 0.25, 0.25;
  transition_p <<
    0.1, 0.3, 0.6,   0,
    0.3, 0.1, 0.6,   0,
    0.2, 0.1,   0, 0.7,
      0,   0,   1,   0;

  auto orig_dtmc = std::make_shared<Dtmc>(states_count, init_p, transition_p);
  std::cout << "Original DTMC parameters: \n" << orig_dtmc->Str() << std::endl;

  // Map trace of state to label
  std::unordered_map<int, int> label;
  label.insert(std::pair<int, int>(0, 0));
  label.insert(std::pair<int, int>(1, 1));
  // state 3 and 4 has the same label
  label.insert(std::pair<int, int>(2, 2));
  label.insert(std::pair<int, int>(3, 2));
  // Make label trace
  LabelTrace rtrace;
  for (auto i = 0; i < n_steps; i++) {
    auto s = orig_dtmc->Next();
    rtrace.Append(label.at(s));
  }
  SaveTrace(fpath, 3, rtrace);
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cerr << "Usage: simulation <trace file> \n";
    return EXIT_FAILURE;
  }

  auto trace_file = std::string(argv[1]);
  GenerateTrace(trace_file, 1000);
  return EXIT_SUCCESS;
}
