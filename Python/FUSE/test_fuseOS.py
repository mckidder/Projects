#!/usr/bin/env python


# import needed libraries for operations
import os
import time

# define variables needed
file = "./fusemount/geiger"								# var for file to be used
fd = os.open(file, os.O_RDONLY)							# var for open file in read only
# list var for the various amount of bytes to read, with numbers ranging large
# enough to deplete the geiger generated and move to random generated
byteslist = [10, 25, 50, 100, 250, 500, 1000, 2500]	

# loop to step through list of bytes to be read and print set out
for bytestoread in byteslist:
    read = os.read(fd, bytestoread)						# read requested number of bytes from file to var
    print("\nPrinting %d bytes!\n" %bytestoread)		# print what set of numbers is to be displayed
    print(read)											# print that set of bytes
    time.sleep(5)										# pause for 5 seconds
os.close(fd)											# close file
