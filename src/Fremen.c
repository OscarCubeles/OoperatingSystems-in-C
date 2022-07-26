/***********************************************
*
* @Purpose: Implementation of Fremen program
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
* @Last Modifcation Date: January 2022
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

#include "mystring.h"
#include "command.h"
#include "readfile.h"
#include "connection.h"
#include "myprint.h"
#include "user.h"

void FREMEN_freeMemory();
void FREMEN_exit(char * exitMessage, int freeMem);
void FREMEN_checkInput(char *command, char *fullLine, int *sockfd,struct sockaddr_in s_addr);
int FREMEN_executeCommand(char *command, char **args);
void FREMEN_sigPipe();
void FREMEN_alarm();
void FREMEN_idle();




struct ServerFremen server;
struct UserFremen userFremen;
char *inBuffer;
char *command;
char *auxBuffer;
char *userName;

struct in_addr ipAddress;
struct sockaddr_in s_addr;
uint16_t portNumber;

int sockfd;
int exitIn = 0;
int done = 1;
int control = 1;


int main(int argc, char **argv) {
  int readFlag = 0;

	// If num arguments is not correct --> exit
	if(argc != 2) FREMEN_exit(HRKN_ERR_ARGS,0);

	// Reading config file, if error --> exit
	if (READFILE_readFremenServerFile(&server, argv[1]) == 0) FREMEN_exit(FRMN_ERR_FILE,0);

	// Checking Port number, if invalid --> exit
  if(CONNECTION_portValid(server.port) == 1) FREMEN_exit(FRMN_ERR_PORT,1);
  portNumber = server.port;

  // Checking if the IP is correct, if not --> exit
	if(inet_aton(server.IP,&ipAddress) == 0) FREMEN_exit(FRMN_ERR_IP,1);

	// Filling the structure with IP and Port Specifications
  bzero (&s_addr, sizeof (s_addr));
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons (portNumber);
  s_addr.sin_addr = ipAddress;

  // Ctrl+C -> Free all the dynamic memory
  signal(SIGINT, FREMEN_freeMemory);
  // Handler for SIGPIPE
  signal(SIGPIPE, FREMEN_sigPipe);
  // Set timer alarm to clean the old photos
  signal(SIGALRM, FREMEN_alarm);
  alarm(server.launchTime);

  //Initalizing the user of the Fremen program
  USER_initUser(&userFremen);

	// Printing welcome message
	MYSTRING_print(FRMN_MSG_WEL);

	// Listening to input commands
	while(exitIn != 1){

		// Printing Dollar Message on screen
		MYSTRING_print(FRMN_CMD_DOLLAR);

			// Reading the input of the user
			inBuffer = MYSTRING_read_until(STDIN_FILENO,'\n',&readFlag);
			inBuffer[strlen(inBuffer)-1] = '\0';
			auxBuffer = (char*)malloc(sizeof(char)*strlen(inBuffer)+1);
			strcpy(auxBuffer, inBuffer);

			// Splitting the input & executing the command
			command = strtok(inBuffer," ");

			// Executing the commands
			if(command != NULL && readFlag == 0){
				FREMEN_checkInput(command,auxBuffer, &sockfd,s_addr);

			}else if (exitIn != 1 && readFlag == 0) {
				MYSTRING_print(FRMN_ERR_NULL);
			}

		// Freeing the memory used in this loop in each iteration
		free(inBuffer);
		free(auxBuffer);
	}

	return(0);
}

/***********************************************
*
* @Purpose:     Frees all dynamic memory allocated and closes FD in program exit
* @Parameters:  -
* @Return:      -
*
************************************************/
void FREMEN_freeMemory() {
  if (control && done) {
    // Flags to avoid read blocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    // If the user is connected with the server, disconnect it
    if(userFremen.isConnected == 1){
      COMMAND_serverLogout(sockfd, userFremen.name, userFremen.ID);
    }
    // Free memory
  	free(server.IP);
  	free(server.folder);
    free(userFremen.name);
    free(userFremen.postalCode);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    close(sockfd);
    exitIn = 1;
  } else {
    control = 1;
    // Resetting the SIGINT signal
    signal(SIGINT, FREMEN_freeMemory);
  }
}


/***********************************************
*
* @Purpose:     Checks the user input array and executes a command according its value
* @Parameters:  char* command: Linux Command to be executed without modifiers
*               char* fullLine: Linux Command to be executed with modifiers
*               int * sockfd: socket file descriptor
*               sockaddr_in s_addr: structure with IP and port information
* @Return:      -
*
************************************************/
void FREMEN_checkInput(char *command, char *fullLine,  int *sockfd, struct sockaddr_in s_addr){
	int i = 0;
	// Log In
	if (!strcasecmp(command, FRMN_CMD_LIN)) {
    // Check if a user has already logged in
    if(userFremen.isConnected != 1){
      free(userFremen.name);
      free(userFremen.postalCode);
  		command = strtok(NULL, " ");
  		userFremen = COMMAND_logIn(command,  sockfd,s_addr);
    }else{
      MYSTRING_print(FRMN_ERR_LOGIN);
    }
	// Search
	} else if (!strcasecmp(command, FRMN_CMD_SRCH)) {
    // Checking that the connection with the server has been stablished
    if(userFremen.isConnected == 1){
  		command = strtok(NULL, " ");
  		COMMAND_searchPeople(command, userFremen.ID, userFremen.name, sockfd, &userFremen.isConnected);
    }else{
      MYSTRING_printErr(FRMN_ERR_CONNECT);
    }

	// Send
	} else if (!strcasecmp(command, FRMN_CMD_SND)) {

    // Checking if the connection has been established
    if(userFremen.isConnected == 1) {
      // Global variables to temporarily disable ^C
      done = 0;
      control = 0;
      // Temporarily disable the image erase function after some time
      signal(SIGALRM, FREMEN_idle);
      command = strtok(NULL, " ");
  		COMMAND_sendImage(command, server.folder, sockfd, &userFremen.isConnected);
      // Activate again ^C and check if there has been a ^C while the Send command
      done = 1;
      raise(SIGINT);
      control = 1;
      // Activate again the image erase function
      signal(SIGALRM, FREMEN_alarm);
      alarm(server.launchTime);
    }else{
      MYSTRING_printErr(FRMN_ERR_CONNECT);
    }

	// Photo
	} else if (!strcasecmp(command, FRMN_CMD_PHT)) {

    // Checking if the connection has been established
    if(userFremen.isConnected == 1){
      // Global variables to temporarily disable ^C
      done = 0;
      control = 0;
      // Temporarily disable the image erase function after some time
      signal(SIGALRM, FREMEN_idle);
      command = strtok(NULL, " ");
  		COMMAND_downloadPhoto(command, server.folder, sockfd, &userFremen.isConnected);
      // Activate again ^C and check if there has been a ^C while the Send command
      done = 1;
      raise(SIGINT);
      control = 1;
      // Activate again the image erase function
      signal(SIGALRM, FREMEN_alarm);
      alarm(server.launchTime);
    }else{
      MYSTRING_printErr(FRMN_ERR_CONNECT);
    }

	// Logout
	} else if (!strcasecmp(command, FRMN_CMD_LOUT)) {
		command = strtok(NULL, " ");
		FREMEN_freeMemory();
		COMMAND_logOut(command);

	// Bash command
	} else {
			char **fullComand = MYSTRING_convertToArr(fullLine," ");
			FREMEN_executeCommand(command,fullComand);
			// Freeing the double array
			while(fullComand[i] != NULL){
				free(fullComand[i]);
				i++;
			}
			free(fullComand);
	}
}

/***********************************************
*
* @Purpose:     Function that executes a Linux command
* @Parameters:  char* command: Linux Command to be executed without modifiers
*               char** args: Command modifiers
* @Return:      -
*
************************************************/
int FREMEN_executeCommand(char *command, char **args){
    pid_t pid;
    int status;
    int i = 0;
    // Leave the function if the string is null
    if (command == NULL){
        return(1);
    }
    // Forking
    if ((pid = fork()) < 0){
        // Error Forking
        status = -1;
    }else if (pid == 0){
        // Child process
        // Executing the command in the child process using execlp
        if(execvp(command,args) == -1){
            MYSTRING_print("Invalid cmd command\n");
            while(args[i] != NULL){
      				free(args[i]);
      				i++;
      			}
            free(args);
            free(server.IP);
            free(server.folder);
            free(userFremen.name);
            free(userFremen.postalCode);
            free(inBuffer);
            free(auxBuffer);
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            close(sockfd);
        }
        _exit(0);
    }else if(pid > 0){
        // Parent process waiting for the child process to finish, storing the exit status in &status
        waitpid(pid, &status, 0);
    }
    return(status);
}

/***********************************************
*
* @Purpose:     Function that executes a Linux command
* @Parameters:  -
* @Return:      -
*
************************************************/
void FREMEN_alarm() {
  char buffer[50];
  char *files;
  char **filesList;
  char *aux;
  char *aux2;
  int i = 0;
  aux = strtok(server.folder, "/");
  files = COMMAND_getFiles(aux);
  filesList = MYSTRING_convertToArr(files, "\n");

  while(filesList[i] != NULL){
    sprintf(buffer, "%s/%s", aux, filesList[i]);
    aux2 = strtok(filesList[i], ".");
    aux2 = strtok(NULL, ".");

    if (!strcasecmp(aux2, "jpg")) {
      remove(buffer);
    }
    i++;
  }

  i = 0;
  if (filesList != NULL) {
    while(filesList[i] != NULL){
      free(filesList[i]);
      i++;
    }
    free(filesList);
    free(files);
  }

  signal(SIGALRM, FREMEN_alarm);
  alarm(server.launchTime);
}

/***********************************************
*
* @Purpose:     Function that sets the program idle for a moment
* @Parameters:  -
* @Return:      -
*
************************************************/
void FREMEN_idle(){}

/***********************************************
*
* @Purpose:     Function that exits the program if an error occurrs in initalization
* @Parameters:  char * exitMessage: Message that will be displayed in exit
*               int freeMem: Flag to control if memory needs to be freed
* @Return:      -
*
************************************************/
void FREMEN_exit(char * exitMessage, int freeMem){
  if(freeMem == 1){
    free(server.IP);
  	free(server.folder);
  }
  MYSTRING_printErr(exitMessage);
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  exit (EXIT_FAILURE);
}

/***********************************************
*
* @Purpose:     Function that handles SIGPIPE
* @Parameters:
* @Return:      -
*
************************************************/
void FREMEN_sigPipe() {
  MYSTRING_print(FRMN_ERR_SERVER);
  signal(SIGPIPE, FREMEN_idle);
}
