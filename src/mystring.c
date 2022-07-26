/***********************************************
*
* @Purpose: Implementation of the functions for mystring module
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "mystring.h"


/***********************************************
*
* @Purpose:     Function thet prints *string in screen
* @Parameters:  char *string: string to be printed in screen
*
* @Return:      -
*
************************************************/
void MYSTRING_print(char *string){
    char buffer[500];
    bzero(buffer, sizeof(buffer));
    sprintf(buffer,"%s",string);
    write(STDOUT_FILENO,buffer,strlen(buffer));

}


/***********************************************
*
* @Purpose:     Function that prints the *string in screen as error
* @Parameters:  char *string: string to be printed in screen
*
* @Return:      -
*
************************************************/
void MYSTRING_printErr(char *string){
    char buffer[500];
    bzero(buffer, sizeof(buffer));
    sprintf(buffer,"%s",string);
    write(STDERR_FILENO,buffer,strlen(buffer));

}

/***********************************************
*
* @Purpose:     Function that writes *string in the file descriptor specified
* @Parameters:  char *string: string to be printed in screen
*               int fd: file descriptor to be written in
*
* @Return:      -
*
************************************************/
void MYSTRING_printFD(int fd, char *string){
    char buffer[500];
    bzero(buffer, sizeof(buffer));
    sprintf(buffer,"%s",string);
    write(fd,buffer,strlen(buffer));

}


/***********************************************
*
* @Purpose:     Function that returns a string read in the fd file descriptor unitl the end char
* @Parameters:  char end: delimeter character
*               int fd: file descriptor to be written in
*
* @Return:      Returns a dynamic string read in fd until the end delimeter
*
************************************************/
char* MYSTRING_getMessage(int fd, char end) {
    int i = 0, size = 0;
    char c = '\0';
    char* string = (char*)malloc(sizeof(char));

    while (1) {
        size = read(fd, &c, sizeof(char));
        if (c != end && size > 0 && c != '&') {
            string = (char*)realloc(string, sizeof(char) * (i + 1));
            string[i++] = c;
        } else {
            i++; //For the \0
            break;
        }
    }

    string[i - 1] = '\0';
    return string;
}

/***********************************************
*
* @Purpose:     Function that returns a string read in the fd file descriptor unitl the end char
* @Parameters:  char end: delimeter character
*               int fd: file descriptor to be written if there has been an error reading
*               int *flag: flag that tells if
*
* @Return:      Returns a dynamic string read in fd until the end delimeter
*
************************************************/
char *MYSTRING_read_until(int fd, char delimiter, int *flag) {
	char *msg = (char *) malloc(1);
	char current;
	int i = 0;

	while (read(fd, &current, 1) > 0 ) {
		msg[i] = current;
		msg = (char *) realloc(msg, ++i + 1);

		if (current == delimiter)
			break;
	}
  if(errno == EAGAIN){
    *flag = 1;
  }else{
    flag = 0;
  }

	msg[i] = '\0';

	return msg;
}

/***********************************************
*
* @Purpose:     Function that returns a string read in the fd file descriptor unitl the end char
* @Parameters:  char end: delimeter character
*               int fd: file descriptor to be written if there has been an error reading
*
* @Return:      Returns a dynamic string read in fd until the end delimeter
*
************************************************/
char *MYSTRING_read_until_no_flag(int fd, char delimiter) {
	char *msg = (char *) malloc(1);
	char current;
	int i = 0;

	while (read(fd, &current, 1) > 0 ) {
		msg[i] = current;
		msg = (char *) realloc(msg, ++i + 1);

		if (current == delimiter)
			break;
	}

	msg[i-1] = '\0';

	return msg;
}




/***********************************************
*
* @Purpose:    Function that converts the *line string into an array of strings according to the *sep separator
* @Parameters:  char *sep: delimeter character
*               char *line: string with all the content
*
*
* @Return:      Returns all substrings found in line separated by *sep
*
************************************************/
char ** MYSTRING_convertToArr( char *line, const char *sep ){
    char **array = malloc( sizeof( char * ) );

    if ( array ){
        size_t n = 1;
        char *token = strtok( line, sep );

        while ( token ){
            char **tmp = realloc( array, ( n + 1 ) * sizeof( char * ) );
            if ( tmp == NULL ) break;
            array = tmp;
            ++n;
            array[ n - 2 ] = malloc( strlen( token ) + 1 );
            if ( array[ n - 2 ] != NULL ) strcpy( array[ n - 2 ], token );
            token = strtok( NULL, sep );
        }

        array[ n - 1 ] = NULL;
    }
    return array;
}

/***********************************************
*
* @Purpose:    Function that detects if there is a charater different than a number in *string
* @Parameters:
*               char *string: string with all the content
*
*
* @Return:      Returns one if there was no character, zero otherwise
*
************************************************/
int MYSTRING_detectCharacter(char *string){
  int i = 0;
  for(i = 0; i < (int) strlen(string); i++){
      if(string[i] < '0' || string[i] > '9'){
        return 0;
      }
  }
  return 1;
}
