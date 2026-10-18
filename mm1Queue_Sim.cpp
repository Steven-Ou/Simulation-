#include <iostream>
#include <random>
#include <cmath>
#include <iomanip>
#include <limits>

using namespace std;

int main() {
    // Queue Parameters
    double lambda_rate = 0.8; // Arrival rate
    double mu_rate = 1.0;     // Service rate
    // rho = lambda / mu = 0.8
    
    // Random Number Generator Setup (Fixed seed for reproducibility)
    mt19937 rng(42); 
    uniform_real_distribution<double> uniform_dist(0.0, 1.0);
    
    // Exponential distribution via inverse transform sampling
    auto exponential = [&](double rate) {
        double u = uniform_dist(rng);
        while (u == 0.0) u = uniform_dist(rng); // Prevent log(0)
        return -log(u) / rate;
    };
    
    // System State Variables
    double clock_time = 0.0;
    int num_in_queue = 0;
    int server_status = 0; // 0 = idle, 1 = busy
    
    // Event List
    double next_arrival_time = exponential(lambda_rate);
    double next_departure_time = numeric_limits<double>::infinity();
    
    // Statistics & Counters
    int customers_departed = 0;
    int trace_count = 0;
    
    double total_busy_time = 0.0;
    int num_busy_periods = 0;
    double current_busy_start = 0.0;
    
    double sum_interarrival = next_arrival_time; // Track first generated arrival
    int arrivals_generated = 1;
    
    cout << "--- Trace of First 25 Events ---" << endl;
    cout << left << setw(12) << "Event" << setw(15) << "Clock Time" 
         << setw(15) << "Queue Length" << setw(15) << "Server Status" << endl;
    
    // Main Simulation Loop
    while (customers_departed < 1000) {
        if (next_arrival_time <= next_departure_time) {
            // Processing an Arrival Event
            clock_time = next_arrival_time;
            
            if (server_status == 0) {
                // Server is idle; it becomes busy and a new busy period starts
                server_status = 1;
                current_busy_start = clock_time;
                next_departure_time = clock_time + exponential(mu_rate);
            } else {
                // Server is busy; customer joins the queue
                num_in_queue++;
            }
            
            // Print to trace
            if (trace_count < 25) {
                cout << left << setw(12) << "Arrival" << setw(15) << fixed << setprecision(4) << clock_time
                     << setw(15) << num_in_queue << setw(15) << server_status << endl;
                trace_count++;
            }
            
            // Schedule the next arrival
            double generated_inter = exponential(lambda_rate);
            sum_interarrival += generated_inter;
            next_arrival_time = clock_time + generated_inter;
            arrivals_generated++;
            
        } else {
            // Processing a Departure Event
            clock_time = next_departure_time;
            customers_departed++;
            
            if (num_in_queue > 0) {
                // Customer leaves queue and enters service
                num_in_queue--;
                next_departure_time = clock_time + exponential(mu_rate);
            } else {
                // Queue is empty; server becomes idle
                server_status = 0;
                next_departure_time = numeric_limits<double>::infinity();
                
                // End the current busy period
                double busy_duration = clock_time - current_busy_start;
                total_busy_time += busy_duration;
                num_busy_periods++;
            }
            
            // Print to trace
            if (trace_count < 25) {
                cout << left << setw(12) << "Departure" << setw(15) << fixed << setprecision(4) << clock_time
                     << setw(15) << num_in_queue << setw(15) << server_status << endl;
                trace_count++;
            }
        }
    }
    
    // Close out final active busy period for accurate statistics
    if (server_status == 1) {
        total_busy_time += (clock_time - current_busy_start);
        num_busy_periods++;
    }
    
    // Final Statistical Calculations
    double avg_interarrival = sum_interarrival / arrivals_generated;
    double avg_busy_period = (num_busy_periods > 0) ? (total_busy_time / num_busy_periods) : 0.0;
    
    cout << "\n--- Simulation Results ---" << endl;
    cout << "Total Customers Departed: " << customers_departed << endl;
    cout << "Simulated Average Interarrival Time: " << avg_interarrival << " (Expected: 1.25)" << endl;
    cout << "Simulated Average Busy Period: " << avg_busy_period << " (Expected: 5.00)" << endl;
    
    return 0;
}