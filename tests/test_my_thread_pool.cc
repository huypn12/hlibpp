#include "my_thread_pool.h"

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>

namespace {

class TestMyThreadPool : public testing::Test {
protected:
  std::unique_ptr<Mylibpp::ThreadPool> pool_;
  std::atomic<int> count_;
  void SetUp() override {
    pool_ = std::make_unique<Mylibpp::ThreadPool>();
    count_ = 0;
  }
  void TearDown() override { count_ = 0; }
};

TEST_F(TestMyThreadPool, TestSubmitTaskArgumentPassing) {
  auto task_count = 10;
  auto sum_1 = 0, sum_2 = 0;
  std::function<int(int)> foo = [](const int &i) { return 100 * i; };
  for (int i = 0; i < task_count; i++) {
    sum_1 += foo(i);
    auto future = pool_->SubmitTask(foo, i);
    sum_2 += future.get();
  }
  EXPECT_EQ(sum_1, sum_2);
}

TEST_F(TestMyThreadPool, TestSubmitTaskArgumentPassingWithLambda) {
  auto task_count = 10;
  auto sum_1 = 0, sum_2 = 0;
  for (int i = 0; i < task_count; i++) {
    sum_1 += 100 * i;
    auto future = pool_->SubmitTask([](const int &i) { return 100 * i; }, i);
    sum_2 += future.get();
  }
  EXPECT_EQ(sum_1, sum_2);
}

TEST_F(TestMyThreadPool, TestSubmitTaskCaptureReference) {
  auto task_count = 10;
  auto sum_1 = 0, sum_2 = 0;
  std::function<void(int &)> foo = [&sum_1](int &sum1) { sum_1 += 1; };
  for (int i = 0; i < task_count; i++) {
    sum_2 += 1;
    pool_->SubmitTask(foo, sum_1);
  }
  std::this_thread::sleep_for(
      std::chrono::milliseconds(50)); // TODO: replace by SyncThreads() barrier
  EXPECT_EQ(sum_1, sum_2);
}

} // namespace