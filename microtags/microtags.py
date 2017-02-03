#!/usr/bin/python

import sys


#
# _____________________________________________________________________________
#
class Microtag(object):

    def __init__(self):
        self.data = None
        self.id = None

    def __str__(self):
        if self.data is None or self.id is None:
            raise Exception('Undefined microtag')
        return '{0:04X}:{1:08X}'.format(self.id, self.data)

    def isStartTag(self):
        return True if self.id >= 0x0000 and self.id <= 0x3FFF else False

    def isStopTag(self):
        return True if self.id >= 0x4000 and self.id <= 0x7FFF else False

    def isEventTag(self):
        return True if self.id >= 0x8000 and self.id <= 0xBFFF else False

    def isDataTag(self):
        return True if self.id >= 0xC000 and self.id <= 0xEFFF else False

    def isTickBasedTag(self):
        if self.isStartTag() or self.isStopTag() or self.isEventTag():
            return True
        else:
            return False

    def importFromCode(self, code):

        # code must be a string of length 8
        if not isinstance(code, str) or len(code) != 8:
            raise Exception('Invalid code "{0}"'.format(code))

        # extract data and id from base64-encoded tag
        hexCode = code.decode('base64').encode('hex')
        self.data = int(hexCode[0:8], base = 16)
        self.id = int(hexCode[8:12], base = 16)

    def exportCode(self):
        hexCode = '{0:08X}{1:04X}'.format(self.data, self.id)
        return hexCode.encode('base64')


#
# _____________________________________________________________________________
#
class MicrotagList(object):

    def __init__(self, idDict=None, tickToTime=None):
        self.microtags = []
        self.idDict = idDict if idDict is not None else {}
        self.tickToTime = tickToTime

    def __len__(self):
        return len(self.microtags)

    def __str__(self):
        lines = []

        idNameWidth = max([len(s) for s in self.idDict.values()])

        for i, tag in enumerate(self.microtags):
            line = '[{0:{1}}] '.format(i, 5)

            if self.idDict is not None:
                line += '{0:{1}}'.format(self.idDict.get(tag.id, ''), idNameWidth)

            if tag.isStartTag():
                line += 'Start({0:04X}) '.format(tag.id)
            elif tag.isStopTag():
                line += 'Stop ({0:04X}) '.format(tag.id)
            elif tag.isEventTag():
                line += 'Event({0:04X}) '.format(tag.id)
            elif tag.isDataTag():
                line += 'Data ({0:04X}) '.format(tag.id)

            if tag.isTickBasedTag() and self.tickToTime is not None:
                line += '{0} s'.format(self.tickToTime(tag.data))

            lines += [line]

        return '\n'.join(lines)

    def __iter__(self):
        return iter(self.microtags)

    def __getitem__(self, index):
        if isinstance(index, int):
            return self.microtags[index]

    def importFromCodes(self, codes):
        lenBefore = len(self.microtags)
        for code in codes.split('\n'):
            try:
                tag = Microtag()
                tag.importFromCode(code)
                self.microtags += [tag]
            except:
                pass
        # return the number of tags imported
        return len(self.microtags) - lenBefore

    def importFromFile(self, filename):
        f = open(filename, 'r')
        lines = [line.strip() for line in f]
        codes = '\n'.join([line for line in lines
                if len(line) == 8 and line[0] != '#'])
        f.close()
        if len(codes) > 0:
            return self.importFromCodes(codes)
        else:
            return 0

    def printList(self):
        pass


#
# _____________________________________________________________________________
#
def main(argv):

    if len(argv) == 1:
        filename = argv[0]
    else:
        print "Wrong number of arguments. Stopping."
        print "Expecting <input-file>"
        return

    # read input file
    microtags = MicrotagList({ 0x0000 : 'Delay', 0x8000 : 'Event' }, lambda c: c / 84E6)

    try:
        n = microtags.importFromFile(filename)
        print('Imported {0} microtag(s).'.format(n))
    except:
        print "Failed to read/parse input file. Stopping."
        return

    print(microtags)


#
# _____________________________________________________________________________
#
if __name__ == "__main__":
    main(sys.argv[1:]);

