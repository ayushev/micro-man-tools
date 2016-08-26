#!/usr/bin/python

import sys
from timestamps import *

tag1a = 0x02
tag2b = 0x03
tag3b = 0x04
tag4a = 0x05


#
# _____________________________________________________________________________
#
def main(argv):

    if len(argv) == 2:
        filenameA = argv[0]
        filenameB = argv[1]
    else:
        print "Wrong number of arguments. Stopping."
        print "Expecting <input-file-A> <input-file-B>"
        return

    # read input files
    timestampsA = TimestampList()
    timestampsB = TimestampList()
    try:
        timestampsA.parseFile(filenameA)
        timestampsB.parseFile(filenameB)
    except:
        print "Failed to read/parse input files. Stopping."
        return

    timestamps1a = [ts for ts in timestampsA if ts.tag == tag1a]
    timestamps2b = [ts for ts in timestampsB if ts.tag == tag2b]
    timestamps3b = [ts for ts in timestampsB if ts.tag == tag3b]
    timestamps4a = [ts for ts in timestampsA if ts.tag == tag4a]

    if len(set([len(timestamps1a), len(timestamps2b), \
            len(timestamps3b), len(timestamps4a)])) != 1:
        print "Mismatching number of synchronization points"
        return
       
    for i in range(len(timestamps1a)):
        counter1a = timestamps1a[i].counter
        counter2b = timestamps2b[i].counter
        counter3b = timestamps3b[i].counter
        counter4a = timestamps4a[i].counter

        delta = counter2b - 0.5*(counter4a - counter1a - \
            counter3b + counter2b) - counter1a
                
        print('Counter delta A-->B is: 0x{0:06x}'.format(int(delta) % 16**6))


#
# _____________________________________________________________________________
#
if __name__ == "__main__":
    main(sys.argv[1:]);


	



