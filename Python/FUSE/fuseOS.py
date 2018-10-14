from __future__ import with_statement, absolute_import

import os 
import sys
import errno
import time
import stat
import random
import logging 
import numpy as np  
np.set_printoptions(threshold='nan') 
from fuse import FUSE, FuseOSError, Operations, LoggingMixIn

# Using Fusypy RC April 5, 2018
class testOS(LoggingMixIn, Operations):
    def __init__(self): # Need to add more parameters and more init values
        self.files = {} # File stats for each virtual file
        self.fileName = 'testfile.txt' #interval file name
        self.filehandle = {}
        self.last = 1 # holds last number acquired for seed in case rngs run out
        self.position = 0 # holds position in interval file
        
        now = time.time() #times for virtual files set to when object instantiated
        #set properties for virtual files
        self.files['/'] = dict(st_mode=(stat.S_IFDIR | 0o777), st_ctime=now, st_mtime=now, st_atime=now, st_nlink=2)
        self.files['/geiger'] =  dict(st_mode=(stat.S_IFREG | 0o444), st_ctime=now, st_mtime=now, st_atime=now, st_nlink=2)
        self.files['/countsPerMinute'] =  dict(st_mode=(stat.S_IFREG | 0o444), st_ctime=now, st_mtime=now, st_atime=now, st_nlink=2)

        #set default values for counts per minute display
        self.cpm_avg = 0 
        self.cpm_seconds = 0
        self.cpm_intervals = 0       
        
    # getattr using Fusepy and the os module RC April 5, 2018
    def getattr(self, path, fh=None):
        if path not in self.files:    # Path is not in file
            raise FuseOSError(errno.ENOENT) # Error No Entity

        #all property times set to time of object instantiation
        self.files[path]['st_ctime'] = os.stat(self.fileName).st_ctime
        self.files[path]['st_mtime'] = os.stat(self.fileName).st_mtime
        self.files[path]['st_atime'] = os.stat(self.fileName).st_atime
        self.files[path]['st_size'] = os.stat(self.fileName).st_size
        return self.files[path]

    def read(self, path, size, offset, fh):
        if (path == "/geiger"):        
            return self.geigerRead(path, size, fh)
        if (path == "/countsPerMinute"):
            return self.cpmread(path, size)
        else:
            return "Invalid Selection"
            
    def readdir(self, path, fh):
         yield '.'
         yield '..'
         yield 'countsPerMinute' #show virtual files when dir enumerated
         yield 'geiger'
 

    def open(self, path, flags):
        if (path == "/geiger" or path == "/countsPerMinute"):
            self.filehandle = os.open(self.fileName, flags)
            return self.filehandle
        else:
            return "Invalid Operation"
        
    def cpmread(self, path, size):
            output = (
            "Seconds: " + str(self.cpm_seconds) + "\n " +
            "Intervals: " + str(self.cpm_intervals)  + "\n" +
            "Average Counter Per Minute: " + str(self.cpm_avg) + "\n")
            return output
    
    def randproc(self, path, interv_list):
        #set globals to track counts per min across multiple iterations
        
        i = -1   #simple iterator to track interval pairs
        bit_list = []  #list to hold the bits converted from intervals
        byte_list = [] #list to build out return value
#       cpm = open(self.cpmName, 'w+')	Create placeholder to write cpm to actual file if needed
        #iterate through interval list to make bits
        for intervals in interv_list:
            
            i+=1
           
            #reset iterator at max
            if i > 1:
                i == 0
            #identify intervals for comparison and update total counts per min
            if i == 0:
                fir_interv = float(intervals)
                self.cpm_seconds+=float(intervals)
                self.cpm_intervals+=1.0
            if i == 1:
                sec_interv = float(intervals)
                self.cpm_seconds+=float(intervals)
                self.cpm_intervals+=1.0
            #interval pair identified, compare and write integers
            if i == 1 and sec_interv > fir_interv:
                bit_list.append('0')         
                i = -1

            if i == 1 and fir_interv > sec_interv:
                bit_list.append('1')
                i = -1

            #if somehow the intervals were the same, reset iterator and
            #generate a random 1 or 0 in their place
            if i == 1 and fir_interv == sec_interv:
                np.random.seed()
                equal_interv = np.random.random_integers(0,1)
                bit_list.append(str(equal_interv))
                i = -1
            #calc total average and write    
            self.cpm_avg = (self.cpm_intervals / self.cpm_seconds) * 60.0
#            cpm.write(cpm_avg)
#            cpm.flush()        More placeholder for cpm write
#            cpm.close()
        #convert bits to bytes
		#initial script written by Ian
        bit_list = map(int, bit_list)
        byte_list = np.packbits(bit_list)
		
        return byte_list
    
    def getPRNG(self, bytesNeeded, seed): #return number of missing intervals needed
        random.seed(seed)
        returnList = []
        while bytesNeeded:
            returnList.append(random.getrandbits(8))#add random byte to array
            bytesNeeded -= 1
        return returnList
        
    def geigerRead(self, path, size,fh):
        newHandle = os.fdopen(fh) #convert file descriptor to handle useable by python readline
        newHandle.seek(self.position) #set current position to last read position
        ivArray = [] # to hold intervals
        lines =  16 # need 16 lines for a byte
        numBytes = lines * size # bytes requested time umber of lines per byte
        missingIntervals = 0 #how short on intervals if run out of geiger numbers
        while numBytes:
            while lines:
                beenRead = (newHandle.readline())[:-1] #omit newline char
                if beenRead == "": #reached end of interval file
                    missingIntervals += 1 
                    if (len(ivArray) + (missingIntervals)) > size:
                        break
                else:
                    ivArray.append(beenRead) # add interval to array
                lines -= 1
            lines = 16 # reset variable for loop
            numBytes -= 1        
        self.position = newHandle.tell() # get current position in file and save it for next read

        if missingIntervals: #generate PRNGs if reached end of file
            if ivArray:
                ivArray.extend((self.getPRNG(missingIntervals/2, random.choice(ivArray)))) # if available take random interval for seed
                self.last = random.choice(ivArray)
            else:
                 ivArray.extend((self.getPRNG(missingIntervals/2, self.last)))
                 self.last = random.choice(ivArray) #if unavailable use last interval for seed
                        
        processed = self.randproc(path, ivArray) #process array of intervals to bytes
    
        return processed.tostring()         
    
logging.basicConfig(level=logging.DEBUG) # enable verbose logging    
FUSE(testOS(), sys.argv[1], nothreads=True, foreground=True, ro=False, rw=True)
