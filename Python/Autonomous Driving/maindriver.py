########################################################################
#General purpose driver to test avoidance algorithm and traversal.
#Specialized values set to identify "goals" for moving around my kitchen
#hallway and living room.
#
#Programmer: Matt Kidder
########################################################################

from gopigo import *
import wiringpi as wp
import time
import itertools

#Define sensor inputs
wp.wiringPiSetupGpio()

leftTouch = 19
rightTouch = 26
midIR = 21
bakIR = 20

wp.pinMode(leftTouch, 0)
wp.pinMode(rightTouch, 0)
wp.pinMode(midIR, 0)
wp.pinMode(bakIR, 0)

wp.pullUpDnControl(leftTouch, 2)
wp.pullUpDnControl(rightTouch, 2)
wp.pullUpDnControl(midIR, 2)
wp.pullUpDnControl(bakIR, 2)

#Define iterator to tell between left/right directions
lastdirection = itertools.cycle([0,1]).next

#Create some constants/default values sensors()
data = 0		#Global holder for status info
wentleft = 0	#0 indicates lastdirection() was left
badIRf = 1		#Set constants to identify each sensor
badIRb = 2
badbuttonL = 3
badbuttonR = 4
uhohS = 0		#Track total amount of sensor failures during test run
uhohmax = 5		#Set max times one of the sensors is allowed to fail

#Try to adjust so bot moves straight (one motor is bad so we have to slow the other)
set_speed(120)
trim_write(-2)

#Adjust for minimum acceptable distance (cm) of objects
min_distance = 30

#Stop bot if program interrupted by keyboard
def exit_handler():
	stop()

atexit.register(exit_handler)

#Create status checking method				
def status():
	global data 
	data = 0	#Reset status bit
	
	#Define individual sensors
	buttonpressL = wp.digitalRead(leftTouch)
	buttonpressR = wp.digitalRead(rightTouch)
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
			
	if not buttonpressL:
		data = badbuttonL
		print "Left button pressed.\n"
		return False
		
	if not buttonpressR:
		data = badbuttonR
		print "Right button pressed.\n"
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
		wentleft = lastdirection()
		if wentleft == 1:	#Left was last direction, backup and reposition
			right()
			time.sleep(.7)
			stop()
		else:				#Right was last direction, backup and reposition
			left()
			time.sleep(.7)
			stop()
			
	if data == badIRb:
		print "Rescanning, moving backwards is not safe\n"
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
		time.sleep(.6)
		wentleft = lastdirection()
		if wentleft == 1:	#Left was last direction, backup and reposition
			right()
			time.sleep(.7)
			stop()
		else:				#Right was last direction, backup and reposition
			left()
			time.sleep(.7)
			stop()
	
	if data == badbuttonL:
		print "Rescanning, moving forward is not safe\n"
		uhohS = uhohS + 1
		if uhohS == uhohmax:
			stop()
			uhohS = 0
			print "Button sensors got stuck, stopping and blinking LEDs\n"
			while True:
				led_on(LED_L)
				time.sleep(.5)
				led_off(LED_L)
				led_on(LED_R)
				time.sleep(.5)
				led_off(LED_R)
		bwd()
		time.sleep(.6)
		wentleft = lastdirection()
		if wentleft == 1:	#Left was last direction, backup and reposition
			right()
			time.sleep(.7)
			stop()
		else:				#Right was last direction, backup and reposition
			left()
			time.sleep(.7)
			stop()
			
	if data == badbuttonR:
		print "Rescanning, moving forward is not safe\n"
		uhohS = uhohS + 1
		if uhohS == uhohmax:
			stop()
			uhohS = 0
			print "Button sensors got stuck, stopping and blinking LEDs\n"
			while True:
				led_on(LED_L)
				time.sleep(.5)
				led_off(LED_L)
				led_on(LED_R)
				time.sleep(.5)
				led_off(LED_R)
		bwd()
		time.sleep(.6)
		wentleft = lastdirection()
		if wentleft == 1:	#Left was last direction, backup and reposition
			right()
			time.sleep(.7)
			stop()
		else:				#Right was last direction, backup and reposition
			left()
			time.sleep(.7)
			stop()	

#Create basic obstacle avoidance
def autonomy():
    enc=read_enc_status()
    while not (enc == 0):	#Run until encoder limit reached
        
        #Center servo and get forward distance from ultrasonic
        servo(70)
        enc=read_enc_status()
        dist = us_dist(15)
        
        while not status():	#Check status for safety
			sensors()
			
		#Begin decision making based on distance	
        if dist > min_distance:
            print('Forward is safe', dist)
            fwd()
            while not status():
				sensors()
            time.sleep(1)
        else:
            print('Object in the way', dist)
            stop()
            servo(28)		#Get left distance scan
            time.sleep(1)
            left_dir = us_dist(15)
            time.sleep(1)
            servo(112)		#Get right distance scan
            right_dir = us_dist(15)
            time.sleep(1)
            
			#Choose safest direction
            if left_dir > right_dir and left_dir > min_distance:
                print('Choose left!')
                left()
                while not status():
					sensors()
                lastdirection()
                time.sleep(1)
            elif left_dir < right_dir and right_dir > min_distance:
                print('Choose Right!')
                right()
                while not status():
					sensors()
                lastdirection()
                time.sleep(1)
            else:
                print('No good option, REVERSE!')
                bwd()
                while not status():
					sensors()
                time.sleep(1)

            stop()
            
    return
                
stop()
enable_servo()
servo(70)
time.sleep(1)
enc_tgt(1,1,320)	#Set goal to move down kitchen
autonomy()
print "Phase 1 complete\n"
left()
time.sleep(.8)
stop()
enc_tgt(1,1,60)		#Set goal to move into hallway
autonomy()
print "Phase 2 complete\n"
stop()
left()
time.sleep(.4)
enc_tgt(1,1,325)	#Set goal to move down hallway
autonomy()
print "Phase 3 complete"
left()
time.sleep(.8)
stop()
enc_tgt(1,1,60)		#Set goal to move into kitchen
autonomy()
print "Phase 4 complete"
left()
time.sleep(.8)
stop()
enc_tgt(1,1,18)		#Set goal to move back to start point
autonomy()
print "All phases done"
stop()
disable_servo()
		
