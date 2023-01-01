#include "labelled_dtmc.hh"

using namespace org::mcss;

double LabelledDtmc::UpdateParams(const Eigen::VectorXd &new_initial,
                                  const Eigen::MatrixXd &new_transition) {
  auto norm_diff = 0.0;
  norm_diff += (new_initial - dtmc().initial_p()).norm();
  norm_diff += (new_transition - dtmc().transition_p()).norm();

  dtmc().initial_p(new_initial);
  dtmc().transition_p(new_transition);

  return norm_diff;
}

double LabelledDtmc::UpdateParams(const Eigen::MatrixXd &new_transition) {
  auto norm_diff = 0.0;
  norm_diff += (new_transition - dtmc().transition_p()).norm();

  dtmc().transition_p(new_transition);

  return norm_diff;
}

double LabelledDtmc::Maximization(const LabelTrace &observation) {
  auto T = observation.size();
  auto state_count = dtmc().state_count();

  //Eigen::VectorXd new_initial = gamma().rowwise().sum() / T;
  Eigen::MatrixXd new_transition =
      sigma_xi().array().colwise() / gamma().rowwise().sum().array();
  //auto norm_diff = UpdateParams(new_initial, new_transition);
  auto norm_diff = UpdateParams(new_transition);
  return norm_diff;
}
