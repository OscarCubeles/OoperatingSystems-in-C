/***********************************************
*
* @Purpose: Module that implements functions to manage users
* @Autor/s: Óscar Cubeles Ollé & Guillermo González López
* @Date creacio: October 2021
* @Last Modifcation Date: January 2022
*
************************************************/
#ifndef USER_H
    #define USER_H
    /***********************************************
    *
    * @Purpose:     Initializes the user of Fremen.
    * @Parameters:  UserFremen *userFremen:  user to be initialized
    * @Return: -
    *
    ************************************************/
    void USER_initUser(struct UserFremen *userFremen);

    /***********************************************
    *
    * @Purpose:     Creates a user given its name and postalcode
    * @Parameters:  -
    * @Return:      Returns the user with its data
    *
    ************************************************/
    struct User USER_createUser(char *userName, char *postalCode, int numUsers);

#endif
