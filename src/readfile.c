/***********************************************
*
* @Purpose: Implementation of the functions of the readfile module
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
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
* @Purpose:     Function that adds a user on file
* @Parameters:  char *file: file where the user list is located
*               User newUser: user that will be added on the file
*               int numUsers: number of users in the server
* @Return:      -
*
************************************************/
void READFILE_addUser(const char *file, struct User newUser, int numUsers){
	int fd;

	char newUserLine[256];
	// Opening the txt file
	fd = open (file, O_WRONLY);
	// Checking if it was opened correctly
	if(fd < 0) return;

	// Writting the new amount of users to the file
	lseek(fd,0,SEEK_SET);
	bzero(newUserLine, sizeof(newUserLine));
	sprintf(newUserLine,"%d\n",numUsers);
	MYSTRING_printFD(fd,newUserLine);

	// Going to the end of the file and Writting the new user
	lseek(fd,0,SEEK_END);
	bzero(newUserLine, sizeof(newUserLine));
	sprintf(newUserLine,"%s-%s-%d-%s\n",newUser.name, newUser.postalCode, numUsers, newUser.photo);
	write(fd,newUserLine,strlen(newUserLine));

	/*
	// Free all the list of users & re-read it again
	for(int i = 0; i < *numUsers; i++){
		free(users[i].name);
		free(users[i].photo);
	}
	free(users);
	// Filling again the updated list of users
	users = READFILE_readUsersFile(USERS_FILE,numUsers);
	*/
	close (fd);
}

/***********************************************
*
* @Purpose:     Function reads and returns all the users in file
* @Parameters:  char *file: file where the user list is located
*               User newUser: number of users in the system
* @Return:      Returns the list with all the users in the file
*
************************************************/
struct User* READFILE_readUsersFile( const char *file, int*numUsers){
	int fd;
	int i = 0;
	char *aux;
	// Opening the txt file
	fd = open (file, O_RDONLY);

	// Checking if it was opened correctly
	if(fd < 0) return NULL;

	// Reading the amount of users in the system
	aux = MYSTRING_read_until_no_flag(fd,'\n');
	//printf("%s\n",aux);
	*numUsers = atoi(aux);
	free(aux);
	// Allocating memory for the users
	struct User *users = malloc (sizeof (struct User)*(*numUsers));

	// Iterating to fill the structure of users
	for(i = 0; i < *numUsers; i++){
		users[i].name = MYSTRING_read_until_no_flag(fd,'-');
		users[i].postalCode = MYSTRING_read_until_no_flag(fd,'-');
		aux = MYSTRING_read_until_no_flag(fd,'-');
		users[i].ID = atoi(aux);
		free(aux);
		users[i].photo = MYSTRING_read_until_no_flag(fd,'\n');
	}


	//for(int i = 0; i < *numUsers; i++){
	//	printf("%s, %s, %d, %s.\n", users[i].name, users[i].postalCode, users[i].ID, users[i].photo);
	//}

	// Closing and freeing everything
	//free(aux);
	close(fd);
	return users;

}


/***********************************************
*
* @Purpose:     Function that is used to read the config file of the server in Fremen
* @Parameters:  char *file: file with the config file name
*               ServerFremen *server: struct where the information of the config file will be stored
* @Return:      Returns one if successful, zero otherwise
*
************************************************/
int READFILE_readFremenServerFile (struct ServerFremen *server,  char *file) {
	int fd;
	int n;
	int i = 0;
	int j = 0;
	char buffer[25];
	char character;


	// Opening the txt file
	fd = open (file, O_RDONLY);

		// Checking if it was opened correctly
	if(fd < 0) return(0);


	// Reading the file & storing the information
	n = read(fd, &character, 1);
	while(n != 0) {
		if(character != '\n') {
			buffer[i] = character;
			i++;
		}else {
			buffer[i] = '\0';

			if (j == 0) {
				server->launchTime = atoi(buffer);
				j++;
			}else if (j == 1) {
				server->IP = (char*) malloc(sizeof(char)*(strlen(buffer) + 1));
				strcpy(server->IP, buffer);
				j++;
			}else if (j == 2) {
				server->port = atoi(buffer);
				j++;
			}else {
				server->folder = (char*) malloc(sizeof(char)*(strlen(buffer) + 1));
				strcpy(server->folder, buffer);
			}
			i = 0;
			bzero(buffer, sizeof(buffer));
		}
		n = read(fd,&character,1);
	}

	close(fd);
	return(1);
}

/***********************************************
*
* @Purpose:     Function that is used to read the config file of the server in Atreides
* @Parameters:  char *file: file with the config file name
*               ServerAtreides *server: struct where the information of the config file will be stored
* @Return:      Returns one if successful, zero otherwise
*
************************************************/
int READFILE_readAtreidesServerFile (struct ServerAtreides *server,  char *file) {
	int fd;
	int n;
	int i = 0;
	int j = 0;
	char buffer[25];
	char character;


	// Opening the txt file
	fd = open (file, O_RDONLY);

		// Checking if it was opened correctly
	if(fd < 0) return(0);


	// Reading the file & storing the information
	n = read(fd, &character, 1);
	while(n != 0) {
		if(character != '\n') {
			buffer[i] = character;
			i++;
		}else {
			buffer[i] = '\0';

			if (j == 0) {
				server->IP = (char*) malloc(sizeof(char)*(strlen(buffer) + 1));
				strcpy(server->IP, buffer);
				j++;
			}else if (j == 1) {
				server->port = atoi(buffer);
				j++;
			}else {
				server->folder = (char*) malloc(sizeof(char)*(strlen(buffer) + 1));
				strcpy(server->folder, buffer);
			}
			i = 0;
			bzero(buffer, sizeof(buffer));
		}
		n = read(fd,&character,1);
	}


	close(fd);
	return(1);
}

/***********************************************
*
* @Purpose:     Function that counts and returns the number of bytes in file
* @Parameters:  char *file: file to be counted
*
* @Return:      Returns one if successful, zero otherwise
*
************************************************/
int READFILE_countBytes(char* fileName) {
    int fd = open(fileName, O_RDONLY);
    char current;
    if (fd < 0) {
        return 0;
    }

    int i = 0;
    while (read(fd, &current, 1) > 0) {
        i++;
    }
    close(fd);


    return i;
}

/***********************************************
*
* @Purpose:     Function that is used to send an image
* @Parameters:  char *filename: image to be sent
*               int fremen: flag to check who is the source of the send
*               int sockfd: socket file descriptor where the image is sent
* @Return:      -
*
************************************************/
void READFILE_sendImage(char* fileName, int sockfd, int fremen) {
		int image;
		int n;
		struct Frame frame_sent;

		bzero (frame_sent.source, sizeof (frame_sent.source));
		bzero (frame_sent.data, sizeof (frame_sent.data));
		frame_sent.type = 'D';

		if (fremen) {
			strcpy(frame_sent.source, F_RQ_SRC);
		} else {
			strcpy(frame_sent.source, F_OK_SRC);
		}

		image = open(fileName, O_RDONLY);

		if (image > 0) {
			n = read(image, frame_sent.data, 240);

			while (n == 240) {
				CONNECTION_writeImageFrame(sockfd, frame_sent.source, frame_sent.type, frame_sent.data);
				bzero (frame_sent.data, sizeof (frame_sent.data));
				n = read(image, frame_sent.data, 240);
			}

			close(image);

			if (n > 0) {
				CONNECTION_writeImageFrame(sockfd, frame_sent.source, frame_sent.type, frame_sent.data);
			}
		}
}
