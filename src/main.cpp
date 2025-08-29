#include <iostream>
#include <random>
#include <cmath>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "random_utils.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define TEXTURE_RADIUS 30

#define FRAMES_PER_SECOND_MS 1000 / 60

// Need to make more dynamic based on normal vector
#define ENERGY_LOSS_BOUNCE_WALL 0.9
#define ENERGY_LOSS_BOUNCE_FLOOR 0.8
#define ENERGY_LOSS_FRICTION 0.97

#define GRAVITY 3

// How far back to track cursor positions for calculating velocity after throw
#define NUM_PREV_CURSOR_POS 10

// How many of the most recent positions should it ignore (in case cursor stopped
// at the end of a throw unintentionally)
#define NUM_CURSOR_TRACK_BUFFER 2

#define CURSOR_VELOCITY_SCALAR 1.5

class Ball {
	public:
		SDL_Texture* texture;
		std::array<double, 2> position;
		std::array<double, 2> velocity;
		std::array<std::array<double, 2>, NUM_PREV_CURSOR_POS + NUM_CURSOR_TRACK_BUFFER> cursor_positions{};
		int cursor_pos_index;

		Ball(SDL_Renderer* renderer, SDL_Texture* texture){
			// Create texture
			this->texture = texture;

			// Get random starting direction (only bottom right quadrant direction)
			double vec {generate_rand_vector()};

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
			r.w = TEXTURE_RADIUS;
			r.h = TEXTURE_RADIUS;

			// Send new rectangle to render buffer
			SDL_RenderTexture(renderer, this->texture, nullptr, &r);
		}

		void update_position(){
			this->velocity[1] += GRAVITY;

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
			if(this->position[0] + TEXTURE_RADIUS >= WINDOW_WIDTH){
				this->velocity[0] *= -ENERGY_LOSS_BOUNCE_WALL;
				this->position[0] = WINDOW_WIDTH - TEXTURE_RADIUS - 1;
				return;
			}

			// If it hits the left side
			if (this->position[0] <= 0)
			{
				this->velocity[0] *= -ENERGY_LOSS_BOUNCE_WALL;
				this->position[0] = 1;
				return;
			}

			// If it hits the bottom
			if (this->position[1] + TEXTURE_RADIUS >= WINDOW_HEIGHT)
			{
				this->velocity[0] *= ENERGY_LOSS_FRICTION;

				this->velocity[1] *= -ENERGY_LOSS_BOUNCE_FLOOR;
				this->position[1] = WINDOW_HEIGHT - TEXTURE_RADIUS - 1;
				return;
			}

			// If it hits the top
			if (this->position[1] <= 0)
			{
				this->velocity[1] *= -ENERGY_LOSS_BOUNCE_WALL;
				this->position[1] = 1;
				return;
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

			this->velocity[0] = (x_temp / (this->cursor_positions.size() - NUM_CURSOR_TRACK_BUFFER)) * CURSOR_VELOCITY_SCALAR;
			this->velocity[1] = (y_temp / (this->cursor_positions.size() - NUM_CURSOR_TRACK_BUFFER)) * CURSOR_VELOCITY_SCALAR;
		}
};

SDL_Texture* create_circle_texture(SDL_Renderer* renderer){
	int radius = TEXTURE_RADIUS;
	SDL_Surface *surface = SDL_CreateSurface(radius*2, radius*2, SDL_PIXELFORMAT_RGBA8888);
	SDL_FillSurfaceRect(surface, NULL, SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), nullptr, 255, 255, 255, 0));

	for(int i = -radius; i < radius; i++){
		for (int j = -radius; j < radius; j++){
			if(i*i + j*j <= radius*radius){
				SDL_WriteSurfacePixel(surface, i + radius, j + radius, 255, 255, 255, 255);
			}
		}
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	return texture;
}

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

	SDL_Texture* texture = create_circle_texture(renderer);

	Ball b{renderer, texture};

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
			b.update_position(x - TEXTURE_RADIUS / 2, y - TEXTURE_RADIUS);
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
