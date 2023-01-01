#include "hmm.hh"

#include <cmath>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

using namespace org::mcss;

Hmm::Hmm(const int &states_size, const int &alphabet_count)
    : dtmc_(states_size), alphabet_count_(alphabet_count), emission_p_{} {
  emission_p_ = Eigen::MatrixXd::Zero(states_size, alphabet_count);
}

Hmm::Hmm(const int &state_count, const int &alphabet_count,
         const Eigen::VectorXd &initial_p, const Eigen::MatrixXd &transition_p,
         const Eigen::MatrixXd &emission_p)
    : dtmc_(state_count, initial_p, transition_p),
      alphabet_count_(alphabet_count), emission_p_{} {
  emission_p_ = emission_p;
}

std::string Hmm::Str() {
  std::stringstream ss;
  ss << dtmc_.Str() << std::endl;
  ss << "Observation cardinality: " << alphabet_count_ << std::endl;
  ss << "Emission probabilities: \n" << emission_p_ << std::endl;
  return ss.str();
}

// Simulate trace
int Hmm::Next() {
  int state = dtmc_.Next();
  int observation = rand_.ChooseDirichlet(emission_p_.row(state));
  return observation;
}

void Hmm::Forward(const LabelTrace &observation) {
  auto T = observation.size();
  auto state_count = dtmc_.state_count();
  alpha_.resize(state_count, T);
  alpha_.setZero();
  // basis step
  alpha_.col(0) =
      dtmc_.initial_p().cwiseProduct(emission_p_.col(observation[0]));
  alpha_.col(0) = alpha_.col(0) / alpha_.col(0).sum();
  // inductive step
  for (int t = 1; t < T; t++) {
    alpha_.col(t) = alpha_.col(t - 1).transpose() * dtmc_.transition_p();
    alpha_.col(t) = alpha_.col(t).cwiseProduct(emission_p_.col(observation[t]));
    alpha_.col(t) = alpha_.col(t) / alpha_.col(t).sum();
  }
}

void Hmm::Backward(const LabelTrace &observation) {
  auto T = observation.size();
  auto state_count = dtmc_.state_count();
  beta_.resize(state_count, T);
  beta_.setZero();
  // basis step
  beta_.col(T - 1).setConstant(1 / emission_p_.col(observation[T - 1]).sum());
  // inductive step
  for (int t = T - 2; t >= 0; t--) {
    beta_.col(t) =
        beta_.col(t + 1).cwiseProduct(emission_p_.col(observation[t + 1]));
    beta_.col(t) = (dtmc_.transition_p() * beta_.col(t)).transpose();
    auto sum = beta_.col(t).cwiseProduct(emission_p_.col(observation[t])).sum();
    beta_.col(t) = beta_.col(t) / sum;
  }
}

const Eigen::MatrixXd &Hmm::Posterior(const LabelTrace &observation) {
  auto T = observation.size();
  auto state_count = dtmc_.state_count();
  gamma_ = Eigen::MatrixXd(state_count, T);
  Forward(observation);
  Backward(observation);
  for (int t = 0; t < T; t++) {
    gamma_.col(t) = alpha_.col(t).cwiseProduct(beta_.col(t));
    gamma_.col(t) = gamma_.col(t) / gamma_.col(t).sum();
  }
  return gamma_;
}

void Hmm::InitRandom() {
  dtmc_.InitRandom();
  emission_p_ =
      rand_.RandomStochasticMatrix(dtmc_.state_count(), alphabet_count_);
}

void Hmm::Expectation(const LabelTrace &observation) {
  auto T = observation.size();
  auto state_count = dtmc_.state_count();

  Posterior(observation);
  sigma_xi_.resize(state_count, state_count);
  sigma_xi_.setZero();
  for (int t = 0; t < T - 1; t++) {
    auto xi = Eigen::MatrixXd(dtmc_.transition_p());
    xi = xi.array().colwise() * alpha_.col(t).array();
    xi = xi.array().colwise() * emission_p_.col(observation[t + 1]).array();
    xi = xi.array().rowwise() * beta_.col(t + 1).transpose().array();
    xi = xi / xi.sum();
    sigma_xi_ += xi;
  }
}

double Hmm::UpdateParams(const Eigen::VectorXd &new_initial,
                         const Eigen::MatrixXd &new_transition,
                         const Eigen::MatrixXd &new_emission) {
  auto norm_diff = 0.0;
  norm_diff += (new_initial - dtmc_.initial_p()).norm();
  norm_diff += (new_transition - dtmc_.transition_p()).norm();
  norm_diff += (new_emission - emission_p_).norm();

  dtmc_.initial_p(new_initial);
  dtmc_.transition_p(new_transition);
  emission_p_ = new_emission;

  return norm_diff;
}

void Hmm::Evaluate(const LabelTrace &observation)
{
  auto T = observation.size();
  auto state_count = dtmc_.state_count();
  alpha_.resize(state_count, T);
  alpha_.setZero();
  // basis step
  alpha_.col(0) =
      dtmc_.initial_p().cwiseProduct(emission_p_.col(observation[0]));
  alpha_.col(0) = alpha_.col(0) / alpha_.col(0).sum();
  // inductive step
  for (int t = 1; t < T; t++) {
    alpha_.col(t) = alpha_.col(t - 1).transpose() * dtmc_.transition_p();
    alpha_.col(t) = alpha_.col(t).cwiseProduct(emission_p_.col(observation[t]));
    if (t == T - 1) {
      break;
    }
    alpha_.col(t) = alpha_.col(t) / alpha_.col(t).sum();
  }
  log_likelihood_ = log(alpha_.col(T - 1).sum());
  auto param_count =
      state_count * state_count + state_count * alphabet_count_ + state_count;
  aic_ = -2 * log_likelihood_ + 2 * param_count;
}

double Hmm::Maximization(const LabelTrace &observation) {
  auto T = observation.size();
  auto state_count = dtmc_.state_count();

  Eigen::VectorXd new_initial = gamma_.rowwise().sum() / T;
  Eigen::MatrixXd new_transition =
      sigma_xi_.array().colwise() / gamma_.rowwise().sum().array();
  Eigen::MatrixXd new_emission =
      Eigen::MatrixXd::Zero(state_count, alphabet_count_);
  for (int t = 0; t < T; t++) {
    auto o = observation[t];
    new_emission.col(o) += gamma_.col(t);
  }
  new_emission =
      new_emission.array().colwise() / gamma_.rowwise().sum().array();

  auto norm_diff = UpdateParams(new_initial, new_transition, new_emission);
  return norm_diff;
}

void Hmm::Fit(const LabelTrace &observation, const int &max_iters,
              const double &eps) {
  auto T = observation.size();
  for (int i = 0; i < max_iters; last_iter_ = ++i) {
    Expectation(observation);
    auto norm_diff = Maximization(observation);
    Evaluate(observation);
    if (norm_diff <= eps) {
      break;
    }
  }
}

// Observation explanation: viterbi
std::vector<int> Decode(const std::vector<int> &observation) {
  // TODO: implement viterbi algorithm
  return observation;
}
