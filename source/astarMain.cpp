#include "./headers/astarMain.h"



int main(int argc, char **argv){

	/*Initialize a pointer to a board class*/
	Board* myBoard = nullptr;

	if(argc>1){
		/*Initialize the board from .txt file given in commandline argument argv*/
		myBoard = createBoard(argv[1]);
	}
	else{
		/*Give user option to provide path to boardfile*/
		std::string inputfile;
		char choose = 'n';
		std::cout << "Do you want to choose the input board file yourself? (y/n): " << std::endl;
		std::cin >> choose;
		if(choose == 'y'){
			std::cout << "Type file path relative to current dir.: " << std::endl;
			std::cin >> inputfile;
			myBoard = createBoard(inputfile.c_str());
		}
		else{
			/*Initializes to defaultfilepath if no argument is given*/
			myBoard = createBoard(defaultfilepath);
		}
	}

	/*Check if board was successfully created*/
	if(!myBoard){
		std::cout<<"Failed to create board, see errorlog for details."<<std::endl;
		exit(0);
	}

	/*Creates the meshgraph for the algorithm*/
	myBoard->createmesh();

	/*Initializes SDL for graphical display*/
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		SDLerrorLogger("SDL initialization");
		std::cout<<"Failed to initialize SDL, see errorlog for details."<<std::endl;
		return 1;
	}

	/*Disables pesky screensavers while our wonderful graphics are beeing displayed*/
	SDL_DisableScreenSaver();

	/*Initializes a window to render graphics in*/
	SDL_Window *win = SDL_CreateWindow("Pathfinder", 0, 0, myBoard->columns*windowScale, myBoard->rows*windowScale, 0);
	if (win == nullptr){
		SDLerrorLogger("SDL_CreateWindow");
		std::cout<<"Failed to create SDL window, see errorlog for details."<<std::endl;
		return 1;
	}

	/*Initializes the renderer to draw in*/
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDLerrorLogger("SDL_CreateRenderer");
		std::cout<<"Failed to create SDL renderer, see errorlog for details."<<std::endl;
		return 1;
	}

	/*Renders the board to the window*/
	renderBoard(myBoard, windowScale, margin, ren);
	SDL_RenderPresent(ren);
	
	bool donealg;
	int astarvalue = -1;

	/*Main while loop for the algorithm*/
	while(!donealg){
		/*If a third argument is given via command line, the process is animated*/
		if(argc>2){
			astarvalue = myBoard->algorithm(iterations);
		}
		/*If not, it completes before it prints*/
		else{
			astarvalue = myBoard->algorithm(0);
		}
		if(astarvalue == 1){
			donealg = true;
			std::cout<<"Solution found!"<<std::endl;
		}
		else if(astarvalue == -1){
			donealg = true;
			std::cout<<"No solution found"<<std::endl;
		}
		renderBoard(myBoard, windowScale, margin, ren);
		SDL_RenderPresent(ren);
		usleep(iterationdelay*1000);
	}

	/*Waits for user input before quitting*/
    waitForEvent();

}

void waitForEvent(){
	SDL_Event event;
	bool done = false;
	while((!done) && (SDL_WaitEvent(&event))) {
        switch(event.type) {
    
            case SDL_KEYDOWN:
                done = true;
                break;


            case SDL_QUIT:
                done = true;
                break;
                
            default:
                break;
        } 
            
    }
}