#!/usr/bin/env python
import time
import serial
import sys

def main():
    s = serial.Serial("/dev/ttyS0", 57600)
    cmd = sys.argv[1]
    print cmd
    s.write(cmd)

if __name__=="__main__":
    main()
