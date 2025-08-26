#include <iostream>
#include <random>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

std::array<int, 3> generate_rand(){
	static std::random_device rnd_device;
	static std::mt19937 rnd_engine {rnd_device()};
	static std::uniform_int_distribution<int> dist {0, 255};

	auto gen = [&](){
		return dist(rnd_engine);
	};

	std::array<int, 3> rgb;
	std::generate(rgb.begin(), rgb.end(), gen);

	return rgb;
}

int main(int arg, char* argv[]){
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window;
	window = SDL_CreateWindow("Window", 600, 600, 0);
	if(window == nullptr){
		SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer* renderer;
	renderer = SDL_CreateRenderer(window, nullptr);
	if(renderer == nullptr){
		SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
		return -1;
	}

	bool done = false;
	while(!done){
		SDL_Event event;

		while(SDL_PollEvent(&event)){
			if(event.type == SDL_EVENT_QUIT){
				done = true;
				break;
			}

			if(event.type == SDL_EVENT_KEY_DOWN){
				if(event.key.key == SDLK_SPACE){
					std::array<int, 3> rgb{generate_rand()};
					SDL_SetRenderDrawColor(renderer, rgb[0], rgb[1], rgb[2], SDL_ALPHA_OPAQUE);
					SDL_RenderClear(renderer);
					SDL_RenderPresent(renderer);
				}
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
