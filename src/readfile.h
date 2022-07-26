/***********************************************
*
* @Purpose: Module that implements functions to manage read/write information of files
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#ifndef READFILE_H
    #define READFILE_H
    #define USERS_FILE   "users.txt"

    struct ServerFremen{
        int launchTime;
        char *IP;
        int port;
        char *folder;
    };

    struct ServerAtreides{
        char *IP;
        int port;
        char *folder;
    };


    struct User{
      char *name;
      char *postalCode;
      int ID;
      char *photo;
      int isConnected;
    };

    struct UserFremen{
      char *name;
      char *postalCode;
      int ID;
      char *photo;
      int isConnected;
    };

    /***********************************************
    *
    * @Purpose:     Function that is used to read the config file of the server in Fremen
    * @Parameters:  char *file: file with the config file name
    *               ServerFremen *server: struct where the information of the config file will be stored
    * @Return:      Returns one if successful, zero otherwise
    *
    ************************************************/
    int READFILE_readFremenServerFile (struct ServerFremen *server,  char *file);

    /***********************************************
    *
    * @Purpose:     Function that is used to read the config file of the server in Atreides
    * @Parameters:  char *file: file with the config file name
    *               ServerAtreides *server: struct where the information of the config file will be stored
    * @Return:      Returns one if successful, zero otherwise
    *
    ************************************************/
    int READFILE_readAtreidesServerFile (struct ServerAtreides *server,  char *file);

    /***********************************************
    *
    * @Purpose:     Function reads and returns all the users in file
    * @Parameters:  char *file: file where the user list is located
    *               User newUser: number of users in the system
    * @Return:      Returns the list with all the users in the file
    *
    ************************************************/
    struct User* READFILE_readUsersFile( const char *file, int*numUsers);

    /***********************************************
    *
    * @Purpose:     Function that adds a user on file
    * @Parameters:  char *file: file where the user list is located
    *               User newUser: user that will be added on the file
    *               int numUsers: number of users in the server
    * @Return:      -
    *
    ************************************************/
    void READFILE_addUser(const char *file, struct User newUser, int numUsers);
    
    /***********************************************
    *
    * @Purpose:     Function that counts and returns the number of bytes in file
    * @Parameters:  char *file: file to be counted
    *
    * @Return:      Returns one if successful, zero otherwise
    *
    ************************************************/
    int READFILE_countBytes(char* fileName);

    /***********************************************
    *
    * @Purpose:     Function that is used to send an image
    * @Parameters:  char *filename: image to be sent
    *               int fremen: flag to check who is the source of the send
    *               int sockfd: socket file descriptor where the image is sent
    * @Return:      -
    *
    ************************************************/
    void READFILE_sendImage(char* fileName, int sockfd, int fremen);
#endif
