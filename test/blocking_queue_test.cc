#include <atomic>
#include <functional>
#include <gtest/gtest.h>
#include <string>
#include <thread>
#include <utility>
#include "../src/blocking_queue.h"

TEST(BlockingQueue, TestSize) {
  cpply::BlockingQueue<int> queue;
  ASSERT_EQ(0, queue.Size());
  queue.Put(1);
  ASSERT_EQ(1, queue.Size());
  queue.Put(2);
  queue.Put(2);
  ASSERT_EQ(3, queue.Size());
  queue.Take(); 
  ASSERT_EQ(2, queue.Size());
  queue.Take(); 
  queue.Take(); 
  ASSERT_EQ(0, queue.Size());
}

TEST(BlockingQueue, TestEmpty) {
  cpply::BlockingQueue<int> queue;
  ASSERT_EQ(true, queue.Empty());
  queue.Put(1);
  queue.Put(1);
  ASSERT_EQ(false, queue.Empty());
  queue.Take(); 
  ASSERT_EQ(false, queue.Empty());
  queue.Take(); 
  ASSERT_EQ(true, queue.Empty());
}

TEST(BlockingQueue, TestPut) {
  cpply::BlockingQueue<std::string> queue;
  const std::string base("Hello world");
  const std::string empty;
  std::string s1(base);
  queue.Put(s1);
  ASSERT_EQ(base, s1);
  std::string s2(base); 
  queue.Put(std::move(s2));
  ASSERT_EQ(empty, s2);
}

TEST(BlockingQueue, TestTake) {
  cpply::BlockingQueue<int> queue;
  queue.Put(1);
  ASSERT_EQ(1, queue.Take());
  queue.Put(2);
  queue.Put(2);
  ASSERT_EQ(2, queue.Take());
  ASSERT_EQ(2, queue.Take());
  queue.Put(3);
  queue.Put(3);
  queue.Put(3);
  ASSERT_EQ(3, queue.Take());
  ASSERT_EQ(3, queue.Take());
  ASSERT_EQ(3, queue.Take());
}

// require : count % w == 0 && count % r == 0
void RWProcess(int w, int r, int count) {
  cpply::BlockingQueue<int> queue;
  std::atomic<long long> write_sum(0);
  std::atomic<long long> read_sum(0);
  std::thread writer[w];
  std::thread reader[r];
  for (int i = 0; i < w; ++i) {
    writer[i] = std::thread([&,i] {
          int l = count/w * (i);
          int r = count/w * (i+1);
          for (int j = l; j < r; ++j) {
            queue.Put(j);
            write_sum.fetch_add(j);
          }
        });
  }
  for (int i = 0; i < r; ++i) {
    reader[i] = std::thread([&] {
          int cnt = count / r;
          while (cnt-- > 0) {
            int j = queue.Take();
            read_sum.fetch_add(j);
          } 
        });
  }
  for (size_t i = 0; i < w; ++i) {
    writer[i].join();
  }
  for (size_t i = 0; i < r; ++i) {
    reader[i].join();
  }
  ASSERT_EQ(write_sum, read_sum);
}

TEST(BlockingQueue, SingleWriterSingleReader) {
  RWProcess(1, 1, 5000);
  RWProcess(1, 1, 1000000);
}

TEST(BlockingQueue, SingleWriterMulitReader) {
  RWProcess(1, 10, 5000);
  RWProcess(1, 10, 1000000);
  RWProcess(1, 5, 5000);
  RWProcess(1, 5, 1000000);
}

TEST(BlockingQueue, MulitWriterSingleReader) {
  RWProcess(10, 1, 5000);
  RWProcess(10, 1, 1000000);
  RWProcess(5, 1, 5000);
  RWProcess(5, 1, 1000000);
}

TEST(BlockingQueue, MulitWriterMulitReader) {
  RWProcess(10, 10, 5000);
  RWProcess(10, 10, 1000000);
  RWProcess(5, 5, 5000);
  RWProcess(5, 5, 1000000);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}   
