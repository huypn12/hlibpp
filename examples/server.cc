#include "my_thread_pool.h"

int main() {
  Mylibpp::ThreadPool<std::function<void()>> pool(3);
  return EXIT_SUCCESS;
}