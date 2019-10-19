import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import math

N = 4
M = 4
random_walk_length = 60

true_sudoku_counts = {
    (2, 2): 288,
    (2, 3): 28200960,
    (2, 4): 29136487207403520,
    (2, 5): 1903816047972624930994913280000,
    (2, 6): 38296278920738107863746324732012492486187417600000,
    (3, 3): 6670903752021072936960,
    (3, 4): 81171437193104932746936103027318645818654720000,
    (4, 4): 595840000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 # estimate, real value unknown
}

def reduced_factor(k, n):
    return (math.factorial(n) * math.factorial(n - 1)) / math.factorial(n - k)

# Number of reduced Latin Rectangles, given N, M
latin_rectangle_counts = {
    (2, 2): 3,
    (2, 3): 1064,
    (2, 4): 420909504,
    (2, 5): 746988383076286464,
    (2, 6): (1 << 17) * 9 * 5 * 131 * 110630813 *65475601447957,
    (3, 3): 103443808,
    (3, 4): (1 << 9) * 27 * 7 * 1945245990285863,
    (4, 4): (1 << 14) * 243 * 2693 * 42787 * 1699482467 * 8098773443
}

file_path = '../Results/results_{}x{}_s={}.txt'.format(N, M, random_walk_length)
# Change these values based on the Sudoku size
latin_rectangle_count = reduced_factor(M, N * M) * latin_rectangle_counts[(N, M)]
true_sudoku_count     = true_sudoku_counts[(N, M)]

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
plt.title('{} x {} Sudoku Solution Estimate - Running Average - Random walk length: {}'.format(N, M, random_walk_length))
plt.show()