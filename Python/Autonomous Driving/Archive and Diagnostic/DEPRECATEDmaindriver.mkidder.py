######################################################################################
#
#
#
#Please note, this code is only provided for reference I ended up 
#scrapping this development and moving toward a more simple solution.
#
#It is functional, but not in a way that I could use effectively.
#
#
#
#######################################################################################                                                               
# This creates a LIDAR like map using an ultrasonic sensor and a servo with the GoPiGo

########################################################################################                             

from gopigo import *
import sys
import wiringpi as wp
from collections import Counter
import math
from collections import namedtuple

#Set time constants/intital state
maxtime = 240	#run for 4 minutes
starttime = time.time()  #remember when we started

#Set wiringPi constants/inputs
wp.wiringPiSetupGpio()

leftTouch = 19
rightTouch = 26
midIR = 21
#bakIR = 20

wp.pinMode(leftTouch, 0)
wp.pinMode(rightTouch, 0)
wp.pinMode(midIR, 0)
#wp.pinMode(bakIR, 0)

wp.pullUpDnControl(leftTouch, 2)
wp.pullUpDnControl(rightTouch, 2)
wp.pullUpDnControl(midIR, 2)
#wp.pullUpDnControl(bakIR, 2)

#Create some constants/default values for wheretogo() and sensors()
data = 0
badIRf = 1
#badIRb = 2
badbuttonL = 3
badbuttonR = 4
uhohW = 0
uhohS = 0
wentleft = 0
min_distance = 20
statusval = True


#setup us_map() output results
class Results(object):
  def __init__(self, neardist, safe):
     self.neardist = neardist
     self.safe = safe
result = Results(0, 'fwd')

#Create encoder runtime method
def enctimeout():
	while True:
		enc=read_enc_status()
		ts=read_timeout_status()
		time.sleep(.05)
		print enc,ts
		if enc == 0:	#Stop when target is reached
			break
		if  ts==0:
			break

#Create sensor array logic method			
def sensors():
	stop()
	enc_tgt(1,1,18)	#reset encoder target
	
	if data == badIRf:
		print "Rescanning, moving forward is not safe\n"
		uhohS = uhohS + 1
		if uhohS == 3:
			stop()
			uhohS = 0
			print "IR sensors got stuck, stopping and blinking LEDs\n"
			while (time.time() - starttime) < maxtime:
				led_on()
				time.sleep(.5)
				led_off()
		bwd()
		enctimeout()
		if wentleft == 0:
			wentleft = 1
			left()
			enctimeout()
		else:
			wentleft = 0
			right()
			enctimeout()
			
	#Copy badIRf code here for badIRb, change bwd() to fwd()
	
	if data == badbuttonL:
		print "Rescanning, moving forward is not safe\n"
		uhohS = uhohS + 1
		if uhohS == 3:
			stop()
			uhohS = 0
			print "Button sensors got stuck, stopping and blinking LEDs\n"
			while (time.time() - starttime) < maxtime:
				led_on()
				time.sleep(.5)
				led_off()
		bwd()
		enctimeout()
		if wentleft == 0:
			wentleft = 1
			left()
			enctimeout()
		else:
			wentleft = 0
			right()
			enctimeout()
			
	if data == badbuttonR:
		print "Rescanning, moving forward is not safe\n"
		uhohS = uhohS + 1
		if uhohS == 3:
			stop()
			uhohS = 0
			print "Button sensors got stuck, stopping and blinking LEDs\n"
			while (time.time() - starttime) < maxtime:
				led_on()
				time.sleep(.5)
				led_off()
		bwd()
		enctimeout()
		if wentleft == 0:
			wentleft = 1
			left()
			enctimeout()
		else:
			wentleft = 0
			right()
			enctimeout()		

#Create direction logic method			
def wheretogo():
		
	if result.safe == "left":
		print "Turning left\n"
		left()
		enc_tgt(1,1,4)	#reset encoder target
		enctimeout()
			
	elif result.safe == "right":
		print "Turning right\n"
		right()
		enc_tgt(1,1,4)	#reset encoder target
		enctimeout()
				
	elif result.safe == "fwd":
		print "Rescanning, last scan said fwd safe\n"
		enable_servo()
		us_map()
		disable_servo()
		uhohW = uhohW + 1
		if uhohW == 3:
			stop()
			uhohW = 0
			print "Wheretogo got stuck, stopping and blinking LEDs\n"
			while (time.time() - starttime) < maxtime:
				led_on()
				time.sleep(.5)
				led_off()
					
		wheretogo()
	else:
		print "Going backward\n"
		motor_bwd()
		enc_tgt(1,1,18)	#reset encoder target
		time.sleep(.2)
		enctimeout()

#Create status checking method				
def status(data):
	data = 0	#Reset status bit
	buttonpressL = wp.digitalRead(leftTouch)
	buttonpressR = wp.digitalRead(rightTouch)
	beamF = wp.digitalRead(midIR)
	#beamB = wp.digitalRead(bakIR)
	
	if beamF:
		data = badIRf
		print "Front IR broken.\n"
		return False
	
	#if beamB:
		#data = badIRb
		#print "Back IR broken.\n"
		#return False
			
	if not buttonpressL:
		data = badbuttonL
		print "Left button pressed.\n"
		return False
		
	if not buttonpressR:
		data = badbuttonR
		print "Right button pressed.\n"
		return False
		
	return True

#Create lidar map and find closest object/safest distance		
def us_map():
	delay=.02
	debug =0					#True to print all raw values
	num_of_readings=45			#Number of readings to take 
	incr=180/num_of_readings	#increment of angle in servo
	ang_l=[0]*(num_of_readings+1)	#list to hold the angle's of readings
	dist_l=[0]*(num_of_readings+1)	#list to hold the distance at each angle
	x=[0]*(num_of_readings+1)	#list to hold the x coordinate of each point
	y=[0]*(num_of_readings+1)	#list to hold the y coordinate of each point
	maxdisty = 0				#used with container to identify safest distance
	maxdistx = 0				#used with container to identify left or right

	buf=[0]*40
	ang=0
	lim=250		#maximum limit of distance measurement (any value over this which be initialized to the limit value)
	index=0
	sample=2	#Number of samples for each angle (more the samples, better the data but more the time taken)
	print "Getting the data"
	
	while True:
		#Take the readings from the Ultrasonic sensor and process them to get the correct values
		for i in range(sample):
			dist=us_dist(15)
			if dist<lim and dist>=0:
				buf[i]=dist
			else:
				buf[i]=lim
		
		#Find the sample that is most common among all the samples for a particular angle
		max=Counter(buf).most_common()	
		rm=-1
		for i in range (len(max)):
			if max[i][0] <> lim and max[i][0] <> 0:
				rm=max[i][0]
				break
		if rm==-1:
			rm=lim
		
		if debug==1:
			print index,ang,rm
		ang_l[index]=ang
		dist_l[index]=rm
		index+=1

		#Move the servo to the next angle
		servo(ang)	
		time.sleep(delay)
		ang+=incr
		#print ang
		if ang>173:	#due to how breadboard is mounted, need a little clearence so not scanning full 180
			break
	
	#Print the values in a grid of 51x51 on the terminal
	grid_size=51
	
	#Convert the distance and angle to (x,y) coordinates and scale it down
	for i in range(num_of_readings+1):	
		x[i]=(int(dist_l[i]*math.cos(math.pi*(ang_l[i])/180))/10)
		y[i]=int(dist_l[i]*math.sin(math.pi*ang_l[i]/180))/10
		print "X coord is "
		print x[i]
		print "Y coord is "
		print y[i]
		print " \n"
	
	#Use furthest y reading to set x coord
	for i in range(num_of_readings+1):	
		if (y[i] > maxdisty):
			maxdisty = y[i]
			maxdistx = x[i]
			
	#Update result class with safest left/right choice
	if maxdistx < 0:
		result.safe = "left"
	elif maxdistx > 0:
		result.safe = "right"
	elif maxdistx == 0:
		result.safe = "fwd"
	else:
		result.safe = "back"
		
	#Rotate the readings so that it is printed in the correct manner
	for i in range(num_of_readings+1):	
		x[i]=(grid_size/2)-x[i]
		y[i]=(grid_size/2)-y[i]
		
	#Create a grid
	grid = [[0 for a in xrange(grid_size)] for a in xrange(grid_size)] 
	for i in range (num_of_readings+1):
		if dist_l[i]<>lim:
			grid[y[i]][x[i]]=1	
	fence='-'*(grid_size+1)
	
	#Print the map
	print "Map:"
	print fence*2
	for i in range(grid_size/2):
		print "|",
		for j in range (grid_size):
			if (j==grid_size/2)and i==(grid_size/2)-1:
				print 'x',
			elif grid[i][j]==0:
				print ' ',
			else:
				print 'o',
		print "|"
	print fence*2
	result.neardist = min(dist_l) #Set the closest distance in all directions
	return min(dist_l)
	
stop()	#Set default start state to not moving

#Run for set time
while (time.time() - starttime) < maxtime:
	#Check status before moving
	statusval = status(data)
	while not statusval:
		sensors()
		statusval = status(data)	
	#GoPiGo moves forward, stops makes a map and again moves forward
	enable_com_timeout(2000)
	enc_tgt(1,1,18)	#Set encoder targetting. Stop after 4 rotations of both the wheels
	fwd()
	time.sleep(.2)
	enctimeout()
	#Check status after moving
	statusval = status(data)
	while not statusval:
		sensors()
		statusval = status(data)
	time.sleep(1)
	enable_servo()
	us_map()
	print "Neardist is "
	print result.neardist
	print " \n"
	print "Safe is "
	print result.safe
	print " \n"
	if result.neardist < min_distance:	#If any obstacle is closer than 20 cm, find where to go
		wheretogo()
		
	disable_servo()
	
print "Time limit reached, stopping"
stop()
