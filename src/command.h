/***********************************************
*
* @Purpose: Module that implements functions to manage Linux & custom commands for the system
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#ifndef COMMAND_H
    #define COMMAND_H

    /***********************************************
    *
    * @Purpose:     Function that executes the login command on Fremen
    * @Parameters:  char *command: command entered by the user
    *               int *sockfd: socket file descriptor for the transmission with the server
    *               sockaddr_in s_addr: struct with IP and port information
    * @Return:      returns the user with its information
    *
    ************************************************/
   struct UserFremen COMMAND_logIn(char *command,   int*sockfd,struct sockaddr_in s_addr);
   /***********************************************
   *
   * @Purpose:     Function that executes the search command on Fremen
   * @Parameters:  char *command: command entered by the user
   *               int *sockfd: socket file descriptor for the transmission with the server
   *               int ID: ID with the user logging out
   *               int *userIsConnected: flag to indicate whether the user is connected
   *               char *userName: user who is executing the search
   * @Return:      -
   *
   ************************************************/
   void COMMAND_searchPeople(char *command, int ID, char* userName, int *sockfd, int *userIsConnected);

   /***********************************************
   *
   * @Purpose:     Function that calculates the MD5SUM of a given file
   * @Parameters:  char *fileName: name of the file for which MD5SUM is to be performed
   *
   * @Return:      Return the MD5SUM of the given file
   *
   ************************************************/
   char* COMMAND_checkMD5SUM (char *fileName);

   /***********************************************
   *
   * @Purpose:     Function that executes the send command on Fremen
   * @Parameters:  char *command: command entered by the user
   *               int *sockfd: socket file descriptor for the transmission with the server
   *               int *userIsConnected: flag to indicate whether the user is connected
   *               char *folder: folder path indicating the folder of the user  who is executing the search
   * @Return:      -
   *
   ************************************************/
   void COMMAND_sendImage(char *command, char folder[], int *sockfd, int *userIsConnected);

   /***********************************************
   *
   * @Purpose:     Function that executes the photo command Fremen
   * @Parameters:  char *command: command entered by the user
   *               int *sockfd: socket file descriptor for the transmission with the server
   *               int *userIsConnected: flag to indicate whether the user is connected
   *               char *folder: folder path indicating the folder of the user  who is executing the search
   * @Return:      -
   *
   ************************************************/
   void COMMAND_downloadPhoto(char *command, char *folder, int *sockfd, int *userIsConnected);

   /***********************************************
   *
   * @Purpose:     Function that executes the logout command on Fremen
   * @Parameters:  char *command: command entered by the user
   *
   * @Return:      -
   *
   ************************************************/
   int COMMAND_logOut(char *command);

   /***********************************************
   *
   * @Purpose:     Function that executes the logout command on Fremen
   * @Parameters:  char *userName: user who is logging out
   *               int sockfd: socket file descriptor for the transmission with the server
   *               int ID: ID with the user logging out
   * @Return:      -
   *
   ************************************************/
   void COMMAND_serverLogout(int sockfd, char * userName, int ID);

   /***********************************************
   *
   * @Purpose:     Function that finds and returns all the PIDs of the user executing Fremen
   * @Parameters:  -
   *
   * @Return:      Return the list of all the PIDs separated with an space
   *
   ************************************************/
   char* COMMAND_scanUserPIDS();

   /***********************************************
   *
   * @Purpose:     Function that finds and returns the Arrakis System Pids
   * @Parameters:  -
   *
   * @Return:      Return the list of all the PIDs separated with an space
   *
   ************************************************/
   char* COMMAND_scanPIDS();

   /***********************************************
   *
   * @Purpose:     Function that removes the photo of filename in Fremen
   * @Parameters:  -
   *
   * @Return:      -
   *
   ************************************************/
   void COMMAND_removeOldPhotos (char *fileName);

   /***********************************************
   *
   * @Purpose:     Function that finds and returns all the files on the folder specified by folder
   * @Parameters:  char * folder: folder where the files will be obtained
   *
   * @Return:      Return the list of files in folder separated with an space
   *
   ************************************************/
   char* COMMAND_getFiles(char *folder);

   /***********************************************
   *
   * @Purpose:     Function that gets the image extension given  a file
   * @Parameters:  char *fileName: string determining the file whose extension is returned
   *               char *path: string determining the path to the file
   *
   * @Return:      Return the image extension
   *
   ************************************************/
   char* COMMAND_getImageExtension (char *fileName, char *path);

#endif
