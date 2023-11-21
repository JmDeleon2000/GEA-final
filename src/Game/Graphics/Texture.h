#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <functional>
#include "PixelShader.h"

class Texture {
	public:
		Texture(SDL_Renderer* renderer);
		~Texture();

		void load(std::string path, PixelShader shader = { nullptr, "" });
		void createEmpty(int x, int y, Uint8 red, Uint8 green, Uint8 blue);

		void render(int x, int y, int w = 0, int h = 0, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

		Uint32 color(Uint8 red, Uint8 green, Uint8 blue);

		bool lockTexture();
		bool unlockTexture();
		void drawOnTexture(SDL_Texture* src, int x, int y, int w = 0, int h = 0, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
		Uint32* getPixels();
    int getPixelCount();
    Uint32 getPixel(int x, int y);
    Uint32 getPixel(int xy);

		int width;
		int height;
		SDL_Texture* texture;
	private:
		void* pixels;
		int pitch;
    Uint32 format;
    SDL_PixelFormat* mappingFormat;

    SDL_Renderer *renderer;
};
