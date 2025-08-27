#include <iostream>
#include <random>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "random_utils.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

#define TEXTURE_WIDTH 10
#define TEXTURE_HEIGHT 10

class Ball {
	public:
		SDL_Texture* texture;
		std::array<double, 2> position;
		std::array<double, 2> direction;

		Ball(SDL_Renderer* renderer){
			// Create texture
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, TEXTURE_WIDTH, TEXTURE_HEIGHT);

			// Get random starting direction (only bottom right quadrant direction)
			double vec {generate_rand_single()};

			// Initialize position and direction (start in the middle)
			this->position[0] = WINDOW_WIDTH/2;
			this->position[1] = WINDOW_HEIGHT/2;
			this->direction[0] = vec;
			this->direction[1] = 1 - vec;
		};

		void render_ball(SDL_Renderer* renderer){
			// Gravity. Need to use framerate to calculate real gravity
			this->direction[1] += 0.01;

			// Velocity changes position
			this->position[0] += this->direction[0];
			this->position[1] += this->direction[1];

			this->check_position();

			// Update texture rectangle position
			SDL_FRect r;
			r.x = this->position[0];
			r.y = this->position[1];
			r.w = TEXTURE_WIDTH;
			r.h = TEXTURE_HEIGHT;

			// Draw new rectangle
			SDL_Surface* surface;
			if(SDL_LockTextureToSurface(this->texture, nullptr, &surface)){
				SDL_FillSurfaceRect(surface, nullptr, SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), nullptr, 255, 255, 255));
				SDL_UnlockTexture(this->texture);
			}

			// Send new rectangle to render buffer
			SDL_RenderTexture(renderer, this->texture, nullptr, &r);
		}

		// Can be optimized
		void check_position(){
			// If it hits the right side
			if(this->position[0] + TEXTURE_WIDTH >= WINDOW_WIDTH){
				this->direction[0] *= -0.9;
				this->position[0] = WINDOW_WIDTH - TEXTURE_WIDTH - 1;
			}

			// If it hits the left side
			if (this->position[0] <= 0)
			{
				this->direction[0] *= -0.9;
				this->position[0] = 1;
			}

			// If it hits the bottom
			if (this->position[1] + TEXTURE_HEIGHT >= WINDOW_HEIGHT)
			{
				// If it's sliding
				if (this->direction[1] < 0.3)
				{
					// Prevent jittering (to an extent)
					this->direction[1] = 0;
					this->position[1] = WINDOW_HEIGHT - TEXTURE_HEIGHT - 1;

					// Friction
					this->direction[0] *= 0.93;
				}

				this->direction[1] *= -0.8;
				this->position[1] = WINDOW_HEIGHT - TEXTURE_HEIGHT - 1;
			}

			// If it hits the top
			if (this->position[1] <= 0)
			{
				this->direction[1] *= -0.9;
				this->position[1] = 1;
			}
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
		}
		SDL_RenderClear(renderer);
		b.render_ball(renderer);
		SDL_RenderPresent(renderer);
		SDL_Delay(2);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
