# cs232-caesar-assmt

Lily McAboy
CS232-B
Spring 2024

Caesar Cipher Client and Server
Client.py is client code
Server.py is server code

The server opens a TCP socket and listens for a client connection
When connected to the client, creates a new thread to handle that client's session

Rotation ciphers client input text -> i.e., given the rotation number 2 and the letter a, return c 
Rotation does not affect numbers or special characters like @, $, %, etc.

When the client inputs the word "quit", the connection is closed and the server is notified. 
