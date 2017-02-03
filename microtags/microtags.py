#!/usr/bin/python

import sys


#
# _____________________________________________________________________________
#
class TextFormatter(object):

    useColor = True

    strColorEnd = '\033[0m'

    @staticmethod
    def makeBoldWhite(s):
        if TextFormatter.useColor:
            return '\033[1m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def makeBoldRed(s):
        if TextFormatter.useColor:
            return '\033[1;31m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def makeBoldGreen(s):
        if TextFormatter.useColor:
            return '\033[1;32m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def makeBoldYellow(s):
        if TextFormatter.useColor:
            return '\033[1;33m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def makeBoldBlue(s):
        if TextFormatter.useColor:
            return '\033[1;34m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def makeBoldPurple(s):
        if TextFormatter.useColor:
            return '\033[1;35m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def makeBoldCyan(s):
        if TextFormatter.useColor:
            return '\033[1;36m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def makeGreen(s):
        if TextFormatter.useColor:
            return '\033[32m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def makeRed(s):
        if TextFormatter.useColor:
            return '\033[31m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def makeBlue(s):
        if TextFormatter.useColor:
            return '\033[34m' + s + TextFormatter.strColorEnd
        return s

    @staticmethod
    def indent(str, level=1):
        lines = [' '*(4 if s else 0)*level + s for s in str.split('\n')]
        return '\n'.join(lines)


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

    def __init__(self, idDict=None, dataToTime=None):
        self.microtags = []
        self.idDict = idDict if idDict is not None else {}

        # conversion function from data (ticks) to time
        if dataToTime is not None:
            self.dataToTime = dataToTime
        else:
            # the default is using ticks
            self.dataToTime = lambda c: (c, 'ticks', 0)

    def dataToTimeStr(self, data):
        time = self.dataToTime(data)
        return '{0:,.{2}f} {1}'.format(time[0], time[1], time[2]) 

    def dataToTimeDiffStr(self, dataStart, dataStop):
        timeStart = self.dataToTime(dataStart)
        timeStop = self.dataToTime(dataStop)
        return '{0:,.{2}f} {1}'.format(
                timeStop[0] - timeStart[0], timeStop[1], timeStop[2])

    def __len__(self):
        return len(self.microtags)

    def __str__(self):

        # prepare output as a list of lines
        lines = []

        # determine length of longest string in tag id alias dictionary
        # (removing leading type definitions, if present)
        widthId = max([len(s if s.find(':') == -1 else s[s.find(':')+1:]) \
                for s in self.idDict.values()] + [8])

        # determine length of highest tag index
        widthIndex = len('{0}'.format(len(self.microtags)))

        # a list of indices referring to unmatched start tags
        unmatchedStarts = []

        for i, tag in enumerate(self.microtags):

            # ===== tag index =====

            line = '{0:{1}}: '.format(i, widthIndex)

            # ===== tag id or id alias =====

            if tag.id in self.idDict:
                # tag id alias from dictionary
                idAlias = self.idDict[tag.id]
                # extract tag type (start, stop, event, data)
                if idAlias.startswith('start:'):
                    # << this is a start tag >>
                    tagType = '<'
                    idAlias = idAlias[6:]
                    idAliasPrinted = TextFormatter.makeBoldGreen('{0:{1}}' \
                            .format(idAlias, widthId + 2))
                elif idAlias.startswith('stop:'):
                    # << this is a stop tag >>
                    tagType = '>'
                    idAlias = idAlias[5:]
                    idAliasPrinted = TextFormatter.makeBoldRed('{0:{1}}' \
                            .format(idAlias, widthId + 2))
                elif idAlias.startswith('event:'):
                    # << this is an event tag >>
                    tagType = '!'
                    idAlias = idAlias[6:]
                    idAliasPrinted = TextFormatter.makeBoldYellow('{0:{1}}' \
                            .format(idAlias, widthId + 2))
                elif idAlias.startswith('data:'):
                    # << this is a data tag >>
                    tagType = 'D'
                    idAlias = idAlias[5:]
                    idAliasPrinted = TextFormatter.makeBoldBlue('{0:{1}}' \
                            .format(idAlias, widthId + 2))
                else:
                    # << tag is in dictionary but its type is unknown >>
                    tagType = '?'                    
                    idAliasPrinted = idAlias
            else:
                # << tag is not in dictionary >>
                tagType = '?'                    
                idAlias = '[0x{0:04X}]'.format(tag.id)
                idAliasPrinted = '{0:{1}}'.format(idAlias, widthId + 2)

            line += tagType + ' ' + idAliasPrinted
        
            # ===== tag content, i.e. time or data =====  

            if tagType in '<>!':
                line += '{0:>20}  '.format(self.dataToTimeStr(tag.data))
            else:
                line += TextFormatter.makeBoldBlue('{0:>20}  ' \
                        .format('[ 0x{0:08X} ]'.format(tag.data)))
        
            # ===== start/stop tag matching =====  
          
            if tagType == '<':

                # add indeces of start tags to list of unmatched start tags
                unmatchedStarts += [i]

            elif tagType == '>':

                # find corresponding start tag
                matchingStarts = [j for j in unmatchedStarts[::-1] \
                        if self.idDict[self.microtags[j].id] \
                                == 'start:{0}'.format(idAlias)]
                matchingStart = matchingStarts[0] \
                        if len(matchingStarts) > 0 else None

                if matchingStart is not None:
                    # matching string
                    line += '[ {0:{1}} ]---({2:^{3}})--->[ {4:{1}} ]' \
                            .format(matchingStart, widthIndex, idAlias, widthId, i)
                    # time difference
                    line += '{0:>20}'.format(self.dataToTimeDiffStr(
                            self.microtags[matchingStart].data, tag.data))

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

    idDict = {
        0x0000 : 'start:Direct',
        0x0001 : 'stop:Direct',
        0x0002 : 'start:Loop',
        0x0003 : 'stop:Loop',
        0x1000 : 'data:Counts'
    }

    # read input file
    microtags = MicrotagList(idDict) #, lambda c: (c / 84E6, 's', 3))

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

