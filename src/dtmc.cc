#include "dtmc.hh"

#include <string>
#include <sstream>

using namespace org::mcss;

Dtmc::Dtmc(int state_count, const Eigen::VectorXd &initial_p,
           const Eigen::MatrixXd &transition_p)
    : state_count_(state_count), initial_p_{}, transition_p_{} {
  initial_p_ = initial_p;
  transition_p_ = transition_p;
}

Dtmc::Dtmc(int state_count)
    : state_count_(state_count), initial_p_{}, transition_p_{} {
  initial_p_ = Eigen::VectorXd::Zero(state_count);
  transition_p_ = Eigen::MatrixXd::Zero(state_count, state_count);
}

void Dtmc::InitRandom() {
  initial_p_ = rand_.RandomStochasticVector(state_count_);
  transition_p_ = rand_.RandomStochasticMatrix(state_count_, state_count_);
}

int Dtmc::Jump() {
  if (current_state_ == kBeginState) {
    return rand_.ChooseDirichlet(initial_p_);
  }
  auto pij = transition_p_.row(current_state_);
  return rand_.ChooseDirichlet(pij);
}

int Dtmc::Next() {
  auto next_state = Jump();
  previous_state_ = current_state_;
  current_state_ = next_state;
  return current_state_;
}

std::string Dtmc::Str() {
  std::stringstream ss;
  ss << "Initial distribution: \n"
     << initial_p_ << std::endl
     << "Transition matrix: \n"
     << transition_p_ << std::endl;
  return ss.str();
}
