#!/usr/bin/env python

# Purpose:To convert 32 bits to bytes

import numpy as np

file = open('bits.txt')# Open source of bits
binArray = []# Array to hold string elements

# Split each new line in the file into lines in a list
data = file.read().splitlines()

# Process one line of string input
for line in data:

    # Read in each element of the string input into an array
    for number in line:
        c = int(number)
        binArray.append(number)
    binArray = map(int, binArray)# Convert array to integer
    intArray = np.packbits(binArray, axis=-1)
    binArray = []# Empty array
    
    outFile = open('int.txt', 'a')
    for item in intArray:
        outFile.write("%d " % item)

    outFile.write("\n")

# Close open files
file.close()
outFile.close()
