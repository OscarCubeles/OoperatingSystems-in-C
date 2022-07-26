/***********************************************
*
* @Purpose: Implementation of the functions of the user module
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Date creacio: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "readfile.h"
#include "mystring.h"
#include "connection.h"
#include "user.h"

/***********************************************
*
* @Purpose:     Initializes the user of Fremen.
* @Parameters:  UserFremen user to be initialized
* @Return: -
*
************************************************/
void USER_initUser(struct UserFremen *userFremen){
  userFremen->name = (char *)malloc(sizeof(char));
  userFremen->postalCode = (char *)malloc(sizeof(char));
  userFremen->isConnected = 0;
}

/***********************************************
*
* @Purpose:     Creates a user given its name and postalcode
* @Parameters:  -
* @Return:      Returns the user with its data
*
************************************************/
struct User USER_createUser(char *userName, char *postalCode, int numUsers){
  struct User newUser;
  newUser.name = userName;
  newUser.postalCode = postalCode;
  // Allocating memory for tne new user
  newUser.name = (char*)malloc(sizeof(char)*(int)(strlen(userName))+1);
  strcpy(newUser.name,userName);
  newUser.postalCode = (char*)malloc(sizeof(char)*(int)(strlen(postalCode))+1);
  strcpy(newUser.postalCode,postalCode);
  newUser.ID = numUsers+1;
  newUser.photo = (char*) malloc(sizeof(char)*4);

  // Default image as IMG string
  strcpy(newUser.photo,"IMG");
  return newUser;
}
