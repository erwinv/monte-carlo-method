#include <ppl.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace concurrency;
using namespace std;

int main(int argc, char* argv[])
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

    // not chunked (PERFORMS HORRIBLY)
    /*
    vector<int> circlePointsVec;
    circlePointsVec.resize(totalIterations);
    iota(begin(circlePointsVec), end(circlePointsVec), 0);

    parallel_transform(begin(circlePointsVec), end(circlePointsVec), begin(circlePointsVec), [](int) {
        thread_local mt19937 gen(random_device{}());
        uniform_real_distribution<> dist(0.0, 1.0);

        auto x = dist(gen);
        auto y = dist(gen);
        if ((x * x + y * y) < (1.0 + numeric_limits<double>::epsilon()))
        {
            return 1;
        }
        return 0;
        });
    auto circlePoints = parallel_reduce(begin(circlePointsVec), end(circlePointsVec), 0);
    */
    // end not chunked

    // chunked
    vector<int> totalPointsChunks;
    for (auto remaining = totalIterations; remaining > 0; remaining -= chunkSize) {
        totalPointsChunks.emplace_back(min(remaining, chunkSize));
    }
    vector<int> circlePointsChunks(totalPointsChunks.size());

    parallel_transform(begin(totalPointsChunks), end(totalPointsChunks), begin(circlePointsChunks), [](int totalPoints) {
        thread_local mt19937 gen(random_device{}());
        uniform_real_distribution<> dist(0.0, 1.0);

        auto circlePoints = 0;
        for (auto i = 0; i < totalPoints; i++) {
            auto x = dist(gen);
            auto y = dist(gen);
            if ((x * x + y * y) < (1.0 + numeric_limits<double>::epsilon()))
            {
                circlePoints++;
            }
        }

        auto piEst = (4.0 * circlePoints) / totalPoints;
        cout << "[PARTIAL] pi ~= " << piEst << endl;

        return circlePoints;
    });
    auto circlePoints = parallel_reduce(begin(circlePointsChunks), end(circlePointsChunks), 0);
    // end chunked

    auto piEst = (4.0 * circlePoints) / totalIterations;
    cout << "Total: " << totalIterations << endl
        << "Circle: " << circlePoints << endl
        << "[FINAL] pi ~= " << piEst << endl;

    return 0;
}
