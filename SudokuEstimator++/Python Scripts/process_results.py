import matplotlib
import matplotlib.pyplot as plt
import numpy as np

file_path = '../Results/results_3x3_s=8.txt'
# Change these values based on the Sudoku size
latin_rectangle_count = 2102110586634240
true_sudoku_count     = 6670903752021072936960

print('Reading file...')

estimates = []

# Load a results file line by line and store it in the array 'estimates'
with open(file_path) as file:
   line = file.readline()
   while line:
       estimates.append(int(line))
       line = file.readline()

print('Transforming estimates...')

cumulative_sum = 0
n = 0

running_average = []

# Multiply each estimate with the number of M x N*M Latin Rectangles
# Calculate the running average of these estimates
for estimate in estimates:
    total_estimate = estimate * latin_rectangle_count

    cumulative_sum += total_estimate
    n += 1

    running_average.append(cumulative_sum / n)

# Plot the data
plt.plot(running_average)
plt.hlines(true_sudoku_count, xmin=0, xmax=n, linestyles='dashed')
plt.xlabel('Iteration Number')
plt.ylabel('Estimate')
plt.title('Sudoku Solution Estimate - Running Average')
plt.show()