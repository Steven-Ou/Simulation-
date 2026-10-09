#include <iostream>
#include <random>
#include <iomanip>

using namespace std;

// Modified to accept 'x' (p1ServeWinProb) as a parameter
bool simulateOneGame(mt19937 &rng, uniform_real_distribution<double> &dist, double p1ServeWinProb)
{
    int myScore = 0;
    int opponentScore = 0;
    bool myServe = true; // You serve first

    while (myScore < 21 && opponentScore < 21)
    {
        double roll = dist(rng);

        if (myServe)
        {
            // You serve: win volley with probability x
            if (roll < p1ServeWinProb)
            {
                myScore++; // Point scored, keep serve
            }
            else
            {
                myServe = false; // Side-out: opponent serves, no point
            }
        }
        else
        {
            // Opponent serves: you win volley with probability 0.40
            if (roll < 0.40)
            {
                myServe = true; // Side-out: regain serve, no point
            }
            else
            {
                opponentScore++; // Opponent scores, keeps serve
            }
        }
    }

    return (myScore == 21);
}

int main()
{
    const int totalSimulations = 10000;

    random_device rd;
    mt19937 rng(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);

    cout << fixed << setprecision(4);
    cout << "Target Win Percentage: ~50% (49% - 51%)\n";
    cout << "Player 1 Win Prob when Player 2 Serves: 0.40\n\n";
    cout << "Simulating " << totalSimulations << " games for each trial probability x...\n";
    cout << "--------------------------------------------------\n";

    // Iterate x from 0.01 to 0.99 by 0.01 increments
    for (double x = 0.01; x < 1.00; x += 0.01)
    {
        int myWins = 0;

        for (int i = 0; i < totalSimulations; ++i)
        {
            if (simulateOneGame(rng, dist, x))
            {
                myWins++;
            }
        }

        double estimatedProbability = static_cast<double>(myWins) / totalSimulations;
        double winPercentage = estimatedProbability * 100.0;

        // Filter to only display 'fair' outcomes
        if (winPercentage >= 49.0 && winPercentage <= 51.0)
        {
            cout << "For x = " << x << " -> Estimated Win %: " << winPercentage << "%\n";
        }
    }

    return 0;
}