'''
Caesar Client and Server
Name: Lily McAboy
CS 232 B
Date: 4/21/2024
'''

# import libraries
import socket
import sys
import _thread
import datetime as dt
import string

#Multithreaded echo server: https://glowingpython.blogspot.com/2011/04/how-to-implment-multithread-echo-server.html 
#_thread library: https://docs.python.org/3/library/_thread.html 

def handle_connection(connection, addr): #create a function to take in who is communicating and its address
    client_ip_addr = addr[0]
    timestamp = dt.datetime.now() #get time stamp when connection starts
    print("Connecting from " + client_ip_addr + " at " + str(timestamp))
    response = connection.recv(1024).decode() #receive the response and decode it
    print("Got response " + response)
    if response == "quit": #if the client response is "quit"
        connection.close() #close the socket
        return 0 #and break out of our loop
    elif response.isdigit() and 1 <= int(response) <= 24: #if the rotation is within our legal bounds (1-24 inclusive
        rotation = int(response) #force to be an int
        print(client_ip_addr + " set rotation to: " + str(rotation)) #print on server that the rotation is set to 
        connection.send(response.encode()) #echo back the rotation amount. 
    else:
        print("Received invalid rotation input from " + client_ip_addr) #If you send an invalid rotation, throw an error and close socket
        connection.close()
        return 0
    while True:
        client_input = connection.recv(1024).decode()
        if client_input == "": #if client input is empty, close the connection
            connection.close() 
            print(client_ip_addr + " has closed connection.")
            return 0
        else: 
            rotated_text = rotate_text(client_input, rotation) #rotate text
            print(client_ip_addr + " sent " + client_input + ". Sending back " + rotated_text + ".") #have the server print out what it's going to do 
            connection.send(rotated_text.encode()) #send rotated text
        
def rotate_text(client_input, rotation):
    rotated_text = "" #Initialize the rotated text as an empty string
    for char in client_input:
        if char.isalpha(): #Check if the character is part of the alphabet
            new_alphabet = string.ascii_lowercase if char.islower() else string.ascii_uppercase #create a new alphabet of lower and uppercase characters
            rotated_character = new_alphabet[(new_alphabet.index(char) + rotation) % 26] #add rotation and modulo 26 (for the 26 alphabetcal letters)
            rotated_text += rotated_character.upper() if char.isupper() else rotated_character #add it to our rotated text string
        else:
            rotated_text += char #Add the character if it's a number or special char
    return rotated_text #return the text
            

            

#Helpful tips from https://www.geeksforgeeks.org/python-network-programming/
def main():

    hostname = ''
    port = int(sys.argv[1]) #port input from Server in command line
    
    #Create a socket object 
    CaesarServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    #Bind 
    CaesarServer.bind((socket.gethostname(), port))
    #Listen for the client message
    CaesarServer.listen()

    while True:
        #First, accept from the client a connection and its address
        connection, address = CaesarServer.accept()
        #create a new thread to handle that connection from our above function
        _thread.start_new_thread(handle_connection, (connection, address))

if __name__ == "__main__":
    #Call main
    main()



