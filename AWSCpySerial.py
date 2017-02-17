import serial

ser=serial.Serial('/dev/ttyACM0',9600)

text_file = open("log.txt", 'w')

while True:
	message=ser.readline()
	print(message)
	text_file = open("log.txt", 'a')
	text_file.write(message)
	text_file.flush()
	text_file.close()
