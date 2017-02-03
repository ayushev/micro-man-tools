#!/usr/bin/python


#
# _____________________________________________________________________________
#
class Timestamp(object):

    def __init__(self, counter=None, tag=None):
        self.counter = counter
        self.tag = tag

    def applyDelta(self, delta):
        self.counter += delta


class TimestampHex(Timestamp):

    def parseCode(self, code):
        if not isinstance(code, str) or len(code) != 8:
            raise Exception('Invalid code "{0}"'.format(code))
        self.counter = int(code[2:8], base = 16)
        self.tag = int(code[0:2], base = 16)

    def getCode(self):
        return '{0:02X}{1:06X}'.format(self.tag, self.counter % 16**6)

    def __str__(self):
        return '{0:02X}:{1:06X}'.format(self.tag, self.counter % 16**6)


class TimestampBase64(Timestamp):

    def parseCode(self, code):
        if not isinstance(code, str) or len(code) != 8:
            raise Exception('Invalid code "{0}"'.format(code))
        hexStamp = code.decode('base64').encode('hex')
        self.counter = int(hexStamp[0:8], base = 16)
        self.tag = int(hexStamp[8:12], base = 16)

    def getCode(self):
        return '{0:08X}{1:04X}' \
                .format(self.counter % 16**8, self.tag).encode('base64')

    def __str__(self):
        return '{1:04X}:{0:08X}' \
                .format(self.counter % 16**8, self.tag)


#
# _____________________________________________________________________________
#
class TimestampList(object):

    def __init__(self, timestamps=None, base64=False):
        self.timestamps = timestamps
        self.base64 = base64

    def __len__(self):
        return len(self.timestamps) if self.timestamps is not None else 0

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
            ts = TimestampBase64() if self.base64 else TimestampHex()
            ts.parseCode(code)
            if len(self.timestamps):
                if (ts.counter + offset) < self.timestamps[-1].counter:
                    offset += 16**8 if self.base64 else 16**6
            ts.applyDelta(offset)
            self.timestamps += [ts]

    def parseFile(self, filename):
        f = open(filename, 'r')
        lines = [line.strip() for line in f]
        codes = '\n'.join([line for line in lines
                if len(line) == 8 and line[0] != '#'])
        f.close()
        if len(codes) > 0:
            self.parseCodes(codes)

    def getCodes(self):
        return '\n'.join([ts.getCode() for ts in self.timestamps])

    def applyDelta(self, delta):
        for ts in self.timestamps:
            ts.applyDelta(delta)

