#include "Graphics.hpp"

namespace Framework {
	Graphics::Graphics() {

	}

	void Graphics::fill(const Colour& colour) {
		set_colour(colour);

		// NULL means draw rect over whole renderer
		SDL_RenderFillRect(renderer, NULL);
	}

	void Graphics::fill(const Colour& colour, uint8_t alpha) {
		fill(Colour(colour, alpha));
	}

	void Graphics::fill(const Rect& rect, const Colour& colour) {
		set_colour(colour);

		SDLUtils::SDL_RenderFillRect(renderer, rect);
	}

	void Graphics::fill(const Rect& rect, const Colour& colour, uint8_t alpha) {
		fill(rect, Colour(colour, alpha));
	}

	void Graphics::render_line(const vec2& start, const vec2& end, const Colour& colour) {
		set_colour(colour);
		SDLUtils::SDL_RenderDrawLine(renderer, start, end);
	}

	void Graphics::render_poly(const std::vector<vec2> points, const Colour& colour) {
		for (uint16_t i = 0; i < points.size(); i++) {
			// Get next index (for end point)
			uint8_t next_i = i + 1 < points.size() ? i + 1 : 0;

			render_line(points[i], points[next_i], colour);
		}
	}

	void Graphics::render_poly(const std::vector<vec2> points, const vec2& offset, const Colour& colour) {
		for (uint16_t i = 0; i < points.size(); i++) {
			// Get next index (for end point)
			uint8_t next_i = i + 1 < points.size() ? i + 1 : 0;

			render_line(points[i] + offset, points[next_i] + offset, colour);
		}
	}

	void Graphics::render_rect(const Rect& rect, const Colour& colour) {
		render_poly({rect.topleft(), rect.topright(), rect.bottomright(), rect.bottomleft()}, colour);
	}

	void Graphics::render_filled_rect(const Rect& rect, const Colour& colour) {
		set_colour(colour);
		SDLUtils::SDL_RenderFillRect(renderer, rect);
	}

	void Graphics::render_circle(const vec2& centre, float radius, const Colour& colour) {
		set_colour(colour);
		SDLUtils::SDL_RenderDrawCircle(renderer, static_cast<int>(centre.x), static_cast<int>(centre.y), static_cast<int>(radius));
	}

	//void Graphics::render_trimmed_circle(const vec2& centre, float radius, const Rect& trim_rect, const Colour& colour) {
	//	SDLUtils::SDL_RenderDrawTrimmedCircle(renderer, centre, radius, trim_rect, colour); // Has to be fast for now - just uses lots of lines. Probably only good for small sections of a huge circle
	//}


	void Graphics::set_renderer(SDL_Renderer* _renderer) {
		renderer = _renderer;
	}

	SDL_Renderer* Graphics::get_renderer() {
		return renderer;
	}

	void Graphics::set_colour(const Colour& colour) {
		SDLUtils::SDL_SetRenderDrawColor(renderer, colour);
	}
}