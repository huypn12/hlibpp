#ifndef __LABELLED_DTMC_H__
#define __LABELLED_DTMC_H__

#include "hmm.h"

namespace org::mcss {
class LabelledDtmc : public Hmm {
 protected:
  double UpdateParams(const Eigen::VectorXd &, const Eigen::MatrixXd &);
  double UpdateParams(const Eigen::MatrixXd &);
  double Maximization(const LabelTrace &observation);

 public:
  LabelledDtmc(int state_count, int alphabet_count)
      : Hmm(state_count, alphabet_count) {}
};
}  // namespace org::mcss

#endif  // !__LABELLED_DTMC_H__