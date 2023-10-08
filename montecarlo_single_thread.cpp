#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>


double generateNormalRandom() {
    static std::random_device rd;
    static std::mt19937 generator(rd());
    static std::normal_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

double monteCarloCallPrice(double S, double K, double T, double r, double sigma, unsigned long numSimulations) {
    double sumPayoffs = 0.0;

    for (unsigned long i = 0; i < numSimulations; ++i) {
        double ST = S * std::exp((r - 0.5 * sigma * sigma) * T + sigma * std::sqrt(T) * generateNormalRandom());
        double payoff = std::max(ST - K, 0.0);
        sumPayoffs += payoff;
    }

    double optionPrice = std::exp(-r * T) * (sumPayoffs / static_cast<double>(numSimulations));

    return optionPrice;
}

int main() {
    double S = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;
    unsigned long numSimulations = 100000000;

    auto start = std::chrono::high_resolution_clock::now();
    double estimatedCallPrice = monteCarloCallPrice(S, K, T, r, sigma, numSimulations);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Estimated Call Option Price (Monte Carlo): " << estimatedCallPrice << std::endl;
    std::cout << "Execution Time: " << duration.count() << " milliseconds" << std::endl;
    
    return 0;
}
