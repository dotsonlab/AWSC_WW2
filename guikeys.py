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

def _run():
    print "Work?!"

def _quit():
    print 'Exiting...'
    e.set()
    thread.join() #wait for the thread to finish
    root.quit()
    root.destroy()

root = Tk.Tk()
QuitButton = Tk.Button(master=root, text='Quit', command=_quit) #the quit button
QuitButton.pack(side=Tk.BOTTOM)
RunButton = Tk.Button(master=root, text='Run', command=_run)
RunButton.pack(side=Tk.BOTTOM)

thread = threading.Thread(target=do_something, args=())
e = threading.Event()
thread.start()
root.mainloop()
