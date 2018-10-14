
# Purpose:To take a file of timestamps and convert pairs of those
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

fw=open("conv_bits.txt","a")

def mycb(text):
#Use the global, no local
        global bit_iter
        global fir_rd_pr1
        global sec_rd_pr1
        global fir_rd_pr2
        global sec_rd_pr2
        global i
#Convert text to string unless it is the last line of the file
        if text != '\n':
                text = float(text)
        else:
                return
      
#Iteration counter
        i+=1
#Check which iteration this is, assign a time for
#the first round of first intervaltime stamp, second round
#of first interval time stamp, first round of second inerval
#time stamp, and second round of second interval time stamp
        if i == 1:
                fir_rd_pr1 = text
 

        if i == 2:
                sec_rd_pr1 = text
 

        if i == 3:
                fir_rd_pr2 = text


        if i == 4:
                sec_rd_pr2 = text
#Iterator reached the final pair, assign the two intervals

        if i > 3:
                total_pr2 = (sec_rd_pr2 - fir_rd_pr2)
                total_pr1 = (sec_rd_pr1 - fir_rd_pr1)
#Check to see whether the first or second interval was larger
#and assign a 1 or 0 bit as needed
        if i > 3 and total_pr2 > total_pr1:
                fw.write(str(0))
                fw.flush()
                total_pr2 = total_pr1 = 0
                bit_iter+=1
                i = 0


        if i > 3 and total_pr1 > total_pr2:
                total_pr2 = total_pr1 = 0
                fw.write(str(1))
                fw.flush()
                bit_iter+=1
                i = 0

#If somehow the intervals were the same, reset iterator and
#do not write anything
        if i > 3 and total_pr1 == total_pr2:
                i = 0


#If we have reached 32 bits, start a new line        
        if bit_iter == 32:
                fw.write('\n')
                fw.flush()
                bit_iter = 0
#Reset iterator so that we don't run one extra time
        elif i == 4:
                i = 0
#Allows for fall through as needed to keep cb running
        else:

          pass

#Open the source of the intervals and process each line
#until the end of the file
with open('bits.txt', 'r') as fr:

        for text in fr:
                
                mycb(text)
#Verify write file is closed
fw.close()
