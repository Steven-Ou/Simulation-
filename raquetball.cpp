#include <iostream>
#include <random>
#include <iomanip>

using namespace std;

bool simulateOneGame(mt19937& rng, uniform_real_distribution<double>& dist) {
    int myScore = 0;
    int opponentScore = 0;
    bool myServe = true; // You serve first

    while (myScore < 21 && opponentScore < 21) {
        double roll = dist(rng);

        if (myServe) {
            // You serve: win volley with probability 0.60
            if (roll < 0.60) {
                myScore++;       // Point scored, keep serve
            } else {
                myServe = false; // Side-out: opponent serves, no point
            }
        } else {
            // Opponent serves: you win volley with probability 0.50
            if (roll < 0.50) {
                myServe = true;  // Side-out: regain serve, no point
            } else {
                opponentScore++; // Opponent scores, keeps serve
            }
        }
    }

    return (myScore == 21);
}

int main() {
    const int totalSimulations = 10000;
    int myWins = 0;

    random_device rd;
    mt19937 rng(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);

    for (int i = 0; i < totalSimulations; ++i) {
        if (simulateOneGame(rng, dist)) {
            myWins++;
        }
    }

    double estimatedProbability = static_cast<double>(myWins) / totalSimulations;

    cout << fixed << setprecision(4);
    cout << "Simulations Run: " << totalSimulations << "\n";
    cout << "Games Won:       " << myWins << "\n";
    cout << "Estimated Win %: " << (estimatedProbability * 100.0) << "%\n";
    cout << "Estimated Prob:  " << estimatedProbability << "\n";

    return 0;
}