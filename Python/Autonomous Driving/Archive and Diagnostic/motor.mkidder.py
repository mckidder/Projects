from gopigo import *
import wiringpi as wp
import time
import random
import itertools

wp.wiringPiSetupGpio()

leftTouch = 19
rightTouch = 26
midIR = 21
wp.pinMode(leftTouch, 0)
wp.pinMode(rightTouch, 0)
wp.pinMode(midIR, 0)

wp.pullUpDnControl(leftTouch, 2)
wp.pullUpDnControl(rightTouch, 2)
wp.pullUpDnControl(midIR, 2)
val = -1
min_distance = 70
set_speed(150)

speedswitch = itertools.cycle([-1,0]).next
	
enc_tgt(1,1,60)
while True:
	enc=read_enc_status()
	if enc == 0:	#Stop when target is reached
		stop()
		print "Encoder limit reached"
		trim_write(-1)
		break
	fwd()
	while True:
		val = speedswitch()
		trim_write(val)
		time.sleep(.2)
		enc=read_enc_status()
		if enc == 0:	#Stop when target is reached
			stop()
			print "Encoder limit reached"
			break
		
		

