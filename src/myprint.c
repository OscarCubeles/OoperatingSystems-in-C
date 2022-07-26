/***********************************************
*
* @Purpose: Implementation of the functions for myprint module
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
#include "myprint.h"
#include "mystring.h"
#include "connection.h"

/***********************************************
*
* @Purpose:     Function that prints all the information when a logout in atreides is received
* @Parameters:  char message[240]: Data received with the information of who logged out
*
* @Return:      -
*
************************************************/
void MYPRINT_printAtreidesLogout(char message[240]){
  char *aux = strtok(message,"*");
  MYSTRING_print(aux);
  MYSTRING_print(", with ID: ");
  aux = strtok(NULL,"\0");
  MYSTRING_print(aux);
  MYSTRING_print(" has disconnected from Atreides server.\n");

}

/***********************************************
*
* @Purpose:     Function that prints all the information when a search is received
* @Parameters:  -
*
* @Return:      -
*
************************************************/
void MYPRINT_printSearchUsers(char *aux, char *postalCode, int numUsers, int sockfd){
  int i = 0;
  char buffer[600];
  struct Frame frame_received;

  if(numUsers > 0){
    sprintf(buffer,"\nThere are %d users in %s:\n",numUsers,postalCode);
    MYSTRING_print(buffer);
    // Printing on screen the users and their IDs
    for(i = 0; i < numUsers; i++){
      aux = strtok(NULL,"*");
      // If its null, it means that we must read another frame
      if(aux == NULL){
        frame_received = CONNECTION_readImageFrame(sockfd);
         aux = strtok(frame_received.data,"*");
      }
      MYSTRING_print(aux);
      MYSTRING_print(" ");
      aux = strtok(NULL,"*");
      MYSTRING_print(aux);
      MYSTRING_print("\n");
    }
    MYSTRING_print("\n");
  }else{
    sprintf(buffer,"\nThere are 0 users in %s.\n",postalCode);
    MYSTRING_print(buffer);
  }
}

/***********************************************
*
* @Purpose:     Function that prints the information received in a search request from Fremen
* @Parameters:  char message[240]: Data received with the information of the request
*
* @Return:      -
*
************************************************/
char * MYPRINT_printSearchRequest(char data[240]){
  char *aux;
  MYSTRING_print("Received search from ");
  // Parsing the postal code
  aux = strtok(data,"*");
  MYSTRING_print(aux);
  aux = strtok(NULL,"*");
  MYSTRING_print(" ");
  MYSTRING_print(aux);
  aux = strtok(NULL,"*");
  MYSTRING_print(" for postal code: ");
  MYSTRING_print(aux);
  return aux;
}

/***********************************************
*
* @Purpose:     Function that prints the information received in a login request from Fremen
* @Parameters:  char *username: user who requested the login
*               char *postalCode: postal code of the user who requested the login   
*
* @Return:      -
*
************************************************/
void MYSTRING_printLoginRequest(char * userName, char * postalCode){
  char buffer[200];
  bzero(buffer, sizeof(buffer));
  sprintf(buffer,ATR_MSG_USR_LOGIN,userName,postalCode);
  MYSTRING_print(buffer);
}
