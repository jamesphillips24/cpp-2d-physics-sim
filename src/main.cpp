#include <iostream>
#include <random>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "random_utils.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

#define TEXTURE_WIDTH 100
#define TEXTURE_HEIGHT 100

class Ball {
	public:
		SDL_Texture* texture;
		int x;
		int y;
		int direction;

		Ball(SDL_Renderer* renderer){
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, TEXTURE_WIDTH, TEXTURE_HEIGHT);
			this->x = 1;
			this->y = WINDOW_HEIGHT / 2;
			this->direction = 1;
		};

		void render_ball(SDL_Renderer* renderer){
			this->direction *= is_touching_wall() ? -1 : 1;
			this->x += this->direction;

			SDL_FRect r;
			r.x = this->x;
			r.y = this->y;
			r.w = TEXTURE_WIDTH;
			r.h = TEXTURE_HEIGHT;

			SDL_Surface* surface;

			if(SDL_LockTextureToSurface(this->texture, nullptr, &surface)){
				SDL_FillSurfaceRect(surface, nullptr, SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), nullptr, 255, 255, 255));
				SDL_UnlockTexture(this->texture);
			}

			SDL_RenderTexture(renderer, this->texture, nullptr, &r);
		}

		bool is_touching_wall(){
			return this->x + TEXTURE_WIDTH >= WINDOW_WIDTH || this->x <= 0;
		}
};

int main(int arg, char* argv[]){
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window;
	window = SDL_CreateWindow("Window", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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

	Ball b{renderer};

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
		SDL_RenderClear(renderer);
		b.render_ball(renderer);
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
