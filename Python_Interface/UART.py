import serial
import serial.tools.list_ports
import time


class UART:

    def __init__(self, baudRate, portName):
        self.baudRate = baudRate
        self.portName = portName
        self.ser = serial.Serial()
        self.s115 = False
        self.conf = 0
        self.flag = True

    def comFind(self):
        ports = serial.tools.list_ports.comports(include_links=False)
        for x in ports:
            self.portName = x.device

    def confSerial(self):
        self.ser.port = self.portName
        self.ser.baudrate = self.baudRate
        self.ser.parity = serial.PARITY_NONE
        self.ser.bytesize = serial.EIGHTBITS
        self.ser.stopbits = serial.STOPBITS_ONE
        self.ser.timeout = None
        print('Trying to connect to: ' + str(self.portName) + ' at ' + str(self.baudRate) + ' BAUD.')
        try:
            self.ser.open()
            print('Connected to ' + str(self.portName) + ' at ' + str(self.baudRate) + ' BAUD.')
        except:
            print("Failed to connect with " + str(self.portName) + ' at ' + str(self.baudRate) + ' BAUD.')
            exit()
        time.sleep(0.12)

    def writeByte(self, value):
        byte = bytearray()
        byte.append(value)
        self.ser.write(byte)

    def start(self):
        self.s115 = not(self.s115)
        send = bytearray()
        send.append(115)
        self.ser.write(send)

    def getFirstConf(self):
        while True:
            self.ser.timeout = 2
            self.writeByte(113)
            StartConf = self.readData(3)
            if ((StartConf & 0xff0000) >> 16) == 250:
                self.conf = StartConf
                self.ser.timeout = None
                return StartConf

    def readData(self, size):
        get = self.ser.read(size=size)
        val: int = int.from_bytes(get, byteorder='big', signed=False)
        return val

    def close(self):
        if self.s115:
            self.start()
        self.ser.close()

    def getSetting(self):
        self.getConf()

    def startFlag(self):
        return self.s115

    def sendSetting(self, Configuration):
        send = bytearray()
        send.append(250)
        send.append((Configuration & 0x00ff00) >> 8)
        send.append(Configuration & 0x0000ff)
        self.ser.write(send)
        time.sleep(0.1)
