#ifndef CPPLY_BLOCKING_QUEUE_H_
#define CPPLY_BLOCKING_QUEUE_H_

#include <stdint.h>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <utility>

namespace cpply {

template <typename T>
class BlockingQueue {
public:
  BlockingQueue() = default;

  BlockingQueue(const BlockingQueue&) = delete;

  size_t Size() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return que_.size();
  }

  bool Empty() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return que_.empty();
  }

  T Take() {
    std::unique_lock<std::mutex> lock(mutex_);
    // avoid spurious wakeup
    while (que_.empty()) {
      not_empty_.wait(lock); 
    }
    T t(std::move(que_.front()));
    que_.pop_front();
    return t;
  }

  void Put(const T& t) {
    std::unique_lock<std::mutex> lock(mutex_);
    que_.push_back(t);
    not_empty_.notify_all(); 
  }

  void Put(T&& t) {
    std::unique_lock<std::mutex> lock(mutex_);
    que_.push_back(std::move(t));
    not_empty_.notify_all(); 
  }

private:
  std::deque<T> que_;
  mutable std::mutex mutex_;
  std::condition_variable not_empty_;
};

}

#endif // CPPLY_BLOCKING_QUEUE_H_

