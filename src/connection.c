/***********************************************
*
* @Purpose: Implementation of the functions of the connection module
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "connection.h"
#include "mystring.h"
#include "readfile.h"



/***********************************************
*
* @Purpose:     Function to check if the port number is  inside the correct range.d
* @Parameters:  int portNumber: port number to be checked
*
* @Return:      retunrs one if it is valid, zero otherwise
*
************************************************/
int CONNECTION_portValid(int portNumber){
    if(portNumber < 1 || portNumber > 65535){
        return 1;
    }
    return 0;
}


/***********************************************
*
* @Purpose:     FUnction to write a frame on the specified file descriptor
* @Parameters:  int fd: file descriptor to write
*               char source[15]: string determining the source of the frame
*               char type: character determining the type of the frame
*               char data[240]: string determining the data of the frame
*
*
* @Return:      -
*
************************************************/
void CONNECTION_writeFrame(int fd, char source[15], char type, char data[240]){
  char buffer[256];
  bzero(buffer, sizeof(buffer));
  sprintf(buffer,"%s%c%s",source,type,data);
  write(fd,buffer,sizeof (char)*256);
}

/***********************************************
*
* @Purpose:     FUnction to write a frame on the specified file descriptor
* @Parameters:  int fd: file descriptor to write
*               char source[15]: string determining the source of the frame
*               char type: character determining the type of the frame
*               char data[240]: string determining the data of the frame
*
*
* @Return:      -
*
************************************************/
void CONNECTION_writeImageFrame(int fd, char source[15], char type, char data[240]) {
  char buffer[256];
  bzero(buffer, sizeof(buffer));

  for (int i = 0; i < 256; i++) {
    if (i < 15) {
      buffer[i] = source[i];
    } else if (i == 15) {
      buffer[i] = type;
    } else {
      buffer[i] = data[i-16];
    }
  }

  write(fd,buffer,sizeof (char)*256);
}

/***********************************************
*
* @Purpose:     FUnction to read a frame on the specified file descriptor
* @Parameters:  int fd: file descriptor to write
*
*
*
* @Return:      returns the frame read
*
************************************************/
struct Frame CONNECTION_readFrame(int fd){
  struct Frame frame;
  bzero(frame.data, sizeof(frame.data));
  bzero(frame.source, sizeof(frame.source));
  char buffer[256];
  int i = 0;
  int len = 0;
  len = read(fd,buffer,sizeof(buffer));
  // Checking if the read was successful
  if(len != 0){
    int j = 0;
    // Parsing the source of the frame read
    while(1){
      frame.source[j] = buffer[j];
      if(!(frame.source[j] == F_RQ_SRC[j] && j < 6) && !(frame.source[j] == F_OK_SRC[j] && j < 8  )){
        frame.type = frame.source[j];
        frame.source[j] = '\0';
        break;
      }
      j++;
    }
    j++;

    // Parsing the data of the frame read
    while(buffer[j] != '\0'||j>=255){
       frame.data[i] = buffer[j];
      j++;
      i++;
    }
    frame.data[j] = '\0';
  }else{
    // Type 0 -> server down
    frame.type = '0';
  }
  return frame;
}

/***********************************************
*
* @Purpose:     FUnction to read a frame on the specified file descriptor
* @Parameters:  int fd: file descriptor to write
*
*
*
* @Return:      returns the frame read
*
************************************************/
struct Frame CONNECTION_readImageFrame(int fd) {
  struct Frame frame;
  bzero(frame.data, sizeof(frame.data));
  bzero(frame.source, sizeof(frame.source));
  char buffer[256];
  int i = 0;

  // Checking if the read was successful
  if (read(fd,buffer,sizeof(buffer)) != 0) {
    for (i = 0; i < 256; i++) {
      if (i < 15) {
        frame.source[i] = buffer[i];
      } else if (i == 15) {
        frame.type = buffer[i];
      } else {
        frame.data[i-16] = buffer[i];
      }
    }
  }else {
    frame.type = '0';
  }
  return frame;
}

/***********************************************
*
* @Purpose:     Function that returns a negative value if thre received frame is incorrect,
*               otherwise a positive value that will depend on the type of frame
* @Parameters:  Frame frame: frame whose type is  checked
*
*
*
* @Return:      returns a number that depends on the frame type
*
************************************************/
int CONNECTION_checkFrameType(struct Frame frame){
  // Checking that the source of the frame is FREMEN
  if(strcmp(frame.source,F_RQ_SRC) == 0){
    switch(frame.type){
      case 'C':
          return 1;
        break;
      case 'Q':
          return 2;
        break;
      case 'S':
          return 3;
        break;
      case 'F':
          return 4;
        break;
      case 'D':
          return 5;
        break;
      case 'P':
          return 6;
        break;
    }
  }

  return -1;
}
