#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include "./pcg-cpp-master/include/pcg_random.hpp"

std::mutex mtx;  

double generateNormalRandom(pcg32 *rng) {
    std::normal_distribution<double> distribution(0.0, 1.0);
    return distribution(*rng);
}

void monteCarloPartial(double S, double K, double T, double r, double sigma, unsigned long numSimulations, double &result) {
    pcg32 rng;
    rng.seed(pcg_extras::seed_seq_from<std::random_device>()); 
    double sumPayoffs = 0.0;

    for (unsigned long i = 0; i < numSimulations; ++i) {
        double ST = S * std::exp((r - 0.5 * sigma * sigma) * T + sigma * std::sqrt(T) * generateNormalRandom(&rng));
        double payoff = std::max(ST - K, 0.0);
        sumPayoffs += payoff;
    }

    mtx.lock();
    result += std::exp(-r * T) * (sumPayoffs / static_cast<double>(numSimulations));
    mtx.unlock();
}

int main() {
    double S = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;
    unsigned long numSimulations = 100000000;
    int numThreads = std::thread::hardware_concurrency(); 

    auto start = std::chrono::high_resolution_clock::now();

    unsigned long simsPerThread = numSimulations / numThreads;
    double result = 0.0;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(monteCarloPartial, S, K, T, r, sigma, simsPerThread, std::ref(result)));
    }

    for (auto &t : threads) {
        t.join();
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Estimated Call Option Price (Monte Carlo with Multithreading): " << result << std::endl;
    std::cout << "Execution Time: " << duration.count() << " milliseconds" << std::endl;

    return 0;
}
