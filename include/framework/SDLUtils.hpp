#pragma once

#include "SDL.h"
#include "SDL_image.h"

#include <cstdio>
#include <string>

#include "Colour.hpp"

#include "Maths.hpp"

namespace Framework::SDLUtils {

	// Initialises necessary SDL bits, and assigns window and renderer.
	bool init_sdl(SDL_Window*& window, SDL_Renderer*& renderer, const std::string& title, const vec2& size);

	std::string find_assets_path(std::string test_file, uint8_t depth);

	void SDL_SetRenderDrawColor(SDL_Renderer* renderer, const Colour& colour);
	Colour SDL_GetRenderDrawColor(SDL_Renderer* renderer);

	void SDL_SetTextureColorMod(SDL_Texture* texture, const Colour& colour);

	void SDL_UnsetRenderTarget(SDL_Renderer* renderer);

	void SDL_RenderFillRect(SDL_Renderer* renderer, const Rect& rect);
	void SDL_RenderFillRect(SDL_Renderer* renderer, const Rect& rect, const Colour& colour);

	void SDL_RenderDrawLine(SDL_Renderer* renderer, const vec2& start, const vec2& end);
	void SDL_RenderDrawLine(SDL_Renderer* renderer, const vec2& start, const vec2& end, const Colour& colour);

	void SDL_SetPixel(SDL_Surface* surface, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void SDL_SetPixel(SDL_Surface* surface, int x, int y, const Colour& colour);

	void SDL_GetPixel(SDL_Surface* surface, int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
	Colour SDL_GetPixel(SDL_Surface* surface, int x, int y);

	int SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius);
	//void SDL_RenderDrawCircle(SDL_Renderer* renderer, vec2 centre, float _radius, vec2 window_size); // testing
	//void SDL_RenderDrawTrimmedCircle(SDL_Renderer* renderer, const vec2& centre, float radius, const Rect& trim_rect, const Colour& colour);

	// Via trig functions and lines
	void SDL_RenderDrawCircle_Fast(SDL_Renderer* renderer, const vec2& centre, float radius, const Colour& colour);

	void SDL_RenderDrawArcInWindow(SDL_Renderer* renderer, const vec2& centre, float radius, const vec2& window, const Colour& colour);
	//void SDL_RenderDrawCircleTEST(SDL_Renderer* renderer, const vec2& centre, float radiuss, const Colour& colour);

	void SDL_RenderDrawArc(SDL_Renderer* renderer, const vec2& centre, float radius1, float radius2, float angle_start, float angle_stop, const Colour& colour);
	void SDL_RenderDrawArc(SDL_Renderer* renderer, const vec2& centre, float radius, float angle_start, float angle_stop, const Colour& colour);

	SDL_Rect get_sdl_rect(Rect rect);
	SDL_Point get_sdl_point(vec2 vec);
	SDL_RendererFlip get_sdl_renderer_flip(ImageFlip flip);

	Rect get_bounds(const vec2& centre, float radius);

	Rect trim(const Rect& a, const Rect& b);
}