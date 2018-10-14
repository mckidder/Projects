
# Purpose:To take real-time timestamps and convert pairs of those
# to intervals that are then converted to bits after two interval
# pairs have been detected. The result is written to file in a 
# 32-bit string format (partial strings allowed)
import time,pigpio

#Globals
bit_iter = 0
fir_rd_pr1 = 0
sec_rd_pr1 = 0
fir_rd_pr2 = 0
sec_rd_pr2= 0
i = 0

fo=open("bits.txt","a")
def mycb(x,y,z):
#Use the global, no local
        global bit_iter
        global fir_rd_pr1
        global sec_rd_pr1
        global fir_rd_pr2
        global sec_rd_pr2
        global i

	t=time.time()
#Iteration counter
        i+=1
#Check which iteration this is, assign a time for
#the first round of first intervaltime stamp, second round
#of first interval time stamp, first round of second inerval
#time stamp, and second round of second interval time stamp
        if i == 1:
                fir_rd_pr1 = t
 

        if i == 2:
                sec_rd_pr1 = t
 

        if i == 3:
                fir_rd_pr2 = t


        if i == 4:
                sec_rd_pr2 = t
#Iterator reached the final pair, assign the two intervals

        if i > 3:
                total_pr2 = (sec_rd_pr2 - fir_rd_pr2)
                total_pr1 = (sec_rd_pr1 - fir_rd_pr1)
#Check to see whether the first or second interval was larger
#and assign a 1 or 0 bit as needed
        if i > 3 and total_pr2 > total_pr1:
                fo.write(str(0))
                fo.flush()
                total_pr2 = total_pr1 = 0
                bit_iter+=1
                i = 0


        if i > 3 and total_pr1 > total_pr2:
                total_pr2 = total_pr1 = 0
                fo.write(str(1))
                fo.flush()
                bit_iter+=1
                i = 0

#If somehow the intervals were the same, reset iterator and
#do not write anything
        if i > 3 and total_pr1 == total_pr2:
                i = 0


#If we have reached 32 bits, start a new line        
        if bit_iter == 32:
                fo.write('\n')
                fo.flush()
                bit_iter = 0
#Reset iterator so that we don't run one extra time
        elif i == 4:
                i = 0
#Allows for fall through as needed to keep cb running
        else:

          pass

pin=pigpio.pi()
cb=pin.callback(23,pigpio.FALLING_EDGE, mycb)

while True:

        time.sleep(15)
