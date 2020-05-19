import threading
import os
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
from tkinter import *
from UART import UART
from Data import Data       #265 linia dla mojego arduino zakomentować,

ser = 0                     ##uart
data = 0                    ##dane
size = 200                  ##ilość danych
win1 = 0                    ##okno ustawień
CurrentConfiguration = 0    ##akutalna konfiguracja bitowo


def main():
    global ser
    global data
    global CurrentConfiguration
    global win1
    ser = UART(baudRate=19200, portName="")
    UART.comFind(ser)  ##jeśli mamy klika portów COM to tego nie używać tylko wpisać linie wyżej
    UART.confSerial(ser)
    CurrentConfiguration = ser.getFirstConf()
    data = Data(size, CurrentConfiguration)
    t1 = threading.Thread(target=SettingWindow)
    t1.start()
    t2 = threading.Thread(target=UARTget)
    t2.start()
    time.sleep(0.12)  #potrzeba czasu na uruchomienie okna u adama napenwo więcej
    win1.printConf(data.str)
    ser.start()
    time.sleep(0.12)
    sendUpdate()
    data.updateSelf(size, CurrentConfiguration)
    win1.settingUpdate(CurrentConfiguration)
    plotter()


def UARTget():
    state = 0
    flag0 = True
    while True:
        if ser.flag is False:
            if data.AccumulationState() and flag0 is True:
                dat = ser.readData(24)  #jeśli włączona akcelracja to 24 bity 6*4 mają się stracić po synchornie
                flag0 = False
            else:
                if data.bit:
                    dat1 = ser.readData(6)
                    data.tabUpdate(data.X, (dat1 & 0xffff00000000) >> 32)
                    data.tabUpdate(data.Y, (dat1 & 0xffff0000) >> 16)
                    data.tabUpdate(data.Z, (dat1 & 0xffff))
                    data.timeMove()
                else:
                    dat1 = ser.readData(3)
                    data.tabUpdate(data.X, (dat1 & 0xff0000) >> 16)
                    data.tabUpdate(data.Y, (dat1 & 0xff00) >> 8)
                    data.tabUpdate(data.Z, (dat1 & 0xff))
                    data.timeMove()
        else:
            val = ser.readData(1)
            if (val == 45) and (state == 0):
                state = 1
            elif (val == 25) and (state == 1):
                state = 2
            elif (val == 45) and (state == 2):
                state = 0
                ser.flag = False
                flag0 = True
                data.tabReset(size, CurrentConfiguration)  ## tak zapobiegawczo czyszvczenie jakby coś mu tam na początke jeszcze przyszło z popfzednigo


def plotter():
    fig = plt.figure(figsize=(20, 12))
    #fig = plt.figure()
    pltInterval = 50
    fig.suptitle('Accelerometer')
    axs = fig.add_subplot(1, 1, 1)
    def animate(self):
        axs.clear()
        axs.set_ylim(-data.yMax, data.yMax)
        plt.xlabel('Time [S]')
        plt.ylabel('Acceleration m/s^2')
        axs.plot(data.time, data.X, color='blue', label='Axis X')
        axs.plot(data.time, data.Y, color='red', label='Axis Y')
        axs.plot(data.time, data.Z, color='green', label='Axis Z')
        plt.legend(loc="upper left")
    ani = animation.FuncAnimation(fig, animate, interval=pltInterval)
    plt.show()


def SettingWindow():
    global win1
    win1 = Setting(ser, size, CurrentConfiguration)
    win1.mainloop()


class Setting:

    def __init__(self, serialConnection, sizeArrey, FirstConfiguration):
        self.window = Tk()
        self.window.iconbitmap('icon.ico')
        self.bit = 0
        self.Accumulation = BooleanVar()
        self.AccumulationVal = StringVar()
        self.TimeInterval = StringVar()
        self.LowNoise = BooleanVar()
        self.numOfSamples = StringVar()
        self.numOfSamples.set(sizeArrey)
        self.Resolution = IntVar()
        self.bitNum = BooleanVar()
        self.window.geometry("500x330")
        self.window.title("Settings")
        self.ser = serialConnection
        self.labels()
        self.buttons()
        self.spinboxs()
        self.radiobuttons()
        self.checkbuttons()
        self.settingUpdate(FirstConfiguration)

    def labels(self):
        # label1 = Label(self.window, text="Settings", font=("arial", 16, "bold")).place(x=150, y=10)
        label2 = Label(self.window, text="Number of samples", font=("arial", 12)).place(x=10, y=60)
        label3 = Label(self.window, text="Number of bits", font=("arial", 12)).place(x=10, y=120)
        label4 = Label(self.window, text="Resolution", font=("arial", 12)).place(x=10, y=150)
        label5 = Label(self.window, text="Time Interval", font=("arial", 12)).place(x=10, y=90)
        label6 = Label(self.window, text="Low Noise", font=("arial", 12)).place(x=10, y=180)
        label7 = Label(self.window, text="Accumulation", font=("arial", 12)).place(x=10, y=210)

    def buttons(self):
        button1 = Button(self.window, text="Send Setting", font=("arial", 12),command=sendUpdate).place(x=360, y=10)
        button2 = Button(self.window, text="Exit", font=("arial", 12), command=endProgram).place(x=450, y=290)
        button3 = Button(self.window, text="Start/Stop", font=("arial", 12), command=self.start).place(x=10, y=10)

    def spinboxs(self):
        spinbox1 = Spinbox(self.window, from_=8, to=64, increment=8,
                           textvariable=self.AccumulationVal, state=NORMAL, width=15, command=sp1).place(x=180, y=210)
        spinbox2 = Spinbox(self.window, from_=2, to=512, increment=2, textvariable=self.TimeInterval,
                           state=NORMAL, command=sp2).place(x=150, y=90)
        spinbox3 = Spinbox(self.window, from_=10, to=300, increment=10, textvariable=self.numOfSamples,
                           state=NORMAL).place(x=150, y=60)

    def radiobuttons(self):
        r1 = Radiobutton(self.window, text='8', variable=self.bitNum, value=False, command=rb12).place(x=150, y=120)
        r2 = Radiobutton(self.window, text='14', variable=self.bitNum, value=True, command=rb12).place(x=180, y=120)
        r3 = Radiobutton(self.window, text='2G', variable=self.Resolution, value=2, command=rb345).place(x=150, y=150)
        r4 = Radiobutton(self.window, text='4G', variable=self.Resolution, value=4, command=rb345).place(x=190, y=150)
        r5 = Radiobutton(self.window, text='8G', variable=self.Resolution, value=8, command=rb345).place(x=230, y=150)

    def checkbuttons(self):
        ch1 = Checkbutton(self.window, state=ACTIVE, variable=self.Accumulation, onvalue=True,
                          offvalue=False, command=cb1).place(x=150, y=210)
        ch2 = Checkbutton(self.window, text='', state=ACTIVE, variable=self.LowNoise, onvalue=True,
                          offvalue=False, command=cb2).place(x=150, y=180)

    def mainloop(self):
        self.window.mainloop()

    def start(self):
        self.ser.start()

    def printConf(self, string):
        string = str(string)
        labelConf = Label(self.window, text=string, justify=LEFT).place(x=360, y=50)

    def get_numOfSamples(self):
        return self.numOfSamples.get()

    def get_TimeInterval(self):
        return self.TimeInterval.get()

    def get_AccumulationVal(self):
        return self.AccumulationVal.get()

    def get_LowNoise(self):
        return self.LowNoise.get()

    def get_Accumulation(self):
        return self.Accumulation.get()

    def get_bitNum(self):
        return self.bitNum.get()

    def get_Resolution(self):
        return self.Resolution.get()

    def settingUpdate(self, Configuration):
        self.Accumulation.set((Configuration & 0x008000) >> 15)
        self.AccumulationVal.set(str(int(((Configuration & 0b0111000000000000) >> 12) * 8 + 8)))
        self.TimeInterval.set((str(((Configuration & 0xff) * 2) + 2)))
        self.LowNoise.set(((Configuration & 0x000100) >> 8))
        self.Resolution.set(2 ** (1 + ((Configuration & 0b11000000000) >> 9)))
        self.bitNum.set((Configuration & 0b100000000000) >> 11)


def sp1():
    global CurrentConfiguration
    CurrentConfiguration = CurrentConfiguration & 0xff8fff
    CurrentConfiguration = CurrentConfiguration | (int((int(win1.get_AccumulationVal()) - 8) / 8) << 12)


def sp2():
    global CurrentConfiguration
    CurrentConfiguration = CurrentConfiguration & 0xffff00
    CurrentConfiguration = CurrentConfiguration | int((int(win1.get_TimeInterval())-2)/2)


def sp3():
    global size
    size = win1.get_numOfSamples()


def cb1():
    global CurrentConfiguration
    CurrentConfiguration = CurrentConfiguration & 0xff7fff
    if win1.get_Accumulation():
        CurrentConfiguration = CurrentConfiguration | 0x008000


def cb2():
    global CurrentConfiguration
    CurrentConfiguration = CurrentConfiguration & 0xfffeff
    if win1.get_LowNoise():
        CurrentConfiguration = CurrentConfiguration | 0x000100


def rb12():
    global CurrentConfiguration
    CurrentConfiguration = CurrentConfiguration & 0xfff7ff
    if win1.get_bitNum():
        CurrentConfiguration = CurrentConfiguration | 0x000800


def rb345():
    global CurrentConfiguration
    CurrentConfiguration = CurrentConfiguration & 0xfff9ff
    if win1.get_Resolution() == 2:
        CurrentConfiguration = CurrentConfiguration & 0xfff9ff
    elif win1.get_Resolution() == 4:
        CurrentConfiguration = CurrentConfiguration | 0x200
    elif win1.get_Resolution() == 8:
        CurrentConfiguration = CurrentConfiguration | 0x400


def sendUpdate():
    global CurrentConfiguration
    global size
    ser.flag = True
    if (CurrentConfiguration & 0x000100) >> 8:                #warunek dla low noise
        if ((CurrentConfiguration & 0x000600) >> 9) == 2:
            CurrentConfiguration = CurrentConfiguration & 0xfff9ff
            CurrentConfiguration = CurrentConfiguration | 0x000200
    size = win1.get_numOfSamples()
    ser.sendSetting(CurrentConfiguration)
    # data.tabReset(size, CurrentConfiguration)
    # data.makeConfigurationString()
    data.updateSelf(size, CurrentConfiguration)
    win1.printConf(data.str)
    win1.settingUpdate(CurrentConfiguration)


def endProgram():
    ser.close()
    os._exit(1)


if __name__ == '__main__':
    main()

