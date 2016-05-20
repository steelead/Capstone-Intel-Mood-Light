# PSU Capstone Intel Mood Light 2015-2016
# This program functions as the user interface to allow the user to set profiles in the the arduino
# Connect to the arduino over a serial com port connection to send and receive information from the arduino
# 
# Adrian Steele (steelead@pdx.edu)
# Bander Alenezi (alenezi@pdx.edu)
# Dusan Micic (dmicic@pdx.edu)
# Waleed Alhaddad (alhad@pdx.edu)

# Dependencies:
# windows pySerial library

# Tested on Windows 7, python 2.7.6, pyserial 2.7

import argparse
import errno
import os
import sys
import signal
import serial
import socket
import subprocess
import threading
import time


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('comPort', action='store', help="The serial com port that the arduino is connected to")
    args = parser.parse_args()

    #bind a serial connection to a computer communication port
    ser = serial.Serial(args.comPort, 9600, timeout=1)

    #open the bluetooth connection
    if ser.isOpen() == True:
        print "Connected to bluetooth module"
    else:
        print "Could not connect to bluetooth module"
        sys.exit()

    #open second thread to read back information from the arduino
    thread = threading.Thread(target=readMySerial, args=(ser,))
    thread.daemon = True
    thread.start()
        
    #get command from user and send to arduino over serial connection
    while True:
        print "Main Menu"
        print "1) Add profile"
        print "2) Edit existing profile"
        print "3) Remove profile"
        print "4) Quit"
        cmd = int(input("Enter the number of the command to execute: "))

        if cmd == 1:
            addProfile()




    while True:
        c = raw_input("")
        c = c + '!'
        ser.write(c)

#read data from arduino
def readMySerial(ser):
    while True:
        text = ser.readline()
        if text != "":
            print text


def addProfile(ser):
    """
        Add a new profile to the arduino

        Args:
          - ser: serial connection object to the arduino

        Returns:
    """
    name = input("Enter the profile name: ")
    print "Color options:"
    print "1) Green"
    print "2) Red"
    print "3) Blue"
    print "4) Yellow"
    print "5) White"
    print "6) Purple"
    print "7) Orange"
    color = input("Enter the number of the color to set for the profile: ")
    print "Step on the scale to calibrate the user's weight."
    
    ser.write("menu=1,")
    ser.write("name=" + name + ",")
    ser.write("color=" + color + ",")
    ser.write("weight=1")
    
    resp = ser.readline()
    

def getProfiles(ser):
{
    """
        Gets a list of profiles currently stored in the Arduino
        
        Args:
          - ser: serial connection object to the arduino
        
        Returns:
            A tuple containing:
            - Integer representing number of profiles
            - List object containing dictionaries of the profiles
    """
    ser.write("menu=2,")
    line = ser.readline()
    # first thing returned from arduino should be the number of profiles it has
    numProfiles = int(line.split('=')[1].strip())
    
    profiles = []
    
    for i in range(0, numProfiles):
        # each profile will be read in in one line (ending with a newline)
        line = ser.readline()
        
        # each item in the profile will be separated by a comma
        for item in line.split(','):
            #key value pair will be separated by an '='
            if "user=" in item:
                user = item.split('=')[1].strip()
            elif "weight=" in item:
                weight = int(item.split('=')[1].strip())
            elif "color=" in item:
                color = int(item.split('=')[1].strip())
                
        newProfile = {"user": user, "weight": weight, "color": mapVal2Color(color)}
        profiles.append(newProfile)
        
    return (numProfiles, profiles)
        
    
    
def mapVal2Color(colorInt):
    """
        Maps an integer to a color
        
        Args:
          - colorInt: the integer value of the color to map
          
        Returns:
            returns a string of the mapped color value
    """
    colorDict = {1: "Green",
                 2: "Red",
                 3: "Blue",
                 4: "Yellow",
                 5: "White",
                 6: "Purple",
                 7: "Orange",
                }
    return colorDict[colorInt]
    

if __name__ == '__main__':
    main()
