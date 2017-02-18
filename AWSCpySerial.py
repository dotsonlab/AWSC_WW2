import serial
import time, sys
import os.path

now=time.localtime(time.time())
currentmonth=now.tm_mon
currentday=now.tm_mday
currentyear=now.tm_year
filename = "{0}_{1}_{2}_WWT-log.txt".format(currentyear, currentmonth, currentday)

ser=serial.Serial('/dev/ttyACM0',9600)

while True:
	try:
        #get current time
        now=time.localtime(time.time())
        currentmonth=now.tm_mon
        currentday=now.tm_mday
        currentyear=now.tm_year
	filename = "{0}_{1}_{2}_WWT-log.txt".format(currentyear, currentmonth, currentday)
	
	print "Opening ",filename, " for appending..."
	file=open(filename,"a")
	message=ser.readline()
	print(message)
	file.write(message)
	file.flush()
	file.close()
