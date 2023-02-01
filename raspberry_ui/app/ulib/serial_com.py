#!/usr/bin/env python
# -*- coding: utf-8 -*-
# lsusb to check device name
#dmesg | grep "tty" to find port name

# import the threading module
import threading, sys
import serial, time
from .abstract_file import AbstractFile

class SerialComThread(threading.Thread):
    # Thread class with a _stop() method.
    # The thread itself has to check
    # regularly for the stopped() condition.
    def __init__(self, name, id):
        threading.Thread.__init__(self)
        self._stop = threading.Event()
        self.name = name
        self.id = id
        self.isListen = 1 #1 is true

    def is_error(self):
        return self.isError
    
    # function using _stop function
    def stop(self):
        self._stop.set()

    def stopped(self):
        return self._stop.isSet()

    def saveState(self, state):
        af = AbstractFile()
        af.file_save("serial_state.cfg", state, af.data_dir, "wt")
        return 1
    
    def setListen(self, listenVal = 0):
        
        #self.isListen = listenVal
        print(str(listenVal) + "---", file=sys.stderr)

    # def run(self):
    #     while self.isListen:
    #         if self.stopped():
    #             break
    #         print(str(self.name) + " Hello, world!", file=sys.stderr)
    #         time.sleep(1)
    #     return  


    def run(self):
        af = AbstractFile()
        try:
            with serial.Serial("/dev/ttyACM0", 9600, timeout=1) as arduino:
                time.sleep(0.1) #wait for serial to open
                if arduino.isOpen():
                    print("{} connected!".format(arduino.port), file=sys.stderr)
                    self.saveState("SERIAL_THREAD_STARTED")
                    while self.isListen:
                        cmd = af.file_get_content("input_motor_move.csv", af.get_upload_dir(), "rt")
                        #cmd=input("Enter command : ")
                        cmd = cmd + "#"
                        arduino.write(cmd.encode())
                        #time.sleep(0.1) #wait for arduino to answer
                        while arduino.inWaiting()==0: pass
                        if  arduino.inWaiting()>0: 
                            answer=arduino.readline()
                            print(answer, file=sys.stderr)
                            arduino.flushInput() #remove data after reading
        except:
            self.saveState("SERIAL_OPEN_ERROR")
            #raise Exception("Cannot Open Serial Port!")
            print("Cannot Open Serial Port!", file=sys.stderr)
            return 0

# if __name__ == '__main__':
#
#     print('Running. Press CTRL-C to exit.')
#     with serial.Serial("/dev/ttyACM0", 9600, timeout=1) as arduino:
#         time.sleep(0.1) #wait for serial to open
#         if arduino.isOpen():
#             print("{} connected!".format(arduino.port))
#             try:
#                 while True:
#                     cmd=input("Enter command : ")
#                     arduino.write(cmd.encode())
#                     #time.sleep(0.1) #wait for arduino to answer
#                     while arduino.inWaiting()==0: pass
#                     if  arduino.inWaiting()>0: 
#                         answer=arduino.readline()
#                         print(answer)
#                         arduino.flushInput() #remove data after reading
#             except KeyboardInterrupt:
#                 print("KeyboardInterrupt has been caught.")