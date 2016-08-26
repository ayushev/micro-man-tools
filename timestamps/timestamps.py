#!/usr/bin/python


#
# _____________________________________________________________________________
#
class Timestamp(object):

    def __init__(self, counter=None, tag=None):
        self.counter = counter
        self.tag = tag

    def __str__(self):
        return self.getCode()

    def parseCode(self, code):
        self.counter = int(code[2:8], base = 16)
        self.tag = int(code[0:2], base = 16)

    def getCode(self):
        return '{0:02X}{1:06X}'.format(self.tag, self.counter % 16**6)

    def applyDelta(self, delta):
        self.counter += delta


#
# _____________________________________________________________________________
#
class TimestampList(object):

    def __init__(self, timestamps=None):
        self.timestamps = timestamps

    def __len__(self):
        return len(self.timestamps)

    def __str__(self):
        return self.getCodes()

    def __iter__(self):
        return iter(self.timestamps)

    def __getitem__(self, index):
        if isinstance(index, int):
            return self.timestamps[index]

    def parseCodes(self, codes):
        offset = 0
        self.timestamps = []
        for code in codes.split('\n'):
            ts = Timestamp()
            ts.parseCode(code)
            if len(self.timestamps):
                if (ts.counter + offset) < self.timestamps[-1].counter:
                    offset += 16**6
            ts.applyDelta(offset)
            self.timestamps += [ts]

    def parseFile(self, filename):
        f = open(filename, 'r')
        lines = [line.strip() for line in f]
        codes = '\n'.join([line for line in lines
                if len(line) == 8 and line[0] != '#'])
        f.close()
        self.parseCodes(codes)

    def getCodes(self):
        return '\n'.join([str(ts) for ts in self.timestamps])

    def applyDelta(self, delta):
        for ts in self.timestamps:
            ts.applyDelta(delta)

