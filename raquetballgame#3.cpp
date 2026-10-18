// Name: Steven Ou
#include <iostream>
#include <random>
#include <iomanip>
#include <cmath>

using namespace std;

// Simulate a single racquetball game to 21 points
// x: Player A's probability of winning a volley on A's serve
// pb: Player B's probability of winning a volley on B's serve
bool simulate_game(double x, double pb, mt19937& gen) {
    uniform_real_distribution<double> dist(0.0, 1.0);
    int scoreA = 0;
    int scoreB = 0;
    bool A_serving = true; // Player A always serves first
    
    while (scoreA < 21 && scoreB < 21) {
        double r = dist(gen);
        if (A_serving) {
            if (r < x) {
                scoreA++; // A wins volley on own serve
            } else {
                A_serving = false; // A loses volley, serve passes to B
            }
        } else {
            if (r < pb) {
                scoreB++; // B wins volley on own serve
            } else {
                A_serving = true; // B loses volley, serve passes to A
            }
        }
    }
    return scoreA == 21;
}

int main() {
    cout << "Name: Steven Ou\n\n";
    cout << "Target: Find x such that Player A has a 50% win probability.\n";
    cout << "Player B win probability on serve is fixed at 0.4.\n";
    cout << "Simulations per trial: 10,000\n";
    cout << "--------------------------------------------------------\n";

    // Initialize random number generator
    random_device rd;
    mt19937 gen(rd()); 

    int simulations = 10000;
    double low = 0.0;
    double high = 1.0;
    double x = 0.5;
    double pb = 0.4;
    
    // Binary search to find x to 3 decimal places
    // 15 iterations ensures precision well beyond 0.001 (1/32768)
    for (int i = 1; i <= 15; i++) {
        x = low + (high - low) / 2.0;
        
        int winsA = 0;
        for (int j = 0; j < simulations; j++) {
            if (simulate_game(x, pb, gen)) {
                winsA++;
            }
        }
        
        double win_rate = static_cast<double>(winsA) / simulations;
        
        cout << fixed << setprecision(5);
        cout << "Trial " << setw(2) << i << ": x = " << x 
             << " -> Win Rate: " << win_rate 
             << " (" << winsA << " wins)\n";
        
        // Adjust bounds based on the simulated win rate
        if (win_rate > 0.5) {
            high = x; // A is winning too much, decrease x
        } else {
            low = x;  // A is losing too much, increase x
        }
    }
    
    cout << "\nFinal Answer: To make the game fair, x should be approximately " 
         << fixed << setprecision(3) << x << ".\n";
    
    return 0;
}