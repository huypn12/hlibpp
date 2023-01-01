#ifndef __DTMC_H__
#define __DTMC_H__

#include "markov.h"
#include "markov_random.h"
#include <string>

namespace org::mcss {
class Dtmc : public Markov {
private:
  MarkovRandom rand_;

  int state_count_;
  int current_state_ = kBeginState;
  int previous_state_ = kBeginState;

  Eigen::VectorXd initial_p_;
  Eigen::MatrixXd transition_p_;

protected:
  int Jump();

public:
  Dtmc(int state_count);
  Dtmc(int state_count, const Eigen::VectorXd &initial_p,
       const Eigen::MatrixXd &transition_p);
  std::string Str();
  void InitRandom();

  // markov trace stream
  int Next() override;

  const int &state_count() { return state_count_; }
  void state_count(const int &c) { state_count_ = c; }
  const Eigen::VectorXd &initial_p() { return initial_p_; }
  void initial_p(const Eigen::VectorXd &v) { initial_p_ = v; }
  const Eigen::MatrixXd &transition_p() { return transition_p_; };
  void transition_p(const Eigen::VectorXd &m) { transition_p_ = m; }
  const int &current_state() override { return current_state_; };
  const int &previous_state() override { return previous_state_; };
};

} // namespace org::mcss

#endif // __DTMC_H__
