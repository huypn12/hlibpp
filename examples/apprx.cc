#include <iterator>
#include <memory>
#include <string>
#include <unordered_map>

#include "hmm.h"
#include "labelled_dtmc.h"
#include "logger.h"
#include "trace_file.h"

using namespace org::mcss;

static Logger logger;

std::shared_ptr<Dtmc> CreateSampleDtmc(const std::vector<double> &params) {
  double lambda_1 = params[0];
  double lambda_2 = params[1];
  double mu_1 = params[2];
  double mu_2 = params[3];
  Eigen::VectorXd init_p(3);
  init_p << 1.0, 0, 0;
  Eigen::MatrixXd trans_p(3, 3);
  trans_p << 1 - (lambda_1 + lambda_2), lambda_1, lambda_2, mu_1, 1 - mu_1, 0,
      mu_2, 0, 1 - mu_2;
  return std::make_shared<Dtmc>(3, init_p, trans_p);
}

LabelTrace Simulate(std::shared_ptr<Dtmc> model) {
  LabelTrace trace;
  for (int i = 0; i < 100; i++) {
    trace.Append(model->Next());
  }
  return LabelTrace(trace);
}

LabelTrace MapLabels(const LabelTrace& trace) {
  // Map trace of state to label
  std::unordered_map<int, int> label;
  label.insert(std::pair<int, int>(0, 0));
  // state 1 and 2 has the same label
  label.insert(std::pair<int, int>(1, 1));
  label.insert(std::pair<int, int>(2, 1));
  // Make label trace
  LabelTrace new_trace;
  for (auto i = 0; i < trace.size(); i++) {
    new_trace.Append(label.at(trace[i]));
  }
  return new_trace;
}

std::shared_ptr<Hmm> FitTargetModel(const LabelTrace &trace) {
  int state_count = 2;
  int alphabet_count = 2;
  /* auto test_model = std::make_shared<Hmm>(state_count, alphabet_count); */
  auto test_model = std::make_shared<LabelledDtmc>(state_count, alphabet_count);
  test_model->InitRandom();
  Eigen::VectorXd init(2);
  init << 1, 0;
  Eigen::MatrixXd label(2, 2);
  label << 1, 0, 0, 1;
  test_model->initial_p(init);
  test_model->emission_p(label);
  test_model->Fit(trace, 1e-4);
  return test_model;
}

void Experiment(const std::vector<double> &params) {
  auto dtmc = CreateSampleDtmc(params);
  logger.LogInfo("Original DTMC: " + dtmc->Str());
  logger.LogInfo("Label: [0 1 1]");
  auto trace = Simulate(dtmc);
  auto label_trace = MapLabels(trace);
  logger.LogInfo("Trace to fit: " + label_trace.ToStr());
  auto test_model = FitTargetModel(label_trace);
  logger.LogInfo("HMM after fitting \n" + test_model->Str());
}

int main(int argc, char *argv[]) {
  auto log_file = "apprx.log";
  logger.SetLogFile(log_file);
  logger.LogInfo("Start logging...");

  std::vector<double> params;
  if (argc == 1) {
    params = std::vector<double>({0.4, 0.5, 0.7, 0.8});
  } else {
    for (int i = 0; i < argc; i++) {
      try {
        params.push_back(std::stod(argv[i]));
      } catch (const std::exception &ex) {
        std::stringstream ss;
        ss << "Exception: " << ex.what() << " at " << argv[i];
        logger.LogError(ss.str());
        return EXIT_FAILURE;
      }
    }
  }

  Experiment(params);

  return EXIT_SUCCESS;
}
