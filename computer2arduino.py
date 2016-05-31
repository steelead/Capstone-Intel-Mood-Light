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
    """
        Main function, user should specify the COM port that the arduino is connected on when they begin the program.
    """
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

    #get command from user and send to arduino over serial connection
    while True:
        print "Main Menu"
        print "1) Add profile"
        print "2) Edit existing profile"
        print "3) Remove profile"
        print "4) Quit"
        cmd = int(raw_input("Enter the number of the command to execute: "))

        if cmd == 1:
            addProfile(ser)
            
        elif cmd == 2:
            editProfile(ser)

        elif cmd == 3:
            removeProfile(ser)
            
        elif cmd == 4:
            break

            
            
def addProfile(ser):
    """
        Add a new profile to the arduino
        Args:
          - ser: serial connection object to the arduino
        Returns:
            None
    """
    username = raw_input("Enter the profile name: ")
    print "Color options:"
    print "1) Green"
    print "2) Red"
    print "3) Blue"
    print "4) Yellow"
    print "5) White"
    print "6) Purple"
    print "7) Orange"
    color = int(raw_input("Enter the number of the color to set for the profile: "))
    print "Step on the scale to calibrate the user's weight."
    
    ser.write("menu=1,")
    ser.write("name=" + username + ",")
    ser.write("color=" + str(color) + ",")
    ser.write("weight=1,")
    
    while True:
        resp = ser.readline()
        if resp != '':
            break

    try:
        print "Weight calibrated to: %d" % int(resp.split('=')[1].strip())
    except IndexError:
        print "Bluetooth communication error with Arduino, please try again.\n"
        

def getProfiles(ser):
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
    while True:
        line = ser.readline()
        if line != '':
            break
    
    try:
        # first thing returned from arduino should be the number of profiles it has
        numProfiles = int(line.split('=')[1].strip())
    except IndexError:
        print "Bluetooth communication error with Arduino, please try again.\n"
        return (0, [])
        
    profiles = []
    
    for i in range(0, numProfiles):
        # each profile will be read in in one line (ending with a newline)
        line = ser.readline()
        
        try:
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
            
        except IndexError:
            print "Bluetooth communication error with Arduino, please try again.\n"
            return (0, [])
        
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
    
    
def editProfile(ser):
    """
        Edit an existing profile on the arduino
        Args:
          - ser: serial connection object to the arduino
        Returns:
            None
    """
    (numProfiles, profiles) = getProfiles(ser)
    
    if numProfiles == 0:
        print "No existing profiles found on the Arduino.\n"
        return
        
    print "There are currently %d profiles stored on the Arduino." % numProfiles
    
    for i in range(0, len(profiles)):
        print "Profile %d" % (i+1)
        print "    User: %s" % profiles[i]["user"]
        print "    Weight: %d" % profiles[i]["weight"]
        print "    Color: %s\n" % profiles[i]["color"]
        
    editIndex = int(raw_input("Enter the number of the profile you want to make edits to: ")) - 1
    
    if editIndex < 0 or editIndex >= len(profiles):
        print "Error: Specified profile number out of range."
        return
        
    print "Profile %d" % (editIndex+1)
    print "    1) User: %s" % profiles[editIndex]["user"]
    print "    2) Color: %s" % profiles[editIndex]["color"]
    print "    3) Weight: %d\n" % profiles[editIndex]["weight"]
    item = int(raw_input("Enter the number of the item you want to modify: "))
    
    if item < 1 or item > 3:
        print "Error: specified item to edit out of range (1-3)"
        return
    
    if item == 1:
        username = raw_input("Enter the profile name: ")
        ser.write("menu=3,")
        ser.write("profile=" + str(editIndex) + ",");
        ser.write("name=" + username + ",")
        
    elif item == 2:
        print "Color options:"
        print "1) Green"
        print "2) Red"
        print "3) Blue"
        print "4) Yellow"
        print "5) White"
        print "6) Purple"
        print "7) Orange"
        color = int(raw_input("Enter the number of the color to set for the profile: "))
        
        if color < 1 or color > 7:
            print "Error: specified color out of range (1-7)."
            return

        ser.write("menu=3,")
        ser.write("profile=" + str(editIndex) + ",")
        ser.write("color=" + str(color) + ",")

    elif item == 3:
        ser.write("menu=3,")
        ser.write("profile=" + str(editIndex) + ",")
        ser.write("weight=1,");
        print "Step on the scale to calibrate the user's weight."
        resp = ser.readline()
        while True:
            resp = ser.readline()
            if resp != '':
                break
        print [resp]
        print "Weight calibrated to: %d" % int(resp.split('=')[1].strip())

        
        
def removeProfile(ser):
    """
        Remove a profile from the arduino
        Args:
          - ser: serial connection object to the arduino
        Return:
            None
    """
    (numProfiles, profiles) = getProfiles(ser)
    
    if numProfiles == 0:
        print "No existing profiles found on the Arduino.\n"
        return
        
    for i in range(0, len(profiles)):
        print "Profile %d" % (i+1)
        print "    User: %s" % profiles[i]["user"]
        print "    Weight: %d" % profiles[i]["weight"]
        print "    Color: %s\n" % profiles[i]["color"]
        
    removeIndex = int(raw_input("Enter the number of the profile you want to remove: ")) - 1
    
    if removeIndex < 0 or removeIndex >= len(profiles):
        print "Error: Specified profile number out of range."
        return
    
    ser.write("menu=4,")
    ser.write("profile=" + str(removeIndex) + ",")
    
    
    
    
if __name__ == '__main__':
    main()
    