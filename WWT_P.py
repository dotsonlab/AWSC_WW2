import time
import schedule  #will have to pip install schedule (library)
import threading
import Tkinter as Tk
import serial
import sys
import os.path

ser=serial.Serial('/dev/ttyACM0',9600)

def RunAndLog():
    while not e.isSet():
        global message
        schedule.run_pending()
    	message=ser.readline()  #how do we throw away garbled serial transmissions?
    	#print(message)

        parsedMessage = message.split('\t')

        if parsedMessage[0] == 'TANKD:':
            parsedMessage.remove('TANKD:')
            message = parsedMessage
            TankLevel()

        elif parsedMessage[0] == 'PRESSD:':
            parsedMessage.remove('PRESSD:')
            message = parsedMessage
            Pressure()

        elif parsedMessage[0] == 'IFLOWD:':
            parsedMessage.remove('IFLOWD:')
            message = parsedMessage
            iFlow()

        elif parsedMessage[0] == 'TFLOWD:':
            parsedMessage.remove('TFLOWD:')
            message = parsedMessage
            tFlow()

        elif parsedMessage[0] == 'TandPD':
            parsedMessage.remove('TandPD')
            message = parsedMessage
            TandP()

        elif parsedMessage[0] == 'RelayD':
            parsedMessage.remove('RelayD')
            message = parsedMessage
            Relays()

        elif parsedMessage[0] == '1valveD':
            parsedMessage.remove('1valveD')
            message = parsedMessage
            Valves1()

        elif parsedMessage[0] == '2valveD':
            parsedMessage.remove('2valveD')
            message = parsedMessage
            Valves2()

def Relays():
    #get current time
	now=time.localtime(time.time())
	currentmonth=now.tm_mon
	currentday=now.tm_mday
	currentyear=now.tm_year
	filename = "{0}_{1}_{2}_WWT-Relays.txt".format(currentyear, currentmonth, currentday)

        if not (os.path.isfile(filename)):
            file=open(filename,"w")
            file.write("P\tBUB\tO3\tO3pump\tUV\ttime\n")
            file.flush()
            file.close()

    #open file and save serial data from arduino
	file=open(filename,"a")
	#message=ser.readline()
	#print('\t'.join(message))
	file.write('\t'.join(message))
	file.flush()
	file.close()

def Valves1():
    #get current time
	now=time.localtime(time.time())
	currentmonth=now.tm_mon
	currentday=now.tm_mday
	currentyear=now.tm_year
	filename = "{0}_{1}_{2}_WWT-Valves1.txt".format(currentyear, currentmonth, currentday)

        if not (os.path.isfile(filename)):
            file=open(filename,"w")
            file.write("NFPOT\tNFF\tNFFT\tGW\tCFF\ttime\n")
            file.flush()
            file.close()

    #open file and save serial data from arduino
	file=open(filename,"a")
	#message=ser.readline()
	#print('\t'.join(message))
	file.write('\t'.join(message))
	file.flush()
	file.close()

def Valves2():
    #get current time
	now=time.localtime(time.time())
	currentmonth=now.tm_mon
	currentday=now.tm_mday
	currentyear=now.tm_year
	filename = "{0}_{1}_{2}_WWT-Valves2.txt".format(currentyear, currentmonth, currentday)

        if not (os.path.isfile(filename)):
            file=open(filename,"w")
            file.write("ROPOT\tROF\tROFT\tWWT\tWASTE\ttime\n")
            file.flush()
            file.close()

    #open file and save serial data from arduino
	file=open(filename,"a")
	#message=ser.readline()
	#print('\t'.join(message))
	file.write('\t'.join(message))
	file.flush()
	file.close()

def TankLevel():
    #get current time
	now=time.localtime(time.time())
	currentmonth=now.tm_mon
	currentday=now.tm_mday
	currentyear=now.tm_year
	filename = "{0}_{1}_{2}_WWT-TankLevels.txt".format(currentyear, currentmonth, currentday)

        if not (os.path.isfile(filename)):
            file=open(filename,"w")
            file.write("WW\tROF\tNFF\tGW\tWASTE\ttime\n")
            file.flush()
            file.close()

    #open file and save serial data from arduino
	file=open(filename,"a")
	#message=ser.readline()
	#print('\t'.join(message))
	file.write('\t'.join(message))
	file.flush()
	file.close()

def Pressure():
    #get current time
	now=time.localtime(time.time())
	currentmonth=now.tm_mon
	currentday=now.tm_mday
	currentyear=now.tm_year
	filename = "{0}_{1}_{2}_WWT-Pressure.txt".format(currentyear, currentmonth, currentday)

        if not (os.path.isfile(filename)):
            file=open(filename,"w")
            file.write("F\tC1\tC2\tNFR\tROR\ttime\n")
            file.flush()
            file.close()

    #open file and save serial data from arduino
	file=open(filename,"a")
	#message=ser.readline()
	#print('\t'.join(message))
	file.write('\t'.join(message))
	file.flush()
	file.close()

def iFlow():
    #get current time
	now=time.localtime(time.time())
	currentmonth=now.tm_mon
	currentday=now.tm_mday
	currentyear=now.tm_year
	filename = "{0}_{1}_{2}_WWT-iFlow.txt".format(currentyear, currentmonth, currentday)

        if not (os.path.isfile(filename)):
            file=open(filename,"w")
            file.write("C\tNFP\tNFR\tROP\tROR\ttime\n")
            file.flush()
            file.close()

    #open file and save serial data from arduino
	file=open(filename,"a")
	#message=ser.readline()
	#print('\t'.join(message))
	file.write('\t'.join(message))
	file.flush()
	file.close()

def tFlow():
    #get current time
	now=time.localtime(time.time())
	currentmonth=now.tm_mon
	currentday=now.tm_mday
	currentyear=now.tm_year
	filename = "{0}_{1}_{2}_WWT-tFlow.txt".format(currentyear, currentmonth, currentday)

        if not (os.path.isfile(filename)):
            file=open(filename,"w")
            file.write("C\tNFP\tNFR\tROP\tROR\ttime\n")
            file.flush()
            file.close()

    #open file and save serial data from arduino
	file=open(filename,"a")
	#message=ser.readline()
	#print('\t'.join(message))
	file.write('\t'.join(message))
	file.flush()
	file.close()

def TandP():
    #get current time
	now=time.localtime(time.time())
	currentmonth=now.tm_mon
	currentday=now.tm_mday
	currentyear=now.tm_year
	filename = "{0}_{1}_{2}_WWT-TandPD.txt".format(currentyear, currentmonth, currentday)

        if not (os.path.isfile(filename)):
            file=open(filename,"w")
            file.write("OUT\tAC\tDC\tPWRR\tPWRB\ttime\n")
            file.flush()
            file.close()

    #open file and save serial data from arduino
	file=open(filename,"a")
	#message=ser.readline()
	#print('\t'.join(message))
	file.write('\t'.join(message))
	file.flush()
	file.close()

def _RegularDay(t):
    print t, "Regular Day"
    ser.write('D\n')

def _WasteDay(t):
    print t, "Waste Day"
    ser.write('W\n')

def _CartridgeFilter():
    print "Cartridge Filter"
    ser.write('V\n')

def _CartridgeFilterWO():
    print "Cartridge Filter without Rinse"
    ser.write('C\n')

def _Nanofilter():
    print "Nanofilter"
    ser.write('M\n')

def _NanofilterWO():
    print "Nanofilter without Rinse"
    ser.write('N\n')

def _ReverseOsmosis():
    print "Reverse Osmosis"
    ser.write('T\n')

def _ReverseOsmosisWO():
    print "Reverse Osmosis without Rinse"
    ser.write('R\n')

def _quit():
    print 'Exiting...'
    e.set()
    thread.join() #wait for the thread to finish
    root.quit()
    root.destroy()

root = Tk.Tk()
root.wm_title("WWT Control")
RegButton = Tk.Button(master=root, text='Regular Day', command=_RegularDay)
RegButton.grid(row=0, column=4)
RegButton.config(height = 5, width = 16)

WasteButton = Tk.Button(master=root, text='Waste Day', command=_WasteDay)
WasteButton.grid(row=1, column=4)
WasteButton.config(height = 5, width = 16)

QuitButton = Tk.Button(master=root, text='Quit', command=_quit) #the quit button
QuitButton.grid(row=5, column=4)
QuitButton.config(height = 5, width = 16)

CFButton = Tk.Button(master=root, text='CF', command=_CartridgeFilter)
CFButton.grid(row=0, column=0)
CFButton.config(height = 5, width = 16)

CFWOButton = Tk.Button(master=root, text='CF wo R', command=_CartridgeFilterWO)
CFWOButton.grid(row=0, column=2)
CFWOButton.config(height = 5, width = 16)

NFButton = Tk.Button(master=root, text='NF', command=_Nanofilter)
NFButton.grid(row=1, column=0)
NFButton.config(height = 5, width = 16)

NFWOButton = Tk.Button(master=root, text='NF wo R', command=_NanofilterWO)
NFWOButton.grid(row=1, column=2)
NFWOButton.config(height = 5, width = 16)

ROButton = Tk.Button(master=root, text='RO', command=_ReverseOsmosis)
ROButton.grid(row=2, column=0)
ROButton.config(height = 5, width = 16)

ROWOButton = Tk.Button(master=root, text='RO wo R', command=_ReverseOsmosisWO)
ROWOButton.grid(row=2, column=2)
ROWOButton.config(height = 5, width = 16)

schedule.every().monday.at("9:00").do(_RegularDay,'It is 9:00AM, Scheduled Treatment: ')
schedule.every().tuesday.at("9:00").do(_RegularDay,'It is 9:00AM, Scheduled Treatment: ')
schedule.every().wednesday.at("9:00").do(_RegularDay,'It is 9:00AM, Scheduled Treatment: ')
schedule.every().thursday.at("9:00").do(_RegularDay,'It is 9:00AM, Scheduled Treatment: ')
schedule.every().friday.at("9:00").do(_RegularDay,'It is 9:00AM, Scheduled Treatment: ')
schedule.every().saturday.at("9:00").do(_RegularDay,'It is 9:00AM, Scheduled Treatment: ')
schedule.every().sunday.at("9:00").do(_RegularDay,'It is 9:00AM, Scheduled Treatment: ')

thread = threading.Thread(target=RunAndLog, args=())
e = threading.Event()
thread.start()
root.mainloop()
