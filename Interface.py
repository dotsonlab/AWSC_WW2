"""Blah blah blah."""
import os.path
import schedule
import serial
import threading
import time
import tkMessageBox
import Tkinter as tk
from functools import partial

TITLE_FONT = ("Helvetica", 17, "bold")
BUTTON_FONT = ("Helvetica", 14)
NOTIFICATION_FONT = ("Helvetica", 14)

# These are available errors returned as strings.
# To retrieve the string call in an update function: <variable> = lookUpError.<error function>(currentData)
# Then output the string in appropriate place within update functions
class ErrorCheck:
    def __init__(self):
        self.errorTime = {}
        self.error = {}
        self.wasteTank = 0
        self.amountToAdd = 0
        self.reset = False
        self.error[0] = False   # checkEmptyWashTank
        self.error[1] = False   # checkFullWasteTank
        self.error[2] = False   # checkFilters

    def checkEmptyWashTank(self,currentData):
        nowAsString = time.strftime('%H:%M %m/%d/%Y')
        if int(currentData['TANKD:'][0]) is 0:
            if self.error[0] is False:
                self.error[0] = True
                self.errorTime[0] = nowAsString
            return (self.errorTime[0] + ' NOTICE: add 1 gallon to Wash Tank')


        if self.amountToAdd > 0 and not self.reset:
            if int(currentData['TANKD:'][4]) < self.wasteTank:
                self.amountToAdd = self.amountToAdd + self.wasteTank - int(currentData['TANKD:'][4])
            self.wasteTank = currentData['TANKD:'][4]
            return (self.errorTime[0] + ' NOTICE: add '+ str(self.amountToAdd) +' gallon to Wash Tank')
        elif int(currentData['TANKD:'][4]) < self.wasteTank and not self.reset:
            if self.error[0] is False:
                self.error[0] = True
                self.errorTime[0] = nowAsString
                self.amountToAdd = self.amountToAdd +self.wasteTank - int(currentData['TANKD:'][4])
                self.wasteTank = int(currentData['TANKD:'][4])
            return (self.errorTime[0] + ' NOTICE: add '+ str(self.amountToAdd) +' gallon to Wash Tank')
        else:
            self.wasteTank = int(currentData['TANKD:'][4])
            self.reset = False
            self.error[0] = False
            self.amountToAdd = 0
            return ''

    # Used by add water button. Assumes full amount has been added
    def resetButton(self):
        self.reset = True

    # If the Waste tank reaches 40 gallons a message appears to empty it
    def checkFullWasteTank(self, currentData):
        nowAsString = time.strftime('%H:%M %m/%d/%Y')
        if int(currentData['TANKD:'][4]) > 40:
            if self.error[1] is False:
                self.error[1] = True
                self.errorTime[1] = nowAsString
            return self.errorTime[1] + ' NOTICE: Waste Tank needs to be Emptied'
        else:
            self.error[1] = False
            return ''

    # Checks for conditions of when to replace filters
    def checkFilters(self, currentData):
        nowAsString = time.strftime('%H:%M %m/%d/%Y')
        errorString = ''
        diffpressures = {}
        diffpressures[0] = int(currentData['PRESSD:'][0]) - int(currentData['PRESSD:'][1])
        diffpressures[1] = int(currentData['PRESSD:'][1]) - int(currentData['PRESSD:'][2])
        diffpressures[2] = int(currentData['PRESSD:'][2])
        diffpressures[3] = int(currentData['PRESSD:'][0]) - int(currentData['PRESSD:'][3])
        diffpressures[4] = int(currentData['PRESSD:'][0]) - int(currentData['PRESSD:'][4])
        if diffpressures[2] >= 60:
            if self.error[2] is False:
                self.error[2] = True
                self.errorTime[2] = nowAsString
            errorString = errorString + self.errorTime[2] + ' NOTICE:\nReplace 0.2um Filter\n'
        if diffpressures[1] >= 60:
            if self.error[2] is False:
                self.error[2] = True
                self.errorTime[2] = nowAsString
            errorString = errorString + self.errorTime[2] + ' NOTICE:\nReplace 0.45um Filter\n'
        if diffpressures[0] - 15 >= 15:
            if self.error[2] is False:
                self.error[2] = True
                self.errorTime[2] = nowAsString
            errorString = errorString + self.errorTime[2] + ' NOTICE:\nReplace 1.00um Filter\n'
        if self.error[2]:
            return errorString
        else:
            return ''

    # NOTE: set fatal error conditions here!
    def redError(self, currentData):

        return 'ERROR: Maintenance Required'

class Interface(tk.Tk):
    """Interface GUI."""

    def __init__(self):
        """Blah blah blah."""
        tk.Tk.__init__(self)

        # Force fullscreen, uncomment on Raspberry Pi
        # self.overrideredirect(1)
        # Hide mouse pointer, uncomment on Raspberry Pi
        # self.config(cursor="none")
        self.wm_title("Interface")  # Set application title
        self.resizable(width=False, height=False)  # Disable window resizing
        self.geometry('{}x{}'.format(800, 480))  # Set window size

        # Create base frame
        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)

        # Dictionary to find navigation button objects from page names
        self.buttons = {}
        # Create homeowner button
        self.homeownerButton = tk.Button(container, text="Homeowner",
                                         font=BUTTON_FONT, command=lambda:
                                         self.show_frame(Homeowner))
        # Add homeowner button navigation button dictionary
        self.buttons[Homeowner] = self.homeownerButton
        self.homeownerButton.place(x=0, y=430, width=400, height=50)
        # Create advanced user button
        self.advUsrButton = tk.Button(container, text="Maintenance",
                                      font=BUTTON_FONT, command=lambda:
                                      self.show_frame(AdvUser))
        # Add advanced user button navigation button dictionary
        self.buttons[AdvUser] = self.advUsrButton
        self.advUsrButton.place(x=400, y=430, width=400, height=50)

        self.frames = {}  # Dictionary to find frame objects from page names

        # Create large frames (Homeowner AdvUser)
        for F in (AdvUser, Homeowner):
            frame = F(container, self)
            self.frames[F] = frame
            frame.place(x=0, y=0, width=800, height=430)

        # Create small frames (Option, PowerAndTemp, ...)
        for F in (Option, PowerAndTemp, FlowAndPressure,
                  WaterLevel, SystemStatus):
            frame = F(self.frames[AdvUser], self)
            self.frames[F] = frame
            frame.place(x=0, y=50, width=800, height=380)

        self.show_frame(AdvUser)  # Show default frame
        self.after(5000, self.updateScreen)

    def updateScreen(self):
        """."""
        app.frames[WaterLevel].update()
        app.frames[PowerAndTemp].update()
        app.frames[FlowAndPressure].update()
        app.frames[SystemStatus].update()
        app.frames[Homeowner].update()
        self.after(1000, self.updateScreen)

    # Bring selected frame to the front and enable/disable relevant buttons
    def show_frame(self, page_name):
        """Blah blah blah."""
        self.frames[page_name].tkraise()  # Raise frame to top
        if page_name == Homeowner:
            # If Homeowner is selected reenable advUserButton
            self.buttons[AdvUser].config(state="normal", bg='grey')
        elif page_name == AdvUser:
            # If AdvUser is selected reenable homeownerButton
            self.buttons[Homeowner].config(state="normal", bg='grey')
        else:
            for F in (Option, PowerAndTemp, FlowAndPressure,
                      WaterLevel, SystemStatus):
                if page_name != F:
                    # Enable unselected AdvUser buttons
                    self.buttons[F].config(state="normal", bg='grey')
        # Disable selected button
        self.buttons[page_name].config(state="disabled",
                                       disabledforeground='black', bg='grey95')


class Homeowner(tk.Frame):
    """Homeowner frame."""

    def __init__(self, parent, controller):
        """Blah blah blah."""
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.displayHomeowner(parent, controller)

    # display function seperate for updating purposes
    def displayHomeowner(self, parent, controller):
        data = currentData['TANKD:']
        # label = tk.Label(self, text="Homeowner Frame", font=TITLE_FONT)
        # label.pack(side="top", fill="x", pady=10)
        self.renderer = Renderer(self, 800, 420)

        # TO DO: save nowAsString in event of restart restarted
        washTank = lookUpError.checkEmptyWashTank(currentData)
        self.addwashwater = self.renderer.drawFlag(self, 10, 30, 30, 'blue', 'green', washTank)
        wasteTank = lookUpError.checkFullWasteTank(currentData)
        self.emtpytank = self.renderer.drawFlag(self, 10, 80, 30, 'yellow', 'green', wasteTank)

        filterReplace = lookUpError.checkFilters(currentData)
        self.filters = self.renderer.drawFlag(self, 10, 130, 30, 'yellow', 'green', filterReplace)
        # TO DO: if statement for errors in system.
        # May want to activiate a "shut down" mode here
        redErrorMessage = lookUpError.redError(currentData)
        self.redError = tk.Label(self, text=redErrorMessage,
                         font=TITLE_FONT, fg='red')
        self.redError.place(x=10, y=350, height=25)

        # display water levels on Homeowner page
        # note: these are also under class WaterLevel
        self.list = {}
        self.list[0] = self.renderer.drawTank(self, 410, 190, 85, data[0], "Wash")
        self.list[1] = self.renderer.drawTank(self, 520, 190, 85, data[1], "Grey\nWater")
        self.list[2] = self.renderer.drawTank(self, 630, 190+80, 45, data[4], "Waste\nWater")
        self.resetButton = tk.Button(self, text='Added\nWater', font=BUTTON_FONT,
                                     command=lambda: lookUpError.resetButton(), bg='grey')
        self.resetButton.place(x=670, y=50, width=100, height=50)

    def update(self):
        """."""
        self.renderer.coords(self.list[0][0], 412, 190+2*(85-int(currentData['TANKD:'][0])), 509, 364)
        self.list[0][1].config(text=str(int(currentData['TANKD:'][2]))+'/85' + 'gal')
        self.renderer.coords(self.list[1][0], 522, 190+2*(85-int(currentData['TANKD:'][1])), 619, 364)
        self.list[1][1].config(text=str(int(currentData['TANKD:'][3]))+'/85' + 'gal')
        self.renderer.coords(self.list[2][0], 632, 270+2*(45-int(currentData['TANKD:'][4])), 729, 364)
        self.list[2][1].config(text=str(int(currentData['TANKD:'][4]))+'/45' + 'gal')
        # ERROR CHECKING see function at top
        washTank = lookUpError.checkEmptyWashTank(currentData)
        self.addwashwater.config(text = washTank)
        wasteTank = lookUpError.checkFullWasteTank(currentData)
        self.emtpytank.config(text = wasteTank)
        filterReplace = lookUpError.checkFilters(currentData)
        self.filters.config(text=filterReplace)
        redErrorMessage = lookUpError.redError(currentData)
        self.redError.config(text=redErrorMessage)


class AdvUser(tk.Frame):
    """Advanced user frame."""

    def __init__(self, parent, controller):
        """Blah blah blah."""
        tk.Frame.__init__(self, parent)
        controller = controller

        # Create AdvUser navigation buttons
        optionButton = tk.Button(self, text="Options",
                                 font=BUTTON_FONT, command=lambda:
                                 controller.show_frame(Option), bg='grey')
        # Add to navigation button dictionary
        controller.buttons[Option] = optionButton
        optionButton.place(x=640, y=0, width=160, height=50)

        tempButton = tk.Button(self, text="Power and \n Temperature",
                               font=BUTTON_FONT, command=lambda:
                               controller.show_frame(PowerAndTemp), bg='grey')
        # Add to navigation button dictionary
        controller.buttons[PowerAndTemp] = tempButton
        tempButton.place(x=480, y=0, width=160, height=50)

        flowButton = tk.Button(self, text="Flow and \n Pressure", font=BUTTON_FONT, command=lambda:
                               controller.show_frame(FlowAndPressure), bg='grey')
        # Add to navigation button dictionary
        controller.buttons[FlowAndPressure] = flowButton
        flowButton.place(x=320, y=0, width=160, height=50)

        waterButton = tk.Button(self, text="Tank Level", font=BUTTON_FONT, command=lambda:
                                controller.show_frame(WaterLevel), bg='grey')
        # Add to navigation button dictionary
        controller.buttons[WaterLevel] = waterButton
        waterButton.place(x=160, y=0, width=160, height=50)

        statusButton = tk.Button(self, text="Valves and\nRelays", font=BUTTON_FONT, command=lambda:
                                 controller.show_frame(SystemStatus), bg='grey')
        # statusButton is selected by default
        statusButton.config(state="disabled", disabledforeground='black', bg='grey95')
        # Add to navigation button dictionary
        controller.buttons[SystemStatus] = statusButton
        statusButton.place(x=0, y=0, width=160, height=50)


class Option(tk.Frame):
    """Options frame."""

    def __init__(self, parent, controller):
        """These are from Dotson Labs code."""
        tk.Frame.__init__(self, parent)
        self.controller = controller

        RegButton = (tk.Button(self, text='Regular Day',
                     command=lambda:
                     partial(handler.manualCommand('RegularDay\n'))))
        RegButton.grid(row=0, column=4)
        RegButton.config(height=5, width=16)

        WasteButton = (tk.Button(self, text='Waste Day',
                       command=lambda:
                       partial(handler.manualCommand('FullWasteDay\n'))))
        WasteButton.grid(row=1, column=4)
        WasteButton.config(height=5, width=16)

        QuitButton = (tk.Button(self, text='Quit',
                      command=lambda: self.exit()))
        QuitButton.grid(row=5, column=4)
        QuitButton.config(height=5, width=16)

        CFButton = (tk.Button(self, text='CF',
                    command=lambda: handler.manualCommand('CFwithRinse\n')))
        CFButton.grid(row=0, column=0)
        CFButton.config(height=5, width=16)

        CFWOButton = (tk.Button(self, text='CF wo R',
                      command=lambda: handler.manualCommand('CFwoRinse\n')))
        CFWOButton.grid(row=0, column=2)
        CFWOButton.config(height=5, width=16)

        NFButton = (tk.Button(self, text='NF',
                    command=lambda: handler.manualCommand('NFwithRinse\n')))
        NFButton.grid(row=1, column=0)
        NFButton.config(height=5, width=16)

        NFWOButton = (tk.Button(self, text='NF wo R',
                      command=lambda: handler.manualCommand('NFwoRinse\n')))
        NFWOButton.grid(row=1, column=2)
        NFWOButton.config(height=5, width=16)

        ROButton = (tk.Button(self, text='RO',
                    command=lambda: handler.manualCommand('ROwithRinse\n')))
        ROButton.grid(row=2, column=0)
        ROButton.config(height=5, width=16)

        ROWOButton = (tk.Button(self, text='RO wo R',
                      command=lambda: handler.manualCommand('ROwoRinse\n')))
        ROWOButton.grid(row=2, column=2)
        ROWOButton.config(height=5, width=16)

    def exit(self):
        """Blah blah blah."""
        result = (tkMessageBox.askquestion('Exit WWT Interface Confirmation',
                                           'Are you sure you want to quit?',
                                           icon='warning'))
        if result == 'yes':
            app.quit()
        else:
            print 'Exit Program Canceled'


class PowerAndTemp(tk.Frame):
    """Power and temperatures frame."""

    def __init__(self, parent, controller):
        """Blah blah blah."""
        tk.Frame.__init__(self, parent)
        self.controller = controller
        label = tk.Label(self, text="Power and Temperatures Frame",
                         font=TITLE_FONT)
        label.pack(side="top", fill="x", pady=10)
        renderer = Renderer(self, 800, 380)
        fullLine = 'Temperature'
        renderer.drawDataOutput(self, 40, 50, fullLine, 250)
        fullLine = 'Power'
        renderer.drawDataOutput(self, 340, 50, fullLine, 250)
        self.tempsandpower = {}
        # temps are: [0] Outside, [1] AC, [2] DC
        # power is: [0] One, [1] Two, [2] Total Power
        for x in range(0, 6):
            if x < 5:
                numberIn = float(currentData['TandPD'][x])
            if x is 0:
                fullLine = 'Outside' + ':  ' + str("%.0f" %numberIn) + '  ' + 'C'
                #print numberIn
            elif x is 1:
                fullLine = 'AC Panel' + ':  ' + str("%.0f" %numberIn) + '  ' + 'C'
            elif x is 2:
                fullLine = 'DC Panel' + ':  ' + str("%.0f" %numberIn) + '  ' + 'C'
            elif x < 5:
                fullLine = str(x) + ':  ' + str("%.1f" %numberIn) + '  ' + 'Amps'
            else:
                numberIn = float(currentData['TandPD'][3])+float(currentData['TandPD'][4])
                fullLine = ('Total Power' + ':  ' + str("%.1f" %numberIn)
                            + '  ' + 'Amps')
            if x < 3:

                self.tempsandpower[x] = renderer.drawDataOutput(self, 20, x * 40 + 100, fullLine,250)
            else:
                self.tempsandpower[x] = renderer.drawDataOutput(self, 320, x * 40 - 20, fullLine,250)

    def update(self):
        for x in range(0, 6):
            if x < 5:
                numberIn = float(currentData['TandPD'][x])
            if x is 0:
                fullLine = 'Outside' + ':  ' + str("%.0f" %numberIn) + '  ' + 'C'
                #print numberIn
            elif x is 1:
                fullLine = 'AC Panel' + ':  ' + str("%.0f" %numberIn) + '  ' + 'C'
            elif x is 2:
                fullLine = 'DC Panel' + ':  ' + str("%.0f" %numberIn) + '  ' + 'C'
            elif x < 5:
                fullLine = str(x) + ':  ' + str("%.1f" %numberIn) + '  ' + 'Amps'
            else:
                numberIn = float(currentData['TandPD'][3])+float(currentData['TandPD'][4])
                fullLine = ('Total Power' + ':  ' + str("%.1f" %numberIn)
                            + '  ' + 'Amps')
            self.tempsandpower[x].config(text=fullLine)


class FlowAndPressure(tk.Frame):
    """Flows and pressures frame."""

    def __init__(self, parent, controller):
        """Blah blah blah."""
        tk.Frame.__init__(self, parent)
        self.controller = controller

        label = tk.Label(self, text="Flow and Pressure Frame",
                         font=TITLE_FONT)
        label.pack(side="top", fill="x", pady=10)
        renderer = Renderer(self, 800, 380)

        # labels for pressures and valves
        self.names = []
        self.names.append('Feed')  #0
        self.names.append('CF1')   #1
        self.names.append('CF2')   #2
        self.names.append('NF')    #3
        self.names.append('RO')    #4

        fullLine = 'Pressure'
        renderer.drawDataOutput(self, 20, 50, fullLine,250)

        self.pressures = {}
        for x in range(0, 5):
            fullLine = self.names[x]+':    '+str("%.0f" % float(currentData['PRESSD:'][x]))+'  '+'psi'
            self.pressures[x] = renderer.drawJustifiedLabel(self, 50, x*40+100, fullLine,250, 'w')

        self.names.append('CF1')
        self.names.append('CF2')
        self.names.append('CF3')
        self.names.append('NFX')
        self.names.append('ROX')

        fullLine = 'Differential\nPressure'
        renderer.drawDataOutput(self, 250, 50, fullLine,250)
        self.diffpressuresText = {}
        self.diffpressures = {}
        self.diffpressures[0] = float(currentData['PRESSD:'][0]) - float(currentData['PRESSD:'][1])
        self.diffpressures[1] = float(currentData['PRESSD:'][1]) - float(currentData['PRESSD:'][2])
        self.diffpressures[2] = float(currentData['PRESSD:'][2])
        self.diffpressures[3] = float(currentData['PRESSD:'][0]) - float(currentData['PRESSD:'][3])
        self.diffpressures[4] = float(currentData['PRESSD:'][0]) - float(currentData['PRESSD:'][4])
        for x in range(0, 5):
            fullLine = self.names[x+5]+':    '+str("%.0f" % self.diffpressures[x])+'  '+'psi'
            self.diffpressuresText[x] = renderer.drawJustifiedLabel(self, 300, x*40+100, fullLine,250, 'w')

        self.names.append('CF')
        self.names.append('NFP')
        self.names.append('ROP')
        self.names.append('NFR')
        self.names.append('ROR')

        fullLine = 'Flow'
        renderer.drawDataOutput(self, 450, 30, fullLine,250)
        self.flows = {}
        for x in range(0, 5):
            fullLine = (self.names[x+10] + ':  ' + str("%.2f" % float(currentData['IFLOWD:'][x]))
                        + '  ' + 'gpm')
            self.flows[x] = renderer.drawJustifiedLabel(self, 500, x*40+80, fullLine,250, 'w')

    def update(self):
        for x in range(0, 5):
            fullLine = self.names[x] + ':  ' + str("%.0f" % float(currentData['PRESSD:'][x])) + '  ' + 'psi'
            self.pressures[x].config(text=fullLine)

        self.diffpressures[0] = float(currentData['PRESSD:'][0]) - float(currentData['PRESSD:'][1])
        self.diffpressures[1] = float(currentData['PRESSD:'][1]) - float(currentData['PRESSD:'][2])
        self.diffpressures[2] = float(currentData['PRESSD:'][2])
        self.diffpressures[3] = float(currentData['PRESSD:'][0]) - float(currentData['PRESSD:'][3])
        self.diffpressures[4] = float(currentData['PRESSD:'][0]) - float(currentData['PRESSD:'][4])
        for x in range(0, 5):
            fullLine = self.names[x + 5] + ':  ' + str("%.0f" % self.diffpressures[x]) + '  ' + 'psi'
            self.diffpressuresText[x].config(text = fullLine)
        for x in range(0, 5):
            fullLine = (self.names[x+10] + ':  ' + str("%.2f" % float(currentData['IFLOWD:'][x]))
                        + '  ' + 'gpm')
            self.flows[x].config(text=fullLine)


class WaterLevel(tk.Frame):
    """Water levels frame."""

    def __init__(self, parent, controller):
        """Blah blah blah."""
        tk.Frame.__init__(self, parent)
        self.controller = controller

        # note: 3 of these are also called under class Homeowner

        self.renderer = Renderer(self, 800, 380)
        self.list = {}
        self.list[0] = self.renderer.drawTank(self, 50, 95, 85, currentData['TANKD:'][0], "Wash")
        self.list[1] = self.renderer.drawTank(self, 200, 95, 85, currentData['TANKD:'][1], "Grey\nWater")
        self.list[2] = self.renderer.drawTank(self, 350, 95, 85, currentData['TANKD:'][2], "NF\nFeed")
        self.list[3] = self.renderer.drawTank(self, 500, 95, 85, currentData['TANKD:'][3], "RO\nFeed")
        self.list[4] = self.renderer.drawTank(self, 650, 95+80, 45, currentData['TANKD:'][4], "Waste\nWater")

    def update(self):
        self.renderer.coords(self.list[0][0], 52, 95+2*(85-int(currentData['TANKD:'][0])), 149, 264)
        self.list[0][1].config(text=str(currentData['TANKD:'][0])+'/85' + 'gal')
        self.renderer.coords(self.list[1][0], 202, 95+2*(85-int(currentData['TANKD:'][1])), 299, 264)
        self.list[1][1].config(text=str(currentData['TANKD:'][1])+'/85' + 'gal')
        self.renderer.coords(self.list[2][0], 352, 95+2*(85-int(currentData['TANKD:'][2])), 449, 264)
        self.list[2][1].config(text=str(currentData['TANKD:'][2])+'/85' + 'gal')
        self.renderer.coords(self.list[3][0], 502, 95+2*(85-int(currentData['TANKD:'][3])), 599, 264)
        self.list[3][1].config(text=str(currentData['TANKD:'][3])+'/85' + 'gal')
        self.renderer.coords(self.list[4][0], 652, 174+2*(45-int(currentData['TANKD:'][4])), 749, 264)
        self.list[4][1].config(text=str(currentData['TANKD:'][4])+'/45' + 'gal')


class SystemStatus(tk.Frame):
    """System status frame."""


    def __init__(self, parent, controller):
        """Blah blah blah."""
        tk.Frame.__init__(self, parent)
        self.controller = controller
        label = tk.Label(self, text="System Status Frame", font=TITLE_FONT)
        label.pack(side="top", fill="x", pady=10)
        renderer = Renderer(self, 800, 430)

        renderer.drawDataOutput(self, 100, 20, 'Valves', 250)
        renderer.drawDataOutput(self, 500, 20, 'Relays', 250)

        manualOn = False

        valvedata1 = currentData['1valveD']
        valvedata2 = currentData['2valveD']

        relaydata = currentData['RelayD']

        self.valvePosition = []  # this holds the valve on/off positions
        self.valveButton = []
        self.valveAdjust1 = float(valvedata1[0])
        self.valveAdjust2 = float(valvedata2[0])

        self.active = []  # this holds the relay active/inactive status
        self.relayButton = []

        # initial valves and labels


        valve = []
        valve.append('NF membrane')
        valve.append('NF tank')
        valve.append('GW tank')
        valve.append('CF selection')

        valve.append('RO memberane')
        valve.append('RO feed tank')
        valve.append('Wash tank')
        valve.append('Waste selection')
        for x in range(0, 8):
            if x < 4:
                self.valvePosition.append(int(valvedata1[x+1]))
                yposition = 70 + 60*x
                renderer.drawJustifiedLabel(self, 0, yposition, valve[x],130,'e')
            else:
                self.valvePosition.append(int(valvedata2[x-3]))
                yposition = 70 + 60*(x-4)
                renderer.drawJustifiedLabel(self, 210, yposition, valve[x],150, 'e')
        # relay labels

        relay = []
        relay.append('High Pressure Pump')
        relay.append('Soap Removal')
        relay.append('Ozone Disinfection')
        relay.append('Ozone Pump')
        relay.append('UV Disinfection')

        yposition = 70
        for x in range(0, 5):
            renderer.drawJustifiedLabel(self, 420, yposition, relay[x],250,'e')
            yposition = yposition+55

        # initial relays

        for x in range(0, 5):
            self.active.append(relaydata[x])

        # display relays and valves
        relayButtons = self.displayRelays(manualOn, self.active)
        valveButtons = self.displayValves(manualOn, self.valvePosition)
        # self.displayGlobalManualButton(controller, manualOn, relayButtons, valveButtons)
        self.displayValvesAndRelays(parent, controller, renderer, relayButtons, valveButtons)

    def displayValvesAndRelays(self, parent, controller, renderer, relayButtons, valveButtons):
        # two variable valves
        xposition = 10
        yposition = 300
        fullLine = 'NF Fev: ' + str(float(currentData['1valveD'][0])/1024) + '% OPEN'
        self.valveAdjust1 = renderer.drawDataOutput(self, xposition, yposition, fullLine,250)
        yposition = yposition + 40
        fullLine = 'RO Fev: ' + str(float(currentData['2valveD'][0])/1024) + '% OPEN'
        self.valveAdjust2 = renderer.drawDataOutput(self, xposition, yposition, fullLine,250)
        for x in range(0, len(currentData['RelayD'])):
            self.changeRelayButton(int(currentData['RelayD'][x]), relayButtons)
        for x in range(0, len(currentData['1valveD'])):
            self.changeValveButton(int(currentData['1valveD'][x]), valveButtons)

        """
        #MANUAL ON/OFF button not used
        # set manual button for valves and relay
    def displayGlobalManualButton(self, controller,  manualOn, relayButton, valveButton):

                if manualOn:
                    displayText = 'Manual\nMode Off'
                else:
                    displayText = 'Manual\nMode'
                manualButton = tk.Button(self, text=displayText, font=NOTIFICATION_FONT, bg='grey', state="normal",
                                        command=lambda: self.changeGlobalManual(controller, self.invert(manualOn),
                                        relayButton, valveButton))
                manualButton.place(x=675, y=40, width=100)

            def invert(self, manualIn):
                if manualIn:
                    return False
                else:
                    return True


            def changeGlobalManual(self, controller, manualIn,
                                   relayButton, valveButton):

                self.changeRelayManual(manualIn, relayButton)
                self.changeValveManual(manualIn, valveButton)
                self.displayGlobalManualButton(controller, manualIn, relayButton, valveButton)
        """

    # display functions for button initialization
    def displayRelays(self, manualOn, active):
        """Blah Blah Blah."""
        for index in range(0, 5):
            if manualOn:
                    self.relayButton.append(self.makeRelayButton("normal", active, index, self.relayButton))
            else:
                self.relayButton.append(self.makeRelayButton("disabled", active, index, self.relayButton))
            self.relayButton[index].place(y=70+index*55, x=670, width=100)
        return self.relayButton

    def displayValves(self, manualOn, position):
        """Blah Blah Blah."""
        for index in range(0, 8):
            if manualOn:
                self.valveButton.append(
                    self.makeValveButton("normal", position,
                                         index, self.valveButton))
            else:
                self.valveButton.append(
                    self.makeValveButton("disabled", position,
                                         index, self.valveButton))
            if index < 4:
                self.valveButton[index].place(y=70 + index * 60, x=150, width=50)
            else:
                self.valveButton[index].place(y=70 + (index - 4) * 60, x=380, width=50)
        return self.valveButton

    # logic to interact with serial relays should go here
    def makeRelayButton(self, stateIn, active, index, relayButtonIn):
        """Blah Blah Blah."""
        if active[index] > 0:
            color = 'green'
            textIn = 'ACTIVE'
        else:
            color = 'light grey'
            textIn = 'RUN'
        return tk.Button(self, text=textIn, font=NOTIFICATION_FONT, bg=color, state=stateIn, fg='black',
                        disabledforeground='black', command=lambda: self.changeRelayButton(index, active[index]))

    # logic to interact with serial valves should go here
    def makeValveButton(self, stateIn, position, index, valveButton):
        """Blah Blah Blah."""
        if position[index] > 0:
            color = 'green'
            textIn = 'ON'
        else:
            color = 'orangered'
            textIn = 'OFF'
        return tk.Button(self, text=textIn, font=NOTIFICATION_FONT, bg=color, state=stateIn,
                         command=lambda: self.changeValveButton(index, position[index]), disabledforeground='black')

    # when manual is on, these allow the buttons to be turned on or off
    def changeRelayButton(self, index, stateIn):
        """Blah Blah Blah."""
        if self.active[index] > 0 or stateIn > 0:
            self.active[index] = 0
            self.relayButton[index].config(bg='light grey', text='RUN')
        else:
            self.active[index] = 1
            self.relayButton[index].config(bg='green', text='ACTIVE')
        return self.active

    def changeValveButton(self, index, stateIn):
        """Blah Blah Blah."""
        if self.valvePosition[index] > 0 or stateIn > 0:
            self.valvePosition[index] = 0
            self.valveButton[index].config(text='OFF', bg='orangered')
        else:
            self.valvePosition[index] = 1
            self.valveButton[index].config(text='ON', bg='green')
        return self.valvePosition

    # these are called by the GlobalManual Button to enable/disable buttons
    def changeRelayManual(self, manualIn, relayButton):
        """Blah Blah Blah."""
        if manualIn:
            for index in range(0, 5):
                relayButton[index].config(state='normal')
        else:
            for index in range(0, 5):
                relayButton[index].config(state='disabled')

    def changeValveManual(self, manualIn, valveButton):
        """Blah Blah Blah."""
        if manualIn:
            for index in range(0, 8):
                valveButton[index].config(state='normal')
        else:
            for index in range(0, 8):
                valveButton[index].config(state='disabled')

    def update(self):
        for x in range(0, 5):
            if int(currentData['1valveD'][x+1]) < 1:
                self.valveButton[x].config(text='OFF', bg='orangered')
            else:
                self.valveButton[x].config(text='ON', bg='green')
            if int(currentData['2valveD'][x+1]) < 1:
                self.valveButton[x+3].config(text='OFF', bg='orangered')
            else:
                self.valveButton[x+3].config(text='ON', bg='green')
            if int(currentData['RelayD'][x]) < 1:
                self.relayButton[x].config(bg='light grey', text='RUN')
            else:
                self.relayButton[x].config(bg='green', text='ACTIVE')
        fullLine = 'NF Fev: ' + str("%.0f" % (float(currentData['1valveD'][0])/(1024.0/100.0))) + '% OPEN'
        self.valveAdjust1.config(text=fullLine)
        fullLine = 'RO Fev: ' + str("%.0f" % (float(currentData['2valveD'][0])/(1024.0/100.0))) + '% OPEN'
        self.valveAdjust2.config(text=fullLine)


class Renderer(tk.Canvas):
    """Renderer used to draw GUI objects."""

    def __init__(self, parent, width, height):
        """."""
        tk.Canvas.__init__(self, parent)
        self.place(x=0, y=0, width=width, height=height)

    def drawFlag(self, parent, x, y, size, color0, color1, name):
        """Draw a circle flag object with a label to the right."""
        self.create_oval(x+4, y+4, x+size+4, y+size+4, width=2, fill=color0)
        label = tk.Label(parent, text=name, font=NOTIFICATION_FONT)
        label.place(x=x+size+5, y=y+4)
        return label

    def drawTank(self, parent, x, y, size, fill, name):
        """Draw a tank GUI object with a label below and # gallons above."""
        self.create_rectangle(x, y-2, x+100, y+size*2, width=3, fill='grey')
        infill = self.create_rectangle(x+2, y+2*(size-fill), x+99, y+size*2-1,
                                     width=0, fill='midnight blue')

        gals = tk.Label(self,
                        text=str(int(fill))+'/'+str(size)
                        + 'gal', font=NOTIFICATION_FONT)
        gals.place(x=x, y=y-25, width=100, height=20)
        label = tk.Label(self, text=name, font=TITLE_FONT)
        label.place(x=x, y=y+size*2+5, width=100)
        updateObjects = [infill, gals]
        return updateObjects

    def drawDataOutput(self, parent, x, y, fullLine, width):
        """Draw label of something at a value with units."""
        label = tk.Label(parent, text=fullLine, font=NOTIFICATION_FONT)
        label.place(x=x, y=y, width=width, height=40)
        return label

    #same as above but can take in n, ne, e, se, s, sw, w, nw, center
    def drawJustifiedLabel(self, parent, x, y, fullLine, width, justified):
        """Draw label of something at a value with units."""
        label = tk.Label(parent, text=fullLine, font=NOTIFICATION_FONT, anchor=justified)
        label.place(x=x, y=y, width=width, height=40)
        return  label


class DataHandler():
    """Handles serial communications and data managment."""

    def __init__(self):
        """Blah blah blah."""
        self.mesHeadDict = (
            {'TANKD:': {'fileName': 'WWT-TankLevels',
                        'fileHead': 'WW\tROF\tNFF\tGW\tWASTE\ttime\n'},
             'PRESSD:': {'fileName': 'WWT-Pressure',
                         'fileHead': 'F\tC1\tC2\tNFR\tROR\ttime\n'},
             'IFLOWD:': {'fileName': 'WWT-iFlow',
                         'fileHead': 'C\tNFP\tNFR\tROP\tROR\ttime\n'},
             'TFLOWD:': {'fileName': 'WWT-tFlow',
                         'fileHead': 'C\tNFP\tNFR\tROP\tROR\ttime\n'},
             'TandPD': {'fileName': 'WWT-TandPD',
                        'fileHead': 'UT\tAC\tDC\tPWRR\tPWRB\ttime\n'},
             'RelayD': {'fileName': 'WWT-Relays',
                        'fileHead': 'P\tBUB\tO3\tO3pump\tUV\ttime\n'},
             '1valveD': {'fileName': 'WWT-Valves1',
                         'fileHead': 'NFPOT\tNFF\tNFFT\tGW\tCFF\ttime\n'},
             '2valveD': {'fileName': 'WWT-Valves2',
                         'fileHead': 'ROPOT\tROF\tROFT\tWWT\tWASTE\ttime\n'}})

        self.commandDict = (
            {'RegularDay\n':
                {'startMessage': 'Regular Day',
                 'cancelMessage': 'Regular Day Treatment Canceled',
                 'confMessage': 'Regular Treatment Day Confirmation'},
             'FullWasteDay\n':
                {'startMessage': 'Waste Day',
                 'cancelMessage': 'Waste Day Treatment Canceled',
                 'confMessage': 'Waste Treatment Day Confirmation'},
             'HalfWasteDay\n':
                 {'startMessage': 'Half Waste Day',
                  'cancelMessage': '',
                  'confMessage': ''},
             'CFwithRinse\n':
                 {'startMessage': 'Cartridge Filter',
                  'cancelMessage': 'Cartridge Filter Treatment Step Canceled',
                  'confMessage': 'Cartridge Filter Step Confirmation'},
             'CFwoRinse\n':
                 {'startMessage': 'Cartridge Filter without Rinse',
                  'cancelMessage': 'Cartridge Filter without Rinse Treatment \
                                    Step Canceled',
                  'confMessage': 'Cartridge Filter w/o Step Confirmation'},
             'NFwithRinse\n':
                 {'startMessage': 'Nanofilter',
                  'cancelMessage': 'Nanofilter Treatment Step Canceled',
                  'confMessage': 'Nanofilter Step Confirmation'},
             'NFwoRinse\n':
                 {'startMessage': 'Nanofilter without Rinse',
                  'cancelMessage': 'Nanofilter without Rinse Treatment Step \
                                    Canceled',
                  'confMessage': 'Nanofilter w/o Step Confirmation'},
             'ROwithRinse\n':
                 {'startMessage': 'Reverse Osmosis',
                  'cancelMessage': 'Reverse Osmosis Treatment Step Canceled',
                  'confMessage': 'Reverse Osmosis Step Confirmation'},
             'ROwoRinse\n':
                 {'startMessage': 'Reverse Osmosis without Rinse',
                  'cancelMessage': 'Reverse Osmosis without Rinse Treatment \
                                    Step Canceled',
                  'confMessage': 'Reverse Osmosis w/o Step Confirmation'}})

        self.serialCom = serial.Serial('/dev/ttyACM0', 9600)

        schedule.every().monday.at("9:00").do(
            self.scheduledCommand, 'RegularDay\n')
        schedule.every().tuesday.at("9:00").do(
            self.scheduledCommand, 'HalfWasteDay\n')
        schedule.every().wednesday.at("9:00").do(
            self.scheduledCommand, 'RegularDay\n')
        schedule.every().thursday.at("9:00").do(
            self.scheduledCommand, 'RegularDay\n')
        schedule.every().friday.at("09:00").do(
            self.scheduledCommand, 'FullWasteDay\n')
        schedule.every().saturday.at("9:00").do(
            self.scheduledCommand, 'RegularDay\n')
        schedule.every().sunday.at("9:00").do(
            self.scheduledCommand, 'RegularDay\n')

        self.serialListener = threading.Thread(target=self.runAndLog, args=())
        self.serialListenerEvent = threading.Event()
        self.serialListener.start()
        print 'Serial Com Thread Started'

    def runAndLog(self):
        """Blah blah blah."""
        while not self.serialListenerEvent.isSet():
            schedule.run_pending()
            message = self.serialCom.readline()
            parsedMessage = message.split('\t')
            if parsedMessage[0] in self.mesHeadDict:
                dictIndex = parsedMessage[0]
                del parsedMessage[0]
                global currentData
                currentData[dictIndex] = parsedMessage
                message = parsedMessage
                global lookUpError
                lookUpError = ErrorCheck()

                now = time.localtime(time.time())
                fileName = ('{0}_{1}_{2}_'.format(now.tm_year, now.tm_mon,
                                                  now.tm_mday)
                            + self.mesHeadDict[dictIndex]['fileName'] + '.txt')
                # If file is not yet present create then add headers to file
                if not (os.path.isfile(fileName)):
                    file = open(fileName, "w")
                    file.write(self.mesHeadDict[dictIndex]['fileHead'])
                    file.flush()
                    file.close()

                # Open file and save serial data from arduino
                file = open(fileName, "a")
                file.write('\t'.join(message))
                file.flush()
                file.close()
                #print 'Data saved!'

    def manualCommand(self, command):
        """Blah blah blah."""
        result = tk.MessageBox.askquestion(
                    self.commandDict[command]['confMessage'],
                    'Are you sure?', icon='warning')
        if result == 'yes':
            print 'MANUAL ACTIVATED: ' \
                  + self.commandDict[command]['startMessage']
            self.serialCom.write(command)
        else:
            print self.commandDict[command]['cancelMessage']

    def scheduledCommand(self, command):
        """Blah blah blah."""
        print 'It is 9:00AM, Scheduled Treatment: ' \
              + self.commandDict[command]['startMessage']
        self.serialCom.write(command)

    def exit(self):
        """Blah blah blah."""
        print 'Serial Com Thread Closing...'
        self.serialListenerEvent.set()
        self.serialListener.join()  # wait for the thread to finish
        print 'Closed'


if __name__ == "__main__":
    global currentData
    # This caused all to be set to the same so changing one would change all: initialData = [0, 0, 0, 0, 0, 0]
    currentData = ({'TANKD:': [0, 0, 0, 0, 0, 0],
                    'PRESSD:': [0, 0, 0, 0, 0, 0],
                    'IFLOWD:': [0, 0, 0, 0, 0, 0],
                    'TFLOWD:': [0, 0, 0, 0, 0, 0],
                    'TandPD': [0, 0, 0, 0, 0, 0],
                    'RelayD': [0, 0, 0, 0, 0, 0],
                    '1valveD': [0, 0, 0, 0, 0, 0],
                    '2valveD': [0, 0, 0, 0, 0, 0]})
    global lookUpError
    lookUpError = ErrorCheck()

    def testSerial():
        while not testListenerEvent.isSet():
            time.sleep(5)
            for i in range(0, 6):
                if currentData['TANKD:'][i] < 40:
                    currentData['TANKD:'][i] += 5
                else:
                    currentData['TANKD:'][i] = 15
                #print currentData['TANKD:'][i]
                currentData['TandPD'][i] += 1.1
                #print currentData['TandPD'][i]
                currentData['IFLOWD:'][i] += 2.51
                currentData['PRESSD:'][i] += 30.52-10*i
                if currentData['RelayD'][i] is 3:
                    currentData['RelayD'][i] = 0
                    if i > 0:
                        currentData['1valveD'][i] = 0
                        currentData['2valveD'][i] = 0
                else:
                    currentData['RelayD'][i] += 1
                    currentData['1valveD'][i] += 1
                    currentData['2valveD'][i] += 1
            if currentData['1valveD'][0] < 100:
                currentData['1valveD'][0] += i*2.5
                currentData['2valveD'][0] += i*2.4
            else:
                currentData['1valveD'][0] = 0
                currentData['2valveD'][0] = 0
            #print currentData['1valveD'][0]

    #testListener = threading.Thread(target=testSerial, args=())
    #testListenerEvent = threading.Event()
    #testListener.start()

    handler = DataHandler()
    app = Interface()  # Create application
    app.mainloop()
    print 'Exiting...'
    handler.exit()
    #testListenerEvent.set()
    #testListener.join()
    #app.destroy()
