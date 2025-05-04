#pragma once
#include <SDL_image.h>

bool image_init(SDL_Renderer* r);
bool image_load(const char* image);
void image_render(const char* image, float x, float y);
void image_free();
