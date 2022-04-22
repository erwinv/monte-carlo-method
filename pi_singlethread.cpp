#include <iostream>
#include <limits>
#include <random>
#include <string>

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

  thread_local mt19937 gen(random_device{}());
  uniform_real_distribution<> dist(0.0, 1.0);

  auto circlePoints = 0;
  auto totalPoints = 0;
  for (auto i = 0; i < totalIterations; i++)
  {
    auto x = dist(gen);
    auto y = dist(gen);
    if ((x * x + y * y) < (1.0 + numeric_limits<double>::epsilon()))
    {
      circlePoints++;
    }
    totalPoints++;

    if (i % chunkSize == 0)
    {
      auto piEst = (4.0 * circlePoints) / totalPoints;
      cout << "[PARTIAL] pi ~= " << piEst << endl;
    }
  }

  auto piEst = (4.0 * circlePoints) / totalPoints;
  cout << "Total: " << totalPoints << endl
       << "Circle: " << circlePoints << endl
       << "[FINAL] pi ~= " << piEst << endl;

  return 0;
}
