/***********************************************
*
* @Purpose: Module that implements functions to manage connection with sockets
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Creation date: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#ifndef CONNECT_H
    #define CONNECT_H

    #define F_RQ_SRC    "FREMEN"
    #define F_OK_SRC    "ATREIDES"
    #define F_RQ_DATA   "Oscar*08029"
    #define F_OK_DATA   "123"
    #define F_NOK_DATA  "ERROR"
    #define F_OK_IMG  "IMAGE OK"
    #define F_NOK_IMG  "IMAGE KO"
    #define F_NOK_FILE  "FILE NOT FOUND"

    struct Frame{
        char source[15];
        char type;
        char data[240];
    };


    /***********************************************
    *
    * @Purpose:     Function to check if the port number is  inside the correct range.d
    * @Parameters:  int portNumber: port number to be checked
    *
    * @Return:      retunrs one if it is valid, zero otherwise
    *
    ************************************************/
    int CONNECTION_portValid(int portNumber);

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
    void CONNECTION_writeFrame(int fd, char source[15], char type, char data[240]);

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
    void CONNECTION_writeImageFrame(int fd, char source[15], char type, char data[240]);
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
    struct Frame CONNECTION_readFrame(int fd);

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
    struct Frame CONNECTION_readImageFrame(int fd);

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
    int CONNECTION_checkFrameType(struct Frame frame);


#endif
