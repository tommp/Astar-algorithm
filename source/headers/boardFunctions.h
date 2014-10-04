#ifndef ASTARALG_H
#define ASTARALG_H


/*Included headers*/
/*---------------------------------------------*/
#include "./errorlogger.h"
/*---------------------------------------------*/

/*Included dependencies*/
/*---------------------------------------------*/
#include <limits>
#include <vector>
#include <map>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <string>
#include <fstream>
#include <queue>
/*---------------------------------------------*/

/*Header content*/
/*=============================================*/


/*PREPROCESSOR MACROS, MUST BE USED WITH CARE! ======================*/

/*What algorithm to use, 1 for astar 2 for dijkstra and 3 for bfs*/
#define ALG 1

/*Show open and closed nodes or not*/
#define NODEMARKS true

/*===================================================================*/

namespace boardConsts{

/*Symbols for node status*/
const int unexplored = 0;
const int open = 1;
const int closed = 2;
const int currentpath = 3;

/*Linethickness of the closed mark crosses*/
const int linethickness = 3;

/*Sybols for the diffrent types of terrain*/
const char start = 'A';
const char goal = 'B';
const char empty = '.';
const char blocked = '#';
const char water = 'w';
const char mountain = 'm';
const char forest = 'f';
const char grass = 'g';
const char road = 'r';

/*Costs for the diffrent tiles*/
const unsigned int watercost = 100;
const unsigned int mountaincost = 50;
const unsigned int forestcost = 10;
const unsigned int grasscost = 5;
const unsigned int roadcost = 1;
const unsigned int emptycost = 1;
const unsigned int startendcost = 0;
const unsigned int blockedcost = std::numeric_limits<unsigned int>::max();

/*Map with tilesymbols as keys and costs as values*/
const std::map<const char, const unsigned int> costmap = {
{start, startendcost}, 
{goal, startendcost}, 
{empty, emptycost}, 
{blocked, blockedcost}, 
{water, watercost}, 
{forest, forestcost}, 
{mountain, mountaincost},
{grass, grasscost}, 
{road, roadcost}, 
};

/*Colours for the board graphical representation*/
const int color1[] = {255, 0, 0};/*startcolour*/
const int color2[] = {0, 255, 0};/*goalcolour*/
const int color3[] = {0, 0, 255};/*wallcolour*/
const int color4[] = {0, 0, 155};/*watercolour*/
const int color5[] = {100, 100, 100};/*mountaincolour*/
const int color6[] = {70, 110, 70};/*forestcolour*/
const int color7[] = {0, 155, 0};/*grasscolour*/
const int color8[] = {120, 66, 0};/*roadcolour*/
const int color9[] = {0, 0, 0};/*background color*/
const int color10[] = {255, 255, 255};/*unknown symbol color*/
const int color11[] = {255, 0, 0};/*pathcolor*/
const int color12[] = {0, 255, 255};/*closedcolor*/
const int color13[] = {255, 255, 255};/*opencolor*/

/*Map with tilesymbols as keys and colors as values*/
const std::map<const char, const int(*)[3]> colormap = {
{start, &color1}, 
{goal, &color2}, 
{empty, &color9}, 
{blocked, &color3}, 
{water, &color4},
{mountain, &color5}, 
{forest, &color6}, 
{grass, &color7}, 
{road, &color8}, 
};

};/*BOARDCONSTS NAMESPACE*/

/*Class representing a search node*/
class Searchnode;
/*Class used to describe a nodestate, its position will be used in this case*/
class State;
/*Class representing a board/terrain*/
class Board;

class State{
	friend class Searchnode;
	private:
		unsigned int xpos;
		unsigned int ypos;
	public:
		unsigned int getXpos()const {return xpos;}
		unsigned int getYpos()const {return ypos;}
		bool operator==(const State &state);
		void setState(unsigned int x, unsigned int y){ xpos = x; ypos = y;}
};


class Searchnode{
	friend class Board;
	private:
		State state;
		unsigned int gcost;
		unsigned int hcost;
		int status;
		Searchnode* parent;
		std::vector<Searchnode*> successors;
	public:

		/*Get members*/
		unsigned int getGCost()const {return gcost;}

		#if ALG == 2
		unsigned int getFCost()const {return gcost;}
		#else
		unsigned int getFCost()const {return gcost+hcost;}
		#endif

		int getStatus(){return status;}
		Searchnode* getParent(){return parent;}
		std::vector<Searchnode*> getSuccessors(){return successors;}
		State getState(){return state;}

		/*Set members*/
		void setGCost(unsigned int newg){gcost = newg;}
		void setHCost(unsigned int newh){hcost = newh;}
		void addSuccessor(Searchnode* node){successors.push_back(node);}
		void setParent(Searchnode* node){parent = node;}
		void setStatus(const int & newstat){status = newstat;}
		void setState(State newstate){state = newstate;}
};

/*Overridden comparator for the priority queue*/
struct Comparator : public std::binary_function<Searchnode*, Searchnode*, bool>                                                                                       
{  
  bool operator()(const Searchnode* a, const Searchnode* b){
		return (a->getFCost() > b->getFCost());
	}
};



class Board {
public:
	char** board;
	int** pathmask;
	Searchnode** mesh;

	#if ALG == 3
	std::queue<Searchnode*> agenda;
	#else 
	std::priority_queue<Searchnode*, std::vector<Searchnode*>, Comparator> agenda;
	#endif
	unsigned int rows;
	unsigned int columns;
	unsigned int startpos[2];
	Searchnode* currentpos;
	unsigned int goalpos[2];

	~Board(){
		for(unsigned int currRowIndex = 0; currRowIndex<this->rows; currRowIndex++){
			delete[] board[currRowIndex];
			delete[] pathmask[currRowIndex];
			delete[] mesh[currRowIndex];
		}/*FOR*/
	delete[] board;
	delete[] pathmask;
	delete[] mesh;
	}

	/*Calculate and set hcost for node at the given position in the graph*/
	void calculateH(int xpos, int ypos);

	/*Create a meshgraph of nodes*/
	void createmesh();

	/*Merges all successors of node into the priority queue*/
	void expand(Searchnode* node);

	/*Propogate path improvements to a node*/
	void propagate_path_improvements(Searchnode* node);

	/*Run the algorithm for the given number of iterations*/
	int algorithm(unsigned int iterations);
};



/*Takes a .txt file with symbols representing terrain as input and returns a board class pointer*/
Board* createBoard(const char* filename);

/*Copy a board to the renderer (draws it in colourcoding and prepares for outputting to window)*/
void renderBoard(Board* board, const unsigned int windowScale, const unsigned int margin, SDL_Renderer* renderer);

/*=============================================*/

#endif
