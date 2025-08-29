#include <iostream>
#include <random>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "random_utils.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

#define TEXTURE_WIDTH 10
#define TEXTURE_HEIGHT 10

// Need to make more dynamic based on normal vector
#define ENERGY_LOSS_BOUNCE_WALL 0.9
#define ENERGY_LOSS_BOUNCE_FLOOR 0.8
#define ENERGY_LOSS_FRICTION 0.97

// How far back to track cursor positions for calculating velocity after throw
#define NUM_PREV_CURSOR_POS 10
// How many of the most recent positions should it ignore (in case cursor stopped
// at the end of a throw unintentionally)
#define NUM_CURSOR_TRACK_BUFFER 2

#define FRAMES_PER_SECOND_MS 1000/60

class Ball {
	public:
		SDL_Texture* texture;
		std::array<double, 2> position;
		std::array<double, 2> velocity;
		std::array<std::array<double, 2>, NUM_PREV_CURSOR_POS + NUM_CURSOR_TRACK_BUFFER> cursor_positions{};
		int cursor_pos_index;

		Ball(SDL_Renderer* renderer){
			// Create texture
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, TEXTURE_WIDTH, TEXTURE_HEIGHT);

			// Get random starting direction (only bottom right quadrant direction)
			double vec {generate_rand_single()};

			// Initialize position and direction (start in the middle)
			this->position[0] = WINDOW_WIDTH/2;
			this->position[1] = WINDOW_HEIGHT/2;
			this->velocity[0] = vec;
			this->velocity[1] = 1 - vec;

			this->cursor_pos_index = 0;
		};

		void render_ball(SDL_Renderer* renderer){
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

		void update_position(){
			// Gravity. Need to use framerate to calculate real gravity
			this->velocity[1] += 3;

			// Velocity changes position
			this->position[0] += this->velocity[0];
			this->position[1] += this->velocity[1];

			this->check_position();
		}

		void update_position(float x, float y){
			this->position[0] = x;
			this->position[1] = y;
		}

		// Can be optimized
		void check_position(){
			// If it hits the right side
			if(this->position[0] + TEXTURE_WIDTH >= WINDOW_WIDTH){
				this->velocity[0] *= -ENERGY_LOSS_BOUNCE_WALL;
				this->position[0] = WINDOW_WIDTH - TEXTURE_WIDTH - 1;
			}

			// If it hits the left side
			if (this->position[0] <= 0)
			{
				this->velocity[0] *= -ENERGY_LOSS_BOUNCE_WALL;
				this->position[0] = 1;
			}

			// If it hits the bottom
			if (this->position[1] + TEXTURE_HEIGHT >= WINDOW_HEIGHT)
			{
				this->velocity[0] *= ENERGY_LOSS_FRICTION;

				this->velocity[1] *= -ENERGY_LOSS_BOUNCE_FLOOR;
				this->position[1] = WINDOW_HEIGHT - TEXTURE_HEIGHT - 1;
			}

			// If it hits the top
			if (this->position[1] <= 0)
			{
				this->velocity[1] *= -ENERGY_LOSS_BOUNCE_WALL;
				this->position[1] = 1;
			}
		}

		void track_cursor_position(std::array<float, 2> pos){
			this->cursor_positions[this->cursor_pos_index][0] = pos[0];
			this->cursor_positions[this->cursor_pos_index][1] = pos[1];

			if(this->cursor_pos_index == this->cursor_positions.size() - 1){
				this->cursor_pos_index = 0;
			}
			else{
				this->cursor_pos_index++;
			}
		}

		void update_cursor_velocity(){
			double x_temp{0};
			double y_temp{0};

			int starting_index {(int)((this->cursor_pos_index - NUM_CURSOR_TRACK_BUFFER - 1 + this->cursor_positions.size()) % this->cursor_positions.size())};
			int i {0};
			for (i = starting_index;; i--)
			{
				if (i == -1)
				{
					i = this->cursor_positions.size() - 1;
				}

				x_temp -= this->cursor_positions[i][0] - this->cursor_positions[(i - 1 + this->cursor_positions.size()) % this->cursor_positions.size()][0];
				y_temp -= this->cursor_positions[i][1] - this->cursor_positions[(i - 1 + this->cursor_positions.size()) % this->cursor_positions.size()][1];

				if(i == this->cursor_pos_index)
				{
					break;
				}
			}

			this->velocity[0] = (x_temp / (this->cursor_positions.size() - NUM_CURSOR_TRACK_BUFFER));
			this->velocity[1] = (y_temp / (this->cursor_positions.size() - NUM_CURSOR_TRACK_BUFFER));
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

	Uint64 frameStart;
	int frameTime;
	bool done = false;
	bool mouse_up {true};
	while(!done){
		frameStart = SDL_GetTicks();

		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_EVENT_QUIT){
				done = true;
				break;
			}
			if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
				mouse_up = false;
			}
			if(event.type == SDL_EVENT_MOUSE_BUTTON_UP){
				mouse_up = true;
			}
		}

		if(!mouse_up){
			float x;
			float y;

			SDL_GetMouseState(&x, &y);
			b.update_position(x - TEXTURE_WIDTH / 2, y - TEXTURE_HEIGHT);
			b.track_cursor_position({x, y});
			b.update_cursor_velocity();
		}

		SDL_RenderClear(renderer);
		b.update_position();
		b.render_ball(renderer);
		SDL_RenderPresent(renderer);

		frameTime = SDL_GetTicks() - frameStart;
		if(frameTime < FRAMES_PER_SECOND_MS){
			SDL_Delay(FRAMES_PER_SECOND_MS - frameTime);
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
