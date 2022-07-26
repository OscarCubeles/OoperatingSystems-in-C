/***********************************************
*
* @Purpose: Implementation of Atreides program
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021* @Last Modifcation Date: January 2022
*
************************************************/
#define _GNU_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <strings.h>
#include <sys/wait.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include <math.h>


#include "mystring.h"
#include "command.h"
#include "readfile.h"
#include "connection.h"
#include "myprint.h"
#include "user.h"

#define MAX_LISTEN 5

struct ServerAtreides server;
struct User *users;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t *threads;
int num_threads = 0;
int *threadsocks;
int numUsers;
int sock;

int ATREIDES_userExists(const char *userName);
int ATREIDES_getUserID(const char *userName);
void *ATREIDES_dedicatedThread(void *arg);
void ATREIDES_sendSearch( char* postalCode, int sockfd);
void ATREIDES_exitThread();
void ATREIDES_waitPhoto();
void ATREIDES_exit();
void ATREIDES_terminate(char *message, int flagMem);

int main (int argc, char *argv[]){
  struct sockaddr_in s_addr;
  uint16_t port;
  int threadStatus;

  // Allocating initial memory for threads
  threads = (pthread_t*) malloc(sizeof(pthread_t));
  threadsocks = (int*) malloc(sizeof(int));

  // Controlling the CTRL+C signal
  signal(SIGINT, ATREIDES_exit);

  // Checking the number of arguments, if incorrect --> exit
  if(argc != 2) ATREIDES_terminate(ATR_ERR_ARG,0);

  // Loading the  users in USERS_FILE
  users = READFILE_readUsersFile(USERS_FILE,&numUsers);

  // Checking the correct read from USERS_FILE
  if(users == NULL) ATREIDES_terminate(ATR_ERR_USR_FILE,0);

  // Checking if config file was successfully read, if not --> exit
  if (READFILE_readAtreidesServerFile(&server, argv[1]) == 0) ATREIDES_terminate(ATR_ERR_FILE,1);

  // Check if the port is valid, if not --> exit
  if (server.port < 1 || server.port > 65535) ATREIDES_terminate(ATR_ERR_PORT,1);
  port = server.port;

  // Create the socket file descriptor, if incorrect --> exit
  sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0) ATREIDES_terminate(ATR_ERR_SCKFD,1);

  // Specifyig IP and port
  bzero (&s_addr, sizeof (s_addr));
  s_addr.sin_family = AF_INET;
  // Port to correct format
  s_addr.sin_port = htons (port);
  // Don't care about Ip -> INADDR_ANY
  s_addr.sin_addr.s_addr = INADDR_ANY;

  // Binding
  if (bind (sock, (void *) &s_addr, sizeof (s_addr)) < 0) ATREIDES_terminate(ATR_ERR_BIND,1);

  // We now open the port MAX_LISTEN backlog queue
  listen (sock, MAX_LISTEN);
	// Printing Atreides initial message
	MYSTRING_print(ATR_MSG_WEL);

  MYSTRING_print(ATR_MSG_WAIT);
    while (1){
        struct sockaddr_in c_addr;
        socklen_t c_len = sizeof (c_addr);
        // When executing accept we should add the same cast used in the bind function
        int newsock = accept (sock, (void *) &c_addr, &c_len);
        if (newsock < 0){
            MYSTRING_printErr(ATR_ERR_ACCPT);
            exit (EXIT_FAILURE);
        }

        threadsocks = (int*)realloc(threadsocks, sizeof(int) * (num_threads + 1));

        threadsocks[num_threads] = newsock;

        // Creating the new thread for the new connection
        threads = (pthread_t*)realloc(threads, sizeof(pthread_t) * (num_threads + 1));
        threadStatus =  pthread_create(&threads[num_threads], NULL, ATREIDES_dedicatedThread,(void*) &newsock);
        if(threadStatus  != 0){
          MYSTRING_printErr(ATR_ERR_THREAD);
        }else{
          num_threads++;
        }
    }

    close (sock);
    return EXIT_SUCCESS;
}

/***********************************************
*
* @Purpose:     Frees all dynamic memory allocated and closes FD in program exit
* @Parameters:  -
* @Return:      -
*
************************************************/
void ATREIDES_exit(){
    int i = 0;
    for(i = 0; i < numUsers; i++){
      free(users[i].name);
      free(users[i].postalCode);
      free(users[i].photo);
    }
    free(users);
    free(server.IP);
    free(server.folder);
    for(i = 0; i < num_threads; i++){
      close(threadsocks[i]);
    }
    for(i = 0; i < num_threads; i++){
      pthread_cancel(threads[i]);
      pthread_join (threads[i], NULL);
      pthread_detach(threads[i]);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    free(threads);
    free(threadsocks);
    close(sock);
    exit(0);
}

/***********************************************
*
* @Purpose:     Function that prints a message in screen saying that it is waiting for a transmission to end
* @Parameters:  -
*
* @Return:      -
*
************************************************/
void ATREIDES_waitPhoto(){
  MYSTRING_print(ATR_PHTO_TRMT);
}

/***********************************************
*
* @Purpose:     Function that returns if a user exists or not
* @Parameters:  char *userName: user whose existance is requested
*
* @Return:      One if the user exists, zero otherwise
*
************************************************/
int ATREIDES_userExists(const char *userName){
  int i = 0;
  for(i = 0; i < numUsers; i++){
    if(strcmp(userName,users[i].name) == 0){
      return 1;
    }
  }
  return 0;
}

/***********************************************
*
* @Purpose:     Function that gets the user ID given its name
* @Parameters:  char *userName: user whose ID is requested
*
* @Return:      Returns the user ID
*
************************************************/
int ATREIDES_getUserID(const char *userName){
  int i = 0;
  for(i = 0; i <numUsers; i++){
    if(strcmp(userName,users[i].name) == 0){
      return users[i].ID;
    }
  }
  return 0;
}

/***********************************************
*
* @Purpose:     Function that fills the data for one search request
* @Parameters:  int sockfd: socket file descriptor where the users will be written
*               char* postalcode: postal code for the search request
* @Return:      -
*
************************************************/
void ATREIDES_sendSearch( char* postalCode, int sockfd){
    int userCounter = 0;
    int sentUsers = 0;
    char buffer[500];
    char aux[300];
    int i = 0;
    int counterBytes = 0;
    int multipleFrames = 0;
    struct Frame sent_frame;

    // Initializing frame
    sent_frame.type = 'L';
    bzero (sent_frame.source, sizeof (sent_frame.source));
    strcpy(sent_frame.source,F_OK_SRC);
    bzero (sent_frame.data, sizeof (sent_frame.data));


    // Adding '\0' to the postalcode
    postalCode[strlen(postalCode)-1]='\0';
    bzero (aux, sizeof (aux));

    // Counting the amount of users on that postal code
    for(i = 0;i< numUsers; i++){
        if(strcmp(users[i].postalCode, postalCode) == 0){
          userCounter++;
        }
    }
    // Printing in atreides terminal the users found in that postal code
    bzero(buffer, sizeof(buffer));
    sprintf(buffer,"\nThere are %d human beings at: %s\n",userCounter,postalCode);
    MYSTRING_print(buffer);

    // Printing the users in that postalcode
    for(i = 0;i< numUsers; i++){
        if(strcmp(users[i].postalCode, postalCode) == 0){
          bzero(buffer, sizeof(buffer));
          sprintf(buffer,"Name: %s  ID: %d\n",users[i].name,users[i].ID);
          MYSTRING_print(buffer);
        }
    }
    MYSTRING_print("\n");

    // Putting the amount of users in the frame data
    sprintf(aux,"%d",userCounter);
    strcat(sent_frame.data,aux);

    // Filling and sending the users in the initial frame
    for(i = 0;i < numUsers; i++){
      if(strcmp(users[i].postalCode, postalCode) == 0){
        sprintf(aux,"*%s*%s",users[i].name,users[i].postalCode);
        counterBytes = counterBytes + (int)strlen(aux);
        if(counterBytes < 240){
          strcat(sent_frame.data,aux);
          bzero (aux, sizeof (aux));
          sentUsers++;
        }else{
          // If more users are required to be sent, send the initial frame and set a flag to tell that multiple frames must be sent
          CONNECTION_writeImageFrame(sockfd,sent_frame.source,sent_frame.type,sent_frame.data);
          bzero (sent_frame.data, sizeof (sent_frame.data));
          multipleFrames = 1;
          counterBytes = 0;
          break;
        }

      }
    }

    // If only one frame is requiered -> send it, otherwise send multiple ones
    if(multipleFrames == 0){
      CONNECTION_writeImageFrame(sockfd,sent_frame.source,sent_frame.type,sent_frame.data);
    }else{

        // Iterating until all the users have been sent
        for(;i < numUsers; i++){
          if(strcmp(users[i].postalCode, postalCode) == 0){
            bzero (aux, sizeof (aux));
            sprintf(aux,"%s*%s*",users[i].name,users[i].postalCode);
            counterBytes = counterBytes + (int)strlen(aux);
            if(counterBytes < 240){
              strcat(sent_frame.data,aux);
              bzero (aux, sizeof (aux));
              sentUsers++;
            }else{
              CONNECTION_writeImageFrame(sockfd,sent_frame.source,sent_frame.type,sent_frame.data);
              bzero (sent_frame.data, sizeof (sent_frame.data));
              i--;
              counterBytes = 0;
            }
            if(sentUsers == userCounter){
              CONNECTION_writeImageFrame(sockfd,sent_frame.source,sent_frame.type,sent_frame.data);
              bzero (sent_frame.data, sizeof (sent_frame.data));
              counterBytes = 0;
              break;
            }
          }
        }
    }
}


/***********************************************
*
* @Purpose:     Function that executes the dedicated thread for an Atreides client
* @Parameters:  void * arg: socket file descriptor for the client
*
* @Return:      -
*
************************************************/
void *ATREIDES_dedicatedThread(void *arg){
    int *newsock = (int *) arg;
    int sock = *newsock;
    //printf("filde descriptor: %d\n", *newsock);
    char *userName;
    int actualUserId;
    //int i = 0;
    char *aux;
    char *aux2;
    char *fileName;
    char *md5sum;
    char buffer[200];
    char buffer2[210];
    char buffer3[50];
    char *postalCode;
    char *searchPostalCode;
    int end = 0;
    int numBytes;
    int newImage;
    char *path;
    path = server.folder;
    path++;
    struct Frame received_frame;
    struct Frame sent_frame;
    while(end == 0){

      // While there is data from client, read it and display it
      received_frame = CONNECTION_readImageFrame(sock);

      // Flushing the memory of the frame to be sent
      bzero (sent_frame.source, sizeof (sent_frame.source));
      bzero (sent_frame.data, sizeof (sent_frame.data));

      // Checking the type of frame received
      switch(CONNECTION_checkFrameType(received_frame)){
          // New login case
          case 1:
            // Mutual exclusion to avoid two threads to user the same users
            pthread_mutex_lock(&mutex);
            // Parsing the user and postal code of the frame received
            userName = strtok (received_frame.data,"*");
            postalCode = strtok (NULL,"*");
            MYSTRING_printLoginRequest(userName, postalCode);
            // Checking if the user exists
            if(ATREIDES_userExists(userName) == 1){
                // Return the ID of the user since the user already exists
                sprintf(sent_frame.data,"%d",ATREIDES_getUserID(userName));
            }else{
                // Create the user and return its ID
                struct User newUser = USER_createUser(userName,postalCode,numUsers);
                // Setting the ID of the new user
                sprintf(sent_frame.data,"%d",numUsers+1);
                // Adding the user to the list of users
                users = (struct User*)realloc(users, sizeof(struct User) * (numUsers + 1));
                users[numUsers] = newUser;
                numUsers++;

                // Add the user to the file
                READFILE_addUser(USERS_FILE,users[numUsers-1],numUsers);
            }
            actualUserId = ATREIDES_getUserID(userName);
            // Sending the frame with the information of the logged user
            sent_frame.type = 'O';
            strcpy(sent_frame.source,F_OK_SRC);
            CONNECTION_writeImageFrame(sock,sent_frame.source,sent_frame.type,sent_frame.data);
            MYSTRING_print(ATR_MSG_REPLY);
            pthread_mutex_unlock(&mutex);
            break;
          // Logout case
          case 2:
            // Print logout messages
            MYPRINT_printAtreidesLogout(received_frame.data);
            // Set end to one to finish the loop
            end = 1;
            break;

          // Search case
          case 3:
              // Mutual exclusion to avoid two threads to user the same users
              pthread_mutex_lock(&mutex);
              // Printing the information received in the search request
              aux = MYPRINT_printSearchRequest(received_frame.data);
              searchPostalCode = (char*)malloc(sizeof(char)*(int)(strlen(aux))+1);
              strcpy(searchPostalCode,aux);
              // Filling and sending the data with the users on that postal code
              ATREIDES_sendSearch(aux,sock);
              free(searchPostalCode);
              pthread_mutex_unlock(&mutex);
              break;

          // Send case
          case 4:
              aux = strtok(received_frame.data,"*");
              fileName = (char*)malloc(sizeof(char)*(int)(strlen(aux))+1);
              strcpy(fileName, aux);
              aux = strtok(NULL,"*");
              numBytes = atoi(aux);
              aux = strtok(NULL,"*");
              md5sum = (char*)malloc(sizeof(char)*(int)(strlen(aux))+1);
              strcpy(md5sum, aux);
              aux = strtok(fileName, ".");
              aux = strtok(NULL, ".");
              sprintf(buffer,"%s/%d.%s", path, actualUserId, aux);
              strcpy(buffer2, buffer);
              break;

          // Receiving image data
          case 5:
              // Removing old photos of the user
              COMMAND_removeOldPhotos(buffer2);

              sprintf(buffer2, "Received send %s.%s from %d\n", fileName, aux, actualUserId);
              MYSTRING_print(buffer2);

              // Opening file to store the image
              newImage = open(buffer, O_WRONLY|O_CREAT|O_TRUNC, 0600);
              int j = 0;
              int i = 0;
              // Calculating the number of times necessary to store the image received completely
              j = numBytes / 240;

              // Writing part of the image received
              write(newImage, received_frame.data, sizeof(received_frame.data));

              // Loop to store the remaining data of the image received
              while (i < j-1) {
                received_frame = CONNECTION_readImageFrame(sock);
                write(newImage, received_frame.data, sizeof(received_frame.data));
                i++;
              }

              // Calculating if there is more information data of the image received
              j = (int) floor(numBytes / 240);
              j = j * 240;
              j = numBytes - j;

              // In case that is data remaining to store, save it
              if (j > 0) {
                received_frame = CONNECTION_readImageFrame(sock);
                write(newImage, received_frame.data, j);
              }

              close(newImage);

              sprintf(buffer2,"Stored as %d.%s\n\n", actualUserId, aux);
              MYSTRING_print(buffer2);

              free(fileName);


              // Calculating MD5SUM of the image received
              aux = COMMAND_checkMD5SUM(buffer);

              bzero (sent_frame.source, sizeof (sent_frame.source));
              bzero (sent_frame.data, sizeof (sent_frame.data));
              strcpy(sent_frame.source, F_OK_SRC);

              if (strcmp(md5sum, aux) == 0) {
                // The image received is the same as the one sent
                sent_frame.type = 'I';
                strcpy(sent_frame.data, F_OK_IMG);
              } else {
                // The images received is different from the one sent
                sent_frame.type = 'R';
                strcpy(sent_frame.data, F_NOK_IMG);
              }

              // Send reply frame to Fremen
              CONNECTION_writeImageFrame(sock, sent_frame.source, sent_frame.type, sent_frame.data);

              free(aux);

              free(md5sum);
              break;

          // Photo case
          case 6:
            // Temporarily disable ^C
            signal(SIGINT, ATREIDES_waitPhoto);

            // Get the name of the requested image
            aux = strtok(received_frame.data,"\0");
            // Get the extension of the requested image
            aux2 = COMMAND_getImageExtension(aux, path);

            if (aux2 != NULL) {
              // The image requested exists
              sprintf(buffer,"%s/%s", path, aux2);
              strcpy(buffer3, buffer);
              // Get the number of bytes of the image
              numBytes = READFILE_countBytes(buffer);
            } else {
              // The image does not exists, set the the number of bytes to zero
              numBytes = 0;
            }

            sprintf(buffer2,"\nReceived photo %s from %d\n", aux, actualUserId);
            MYSTRING_print(buffer2);

            if (numBytes == 0) {
              // The photo does not exists or is empty
              MYSTRING_print("No photo registered\n");
              bzero (sent_frame.source, sizeof (sent_frame.source));
              bzero (sent_frame.data, sizeof (sent_frame.data));
              strcpy(sent_frame.source, F_OK_SRC);
              sent_frame.type = 'F';
              strcpy(sent_frame.data, F_NOK_FILE);

              // Send reply frame to Fremen
              CONNECTION_writeImageFrame(sock, sent_frame.source, sent_frame.type, sent_frame.data);

              MYSTRING_print(ATR_MSG_REPLY);
            } else {
              // The image requested exists

              // Calculate the MD5SUM of the image requested
              aux = COMMAND_checkMD5SUM(buffer);

              //The photo exists, send an answer frame to FREMEN
              bzero (sent_frame.source, sizeof (sent_frame.source));
              bzero (sent_frame.data, sizeof (sent_frame.data));
              sent_frame.type = 'F';
              strcpy(sent_frame.source,F_OK_SRC);
              sprintf(sent_frame.data,"%s*%d*%s", aux2, numBytes, aux);
              CONNECTION_writeImageFrame(sock, sent_frame.source, sent_frame.type, sent_frame.data);

              free(aux);

              // Send the image data to Fremen
              READFILE_sendImage(buffer3, sock, 0);

              // Receiving the frame reply from FREMEN
              received_frame = CONNECTION_readImageFrame(sock);

              if(received_frame.type == 'I'){
                // The image received by Atreides is the same as the one sended
                MYSTRING_print("Photo successfully sent to Fremen\n");
              }else if (received_frame.type == 'R'){
                // The image received by Atreides is not the same as the one sended
                MYSTRING_print("Error. The photo has not been sent correctly\n");
              }
            }
            free(aux2);
            // Activating again ^C
            signal(SIGINT, ATREIDES_exit);
            break;

          // Error occurred
          case -1:
            // Frame not okay
            sent_frame.type = 'E';
            strcpy(sent_frame.source,F_OK_SRC);
            strcpy(sent_frame.data, F_NOK_DATA);
            CONNECTION_writeImageFrame(sock,sent_frame.source,sent_frame.type,sent_frame.data);
            break;
      }
    }
    pthread_exit(NULL);
    return NULL;
}

/***********************************************
*
* @Purpose:     Function that exits the program if an error occurrs in initalization
* @Parameters:  char * message: Message that will be displayed in exit
*               int flagMem: Flag to control if memory needs to be freed
* @Return:      -
*
************************************************/
void ATREIDES_terminate(char *message, int flagMem){
  if(flagMem == 1){
    for(int i = 0; i <numUsers; i++){
      free(users[i].postalCode);
      free(users[i].photo);
      free(users[i].name);
    }
    free(users);
    free(server.IP);
    free(server.folder);
  }

  MYSTRING_print(message);
  free(threads);
  free(threadsocks);
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  close(sock);
  exit (EXIT_FAILURE);
}
