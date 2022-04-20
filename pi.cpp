#include <algorithm>
#include <execution>
#include <future>
#include <iostream>
#include <limits>
#include <mutex>
#include <random>
#include <string>
#include <tuple>
#include <vector>
#include "task_system.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  int numIterations = 100'000'000;
  int chunkSize = 100'000;

  if (argc >= 2)
  {
    numIterations = atoi(argv[1]);
  }
  if (argc >= 3)
  {
    chunkSize = atoi(argv[2]);
    if (chunkSize > numIterations)
    {
      cerr << "Chunk size[" << chunkSize << "] > "
        << "iterations=[" << numIterations << "]"
        << endl;
      return 1;
    }
  }

  vector<tuple<future<int>, int>> pairs;

  while (numIterations > 0)
  {
    if (numIterations < chunkSize)
    {
      chunkSize = numIterations;
    }
    numIterations -= chunkSize;
    auto fut = tasksystem::async(
        [](int totalPoints)
        {
          thread_local mt19937 gen(random_device{}());
          uniform_real_distribution<> dist(0.0, 1.0);

          int circlePoints = 0;
          for (auto i = 0; i < totalPoints; i++)
          {
            auto x = dist(gen);
            auto y = dist(gen);
            if ((x * x + y * y) < (1.0 + numeric_limits<double>::epsilon()))
            {
              circlePoints++;
            }
          }

          return circlePoints;
        },
        chunkSize);
    pairs.emplace_back(move(fut), chunkSize);
  }

  int circlePoints = 0;
  int totalPoints = 0;

  mutex mut;
  for_each(execution::par, begin(pairs), end(pairs),
           [&](auto &pair)
           {
             double piEst;
             auto &fut = get<0>(pair);
             auto total = get<1>(pair);
             auto circle = fut.get();
             {
               scoped_lock lk(mut);
               circlePoints += circle;
               totalPoints += total;
               piEst = (4.0 * circlePoints) / totalPoints;
             }
             cout << "[PARTIAL] pi ~= " << piEst << endl;
           });

  auto piEst = (4.0 * circlePoints) / totalPoints;
  cout << "Total: " << totalPoints << endl
    << "Circle: " << circlePoints << endl
    << "[FINAL] pi ~= " << piEst << endl;

  return 0;
}
