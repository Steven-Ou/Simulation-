#include <iostream>
#include <random>
#include <queue>
#include <vector>
#include <iomanip>

using namespace std;

// Define the two types of events
enum EventType
{
    ARRIVAL,
    DEPARTURE
};

// Structure for the Future Event List (FEL)
struct Event
{
    double time;
    EventType type;
    int job_id;

    // The priority queue in C++ needs this to sort events chronologically
    bool operator>(const Event &other) const
    {
        return time > other.time;
    }
};

// Structure for the Delayed List
struct Job
{
    int id;
    double arrival_time;
};

int main()
{
    // Parameters
    double lambda_rate = 0.8;
    double mu_rate = 1.0;

    mt19937 rng(42);
    uniform_real_distribution<double> uniform_dist(0.0, 1.0);
    auto exponential = [&](double rate)
    {
        double u = uniform_dist(rng);
        while (u == 0.0)
            u = uniform_dist(rng);
        return -log(u) / rate;
    };

    // System Components
    priority_queue<Event, vector<Event>, greater<Event>> fel; // The Future Event List
    queue<Job> delayed_list;                                  // The explicit Delayed List

    double clock_time = 0.0;
    int server_status = 0; // 0 = idle, 1 = busy

    // Statistics
    int jobs_created = 0;
    int jobs_completed = 0;
    double total_delay = 0.0;   // Sum of all wait times in the delayed list
    int jobs_delayed_count = 0; // Number of jobs that had a delay > 0

    int trace_count = 0;

    // Schedule the very first event
    jobs_created++;
    fel.push({exponential(lambda_rate), ARRIVAL, jobs_created});

    cout << "--- Trace of First 25 Events ---" << endl;
    cout << left << setw(12) << "Event" << setw(10) << "Job ID"
         << setw(15) << "Clock Time" << setw(18) << "Delayed List Size"
         << setw(15) << "Server Status" << setw(15) << "Job Delay" << endl;

    // Main Simulation Loop
    while (jobs_completed < 1000 || !fel.empty())
    {
        if (jobs_completed == 1000)
            break; // Stop after 1000 departures

        // 1. Pull the next chronological event from the FEL
        Event current_event = fel.top();
        fel.pop();

        // 2. Advance the simulation clock
        clock_time = current_event.time;

        if (current_event.type == ARRIVAL)
        {
            // Schedule the next arrival immediately
            jobs_created++;
            fel.push({clock_time + exponential(lambda_rate), ARRIVAL, jobs_created});

            if (server_status == 0)
            {
                // Server is idle. Job goes straight to server (Delay = 0)
                server_status = 1;
                double service_time = exponential(mu_rate);
                fel.push({clock_time + service_time, DEPARTURE, current_event.job_id});

                if (trace_count < 25)
                {
                    cout << left << setw(12) << "Arrival" << setw(10) << current_event.job_id
                         << setw(15) << fixed << setprecision(4) << clock_time
                         << setw(18) << delayed_list.size() << setw(15) << server_status
                         << setw(15) << "0.0000" << endl;
                    trace_count++;
                }
            }
            else
            {
                // Server is busy. Add job to the explicit delayed list
                delayed_list.push({current_event.job_id, clock_time});

                if (trace_count < 25)
                {∑
                    cout << left << setw(12) << "Arrival" << setw(10) << current_event.job_id
                         << setw(15) << fixed << setprecision(4) << clock_time
                         << setw(18) << delayed_list.size() << setw(15) << server_status
                         << setw(15) << "-" << endl; // Delay not known yet
                    trace_count++;
                }
            }
        }
        else if (current_event.type == DEPARTURE)
        {
            jobs_completed++;

            if (trace_count < 25)
            {
                cout << left << setw(12) << "Departure" << setw(10) << current_event.job_id
                     << setw(15) << fixed << setprecision(4) << clock_time
                     << setw(18) << delayed_list.size() << setw(15) << server_status
                     << setw(15) << "-" << endl;
                trace_count++;
            }

            if (!delayed_list.empty())
            {
                // A job is waiting. Pull it from the delayed list and start service.
                Job next_job = delayed_list.front();
                delayed_list.pop();

                // Calculate exactly how long this specific job was delayed
                double delay_for_this_job = clock_time - next_job.arrival_time;
                total_delay += delay_for_this_job;
                jobs_delayed_count++;

                // Schedule its departure
                double service_time = exponential(mu_rate);
                fel.push({clock_time + service_time, DEPARTURE, next_job.id});

                if (trace_count < 25)
                {
                    cout << left << setw(12) << "-> Service" << setw(10) << next_job.id
                         << setw(15) << fixed << setprecision(4) << clock_time
                         << setw(18) << delayed_list.size() << setw(15) << server_status
                         << setw(15) << delay_for_this_job << endl;
                    trace_count++;
                }
            }
            else
            {
                // Nobody is waiting. Server becomes idle.
                server_status = 0;
            }
        }
    }

    // Averages and Metrics
    double avg_delay_all_jobs = total_delay / jobs_completed;
    double avg_delay_delayed_jobs = total_delay / jobs_delayed_count;

    cout << "\n--- Simulation Results ---" << endl;
    cout << "Total Jobs Completed: " << jobs_completed << endl;
    cout << "Average Delay for ALL Jobs: " << fixed << setprecision(4) << avg_delay_all_jobs
         << " (Theoretical Expected: 3.2000)" << endl;
    cout << "Average Delay for DELAYED Jobs: " << avg_delay_delayed_jobs << endl;

    return 0;
}