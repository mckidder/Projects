########################################################################
#Specialized driver to find a cliff or falling edge and maneuver
#out of it
#
#Programmer: Matt Kidder
########################################################################

from gopigo import *
import wiringpi as wp
import time
import itertools
import atexit

#Define sensor inputs
wp.wiringPiSetupGpio()

midIR = 21
bakIR = 20

wp.pinMode(midIR, 0)
wp.pinMode(bakIR, 0)

wp.pullUpDnControl(midIR, 2)
wp.pullUpDnControl(bakIR, 2)

#Set time constants/intital state
maxtime = 30			 #run for 30 seconds
starttime = time.time()  #remember when we started

#Create some constants/default values sensors()
data = 0		#Global holder for status info
badIRf = 1		#Set constants to identify each sensor
badIRb = 2
uhohS = 0		#Track total amount of sensor failures
uhohmax = 100		#Set max times one of the sensors is allowed to fail

#Try to adjust so bot moves straight (one motor is bad so we have to slow the other)
set_speed(100)
trim_write(-2)

#Stop bot if program interrupted by keyboard
def exit_handler():
	stop()

atexit.register(exit_handler)

#Create status checking method				
def status():
	global data 
	data = 0	#Reset status bit
	
	#Define individual sensors
	beamF = wp.digitalRead(midIR)
	beamB = wp.digitalRead(bakIR)
	
	#Update what sensor has failed
	if beamF:
		data = badIRf
		print "Front IR broken.\n"
		return False
	
	if beamB:
		data = badIRb
		print "Back IR broken.\n"
		return False
		
	return True

#Create sensor-array logic method			
def sensors():
	stop()
	global uhohS
	
	#See which sensor has failed and try to get out of it
	if data == badIRf:
		print "Rescanning, moving forward is not safe\n"
		uhohS = uhohS + 1
		if uhohS == uhohmax:
			stop()
			uhohS = 0
			print "IR sensors got stuck, stopping and blinking LEDs\n"
			while True:
				led_on(LED_L)
				time.sleep(.5)
				led_off(LED_L)
				led_on(LED_R)
				time.sleep(.5)
				led_off(LED_R)
		bwd()
		time.sleep(.6)
		right()
		time.sleep(.7)
		stop()
			
	if data == badIRb:
		print "Rescanning, moving backware is not safe\n"
		uhohS = uhohS + 1
		if uhohS == uhohmax:
			stop()
			uhohS = 0
			print "IR sensors got stuck, stopping and blinking LEDs\n"
			while True:
				led_on(LED_L)
				time.sleep(.5)
				led_off(LED_L)
				led_on(LED_R)
				time.sleep(.5)
				led_off(LED_R)
		fwd()
		time.sleep(.5)
		right()
		time.sleep(.05)
		stop()
		
 
stop()
while (time.time() - starttime) < maxtime:
	if not status():	#Check statues for safety before/after move
		sensors()
	fwd()
	if not status():
		sensors()
stop()
disable_servo()
		
