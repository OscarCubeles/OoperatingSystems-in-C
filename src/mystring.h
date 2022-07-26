/***********************************************
*
* @Purpose: Module that implements functions to manage strings
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#ifndef MYSTRING_H
    #define MYSTRING_H

    /***********************************************
    *
    * @Purpose:     Function thet prints *string in screen
    * @Parameters:  char *string: string to be printed in screen
    *
    * @Return:      -
    *
    ************************************************/
    void MYSTRING_print(char *string);

    /***********************************************
    *
    * @Purpose:     Function that prints the *string in screen as error
    * @Parameters:  char *string: string to be printed in screen
    *
    * @Return:      -
    *
    ************************************************/
    void MYSTRING_printErr(char *string);

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
    char *MYSTRING_read_until(int fd, char delimiter, int *flag);

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
    char ** MYSTRING_convertToArr( char *line, const char *sep );

    /***********************************************
    *
    * @Purpose:     Function that returns a string read in the fd file descriptor unitl the end char
    * @Parameters:  char end: delimeter character
    *               int fd: file descriptor to be written in
    *
    * @Return:      Returns a dynamic string read in fd until the end delimeter
    *
    ************************************************/
    char* MYSTRING_getMessage(int fd, char end);

    /***********************************************
    *
    * @Purpose:     Function that writes *string in the file descriptor specified
    * @Parameters:  char *string: string to be printed in screen
    *               int fd: file descriptor to be written in
    *
    * @Return:      -
    *
    ************************************************/
    void MYSTRING_printFD(int fd, char *string);

    /***********************************************
    *
    * @Purpose:     Function that returns a string read in the fd file descriptor unitl the end char
    * @Parameters:  char end: delimeter character
    *               int fd: file descriptor to be written if there has been an error reading
    *
    * @Return:      Returns a dynamic string read in fd until the end delimeter
    *
    ************************************************/
    char *MYSTRING_read_until_no_flag(int fd, char delimiter);
    
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
    int MYSTRING_detectCharacter(char *string);
#endif
