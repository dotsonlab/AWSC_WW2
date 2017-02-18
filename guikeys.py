import time
import threading
import Tkinter as Tk

def do_something():
    T0 = time.clock()
    while (time.clock() - T0) < 60 and not e.isSet(): #as long as 60s haven't elapsed
                                                      #and the flag is not set
        #here do a bunch of stuff
        print "Hi Mom!"
        time.sleep(.1)

def _RegularDay():
    print "Regular Day Treatment"
    #ser.write(cmd)

def _WasteDay():
    print "Waste Day Treatment"
    #ser.write(cmd)

def _CartridgeFilter():
    print "Cartridge Filter"
    #ser.write(cmd)

def _CartridgeFilterWO():
    print "Cartridge Filter with Rinse"
    #ser.write(cmd)

def _Nanofilter():
    print "Nanofilter"
    #ser.write(cmd)

def _NanofilterWO():
    print "Nanofilter without Rinse"
    #ser.write(cmd)

def _ReverseOsmosis():
    print "Reverse Osmosis"
    #ser.write(cmd)

def _ReverseOsmosisWO():
    print "Reverse Osmosis without Rinse"
    #ser.write(cmd)

def _quit():
    print 'Exiting...'
    e.set()
    thread.join() #wait for the thread to finish
    root.quit()
    root.destroy()

root = Tk.Tk()
RegButton = Tk.Button(master=root, text='Regular Day Treatment', command=_RegularDay) #the quit button
RegButton.grid(row=0, column=3)
WasteButton = Tk.Button(master=root, text='Waste Day Treatment', command=_WasteDay) #the quit button
WasteButton.grid(row=1, column=3)
QuitButton = Tk.Button(master=root, text='Quit', command=_quit) #the quit button
QuitButton.grid(row=5, column=2)
CFButton = Tk.Button(master=root, text='Cartridge Filter', command=_CartridgeFilter)
CFButton.grid(row=0, column=0)
CFWOButton = Tk.Button(master=root, text='Cartridge Filter without Rinse', command=_CartridgeFilterWO)
CFWOButton.grid(row=0, column=1)
NFButton = Tk.Button(master=root, text='Nanofilter', command=_Nanofilter)
NFButton.grid(row=1, column=0)
NFWOButton = Tk.Button(master=root, text='Nanofilter without Rinse', command=_NanofilterWO)
NFWOButton.grid(row=1, column=1)
ROButton = Tk.Button(master=root, text='Reverse Osomisis', command=_ReverseOsmosis)
ROButton.grid(row=2, column=0)
ROWOButton = Tk.Button(master=root, text='Reverse Osmosis without Rinse', command=_ReverseOsmosisWO)
ROWOButton.grid(row=2, column=1)


thread = threading.Thread(target=do_something, args=())
e = threading.Event()
thread.start()
root.mainloop()
