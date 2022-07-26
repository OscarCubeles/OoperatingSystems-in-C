/***********************************************
*
* @Purpose: Implementation of the Harkonen program
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

int attackSeconds = 0;
char *arrakisPIDs;
char *userPIDs;
char **arrakisPIDList;
char **userPIDList;
int *validPIDList;

void HARKONEN_alarm();
void HARKONEN_exit();
void HARKONEN_freeMemory();
int HARKONEN_fillValidPIDList();

int main(int argc, char **argv) {

  // Checking the number of arguments
  if(argc != 2) {
    MYSTRING_printErr(HRKN_ERR_ARGS);
    exit (EXIT_FAILURE);
  }

  // Checking that the parameter passed is valid
  if(MYSTRING_detectCharacter(argv[1]) == 0){
    MYSTRING_printErr(HRKN_ERR_ARG);
    exit (EXIT_FAILURE);
  }

  // Storing the period in secods per attack
  attackSeconds = atoi(argv[1]);
  // Printing initial Harkonen message
  MYSTRING_print(HRKN_MSG_START);

  // Setting the alarm signal for the attacks
  signal(SIGALRM,HARKONEN_alarm);
  signal(SIGINT,HARKONEN_exit);
  alarm(attackSeconds);

  // Harkonen is idle until a signal arrives
  while(1){
    pause();
  }

  return 0;
}


/***********************************************
*
* @Purpose:     Alarm routine where harkonen finds and kills one  Arrakis System processes
* @Parameters:  -
*
* @Return:      -
*
************************************************/
void HARKONEN_alarm(){
  char buffer[200];
  int numValidPIDs = 0;
  int randomIndex = 0;

  // Printing the scanning pids message
  MYSTRING_print(HRKN_MSG_SCAN);
  // Scanning the PIDs of the arrakis system for all users (command: pidof Fremen Atreides)
  arrakisPIDs = COMMAND_scanPIDS();
  arrakisPIDList = MYSTRING_convertToArr(arrakisPIDs," ");
  // Scanning all user PIDs (command: ps -ho pid)
  userPIDs = COMMAND_scanUserPIDS();
  userPIDList = MYSTRING_convertToArr(userPIDs,"\n");

  // Filling the list of valid pids and returning the amount of valid ones
  numValidPIDs = HARKONEN_fillValidPIDList();

  // Checking if any valid PID was found
  if(numValidPIDs > 0){
    // Getting a random index to kill a random PID among all valid
    randomIndex = rand() % (numValidPIDs);
    // Killing the process
    kill(validPIDList[randomIndex],SIGINT);
    // Writting killed message
    bzero(buffer, sizeof(buffer));
    sprintf(buffer,HRKN_MSG_KILL,validPIDList[randomIndex]);
    write(STDOUT_FILENO,buffer,strlen(buffer));
  }else{
    // Printing on screen that no PIDs to kill were found
    MYSTRING_print(HRKN_MSG_NOKILL);
  }

  // Freeing memory
  HARKONEN_freeMemory();

  // Resetting the SIGALRM signal
  signal(SIGALRM,HARKONEN_alarm);
  alarm(attackSeconds);
}

/***********************************************
*
* @Purpose:     Function that that finds all the valid PIDs to be killed by harkonen
* @Parameters:  -
*
* @Return:      returns the number of valid PIDs
*
************************************************/
int HARKONEN_fillValidPIDList(){
  int i = 0;
  int j = 0;
  int numValidPIDs = 0;
  // Allocating memory for the list of PIDs
  validPIDList = (int*) malloc(sizeof(int));
  i = 0;
  // Searching the matching PIDs from all arrakis and the current user
  while(arrakisPIDList[i] != NULL){
    j = 0;
    while(userPIDList[j] != NULL){
      if(atoi(userPIDList[j]) == atoi(arrakisPIDList[i])){
        validPIDList = (int*)realloc(validPIDList, sizeof(int) * (numValidPIDs + 2));
        validPIDList[numValidPIDs] = atoi(arrakisPIDList[i]);
        numValidPIDs++;
      }
      j++;
    }
    i++;
  }
  return numValidPIDs;
}


/***********************************************
*
* @Purpose:     Function that frees the memory allocated in Harkonen
* @Parameters:  -
*
* @Return:      -
*
************************************************/
void HARKONEN_freeMemory(){
  int i = 0;
  free(userPIDs);
  free(arrakisPIDs);
  free(validPIDList);
  i = 0;
  if(arrakisPIDList!=NULL){
    while(arrakisPIDList[i] != NULL){
      free(arrakisPIDList[i]);
      i++;
    }
    free(arrakisPIDList);
  }
  i = 0;
  if(userPIDList!=NULL){
    while(userPIDList[i] != NULL){
      free(userPIDList[i]);
      i++;
    }
    free(userPIDList);
  }
}

/***********************************************
*
* @Purpose:     Function that exits Harkonen printing a message in screen
* @Parameters:  -
*
* @Return:      -
*
************************************************/
void HARKONEN_exit(){
  MYSTRING_print(HRKN_MSG_EXIT);
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  exit (EXIT_SUCCESS);
}
