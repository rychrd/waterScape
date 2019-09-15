#!/usr/bin/env python3

import pigpio                                   # Pi GPIO library http://abyz.me.uk/rpi/pigpio/index.html
import struct

from pynng import Req0, Timeout                 # nng python project https://pynng.readthedocs.io/en/latest/index.html
from time import sleep

# MCP4725 command format 3 bytes : write type + power_mode{0}  : then dac value{int} 12 bits (last 4 aren't used)
# write function to eeprom + DAC: write = 3 ; to DAC only write = 2
# bit shifts adapted from the MCP4725 mBed library code @

pi = pigpio.pi()
devAddr = 0x60                                  # DAC's address on the i2c bus
dial_address = 'tcp://192.168.0.76:54321'       # server address - DAC value callback.


h = pi.i2c_open(1, devAddr)			            # get a handle to the DAC at the given address


def write_to_dac(_val):

    power_mode = 0				                # normal mode, no power down
    write_type = 2				                # fast write, not to eeprom
    data = bytearray(3)

    data[0] = (write_type << 5) | (power_mode << 1)
    data[1] = (_val >> 4)					    # puts 4 high bits of the 12 in the low end of the byte
    data[2] = (_val << 4) & 0xFF  			    # the next 8 bits, with the 4 high ones masked out

    pi.i2c_write_device(h, data)

#   decimal = struct.unpack('>BBB', data)
#   print(decimal)
    return


def read_from_dac():

    # shifts the first 8 bits over into dac_val[1]
    # then shifts the 4 LSBs back over to the low end of the second byte

    num_bytes, dac_settings = pi.i2c_read_device(h, 5)
    dac_val = (dac_settings[1] << 4) | (dac_settings[2] >> 4)
    return dac_val


def request_speed():

    # use the nng context manager -- will look after

    try:
        with Req0(dial=dial_address, recv_timeout=2000, send_timeout=2000) as req:

            req.send(b'request new DAC value')
            dac_val = req.recv()
#            dac_val = dac_val % 4032

            print("DAC value changing to: %s" % int(dac_val))

            return dac_val
        
    except Timeout: 
        raise                                   # don't handle error here, send it back up to the caller


if __name__ == '__main__':

    while True:
        try:
            pump_speed = request_speed()
#	This allows the client to recover if the server disappears for a while            
        except Timeout:
            print('Connection timed out..')
            print('server could be down, re-dialling...')
            continue

        print(pump_speed.decode('utf-8'))
        write_to_dac(int(pump_speed.decode('utf-8')))
        sleep(2)
 #       print('read value %s from dac' % read_from_dac())


