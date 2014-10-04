#ifndef ASTAR_H
#define ASTAR_H


/*Included headers*/
/*---------------------------------------------*/
#include "./boardFunctions.h"
#include "./errorlogger.h"
/*---------------------------------------------*/

/*Included dependencies*/
/*---------------------------------------------*/
#include <iostream>
#include <SDL2/SDL.h>
#include <string>
#include <unistd.h>
/*---------------------------------------------*/

/*Header content*/
/*=============================================*/
/*Width and height of tiles, used to scale window*/
const unsigned int windowScale = 30;

/*Number of algorithm iterations between each print to screen, set to 0 to print the result instantly*/
const unsigned int iterations = 1;

/*Delay between each print to screen, should never be zero for performance issues, set iterations to zero instead*/
const unsigned int iterationdelay = 70;

/*Path to default file to load in case of no argument*/
const char* defaultfilepath = "../boards/board-2-1.txt";

/*Margin between tiles*/
const unsigned int margin = 3;

/*Waits for user input and quits when detected*/
void waitForEvent();
/*=============================================*/

#endif