import os
import re
import subprocess
import matplotlib.pyplot as plt
from datetime import datetime
import pandas as pd
from statistics import mean
import time


def update_input_file(num_threads, k=10, lambda1=10, lambda2=10):
    with open("input.txt", "w") as f:
        f.write(f"{num_threads} {k} {lambda1} {lambda2}")


def parse_log_file(filename):
    request_times = {}
    enter_times = {}

    with open(filename, "r") as f:
        for line in f:
            parts = line.strip().split("|")
            if len(parts) < 4:
                continue

            # Extract thread ID and iteration
            tid_match = re.search(r"TID\s+(\d+)", parts[0])
            iter_match = re.search(r"Iter\s+(\d+)", parts[1])

            if not tid_match or not iter_match:
                continue

            thread_id = int(tid_match.group(1))
            iteration = int(iter_match.group(1))

            # Extract event type and timestamp
            event = parts[2].strip()
            time_str = parts[3].strip()

            # Parse timestamp (HH:MM:SS.milliseconds)
            time_parts = time_str.split(".")
            time_obj = datetime.strptime(time_parts[0], "%H:%M:%S")
            milliseconds = int(time_parts[1]) if len(time_parts) > 1 else 0

            # Convert to total milliseconds for easier calculations
            total_ms = (
                time_obj.hour * 3600 + time_obj.minute * 60 + time_obj.second
            ) * 1000 + milliseconds

            # Store request and enter times
            if event == "Requested":
                request_times[(thread_id, iteration)] = total_ms
            elif event == "Entered":
                enter_times[(thread_id, iteration)] = total_ms

    return request_times, enter_times


def calculate_metrics(request_times, enter_times):
    wait_times = []

    for key in enter_times:
        if key in request_times:
            wait_time = enter_times[key] - request_times[key]
            wait_times.append(wait_time)

    if not wait_times:
        return 0, 0

    avg_wait_time = mean(wait_times)
    worst_wait_time = max(wait_times)

    return avg_wait_time, worst_wait_time


def run_simulation(executable, num_threads):
    update_input_file(num_threads)

    try:
        subprocess.run([executable], check=True)
        return True
    except subprocess.CalledProcessError:
        print(f"Error running simulation with {num_threads} threads")
        return False


def analyze_performance(executable, thread_range, runs_per_config=5):
    avg_results = {
        "tas": {"avg": [], "worst": []},
        "cas": {"avg": [], "worst": []},
        "cas_bounded": {"avg": [], "worst": []},
    }

    for num_threads in thread_range:
        print(f"Testing with {num_threads} threads...")

        tas_avgs = []
        tas_worsts = []
        cas_avgs = []
        cas_worsts = []
        bounded_avgs = []
        bounded_worsts = []

        for run in range(runs_per_config):
            print(f"  Run {run+1}/{runs_per_config}...")

            if not run_simulation(executable, num_threads):
                continue

            # Parse the log files for each algorithm
            # TAS
            request_times, enter_times = parse_log_file("tas.txt")
            avg_time, worst_time = calculate_metrics(request_times, enter_times)
            tas_avgs.append(avg_time)
            tas_worsts.append(worst_time)

            # CAS
            request_times, enter_times = parse_log_file("cas.txt")
            avg_time, worst_time = calculate_metrics(request_times, enter_times)
            cas_avgs.append(avg_time)
            cas_worsts.append(worst_time)

            # Bounded CAS
            request_times, enter_times = parse_log_file("cas_bounded.txt")
            avg_time, worst_time = calculate_metrics(request_times, enter_times)
            bounded_avgs.append(avg_time)
            bounded_worsts.append(worst_time)

            # Sleep a bit to ensure system resources are released
            time.sleep(1)

        # Calculate the average of the runs
        if tas_avgs:
            avg_results["tas"]["avg"].append(mean(tas_avgs))
            avg_results["tas"]["worst"].append(mean(tas_worsts))

        if cas_avgs:
            avg_results["cas"]["avg"].append(mean(cas_avgs))
            avg_results["cas"]["worst"].append(mean(cas_worsts))

        if bounded_avgs:
            avg_results["cas_bounded"]["avg"].append(mean(bounded_avgs))
            avg_results["cas_bounded"]["worst"].append(mean(bounded_worsts))

    return avg_results


def plot_results(thread_range, results):
    """Plot the performance results."""
    plt.figure(figsize=(12, 10))

    # Average wait time plot
    plt.subplot(2, 1, 1)
    plt.plot(thread_range, results["tas"]["avg"], "o-", label="TAS")
    plt.plot(thread_range, results["cas"]["avg"], "s-", label="CAS")
    plt.plot(thread_range, results["cas_bounded"]["avg"], "^-", label="Bounded CAS")
    plt.xlabel("Number of Threads")
    plt.ylabel("Average Wait Time (ms)")
    plt.title("Average Time to Enter Critical Section")
    plt.grid(True)
    plt.legend()

    # Worst-case wait time plot
    plt.subplot(2, 1, 2)
    plt.plot(thread_range, results["tas"]["worst"], "o-", label="TAS")
    plt.plot(thread_range, results["cas"]["worst"], "s-", label="CAS")
    plt.plot(thread_range, results["cas_bounded"]["worst"], "^-", label="Bounded CAS")
    plt.xlabel("Number of Threads")
    plt.ylabel("Worst-case Wait Time (ms)")
    plt.title("Worst-case Time to Enter Critical Section")
    plt.grid(True)
    plt.legend()

    plt.tight_layout()
    plt.savefig("mutex_performance_comparison.png", dpi=300)
    plt.show()


def main():
    # Define the thread range to test
    thread_range = [
        10,
    ]

    # Compile the C++ code if needed
    if not os.path.exists("./mutex_benchmark"):
        print("Compiling the C++ code...")
        subprocess.run(
            ["g++", "-o", "mutex_benchmark", "q01.cpp", "-pthread"], check=True
        )

    # Analyze the performance
    results = analyze_performance("./mutex_benchmark", thread_range, runs_per_config=5)

    # Plot the results
    plot_results(thread_range, results)

    # Save the results to a CSV file for further analysis
    df = pd.DataFrame(
        {
            "Threads": thread_range,
            "TAS_Avg": results["tas"]["avg"],
            "TAS_Worst": results["tas"]["worst"],
            "CAS_Avg": results["cas"]["avg"],
            "CAS_Worst": results["cas"]["worst"],
            "BoundedCAS_Avg": results["cas_bounded"]["avg"],
            "BoundedCAS_Worst": results["cas_bounded"]["worst"],
        }
    )
    df.to_csv("mutex_performance_results.csv", index=False)
    print("Results saved to mutex_performance_results.csv")

    # Generate a summary report
    print("\nPerformance Summary:")
    print("=" * 80)
    print(
        f"{'Threads':<10} | {'TAS Avg':<10} | {'TAS Worst':<10} | {'CAS Avg':<10} | {'CAS Worst':<10} | {'BCAS Avg':<10} | {'BCAS Worst':<10}"
    )
    print("-" * 80)

    for i, threads in enumerate(thread_range):
        print(
            f"{threads:<10} | {results['tas']['avg'][i]:<10.2f} | {results['tas']['worst'][i]:<10.2f} | "
            f"{results['cas']['avg'][i]:<10.2f} | {results['cas']['worst'][i]:<10.2f} | "
            f"{results['cas_bounded']['avg'][i]:<10.2f} | {results['cas_bounded']['worst'][i]:<10.2f}"
        )


if __name__ == "__main__":
    main()
