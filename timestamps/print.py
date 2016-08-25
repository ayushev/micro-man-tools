#!/usr/bin/python

import sys
from timestamps import *

tick = 1E-2

tags = {}

# calibration tags
tags[0xFE] = "TS_CALIBRATION_BEGIN"
tags[0xFF] = "TS_CALIBRATION_END"

# Library init
tags[0x00] = "TS_LIB_INIT_BEGIN"
tags[0x01] = "TS_LIB_INIT_END"

# Pseudo random function
tags[0x10] = "TS_PRF_BEGIN"
tags[0x11] = "TS_PRF_END"

# Encrypt/decrypt pre-master secret with RSA
tags[0x14] = "TS_PMS_DECRYPT_BEGIN"
tags[0x15] = "TS_PMS_DECRYPT_END"
tags[0x16] = "TS_PMS_ENCRYPT_BEGIN"
tags[0x17] = "TS_PMS_ENCRYPT_END"

# Sign DHE key exchange
tags[0x20] = "TS_DHE_SIGN_BEGIN"
tags[0x21] = "TS_DHE_SIGN_HASHED"
tags[0x22] = "TS_DHE_SIGN_END"

# Calculate DHE shared secret
tags[0x24] = "TS_DHE_CALC_SHARED_SEC_BEGIN"
tags[0x25] = "TS_DHE_CALC_SHARED_SEC_END"

# Running handshake hashes
tags[0x2A] = "TS_HASH_INIT_BEGIN"
tags[0x2B] = "TS_HASH_INIT_END"
tags[0x2C] = "TS_HASH_UPDATE_BEGIN"
tags[0x2D] = "TS_HASH_UPDATE_END"

# Calculate session keys
tags[0x30] = "TS_COMP_KEY_BEGIN"
tags[0x31] = "TS_COMP_KEY_END"

# Calculate finish verification hash
tags[0x32] = "TS_COMP_HASH_BEGIN"
tags[0x33] = "TS_COMP_HASH_END"

# Generate CertificateVerify message
tags[0x40] = "TS_CRT_VERF_SIGN_BEGIN"
tags[0x41] = "TS_CRT_VERF_SIGN_END"

# Received handshake message
tags[0xA0] = "TS_RECEIVED_HS_HELLO_REQ"
tags[0xA1] = "TS_RECEIVED_HS_CLIENT_HELLO"
tags[0xA2] = "TS_RECEIVED_HS_SERVER_HELLO"
tags[0xA3] = "TS_RECEIVED_HS_CERT"
tags[0xA4] = "TS_RECEIVED_HS_SRV_KEY_EX"
tags[0xA5] = "TS_RECEIVED_HS_CERT_REQ"
tags[0xA6] = "TS_RECEIVED_HS_SRV_HELLO_DONE"
tags[0xA7] = "TS_RECEIVED_HS_CLI_KEY_EX"
tags[0xA8] = "TS_RECEIVED_HS_CERT_VERIFY"
tags[0xA9] = "TS_RECEIVED_HS_FINISH"

# Received CSS or alert message
tags[0xAE] = "TS_RECEIVED_CCS"
tags[0xAF] = "TS_RECEIVED_ALERT"

# Sent handshake message
tags[0xB0] = "TS_SENT_HS_CLIENT_HELLO"
tags[0xB1] = "TS_SENT_HS_SERVER_HELLO"
tags[0xB2] = "TS_SENT_HS_SRV_HELLO_DONE"
tags[0xB3] = "TS_SENT_HS_CERTIFICATE"
tags[0xB4] = "TS_SENT_HS_SRV_KEY_EX"
tags[0xB5] = "TS_SENT_HS_CERT_REQ"
tags[0xB6] = "TS_SENT_HS_CLI_KEY_EX"
tags[0xB7] = "TS_SENT_HS_CERT_VERIFY"
tags[0xB8] = "TS_SENT_HS_FINISH"

# Sent CSS or alert message
tags[0xBE] = "TS_SENT_CCS"

# Calculate MAC
tags[0x50] = "TS_COMP_MAC_BEGIN"
tags[0x51] = "TS_COMP_MAC_END"

# Symmetric encryption/decryption
tags[0x52] = "TS_STREAM_ENCRYPT_BEGIN"
tags[0x53] = "TS_STREAM_ENCRYPT_END"
tags[0x54] = "TS_STREAM_DECRYPT_BEGIN"
tags[0x55] = "TS_STREAM_DECRYPT_END"
tags[0x56] = "TS_CBC_ENCRYPT_BEGIN"
tags[0x57] = "TS_CBC_ENCRYPT_END"
tags[0x58] = "TS_CBC_DECRYPT_BEGIN"
tags[0x59] = "TS_CBC_DECRYPT_END"

#timestamps to check delay of essl client
tags[0x80] = "TS_CRYPTO_INIT_BEGIN"
tags[0x81] = "TS_CRYPTO_INIT_END"
tags[0x84] = "TS_DHE_INIT_BEGIN"
tags[0x85] = "TS_DHE_INIT_END"
tags[0x88] = "TS_SSLCTX_INIT_BEGIN"
tags[0x89] = "TS_SSLCTX_INIT_END"
tags[0x98] = "TS_SERV_CERT_PRIV_INIT_BEGIN"
tags[0x99] = "TS_SERV_CERT_PRIV_END"

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
    timestamps = TimestampList()
    try:
        timestamps.parseFile(filename)
    except:
        print "Failed to read/parse input file. Stopping."
        return

    # print timestamps
    for i in range(len(timestamps)):
        tagName = tags[timestamps[i].tag]
        time = timestamps[i].counter * tick
        if tagName.endswith("_END"):
            for j in range(i, -1, -1):
                tagName2 = tags[timestamps[j].tag]
                time2 = timestamps[j].counter * tick
                if tagName2 == tagName[0:-4] + '_BEGIN':
                    match = '[{0:2}]---({1:^16})--->[{2:2}] {3:>8.2f} ms' \
                            .format(j, tagName[0:-4], i, time - time2)
                    break
        else:
            match = ''
        print('{0:2}: {1:>8.2f} ms: {2:30} {3}'.format(i, time, tagName, match))



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


	



