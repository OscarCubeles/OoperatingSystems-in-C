/***********************************************
*
* @Purpose: Implementation of the function for the command module
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <math.h>
#include <dirent.h>
#include "command.h"
#include "mystring.h"
#include "connection.h"
#include "readfile.h"
#include "myprint.h"

/***********************************************
*
* @Purpose:     Sets the user as invalid with all its parameters as null and isConnected as 0
* @Parameters:  UserFremen *userFremen: user to be set as invalid
*               char *errString: string to be printed in screen
* @Return:      -
*
************************************************/
void COMMAND_setUserInvalid(struct UserFremen *userFremen, char *errString){
  userFremen->name = NULL;
  userFremen->postalCode = NULL;
  userFremen->ID = -1;
  userFremen->isConnected = 0;
  MYSTRING_print(errString);
}


/***********************************************
*
* @Purpose:     Function that executes the login command on Fremen
* @Parameters:  char *command: command entered by the user
*               int *sockfd: socket file descriptor for the transmission with the server
*               sockaddr_in s_addr: struct with IP and port information
* @Return:      returns the user with its information
*
************************************************/
struct UserFremen COMMAND_logIn(char *command, int* sockfd,struct sockaddr_in s_addr){
    int ID;
    char *postalInput;
    char *userName;
    struct UserFremen userFremen;
    struct Frame frame_sent;
    struct Frame frame_received;
    char buffer[200];
    // Exit if the command is invalid
    if(command == NULL){
        COMMAND_setUserInvalid(&userFremen,ERR_CMD_P);
        return userFremen;
    }else{
        // Saving the username
        userName = command;
        command = strtok(NULL, " ");
        if(command == NULL){
          COMMAND_setUserInvalid(&userFremen,ERR_CMD_P);
            return userFremen;
        }else{
            postalInput = command;
            command = strtok(NULL, " ");
            // Checking if the command is okay
            if(command != NULL){
              COMMAND_setUserInvalid(&userFremen,ERR_CMD_M);
              return userFremen;
            }else if(MYSTRING_detectCharacter(postalInput) == 0){
              COMMAND_setUserInvalid(&userFremen,ERR_ATOI);
              return userFremen;
            }else{
              *sockfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
              if(*sockfd < 0){
                  COMMAND_setUserInvalid(&userFremen,FRMN_ERR_SRCH);
                  close(*sockfd);
                  return userFremen;
              }
              // Try to connect to Atreides, if not possible, return.
              if (connect (*sockfd, (void *) &s_addr, sizeof (s_addr)) < 0){
                  COMMAND_setUserInvalid(&userFremen,FRMN_ERR_SRCH);
                  return userFremen;
              }
              //The command is okay, send a connection frame to Atreides
              bzero (frame_sent.source, sizeof (frame_sent.source));
              bzero (frame_sent.data, sizeof (frame_sent.data));
              frame_sent.type = 'C';
              strcpy(frame_sent.source,F_RQ_SRC);
              sprintf(frame_sent.data,"%s*%s",userName,postalInput);
              CONNECTION_writeImageFrame(*sockfd,frame_sent.source,frame_sent.type,frame_sent.data);
              // Receiving the frame from the server
              frame_received = CONNECTION_readImageFrame(*sockfd);
              // Checking if the frame is correct
              if(frame_received.type == 'O'){
                // Storing the ID received from the server
                ID = atoi(frame_received.data);
                bzero(buffer, sizeof(buffer));
                sprintf(buffer,"\nWelcome %s. You have the ID %d.\nYou are now connected to Atreides.\n\n",userName, ID);
                write(STDOUT_FILENO,buffer,strlen(buffer));

              }else if(frame_received.type == 'E'){
                // Frame is incorrect,
                MYSTRING_printErr("There was an error connecting to Atreides.\n");
              }
          }
        }
    }

    // Filling the user Fremen with its data
    userFremen.name = (char*)malloc(sizeof(char)*(int)(strlen(userName))+1);
    strcpy(userFremen.name,userName);
    userFremen.postalCode = (char*)malloc(sizeof(char)*(int)(strlen(postalInput))+1);
    strcpy(userFremen.postalCode,postalInput);
    userFremen.ID = ID;
    userFremen.isConnected = 1;

    return userFremen;
}

/***********************************************
*
* @Purpose:     Function that executes the logout command on Fremen
* @Parameters:  char *userName: user who is logging out
*               int sockfd: socket file descriptor for the transmission with the server
*               int ID: ID with the user logging out
* @Return:      -
*
************************************************/
void COMMAND_serverLogout(int sockfd, char* userName, int ID){
  struct Frame frame_sent;
  bzero(frame_sent.data, sizeof(frame_sent.data));
  bzero(frame_sent.source, sizeof(frame_sent.source));
  frame_sent.type = 'Q';
  strcpy(frame_sent.source,F_RQ_SRC);
  sprintf(frame_sent.data,"%s*%d",userName,ID);
  CONNECTION_writeImageFrame(sockfd,frame_sent.source,frame_sent.type,frame_sent.data);
}



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
void COMMAND_searchPeople(char *command, int ID, char* userName, int *sockfd, int *userIsConnected){
    char *postalCode;
    int numUsers = 0;
    char *aux;
    struct Frame frame_sent;
    struct Frame frame_received;
    // Checking id the command is correct
    if(command == NULL){
        MYSTRING_print(ERR_CMD_P);
    }else{
        // Storing the postal code
        postalCode = command;
        command = strtok(NULL, " ");
        // Checking if the command is correct
        if(command != NULL){
            MYSTRING_printErr(ERR_CMD_M);
        }else if(MYSTRING_detectCharacter(postalCode) == 0){
            MYSTRING_printErr(ERR_ATOI);
        }else{
             // The command is correct, send a search request frame to Atreides
             bzero (frame_sent.source, sizeof (frame_sent.source));
             bzero (frame_sent.data, sizeof (frame_sent.data));
             frame_sent.type = 'S';
             strcpy(frame_sent.source,F_RQ_SRC);
             sprintf(frame_sent.data,"%s*%d*%s\n",userName,ID,postalCode);
             CONNECTION_writeImageFrame(*sockfd,frame_sent.source,frame_sent.type,frame_sent.data);

             // Receiving the frame reply from ATREIDES
             frame_received = CONNECTION_readImageFrame(*sockfd);
             // Checking if the frame received is correctly
             if(frame_received.type == 'L'){
               // Getting the number of users from that postalCode
               aux = strtok(frame_received.data,"*");
               numUsers = atoi(aux);
               // Printing on screen the users received in the frame received, this function will read more frames if the search needs it
               MYPRINT_printSearchUsers(aux, postalCode,numUsers,*sockfd);
             }else if (frame_received.type == 'K'){
               MYSTRING_print(ERR_SRCH);
             // Case where the server is down, disconnect the user
             }else if(frame_received.type == '0'){
               MYSTRING_print(FRMN_ERR_SERVER);
               *userIsConnected = 0;
               close(*sockfd);
             }
        }
    }

}

/***********************************************
*
* @Purpose:     Function that calculates the MD5SUM of a given file
* @Parameters:  char *fileName: name of the file for which MD5SUM is to be performed
*
* @Return:      Return the MD5SUM of the given file
*
************************************************/
char* COMMAND_checkMD5SUM (char *fileName) {
  char buffer[150];
  char *md5sum;
  char *aux;
  int i = 0;

  sprintf(buffer, "md5sum %s", fileName);

  char **fullComand = MYSTRING_convertToArr(buffer," ");

  sprintf(buffer, "md5sum");

  int pipefd[2];
  pipe(pipefd);

  if (fork() == 0) {
      close(pipefd[0]);    // Close reading end in the child

      dup2(pipefd[1], 1);  // Send stdout to the pipe
      dup2(pipefd[1], 2);  // Send stderr to the pipe

      close(pipefd[1]);    // This descriptor is no longer needed

      execvp(buffer, fullComand); // Execute MD5SUM command
  }
  else {
      // Parent
      wait(NULL);

      close(pipefd[1]);  // Close the write end of the pipe in the parent

      while (read(pipefd[0], buffer, sizeof(buffer)) != 0) {
      }

      close(pipefd[0]);
  }

  while(fullComand[i] != NULL){
    free(fullComand[i]);
    i++;
  }
  free(fullComand);

  md5sum = strtok(buffer, " ");
  aux = strdup(md5sum);

  return aux;
}

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
void COMMAND_sendImage(char *command, char* folder, int *sockfd, int *userIsConnected){
    char *file;
    int numBytes;
    char buffer[100];
    char *aux;
    struct Frame frame_sent;
    struct Frame frame_received;

    if(command == NULL){
        MYSTRING_print(ERR_CMD_P);
    }else{
        // Storing the name of the image file
        file = command;

        command = strtok(NULL, " ");
        if(command != NULL){
            MYSTRING_printErr(ERR_CMD_M);
        }else{
            // The command is correct
            folder++;
            bzero (buffer, sizeof (buffer));
            sprintf(buffer,"%s/%s",folder, file);

            // Reading the number of bytes of the image
            numBytes = READFILE_countBytes(buffer);

            // If the image does not exists or is empty, print error message
            if (numBytes == 0) {
              MYSTRING_print("Error. This photo does not exist.\n");
            } else {

              // Calculating MD5SUM of the image
              aux = COMMAND_checkMD5SUM(buffer);

              //The photo exists, send a SEND request frame to Atreides
              bzero (frame_sent.source, sizeof (frame_sent.source));
              bzero (frame_sent.data, sizeof (frame_sent.data));
              frame_sent.type = 'F';
              strcpy(frame_sent.source,F_RQ_SRC);
              sprintf(frame_sent.data,"%s*%d*%s",file,numBytes,aux);
              CONNECTION_writeImageFrame(*sockfd, frame_sent.source, frame_sent.type, frame_sent.data);

              free(aux);

              // Send image to Atreides
              READFILE_sendImage(buffer, *sockfd, 1);

              // Receiving the frame reply from ATREIDES
              frame_received = CONNECTION_readImageFrame(*sockfd);

              if(frame_received.type == 'I'){
                // The image received by Atreides is the same as the one sended
                MYSTRING_print("Photo successfully sent to Atreides\n");
              }else if (frame_received.type == 'R'){
                // The image received by Atreides is not the same as the one sended
                MYSTRING_print("Error. The photo has not been sent correctly\n");
              }else if(frame_received.type == '0'){
                // The connection with Atreides is down
                MYSTRING_print(FRMN_ERR_SERVER);
                *userIsConnected = 0;
                close(*sockfd);
              }
            }
        }
    }
}


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
void COMMAND_downloadPhoto(char *command, char *folder, int *sockfd, int *userIsConnected) {
     char *ID;
     char *aux;
     char *fileName;
     char *md5sum;
     int newImage;
     int numBytes;
     char buffer[200];
     struct Frame frame_sent;
     struct Frame frame_received;

    if(command == NULL){
        MYSTRING_print(ERR_CMD_P);
    }else{
        ID = command;
        command = strtok(NULL, " ");
        if( command != NULL){
            MYSTRING_printErr(ERR_CMD_M);
        }else if(MYSTRING_detectCharacter(ID) == 0){
          MYSTRING_print("Error. Please enter a valid ID. It can only contain numbers.\n");
        }else{
             // Send a PHOTO request frame to Atreides
             bzero (frame_sent.source, sizeof (frame_sent.source));
             bzero (frame_sent.data, sizeof (frame_sent.data));
             frame_sent.type = 'P';
             strcpy(frame_sent.source, F_RQ_SRC);
             sprintf(frame_sent.data,"%s",ID);
             CONNECTION_writeImageFrame(*sockfd, frame_sent.source, frame_sent.type, frame_sent.data);

             // Receiving the frame reply from ATREIDES
             frame_received = CONNECTION_readImageFrame(*sockfd);

             if(frame_received.type == 'F' && strcmp(frame_received.data, F_NOK_FILE) != 0) {
               // The image requested to Atreides exists
               aux = strtok(frame_received.data,"*");
               fileName = (char*)malloc(sizeof(char)*(int)(strlen(aux))+1);
               strcpy(fileName, aux);
               aux = strtok(NULL,"*");
               numBytes = atoi(aux);
               aux = strtok(NULL,"*");
               md5sum = (char*)malloc(sizeof(char)*(int)(strlen(aux))+1);
               strcpy(md5sum, aux);
               folder++;
               sprintf(buffer,"%s/%s", folder, fileName);

               // Opening file to store the image
               newImage = open(buffer, O_WRONLY|O_CREAT|O_TRUNC, 0600);
               int j = 0;
               int i = 0;
               // Calculating the number of times necessary to store the image received completely
               j = numBytes / 240;

               if (newImage > 0) {
                 // Loop to store the data of the image received
                 while (i < j) {
                   frame_received = CONNECTION_readImageFrame(*sockfd);
                   write(newImage, frame_received.data, sizeof(frame_received.data));
                   i++;
                 }

                 // Calculating if there is more information data of the image received
                 j = (int) floor(numBytes / 240);
                 j = j * 240;
                 j = numBytes - j;

                 // In case that is data remaining to store, save it
                 if (j > 0) {
                   frame_received = CONNECTION_readImageFrame(*sockfd);
                   write(newImage, frame_received.data, j);
                 }

                 close(newImage);

                 free(fileName);

                 // Calculate the MD5SUM of the image received
                 aux = COMMAND_checkMD5SUM(buffer);

                 bzero (frame_sent.source, sizeof (frame_sent.source));
                 bzero (frame_sent.data, sizeof (frame_sent.data));
                 strcpy(frame_sent.source, F_OK_SRC);


                 if (strcmp(md5sum, aux) == 0) {
                   // The image received is the same as the one sent
                   frame_sent.type = 'I';
                   strcpy(frame_sent.data, F_OK_IMG);
                 } else {
                   // The image received is different from the one sent
                   frame_sent.type = 'R';
                   strcpy(frame_sent.data, F_NOK_IMG);
                 }

                 // Send reply to Atreides
                 CONNECTION_writeImageFrame(*sockfd, frame_sent.source, frame_sent.type, frame_sent.data);

                 free(aux);
                 free(md5sum);

                 MYSTRING_print("Photo downloaded\n");
               }

             } else if (frame_received.type == 'F' && strcmp(frame_received.data, F_NOK_FILE) == 0) {
               // The image requested to Atreides does not exist
               MYSTRING_print("Error. The photo requested does not exist\n");
             } else if(frame_received.type == '0'){
               // The connection with Atreides is down
               MYSTRING_print(FRMN_ERR_SERVER);
               *userIsConnected = 0;
               close(*sockfd);
             }
        }
    }

}

/***********************************************
*
* @Purpose:     Function that executes the logout command on Fremen
* @Parameters:  char *command: command entered by the user
*
* @Return:      -
*
************************************************/
int COMMAND_logOut(char *command){
     if(command != NULL){
        MYSTRING_print(ERR_CMD_M);
    }else{
        MYSTRING_print(OK_CMD);
        return 1;
    }
    return 0;
}

/***********************************************
*
* @Purpose:     Function that finds and returns the Arrakis System Pids
* @Parameters:  -
*
* @Return:      Return the list of all the PIDs separated with an space
*
************************************************/
char* COMMAND_scanPIDS(){
  int pipefd[2];
  int pid = 0;
  int status = 0;
  char buffer[512];
  bzero(buffer, sizeof(buffer));
  // Pidof command on fremen and atreides to check the Arrakis active processes
  char *programName = "pidof";
  char *args[] = {programName, "./Fremen", "./Atreides", NULL};
  char *pids;
  // Creating the pipe to redirect the output of the pidof command
  if (pipe(pipefd) == -1){
         MYSTRING_print(HRKN_ERR_PIPE);
        return NULL;
  }

  // Forking
  pid = fork();

  // Error Forking case
  if(pid < 0){
    status = -1;

  // Child case
  }else if (pid == 0){
      // Closing the reading end of the pipe
      close(pipefd[0]);
      // Redirecting stdout and stderr file descriptors to the pipe
      dup2(pipefd[1], 1);
      dup2(pipefd[1], 2);
      // executing the command that will be redirected to the pipe
      execvp(programName, args);
      close(pipefd[1]);
      _exit(0);

  // Parent case
  }else if(pid > 0){
      // Waiting for the child to end
      waitpid(pid, &status, 0);
      // Closing  the write end of the pipe in the parent
      close(pipefd[1]);
      if(read(pipefd[0], buffer, sizeof(buffer)) > 0){
      }else{
      }
      pids = (char*)malloc(sizeof(char)*(int)(strlen(buffer)+1));
      strcpy(pids,buffer);

      // Closing end of the pipe
      close(pipefd[0]);
  }
  return pids;
}

/***********************************************
*
* @Purpose:     Function that finds and returns all the PIDs of the user executing Fremen
* @Parameters:  -
*
* @Return:      Return the list of all the PIDs separated with an space
*
************************************************/
char* COMMAND_scanUserPIDS(){
  int pipefd[2];
  int pid = 0;
  int status = 0;
  char buffer[2048];

  bzero(buffer, sizeof(buffer));
  // Pidof command on fremen and atreides to check the Arrakis active processes
  char *programName = "ps";
  char *args[] = {programName, "-ho","pid", NULL};
  char *pids;
  // Creating the pipe to redirect the output of the pidof command
  if (pipe(pipefd) == -1){
        MYSTRING_print(HRKN_ERR_PIPE);
        return NULL;
  }

  // Forking
  pid = fork();

  // Error Forking case
  if(pid < 0){
    status = -1;

  // Child case
  }else if (pid == 0){
      // Closing the reading end of the pipe
      close(pipefd[0]);
      // Redirecting stdout and stderr file descriptors to the pipe
      dup2(pipefd[1], 1);
      dup2(pipefd[1], 2);
      // executing the command that will be redirected to the pipe
      execvp(programName, args);
      close(pipefd[1]);
      _exit(0);

  // Parent case
  }else if(pid > 0){
      // Waiting for the child to end
      waitpid(pid, &status, 0);
      // Closing  the write end of the pipe in the parent
      close(pipefd[1]);
      if(read(pipefd[0], buffer, sizeof(buffer)) > 0){
      }
      pids = (char*)malloc(sizeof(char)*(int)(strlen(buffer))+1);
      strcpy(pids,buffer);

      // Closing end of the pipe
      close(pipefd[0]);
  }
  return pids;
}

/***********************************************
*
* @Purpose:     Function that finds and returns all the files on the folder specified by folder
* @Parameters:  char * folder: folder where the files will be obtained
*
* @Return:      Return the list of files in folder separated with an space
*
************************************************/
char* COMMAND_getFiles(char *folder) {
  int pipefd[2];
  int pid = 0;
  int status = 0;
  char buffer[512];
  bzero(buffer, sizeof(buffer));
  // Pidof command on fremen and atreides to check the Arrakis active processes
  char *programName = "ls";
  char *args[] = {programName, folder, NULL};
  char *pids;
  // Creating the pipe to redirect the output of the pidof command
  if (pipe(pipefd) == -1){
         MYSTRING_print(HRKN_ERR_PIPE);
        return NULL;
  }

  // Forking
  pid = fork();

  // Error Forking case
  if(pid < 0){
    status = -1;

  // Child case
  }else if (pid == 0){
      // Closing the reading end of the pipe
      close(pipefd[0]);
      // Redirecting stdout and stderr file descriptors to the pipe
      dup2(pipefd[1], 1);
      dup2(pipefd[1], 2);
      // executing the command that will be redirected to the pipe
      execvp(programName, args);
      close(pipefd[1]);
      _exit(0);

  // Parent case
  }else if(pid > 0){
      // Waiting for the child to end
      waitpid(pid, &status, 0);
      // Closing  the write end of the pipe in the parent
      close(pipefd[1]);
      if(read(pipefd[0], buffer, sizeof(buffer)) > 0){
      }else{
      }
      pids = (char*)malloc(sizeof(char)*(int)(strlen(buffer)+1));
      strcpy(pids,buffer);

      // Closing end of the pipe
      close(pipefd[0]);
  }
  return pids;
}

/***********************************************
*
* @Purpose:     Function that removes the photo of filename in Fremen
* @Parameters:  -
*
* @Return:      -
*
************************************************/
void COMMAND_removeOldPhotos (char *fileName) {
  DIR* dp;
  char *aux;
  char *aux2;
  char *aux3;
  char buffer[15];
  struct dirent *dirp;

  aux = strtok(fileName, "/");
  aux2 = strtok(NULL, ".");

  dp = opendir(aux);

  if (dp != NULL) {
    while ((dirp = readdir(dp)) != NULL) {
      aux = strtok(dirp->d_name, ".");
      aux3 = strtok(NULL, " ");

      if (aux != NULL && aux3 != NULL && strcmp(aux, aux2) == 0) {
        sprintf(buffer, "%s/%s.%s", fileName, aux, aux3);
        remove(buffer);
      }
    }

    closedir(dp);
  }
}

/***********************************************
*
* @Purpose:     Function that gets the image extension given  a file
* @Parameters:  char *fileName: string determining the file whose extension is returned
*               char *path: string determining the path to the file
*
* @Return:      Return the image extension
*
************************************************/
char* COMMAND_getImageExtension (char *fileName, char *path) {
  DIR* dp;
  char *extension;
  char *aux;
  char *aux2;
  struct dirent *dirp;

  dp = opendir(path);

  if (dp != NULL) {
    while ((dirp = readdir(dp)) != NULL) {
      aux2 = strdup(dirp->d_name);
      aux = strtok(dirp->d_name, ".");
      extension = strtok(NULL, "\0");

      if (aux != NULL && extension != NULL && strcmp(aux, fileName) == 0) {
        closedir(dp);
        return aux2;
      }
      free(aux2);
    }

    closedir(dp);
  }

  return NULL;
}
