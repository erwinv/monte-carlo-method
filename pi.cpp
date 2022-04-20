#include <future>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include "task_system.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  int totalIterations = 100'000'000;
  int chunkSize = 100'000;

  if (argc >= 2)
  {
    totalIterations = atoi(argv[1]);
  }
  if (argc >= 3)
  {
    chunkSize = atoi(argv[2]);
    if (chunkSize > totalIterations)
    {
      cerr << "Chunk size[" << chunkSize << "] > "
           << "iterations=[" << totalIterations << "]"
           << endl;
      return 1;
    }
  }

  using CirclePointsFuture = future<int>;

  vector<CirclePointsFuture> circlePointsFutureVec;
  vector<int> totalPointsVec;

  for (auto remainingIterations = totalIterations; remainingIterations > 0; remainingIterations -= chunkSize)
  {
    auto thisBatchTotalPoints = std::min(remainingIterations, chunkSize);

    circlePointsFutureVec.emplace_back(tasksystem::async(
        [totalPoints = thisBatchTotalPoints]()
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
        }));
    totalPointsVec.emplace_back(thisBatchTotalPoints);
  }

  int circlePoints = 0;
  int totalPoints = 0;

  // parallelizing this doesn't yield any performance gains
  // so this remains a sequential blocking read of every future
  // and executing on the main thread
  // mutex mut;
  for (auto i = 0; i < circlePointsFutureVec.size(); i++)
  {
    auto &circlePointsFut = circlePointsFutureVec.at(i);
    auto total = totalPointsVec.at(i);

    double piEst;
    auto circle = circlePointsFut.get();
    {
      // scoped_lock lk(mut);
      circlePoints += circle;
      totalPoints += total;
      piEst = (4.0 * circlePoints) / totalPoints;
    }
    cout << "[PARTIAL] pi ~= " << piEst << endl;
  }

  auto piEst = (4.0 * circlePoints) / totalPoints;
  cout << "Total: " << totalPoints << endl
       << "Circle: " << circlePoints << endl
       << "[FINAL] pi ~= " << piEst << endl;

  return 0;
}
