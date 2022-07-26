/***********************************************
*
* @Purpose: Module that implements functions to print on screen and that stores static strings
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#ifndef MYPRINT_H
    #define MYPRINT_H

    // ATREIDES MESSAGES
    #define ATR_ERR_BIND        "Error. Atreides Binding.\n"
    #define ATR_ERR_SCKFD       "Error. Unable to create socket file descriptor\n"
    #define	ATR_ERR_PORT        "Error. The port number is invalid.\n"
    #define ATR_ERR_ARG         "Error. Number of arguments is not correct\n"
    #define ATR_ERR_FILE        "Error. Atreides unable to open the configuration file\n"
    #define ATR_ERR_ACCPT       "Error. Unable to accept socket\n"
    #define ATR_ERR_USR_FILE    "Error. Atreides unable to load the users file"
    #define ATR_ERR_THREAD      "Error. Atreides unable to create thread\n"
    #define ATR_MSG_WEL         "\nATREIDES SERVER\nConfiguration file read\n\n"
    #define ATR_MSG_WAIT        "\nWaiting connections...\n\n"
    #define ATR_MSG_SEARCH      "\nThere are %d human beings at %d:\n"
    #define ATR_MSG_USR_SRCH    "Name: %s  ID: %d\n"
    #define ATR_MSG_USR_LOGIN   "Received  login %s %s\n"
    #define ATR_MSG_REPLY       "Reply Sent.\n\n"
    #define ATR_PHTO_TRMT       "\nPhoto being transmitted, please wait to do ^C\n"


    // FREMEN MESSAGES
    #define FRMN_ERR_CNNCT      "Error connecting with the server\n"
    #define FRMN_ERR_SCKT  	    "Error. Fremen was unable to creating socket file descriptor\n"
    #define FRMN_ERR_CMD	      "Invalid cmd command\n"
    #define	FRMN_ERR_PORT  	    "Error. The port number in the configuration file is invalid.\n"
    #define FRMN_ERR_IP	  	    "Error. The IP number in the configuration file is invalid. It could not be converted from ASCII to Network format.\n"
    #define FRMN_ERR_ARG   	    "Error. number of arguments is not correct\n"
    #define FRMN_ERR_CONNECT    "Error. No Connection has been established with the server. Please log in.\n"
    #define FRMN_ERR_FILE  	    "Error. Fremen unable to open configuration file\n"
    #define FRMN_ERR_LOGIN      "Error. Unable to log in since you have already logged in.\n"
    #define FRMN_ERR_NULL       "Error. Null input entered. Try again.\n"
    #define FRMN_ERR_SRCH       "Error. Fremen could not connect to Atreides. Server may be down.\n"
    #define FRMN_MSG_WEL   	    "\nWelcome to Fremen\n"
    #define FRMN_CMD_LIN   	    "LOGIN"
    #define FRMN_CMD_SRCH  	    "SEARCH"
    #define FRMN_CMD_SND   	    "SEND"
    #define FRMN_CMD_PHT   	    "PHOTO"
    #define FRMN_CMD_LOUT  	    "LOGOUT"
    #define FRMN_CMD_DOLLAR     "$ "
    #define OK_CMD              "Command OK\n"
    #define ERR_CMD_P           "Command KO. Missing parameters\n"
    #define ERR_CMD_M           "Command KO. Too many parameters\n"
    #define ERR_SRCH            "Error. There was a problem receiving the answer from the server\n"
    #define ERR_ATOI            "Error. Please enter a valid postal code. It can only contain numbers\n"
    #define FRMN_ERR_SERVER     "Error. The connection with Atreides is down.\n"


    // Harkonen messages
    #define HRKN_ERR_ARGS       "Error. The number of arguments is incorrect. Please just enter one number.\n"
    #define HRKN_ERR_ARG        "Error. The Argumnet passed contains letters. It can only contain numbers\n"
    #define HRKN_MSG_START      "\nStarting Harkonen...\n\n"
    #define HRKN_MSG_SCAN       "Scanning PIDS...\n"
    #define HRKN_MSG_EXIT       "Exiting...\n"
    #define HRKN_MSG_KILL       "Killing PID %d.\n\n"
    #define HRKN_MSG_NOKILL     "No PIDs to kill were found.\n\n"
    #define HRKN_ERR_PIPE       "Error creating pipe to scan pids\n"



    /***********************************************
    *
    * @Purpose:     Function that prints all the information when a logout in atreides is received
    * @Parameters:  char message[240]: Data received with the information of who logged out
    *
    * @Return:      -
    *
    ************************************************/
    void MYPRINT_printAtreidesLogout(char message[240]);

    /***********************************************
    *
    * @Purpose:     Function that prints all the information when a search is received
    * @Parameters:  -
    *
    * @Return:      -
    *
    ************************************************/
    void MYPRINT_printSearchUsers(char *aux, char *postalCode, int numUsers, int sockfd);

    /***********************************************
    *
    * @Purpose:     Function that prints the information received in a search request from Fremen
    * @Parameters:  char message[240]: Data received with the information of the request
    *
    * @Return:      -
    *
    ************************************************/
    char * MYPRINT_printSearchRequest(char data[240]);

    /***********************************************
    *
    * @Purpose:     Function that prints the information received in a login request from Fremen
    * @Parameters:  char *username: user who requested the login
    *               char *postalCode: postal code of the user who requested the login
    *
    * @Return:      -
    *
    ************************************************/
    void MYSTRING_printLoginRequest(char * userName, char * postalCode);

#endif
