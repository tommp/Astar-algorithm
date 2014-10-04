#include "./headers/boardFunctions.h"


/*Operator overloads ============================================*/
/*Used in the heap to define ordering*/
bool operator<(const State &a, const State &b){
	return (a.getXpos()+a.getYpos())<(b.getXpos()+b.getYpos());
}

/*Used for convinience*/
bool State::operator==(const State &state){
	return  (this->xpos == state.xpos) && (this->ypos == state.ypos);
}

/*===============================================================*/

/*Functions =====================================================*/

/**/
void renderBoard(Board* board, const unsigned int windowScale, const unsigned int margin, SDL_Renderer* renderer){
	int tileDimension = windowScale-(margin*2);
	SDL_Rect rect = {0, 0, tileDimension, tileDimension};
	SDL_Rect pathrect = {0, 0, tileDimension/4, tileDimension/4};
	SDL_SetRenderDrawColor(renderer, 
						boardConsts::color9[0], 
						boardConsts::color9[1], 
						boardConsts::color9[2], 255);
	SDL_RenderClear(renderer);

	/*copy the meshgraph to the pathmask*/
	for(unsigned int row = 0; row<board->rows; row++){
		for(unsigned int col = 0; col<board->columns; col++){
			board->pathmask[row][col] = board->mesh[row][col].getStatus();
		}
	}

	/*Set the states of the current best path*/
	bool pathdrawn = false;
	Searchnode* currentpathnode = board->currentpos;
	while(!pathdrawn){
		board->pathmask[(currentpathnode)->getState().getXpos()][(currentpathnode)->getState().getYpos()] = boardConsts::currentpath;
		if((currentpathnode)->getState().getXpos() == board->startpos[0] &&(currentpathnode)->getState().getYpos() == board->startpos[1]){
			pathdrawn = true;
		}
		else{
			currentpathnode = currentpathnode->getParent();
		}
	}

	/*Draw the tiles*/
	for(unsigned int row = 0; row<board->rows; row++){
		for(unsigned int column = 0; column<board->columns; column++){

			rect.x = (int)(margin+(column*windowScale));
			rect.y = (int)(margin+(row*windowScale));

			SDL_SetRenderDrawColor(renderer, 
								boardConsts::color10[0], 
								boardConsts::color10[1],
								boardConsts::color10[2], 255);
			SDL_RenderFillRect(renderer, &rect);
			
			if(boardConsts::colormap.find(board->board[row][column]) == boardConsts::colormap.end()){
				SDL_SetRenderDrawColor(renderer, 
								boardConsts::color10[0], 
								boardConsts::color10[1],
								boardConsts::color10[2], 255);
				SDL_RenderFillRect(renderer, &rect);
			}
			else{ 
				SDL_SetRenderDrawColor(renderer, 
								(*boardConsts::colormap.find(board->board[row][column])->second)[0], 
								(*boardConsts::colormap.find(board->board[row][column])->second)[1],
								(*boardConsts::colormap.find(board->board[row][column])->second)[2], 255);
				SDL_RenderFillRect(renderer, &rect);
			}

			/*Draw the current best path*/
			if(board->pathmask[row][column] == boardConsts::currentpath){

				pathrect.x = rect.x+(3*tileDimension/8);
				pathrect.y = rect.y+(3*tileDimension/8);
				SDL_SetRenderDrawColor(renderer, 
								boardConsts::color11[0], 
								boardConsts::color11[1],
								boardConsts::color11[2], 255);
				SDL_RenderFillRect(renderer, &pathrect);
			}

			/*Draw open and closed nodes if enabled*/
			#if NODEMARKS
			else if(board->pathmask[row][column] == boardConsts::closed){
				pathrect.x = rect.x+(3*tileDimension/8);
				pathrect.y = rect.y+(3*tileDimension/8);
					thickLineRGBA (renderer, 
						pathrect.x, 
						pathrect.y, 
						pathrect.x+tileDimension/4, 
						pathrect.y+tileDimension/4, 
						boardConsts::linethickness, 
						boardConsts::color12[0], 
						boardConsts::color12[1], 
						boardConsts::color12[2], 255);
					thickLineRGBA (renderer,
						pathrect.x,
						pathrect.y+tileDimension/4,
						pathrect.x+tileDimension/4,
						pathrect.y,
						boardConsts::linethickness, 
						boardConsts::color12[0], 
						boardConsts::color12[1], 
						boardConsts::color12[2], 255);
			}
			else if(board->pathmask[row][column] == boardConsts::open){
				filledCircleRGBA (renderer, 
						rect.x+tileDimension/2, 
						rect.y+tileDimension/2,
						tileDimension/4,
						boardConsts::color13[0], 
						boardConsts::color13[1], 
						boardConsts::color13[2], 255);
			}
			#endif
		}/*FOR*/
	}/*FOR*/
}/*RENDERBOARD*/

Board* createBoard(const char* filename){
	std::ifstream boardFile;
	boardFile.open(filename);
	
	/*Check if boardfile was opened and create a board if true*/
	if(boardFile.is_open()){
		Board *returnme = new Board;
		std::vector<std::string> lines;
		std::string line;
		unsigned int lineLenght = 0;
		unsigned int numLines = 0;

		while(std::getline(boardFile, line)){
			lines.push_back(line);
		}/* WHILE */

		numLines = lines.size();
		lineLenght = lines[0].length();

		/*Initialize board and pathmask*/
		int** pathmask = new int*[numLines];
		char** board = new char*[numLines];
		for(unsigned int i = 0; i < numLines; i++){
			board[i] = new char[lineLenght];
			pathmask[i] = new int[lineLenght];
		}/* FOR */

		/*Set board values*/
		for(unsigned int currLineIndex = 0; currLineIndex<numLines; currLineIndex++){
			line.assign(lines[currLineIndex]);
			for(unsigned int currLetrIndex = 0; currLetrIndex<lineLenght; currLetrIndex++){
					board[currLineIndex][currLetrIndex] = line[currLetrIndex];
					pathmask[currLineIndex][currLetrIndex] = boardConsts::unexplored;
					if(line[currLetrIndex] == boardConsts::start){
						returnme->startpos[0] = currLineIndex;
						returnme->startpos[1] = currLetrIndex;
					}
					if(line[currLetrIndex] == boardConsts::goal){
						returnme->goalpos[0] = currLineIndex;
						returnme->goalpos[1] = currLetrIndex;
					}
			}/*FOR*/
		}/* FOR */
		returnme->board = board;
		returnme->pathmask = pathmask;
		returnme->columns = lineLenght;
		returnme->rows = numLines;
		return returnme;
	}/*IF*/

	/*Log the error if it failed to open*/
	else{
		errorlogger("Failed to open boardfile");
		return nullptr;
	}/*ELSE*/
}/*CREATEBOARD*/

void Board::calculateH(int xpos, int ypos){
	/*Calculates manhattan distance from node to goal*/
	unsigned int xdist = abs(goalpos[0]-xpos);
	unsigned int ydist = abs(goalpos[1]-ypos);
	mesh[xpos][ypos].setHCost(xdist+ydist);
}/*BOARD::CALCULATEH*/

void Board::createmesh(){
	mesh = new Searchnode*[rows];
	for(unsigned int i = 0; i < rows; i++){
		mesh[i] = new Searchnode[columns];
	}/* FOR */

	/*Initialze all nodes*/
	for(unsigned int row = 0; row < rows; row++){
		for(unsigned int col = 0; col < columns; col++){
			mesh[row][col].setParent(nullptr);
			mesh[row][col].setStatus(boardConsts::unexplored);
			calculateH(row, col);
			State* newstate = new(State);
			newstate->setState(row, col);
			mesh[row][col].setState(*newstate);
			delete newstate;

		}/* FOR */
	}/* FOR */

	/*Add successors to all nodes*/
	for(unsigned int row = 0; row < rows; row++){
		for(unsigned int col = 0; col < columns; col++){
			if((row != 0) && (board[row-1][col] != boardConsts::blocked)){
				mesh[row][col].addSuccessor(&(mesh[row-1][col]));
			}/*IF*/
			if((row != rows-1) && (board[row+1][col] != boardConsts::blocked)){
				mesh[row][col].addSuccessor(&(mesh[row+1][col]));
			}/*IF*/
			if((col != 0) && (board[row][col-1] != boardConsts::blocked)){
				mesh[row][col].addSuccessor(&(mesh[row][col-1]));
			}/*IF*/
			if((col != columns-1) && (board[row][col+1] != boardConsts::blocked)){
				mesh[row][col].addSuccessor(&(mesh[row][col+1]));
			}/*IF*/
		}/* FOR */
	}/* FOR */
	currentpos = &(mesh[startpos[0]][startpos[1]]);
	currentpos->setGCost(0);
	agenda.push(currentpos);
}/*BOARD::CREATEMESH*/

void Board::propagate_path_improvements(Searchnode* node){
	unsigned int nodegP;
	for(std::vector<Searchnode*>::iterator it = node->successors.begin(); it != node->successors.end(); ++it){

		/*Map the cost of the node*/
		nodegP = boardConsts::costmap.find(board[(*it)->getState().getXpos()][(*it)->getState().getYpos()])->second;

		/*Set improvement changes if the best path improved*/
		if(node->getGCost() + nodegP < (*it)->getGCost()){
			(*it)->setParent(node);
			(*it)->setGCost(node->getGCost() + nodegP);
			this->propagate_path_improvements(*it);
		}/*IF*/
	}/*FOR*/
}/*PROPOGATE_PATH_IMPROVEMENTS*/

void Board::expand(Searchnode*node){
	unsigned int nodeg;
	for(std::vector<Searchnode*>::iterator it = node->successors.begin(); it != node->successors.end(); ++it){

		/*Map the cost of the node*/
		nodeg = boardConsts::costmap.find(board[(*it)->getState().getXpos()][(*it)->getState().getYpos()])->second;

		/*If node is unexplored, set to open and add to agenda*/
		if((*it)->getStatus() != boardConsts::open && (*it)->getStatus() != boardConsts::closed){
			(*it)->setStatus(boardConsts::open);
			(*it)->setParent(node);
			(*it)->setGCost(node->getGCost()+nodeg);

			agenda.push(*it);
		}/*IF*/
		else{
			/*If open or closed (explored) check if better parent has been found and set + propogate changes if true*/
			if(node->getGCost()+nodeg < (*it)->getGCost()){
				(*it)->setParent(node);
				propagate_path_improvements(*it);
			}/*IF*/
		}/*ELSE*/
	}/*FOR*/
	node->status = boardConsts::closed;
}/*BOARD::EXPAND*/


int Board::algorithm(unsigned int iterations){
	/*Runs until agenda is empty if iterations<=0*/
	if(iterations<=0){
		while(1){
			if(agenda.empty()){
				return -1;
			}

			#if ALG == 3
			currentpos = agenda.front();
			#else
			currentpos = agenda.top();
			#endif
			agenda.pop();

			currentpos->setStatus(boardConsts::closed);
			if(currentpos->getState().getXpos() == goalpos[0] && currentpos->getState().getYpos() == goalpos[1]){
				return 1;
			}
			else{
				expand(currentpos);
			}
		}/*WHILE*/
	}/*IF*/

	/*Runs for a given number of iterations if iterations>0*/
	else{
		for(unsigned int i = 0; i < iterations; i++){
			if(agenda.empty()){
				return -1;
			}

			#if ALG == 3
			currentpos = agenda.front();
			#else
			currentpos = agenda.top();
			#endif
			agenda.pop();

			currentpos->setStatus(boardConsts::closed);
			if((currentpos->getState().getXpos() == goalpos[0]) && (currentpos->getState().getYpos() == goalpos[1])){
				return 1;
			}
			else{
				expand(currentpos);
			}
		}/*FOR*/
		return 0;
	}/*ELSE*/
}/*BOARD::ASTAR*/

/*End ===============================================================*/