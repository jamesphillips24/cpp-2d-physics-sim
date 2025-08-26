#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int arg, char* argv[]){
	std::cout << "Hello!" << "\n";

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window;
	window = SDL_CreateWindow("Window", 600, 600, 0);

	if(window == NULL){
		SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer* renderer;

	bool done = false;
	while(!done){
		SDL_Event event;

		while(SDL_PollEvent(&event)){
			if (event.type == SDL_EVENT_QUIT){
				done = true;
				break;
			}
		}
	}

	SDL_DestroyWindow(window);

	return 0;
}
