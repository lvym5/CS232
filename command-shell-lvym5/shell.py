################################################################
# Creating a command shell in Python
# Name of shell: PORSH --> Python's Obligatory Redundant Shell
# Created by: Lily McAboy
# Date: 2/18/2024
# Class: CS 232 B
################################################################
# NOTES FOR RUNNING:
# Please run in Python 3 (python3 shell.py in terminal) --> it will not work with just python shell.py (lots of trial and error lol)

# Import libraries
import subprocess  #subprocess: https://geekflare.com/learn-python-subprocess/ 
import os #learning the os module: https://www.geeksforgeeks.org/os-module-python-examples/ and https://docs.python.org/3/library/os.html 
    
#Function to change directory
def cd(path):
    global cwd #create global var to change path name when cd
    if not os.path.exists(path):
        print("Path not found: please enter a valid path.")
    elif not os.path.isdir(path):
        print(f"Path '{path}' is not a directory. Please use an existing directory.")
    else:
        os.chdir(path)
        cwd = os.getcwd() #update cwd prompt when the path is changed


#class for getting and parsing the path environment vars
class path_env:
    def __init__ (self):
        self.paths = self.get_value()

    # Get env: https://www.geeksforgeeks.org/python-os-getenv-method/ 
    def get_value(self): #get the value of the path 
        path = os.getenv("PATH")
        if path is None:
            return []
        return path.split(":")
    
    # Path name manipulations: https://docs.python.org/3/library/os.path.html 
    def find(self, cmd):
        for cmds in self.paths:
            user_files = os.scandir(cmds)
            for user_file in user_files:
                if user_file.name == cmd:
                    return os.path.join(cmds, user_file.name)
        return ""
    
#Execute function
def execute(command, argument, ampersand = False): #Python exceptions: https://docs.python.org/3/library/exceptions.html 
    try:
        pid = os.fork()
        if pid == 0:
            print(f"\n[Child pid: {os.getpid()}]")
            os.execvp(command, [command] + argument) # Learning execvp: https://docs.python.org/3/library/os.html
        else:
            if not ampersand:
                _, status = os.waitpid(pid,0) #chatGPT helped me with the _, status
                subprocess.run([command] + argument) #used ChatGPT to help me understand and use subprocesses.run()
                print(f"[{pid} -> {status}]")
    except Exception: #if user types in something unknown, throw an exception
        print("Command not found. Please try again!")

# Main program 
def main(): #define our main function
    path = path_env() #grab our path from our path_env class
    while True:
        userCommand = input(f"{os.getcwd()} $ ").strip() #if a user passes a million spaces, it won't do anything and just go to the next line. 
        # As long as you actually pass something into the shell, execute what the user types
        if not userCommand:
            continue
        if userCommand: 
            #using split in python: https://stackoverflow.com/questions/60304902/using-split-on-an-input-string 
            splitArgs = userCommand.split()
            command = splitArgs[0] #command is the first "word" of the argument
            argument = splitArgs[1:] #arguments are whatever comes after the command (arbitrary amount of arguments)
            if command == "exit":
                os._exit(0) #if command is exit, leave 
            elif command == "cd":
                try: 
                    if not argument or len(argument) == 0:
                        raise ValueError("Invalid argument: Please provide a path")
                    cd(argument[0])
                except ValueError as error:
                    print(error)
            elif command == "pwd": #if asking to print working directory, print cwd
                print(os.getcwd())
            else:
                if "&" in argument: #if ampersand is found in the argument
                    ampersand_index = argument.index("&") 
                    if ampersand_index != len(argument) -1: #confirm that there is nothing after the ampersand: if there is, send an error
                        print("Invalid syntax: No argument after & is allowed.")
                        continue
                    argument.remove("&")
                    ampersand = True
                else:
                    ampersand = False
                execute(command, argument, ampersand) #if not exit, pwd, or cd, then execute the argument using the execute func

if __name__ == "__main__":
    main()
