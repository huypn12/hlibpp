#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <unordered_map>

#include "hmm.h"
#include "logger.h"

#define PANIC -1
#define OK 0

using namespace org::mcss;

static Logger logger;

int ReadTraceFile(const std::string &fpath, int &alphabet_count,
                  std::string &trace_str) {
  auto ifs = std::ifstream(fpath);
  if (ifs.is_open()) {
    auto line = std::string("");
    int i = 0;
    while (std::getline(ifs, line)) {
      if (i == 0) {
        alphabet_count = std::stoi(line);
      } else if (i == 1) {
        trace_str = line;
      } else {
        break;
      }
      i++;
    }
    return OK;
  } else {
    return PANIC;
  }
}

int FitKStateModel(const int &state_count, const std::string &trace_file) {
  int alphabet_count = 0;
  auto trace_str = std::string("");
  if (ReadTraceFile(trace_file, alphabet_count, trace_str) != OK) {
    logger.LogError("Panic: File error " + trace_file);
    return PANIC;
  }
  logger.LogInfo("+++ Hidden states: " + std::to_string(state_count));
  logger.LogInfo("Using data file to train: " + trace_file);
  logger.LogInfo("Label count: " + std::to_string(alphabet_count));
  auto trace = LabelTrace(trace_str);
  logger.LogInfo("Trace: " + trace.ToStr().substr(0, 10) + "... of size " +
                 std::to_string(trace.size()));

  auto test_model = std::make_unique<Hmm>(state_count, alphabet_count);
  test_model->InitRandom();
  logger.LogInfo("Test model before fitting \n" + test_model->Str());
  test_model->Fit(trace, 1000, 1e-4);
  logger.LogInfo("Test model after fitting \n" + test_model->Str());
  logger.LogInfo("Loglikelihood: " +
                 std::to_string(test_model->log_likelihood()));
  logger.LogInfo("AIC: " + std::to_string(test_model->aic()));

  return OK;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: reduction <trace file> <state counts>[] \n";
    return EXIT_FAILURE;
  }

  auto log_file = "reduction.log";
  logger.SetLogFile(log_file);
  logger.LogInfo("Start logging...");
  auto trace_file = std::string(argv[1]);
  for (int i = 2; i < argc; i++) {
    auto state_count = std::stoi(argv[i]);
    FitKStateModel(state_count, trace_file);
  }

  return EXIT_SUCCESS;
}
