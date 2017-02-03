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

    def __init__(self):
        self.microtags = []

    def __len__(self):
        return len(self.microtags)

    def __str__(self):
        return '\n'.join([str(tag) for tag in self.microtags])

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
    microtags = MicrotagList()

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

