#Created by Sridhar Prabhu 
#March 24 2018
 

import serial 

arduinoSerialData = serial.Serial('com11',9600) #Add the correct serial port and baud rate
file_path = "" #Insert the file path

output_file = open(file_path,"w+")


#Infinite loop to mimick loop() in arduino
while True:
	if (arduinoSerialData.inWaiting()>0):
		dataString = arduinoSerialData.readline()
		print dataString
		output_file.write(dataString)
		
		