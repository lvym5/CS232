'''
Caesar Client and Server
Name: Lily McAboy
CS 232 B
Date: 4/21/2024
'''

# import libraries
import socket
import sys

# Network Programming in Python: https://www.geeksforgeeks.org/python-network-programming/ 

def main():
        
    # Initialize host and port that will be taken in from the command line 
    hostname = sys.argv[1] #hostname is the first argument (cs232.cs.calvin.edu)
    port = int(sys.argv[2]) #port is second argument (9876)
    
    try: 
        #Create the TCP/IP socket 
        CaesarClient = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        #Connect the client to the server
        CaesarClient.connect((hostname, port))

        # Echo Client: https://realpython.com/python-sockets/
        print("Welcome to the Caesar Cipher Network! I am so excited for a friend!!")
        #Prompt the user for a rotation amount
        rotation = (input("Please enter your desired rotation amount: "))
        if rotation.isdigit() and 1 > int(rotation) > 24:
            print("User Error: Rotation may not exceed 25.")
            return 0
        elif str(rotation) == "quit":
            return 0
        CaesarClient.send(str(rotation).encode())
        echo_rotation_response = CaesarClient.recv(1024)
        print("Server Response: You chose rotation " + echo_rotation_response.decode() + ". Great choice!")
        while True: 
            client_input = input("Client input: ")
            if client_input == "quit":
                return 0  
            #Send the encoded string to the server 
            CaesarClient.send(str(client_input).encode())
            
            #Decode the response from the server
            response = CaesarClient.recv(1024) #receiving 1024 bytes from server

            #print the response from the client 
            print("Server Response: " + response.decode())
    
    #if there are any issues and have an error, just print the error
    except Exception as e:
        print("Error: " + str(e))
        
    finally:
        #Close the client when we're done
        print("NO wait please come back :( I can do better!")
        CaesarClient.close()
        
if __name__ == "__main__":
    #Call main
    main()
