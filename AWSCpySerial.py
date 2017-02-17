## import the serial library
import serial

## Boolean variable that will represent
## whether or not the arduino is connected
connected = False

## establish connection to the serial port that your arduino
## is connected to.

locations=['/dev/ttyUSB0','/dev/ttyUSB1','/dev/ttyUSB2','/dev/ttyUSB3']

for device in locations:
    try:
        print "Trying...",device
        ser = serial.Serial(device, 9600)
        break
    except:
        print "Failed to connect on",device

## loop until the arduino tells us it is ready
while not connected:
    serin = ser.read()
    connected = True

## open text file to store the current
##gps co-ordinates received from the rover
text_file = open("position4.txt", 'w')
## read serial data from arduino and
## write it to the text file 'position.txt'
while ser.read():
    x=ser.read()
    print(x)
    if x=="\0":
      text_file.seek(0)
      text_file.truncate()
    text_file.write(x)
    text_file.flush()
## close the serial connection and text file
text_file.close()
ser.close()
