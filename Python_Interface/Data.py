class Data:

    def __init__(self, size, CurrentConfiguration):
        self.size = size
        self.bit = 0
        self.AccumulationVal = 0
        self.TimeInterval = 0
        self.LowNoise = 0
        self.yMax = 0
        self.Accumulation = 0
        self.CurrentConfiguration = 0
        self.X = []
        self.Y = []
        self.Z = []
        self.time = []
        self.tabInit(self.X)
        self.tabInit(self.Y)
        self.tabInit(self.Z)
        self.timeInit(self.time, False)
        self.str = ''
        self.updateSelf(size, CurrentConfiguration)

    def updateSelf(self, size, CurrentConfiguration):
        self.size = size
        self.CurrentConfiguration = (CurrentConfiguration & 0xffff)
        self.Accumulation = (CurrentConfiguration & 0b1000000000000000) >> 15
        self.AccumulationVal = (CurrentConfiguration & 0b0111000000000000) >> 12
        self.bit = (CurrentConfiguration & 0b0000100000000000) >> 11
        self.yMax = 10 * 2 ** (1 + ((CurrentConfiguration & 0b0000011000000000) >> 9))
        self.LowNoise = (CurrentConfiguration & 0b0000000100000000) >> 8
        self.TimeInterval = (CurrentConfiguration & 0xff)
        self.makeConfigurationString()

    def tabInit(self, tab):
        for y in range(self.size):
            tab.append(0)

    def timeInit(self, tab, state):
        timeInt = - (self.TimeInterval + 1) * 2 / 1000
        if self.Accumulation:
            timeInt = timeInt * (self.AccumulationVal * 8 + 8)
        for y in range(int(self.size)):
            if state:
                tab.insert(0, timeInt * y)
            else:
                tab.append(y * self.TimeInterval)

    def timeMove(self):
        timeInt = (self.TimeInterval + 1) * 2 / 1000
        if self.Accumulation:
            timeInt = timeInt * (self.AccumulationVal * 8 + 8)
        self.time.pop(0)
        self.time.append(self.time[len(self.time) - 1] + timeInt)

    def U2(self, value):
        if self.bit:
            bit = 14
            value = (value & 0xfffc) >> 2
        else:
            bit = 8
        if (value & (1 << (bit - 1))) != 0:
            value = value - (1 << bit)
        return value

    def tabUpdate(self, tab, value):
        value = self.U2(value)
        if len(tab) > 0:
            if self.yMax == 20:
                value = float(value) * 0.15625
            elif self.yMax == 40:
                value = float(value) * 0.3125
            else:
                value = float(value) * 0.625
            if self.bit:
                value = value / 64
            tab.pop(0)
            tab.append(value)

    def tabReset(self, numOfSamples, CurrentConfiguration):
        for y in range(int(self.size)):
            self.X.pop(0)
            self.Y.pop(0)
            self.Z.pop(0)
            self.time.pop(0)
        self.size = numOfSamples
        for y in range(int(self.size)):
            self.X.append(0)
            self.Y.append(0)
            self.Z.append(0)
        self.updateSelf(numOfSamples, CurrentConfiguration)
        self.timeInit(self.time, True)

    def makeConfigurationString(self):
        self.str = 'Current configuration\n'
        if self.Accumulation:
            self.str = self.str + 'Accumulation Enabled   \n'
        else:
            self.str = self.str + 'Accumulation Disabled    \n'
        self.str = self.str + 'Wartość akumulacji ' + str(self.AccumulationVal * 8 + 8) + '   \n'
        if self.bit:
            self.str = self.str + "Bit resolution : 14   \n"
        else:
            self.str = self.str + "Bit resolution : 8    \n"
        self.str = self.str + 'Resolution ' + str(self.yMax / 10) + 'g           \n'
        if self.LowNoise:
            self.str = self.str + 'LowNoise Enabled              \n'
        else:
            self.str = self.str + 'LowNoise Disabled          \n'
        self.str = self.str + 'Interval time ' + str(self.TimeInterval * 2 + 2) + '    \n'

    def bitInversion(self, Value, numberOfBits):
        return (1 << numberOfBits) - 1 - Value

    def AccumulationState(self):
        return self.Accumulation

