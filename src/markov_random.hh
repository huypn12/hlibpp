#ifndef __MARKOV_RANDOM_H__
#define __MARKOV_RANDOM_H__

#include <Eigen/Eigen>

#include <random>
#include <vector>

namespace org::mcss {
class MarkovRandom {
private:
  int seed_;
  std::mt19937_64 generator_;

public:
  MarkovRandom();
  MarkovRandom(int seed);

  void reset();

  int ChooseUniform(const int &n_states);
  int ChooseDirichlet(const std::vector<double> &distribution);
  int ChooseDirichlet(const Eigen::VectorXd &distribution);

  double RandomProbUniform();

  Eigen::MatrixXd RandomStochasticMatrix(const int &row, const int &col);
  Eigen::VectorXd RandomStochasticVector(const int &dim);
};

} // namespace org::mcss

#endif // __MARKOV_RANDOM_H__
