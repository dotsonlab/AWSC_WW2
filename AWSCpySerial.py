import serial
import time, sys
import os.path

now=time.localtime(time.time())
currentmonth=now.tm_mon
currentday=now.tm_mday
currentyear=now.tm_year
filename = "{0}_{1}_{2}_WWT-log.txt".format(currentyear, currentmonth, currentday)

restart = True

ser=serial.Serial('/dev/ttyACM0',9600)

while True:
	try:
        #get current time
        now=time.localtime(time.time())
        pt=time.asctime(now)  #formatted time for file
        currentmonth=now.tm_mon
        currentday=now.tm_mday
        currentyear=now.tm_year

        if (os.path.isfile(filename) and restart):
            #restart ensures that it will only execute this once.
            restart = False
            #restarting the file
            file = open(filename)
            #grab last non-blank line
            last = None
            for line in (line for line in file if line.rstrip('\n')):
                last = line
            #set totalflow to last known value
            totalflow = float(last.split(",")[4])
        elif not (os.path.isfile(filename)):
            #Initial and daily startup
            flow3total = 0
            lastflow3total = 0
            file=open(filename,"a")
            #informative messaging for starting storage file
            print "Opening ",filename, " for appending..."
            print "reading analog inputs and storing data..."
            file.write("Time,GW-Reject,INT-Reject,INT-Product,WW-Product\n")
            #add first column date/time stamp
            file.write(pt)
            file.write(",%f,%f,%f,%f\n" % (AVE_flow1,AVE_flow2,AVE_flow3,AVE_flow4))
            file.close()
	
	message=ser.readline()
	print(message)
	text_file = open("log.txt", 'a')
	text_file.write(message)
	text_file.flush()
	text_file.close()
