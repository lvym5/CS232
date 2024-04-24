/* Reversing a File: Homework 1

Name: Lily McAboy
Date: 2/9/2024
CS 232

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define _POSIX_SOURCE

void check_num_args(int argc, char *argv[]){
    if(argc != 2){ //don't know how many arguments should be passed?
        perror("Error: Invalid number of arguments."); //if the user did not pass correct num args, send error 
        exit(-1); //and then exit if failed
    }
}
void check_src_file_is_regular(const char *filepath){
    FILE * file; //read only (r) the file from the user 
    file = fopen(filepath, "r"); // fopen() syntax found at: https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
    //check to make sure file exists
    int valid_access = access(filepath, F_OK);
    //stat example found https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file 
    struct stat file_path;
    stat(filepath, &file_path);
    int valid_path = S_ISREG(file_path.st_mode);
    
    if(!valid_access || !valid_path){
        perror("Error: Please enter a valid filename.");
        exit(-1); //is this correct? instr say to return -1 if any error occured.... better way to do this?
    }
    fclose(file); //close the file
}

void copy_src_to_dest(usr_file, rev_file){
    //initialize our files
    FILE * og_file;
    FILE * reverse_file;

    og_file = fopen(usr_file, "r");
    reverse_file = fopen(rev_file, "w"); //We are going to write to the given reversed file. 

    if(og_file == NULL || reverse_file == NULL){
        perror("Error: Cannot open files.");
        exit(-1); //if you cannot open either file, send an error message
    }
    //using fseek(), syntax from https://www.tutorialspoint.com/c_standard_library/c_function_fseek.html
    //fseek examples from https://stackoverflow.com/questions/27549718/behaviour-of-fseek-and-seek-end 
    fseek(og_file, -1, SEEK_END);
    char i = fgetc(og_file); //read contents each as one character
    for(i = SEEK_END; i > 0; i--){
        fputc(i, reverse_file);
    }
}

int main(int argc, char *argv[])
{
	/* These functions each call exit(-1) if there is a problem. */
	check_num_args(argc, argv);
	check_src_file_is_regular(argv[1]);
	copy_src_to_dest(argv[1], argv[2]);
	return 0;   /* no error */
}

