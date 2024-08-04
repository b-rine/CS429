import matplotlib.pyplot as plt
import numpy as np

# Test names
tests = ['ijk', 'ikj', 'gemm block']

# Cycle counts for different scenarios
without_cache = [6611614, 6349534, 7273233]
with_cache = [36283399, 19832938, 16150761]
with_different_cache_parameters = [36258055, 13294186, 11403315]

# Define bar width
bar_width = 0.2

# Create evenly spaced positions for the bars
positions = np.arange(len(tests))

# Plotting
plt.figure(figsize=(10, 6))
plt.bar(positions - bar_width, without_cache, color='b', width=bar_width, label='Without Cache')
plt.bar(positions, with_cache, color='g', width=bar_width, label='With Cache A = 4, B = 32')
plt.bar(positions + bar_width, with_different_cache_parameters, color='r', width=bar_width, label='With Cache A = 8, B = 64')

# Set y-axis limits
plt.ylim(0, 40000000)

# Adding labels and title
plt.xlabel('Test Cases')
plt.ylabel('Cycle Counts (ten millions)')
plt.title('Cycle Counts for Different Cache Configurations')
plt.xticks(positions, tests)  # Set x-axis ticks to test case names
plt.legend()

# Show the plot
plt.savefig("EC_plot")
