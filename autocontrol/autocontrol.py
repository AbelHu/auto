#!/usr/bin/env python
import sys
import time
import socket
import datetime
import serial
import re

def broadcastCommand(host, cmd):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.sendto(cmd, host)
        return s.recv(1024)
    except:
        return None
    finally:
        if s != None:
            s.close()

def sendCommand(host, cmd):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(host)
        s.send(cmd)
        return s.recv(1024)
    except:
        return None
    finally:
        if s != None:
            s.close()

def checkSwitch(host):
    cmd = "AT+RECOSCAN"
    return broadcastCommand(host, cmd)

def enableSwitch(host):
    current_time = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
    cmd = "AT+YZSWITCH=1,on,%s<CR><LF>" % current_time
    ret = sendCommand(host, cmd)
    if re.match("^\+ok", ret):
        return True
    else:
        return False

def disableSwitch(host):
    current_time = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
    cmd = "AT+YZSWITCH=1,off,%s<CR><LF>" % current_time
    ret = sendCommand(host, cmd)
    if re.match("^\+ok", ret):
        return True
    else:
        return False

def querySwitch(host):
    cmd = "AT+YZOUT<CR><LF>"
    return sendCommand(host, cmd)

def main():
    run()

def run():
    switch_addr = "192.168.2.31"
    switch_port = 8899

    with open("/root/records.log", "a+") as records:
        with open("/root/autocontrol.log", "a+") as log:
            while True:
                try:
                    s = serial.Serial("/dev/ttyS0", 57600)
                    message = s.readline()
                    if message.startswith("CMD:"):
                        log.write(time.strftime("%Y-%m-%d %H:%M:%S") + " - "  + message)
                        if "TURN_ON_SWITCH" in message:
                            log.write(time.strftime("%Y-%m-%d %H:%M:%S") + " - Turn on the switch")
                            enableSwitch((switch_addr, switch_port))
                        elif "TURN_OFF_SWITCH" in message:
                            log.write(time.strftime("%Y-%m-%d %H:%M:%S") + " - Turn off the switch")
                            disableSwitch((switch_addr, switch_port))
                        log.flush()
                    elif message.startswith("REC:"):
                        records.write(time.strftime("%Y-%m-%d %H:%M:%S") + " - "  + message)
                        records.flush()
                    else:
                        log.write(time.strftime("%Y-%m-%d %H:%M:%S") + " - "  + message)
                        log.flush()
                except Exception as e:
                    log.write(time.strftime("%Y-%m-%d %H:%M:%S") + " - " + e)
                    log.flush()
                    continue

if __name__=="__main__":
    main()
