import serial
import os

ser = serial.Serial('COM3', 9600) # create a serial connection with the specified port and baud rate of the arduino

def start_recording():
    recording = False  # flag to indicate if recording is in progress
    have_new_file_path = False  # flag to indicate if a new file path is available
    
    while True:
        if not recording and not have_new_file_path:
            file_name = input("Enter filename for the new recording: ") + ".txt"  # prompt the user to enter a filename
            file_path = os.path.join(os.getcwd(), "recordings")  # create a path to the "recordings" directory in the current working directory
            file_path = os.path.join(file_path, file_name)  # append the filename to the path
            have_new_file_path = True  # set the flag to indicate a new file path is available
        
        if ser.inWaiting():  # check if there is data available to be read from the serial connection
            mic_data = ser.readline().decode().strip()  # read a line of data from the serial connection and decode it
            if mic_data == "START":  # check if the received data indicates the start of recording
                if recording:  # if already recording, continue to the next iteration
                    continue
                recording = True  # set the flag to indicate recording is in progress
                f = open(file_path, 'w')  # open the file in write mode
            elif mic_data == "END":  # check if the received data indicates the end of recording
                recording = False  # set the flag to indicate recording has ended
                have_new_file_path = False  # reset the flag for a new file path
                if 'f' in locals():  # check if the file object 'f' is present in the local scope
                    f.close()  # close the file
            elif mic_data and recording:  # check if the received data is not empty and recording is in progress
                f.write(mic_data + "\n")  # write the microphone data to the file
             


start_recording() # start the recording loop