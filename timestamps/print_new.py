#!/usr/bin/python

import sys
from timestamps import *

# 0x0000 - 0x3FFF => start tag (time-based)
# 0x4000 - 0x7FFF => stop tag (time-based)
# 0x8000 - 0xBFFF => single tag (time-based)
# 0xC000 - 0xEFFF => data tag
# 0xF000 - 0xFFFF => reserved

# TODO: allow to read tick frequency from input stream
tick = 1E-2
offset = None

tags = {}


reverse_tags = {v: k for k, v in tags.items()}


#
# _____________________________________________________________________________
#
def main(argv):

    if len(argv) < 1:
        printDefines()
        return
    elif len(argv) == 1:
        filename = argv[0]
    else:
        print "Wrong number of arguments. Stopping."
        print "Expecting <input-file>"
        return

    # read input file
    timestamps = TimestampList(None, True)
    try:
        timestamps.parseFile(filename)
    except:
        print "Failed to read/parse input file. Stopping."
        return

    # the list of encountered start tag indices
    starts = []

    # print timestamps
    for i in range(len(timestamps)):

        tag = timestamps[i].tag

        if tag < 0xC000:

            # <<< this is a time-based tag >>>

            if tag <= 0x3FFF:
                # keep track of start tags
                starts += [i]

            # read time of current 
            time = timestamps[i].counter * tick

            if offset is None:
                # if offset is None the time of the first
                # time-based tag will be used as offset
                offset = time
            if offset is not None:
                # apply offset
                time = time - offset

            

        elif tag >= 0xC000 and tag <= 0xEFFF:

            # <<< this is a data tag >>>

        else:

            # <<< reserved tag >>>

    


        tagName = tags.get(timestamps[i].tag, 'UNKNOWN')



        time = timestamps[i].counter * tick
        if tagName.endswith("_END"):
            for j in range(i, -1, -1):
                tagName2 = tags.get(timestamps[j].tag, 'UNKNOWN')
                time2 = timestamps[j].counter * tick
                if tagName2 == tagName[0:-4] + '_BEGIN':
                    match = '[{0:2}]---({1:^{4}})--->[{2:2}] {3:>8.2f} ms' \
                            .format(j, tagName[0:-4], i, time - time2, maxTagLen)
                    break
        else:
            match = ''
        print('{0:2}: {1:>8.2f} ms: {2:40} {3}'.format(i, time, 
                '{0} (0x{1:02X})'.format(tagName, timestamps[i].tag), match))



#
# _____________________________________________________________________________
#
def printDefines():

    for tag in tags:
        print '#define {0:30} {1:#02x}'.format(tags[tag], tag)


#
# _____________________________________________________________________________
#
if __name__ == "__main__":
    main(sys.argv[1:]);


	



