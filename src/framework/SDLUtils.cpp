#include "SDLUtils.hpp"

namespace Framework::SDLUtils {

	bool init_sdl(SDL_Window*& window, SDL_Renderer*& renderer, const std::string& title, const vec2& size) {
		// Initialise SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL could not initialize!\nSDL Error: %s\n", SDL_GetError());
			SDL_ClearError();
			return false;
		}

		// Initialise SDL_image
		int img_init_flags = IMG_INIT_JPG | IMG_INIT_PNG;
		if ((IMG_Init(img_init_flags) & img_init_flags) != img_init_flags) {
			printf("SDL_IMG could not initialize!\nSDL_IMG Error: %s\n", IMG_GetError());
			IMG_SetError("");
			return false;
		}

		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		// Create window
		window = SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			size.x,
			size.y,
			0
		);

		// Create renderer for window
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		// Set renderer mode
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		// Set scaling so that fullscreen mode is actually the same logical resolution
		SDL_RenderSetLogicalSize(renderer, size.x, size.y);

		return renderer != nullptr && window != nullptr;
	}

	std::string find_assets_path(std::string test_file, uint8_t depth) {
		printf("Attempting to find base folder...\n");

		std::string base_path = SDL_GetBasePath();

		SDL_Surface* test_surface = IMG_Load(test_file.c_str());

		if (test_surface != NULL) {
			base_path = "";
		}

		uint8_t count = 0;
		while (test_surface == NULL && count < depth) {
			printf("path: %s\n", (base_path + test_file).c_str());

			test_surface = IMG_Load((base_path + test_file).c_str());

			if (test_surface == NULL) {
				base_path += "../";
			}

			count++;
		}

		if (test_surface == NULL) {
			printf("Could not find assbaseets folder!\n");
			return "";
		}

		SDL_FreeSurface(test_surface);
		SDL_ClearError();

		std::string message = "Found base folder: " + base_path + "\n\n";

		printf("%s", message.c_str());

		return base_path;
	}

	void SDL_SetRenderDrawColor(SDL_Renderer* renderer, const Colour& colour) {
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
	}

	Colour SDL_GetRenderDrawColor(SDL_Renderer* renderer) {
		Colour c;
		SDL_GetRenderDrawColor(renderer, &c.r, &c.g, &c.b, &c.a);
		return c;
	}

	void SDL_SetTextureColorMod(SDL_Texture* texture, const Colour& colour) {
		SDL_SetTextureColorMod(texture, colour.r, colour.g, colour.b);
	}


	void SDL_UnsetRenderTarget(SDL_Renderer* renderer) {
		::SDL_SetRenderTarget(renderer, NULL);
	}

	// Uses the current colour set by SDL_SetRenderDrawColour
	void SDL_RenderFillRect(SDL_Renderer* renderer, const Rect& rect) {
		SDL_Rect sdl_rect = SDLUtils::get_sdl_rect(rect);
		SDL_RenderFillRect(renderer, &sdl_rect);
	}

	void SDL_RenderFillRect(SDL_Renderer* renderer, const Rect& rect, const Colour& colour) {
		// Keep old colour and set back afterwards?
		//Colour old_colour = SDL_GetRenderDrawColor(renderer);
		SDL_SetRenderDrawColor(renderer, colour);

		SDL_RenderFillRect(renderer, rect);

		//SDL_SetRenderDrawColor(renderer, old_colour);
	}


	void SDL_RenderDrawRect(SDL_Renderer* renderer, const Rect& rect) {
		SDL_Rect sdl_rect = SDLUtils::get_sdl_rect(rect);
		SDL_RenderDrawRect(renderer, &sdl_rect);
	}


	void SDL_RenderDrawLine(SDL_Renderer* renderer, const vec2& start, const vec2& end) {
		SDL_RenderDrawLine(renderer, static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y));
	}
	void SDL_RenderDrawLine(SDL_Renderer* renderer, const vec2& start, const vec2& end, const Colour& colour) {
		// Keep old colour and set back afterwards?
		SDL_SetRenderDrawColor(renderer, colour);

		SDL_RenderDrawLine(renderer, start, end);
	}

	void SDL_SetPixel(SDL_Surface* surface, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		SDL_LockSurface(surface);
		((uint32_t*)surface->pixels)[y * surface->w + x] = SDL_MapRGBA(surface->format, r, g, b, a);
		SDL_UnlockSurface(surface);
	}
	void SDL_SetPixel(SDL_Surface* surface, int x, int y, const Colour& colour) {
		SDL_SetPixel(surface, x, y, colour.r, colour.g, colour.b, colour.a);
	}

	void SDL_GetPixel(SDL_Surface* surface, int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
		SDL_LockSurface(surface);
		SDL_GetRGBA(((uint32_t*)surface->pixels)[y * surface->w + x], surface->format, r, g, b, a);
		SDL_UnlockSurface(surface);
	}
	Colour SDL_GetPixel(SDL_Surface* surface, int x, int y) {
		uint8_t r, g, b, a;
		SDL_GetPixel(surface, x, y, &r, &g, &b, &a);
		return Colour(r, g, b, a);
	}

	int SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius) {
		// From https://gist.github.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c

		int offset_x = 0;
		int offset_y = radius;
		int d = radius - 1;
		int status = 0;

		while (offset_y >= offset_x) {
			status += SDL_RenderDrawPoint(renderer, x + offset_x, y + offset_y);
			status += SDL_RenderDrawPoint(renderer, x + offset_y, y + offset_x);
			status += SDL_RenderDrawPoint(renderer, x - offset_x, y + offset_y);
			status += SDL_RenderDrawPoint(renderer, x - offset_y, y + offset_x);
			status += SDL_RenderDrawPoint(renderer, x + offset_x, y - offset_y);
			status += SDL_RenderDrawPoint(renderer, x + offset_y, y - offset_x);
			status += SDL_RenderDrawPoint(renderer, x - offset_x, y - offset_y);
			status += SDL_RenderDrawPoint(renderer, x - offset_y, y - offset_x);

			if (status < 0) {
				status = -1;
				break;
			}

			if (d >= 2 * offset_x) {
				d -= 2 * offset_x + 1;
				offset_x += 1;
			}
			else if (d < 2 * (radius - offset_y)) {
				d += 2 * offset_y - 1;
				offset_y -= 1;
			}
			else {
				d += 2 * (offset_y - offset_x - 1);
				offset_y -= 1;
				offset_x += 1;
			}
		}

		return status;
	}

	void SDL_RenderDrawCircle_Fast(SDL_Renderer* renderer, const vec2& centre, float radius, const Colour& colour) {
		// Uses trig to render circles. Seems to be faster than rendering point by point but it's messier.
		SDL_RenderDrawArc(renderer, centre, radius, 0, 2 * PI, colour);
	}


	/*
	void SDL_RenderDrawTrimmedCircle(SDL_Renderer* renderer, const vec2& centre, float radius, const Rect& trim_rect, const Colour& colour) {
		// Get rect from circle
		Rect bounds = get_bounds(centre, radius);

		// UNFINISHED

		// Trim bounds to window_size
		Rect trimmed = trim(bounds, trim_rect);

		// If size is 0, we can't render it
		if (trimmed.size == VEC_NULL) return;

		// For testing
		//SDL_RenderDrawRect(renderer, trimmed);
		//printf("c: %f, %f\tr: %f\n", centre.x, centre.y, radius);

		float start_angle = 0.0f;
		float end_angle = 6.0f;

		SDL_RenderDrawArc(renderer, centre, radius, start_angle, end_angle, colour);
	}
	*/

	void SDL_RenderDrawArc(SDL_Renderer* renderer, const vec2& centre, float radius1, float radius2, float angle_start, float angle_stop, const Colour& colour) {
		// Modified from https://github.com/pygame/pygame/blob/main/src_c/draw.c

		SDL_SetRenderDrawColor(renderer, colour);

		// Angle step (radians)
		float a_step = 0.0f;
		// Current angle (radians)
		float a = 0.0f;

		if (angle_stop < angle_start) {
			std::swap(angle_start, angle_stop);
		}

		vec2 last, next;

		float x = centre.x;
		float y = centre.y;

		// Angle step in rad
		if (radius1 < radius2) {
			if (radius1 < 1.0e-4f) {
				a_step = 1.0f;
			}
			else {
				a_step = std::asin(2.0f / radius1);
			}
		}
		else {
			if (radius2 < 1.0e-4f) {
				a_step = 1.0f;
			}
			else {
				a_step = std::asin(2.0f / radius2);
			}
		}

		//a_step = std::max(a_step, 1e-7);

		last.x = x + std::cos(angle_start) * radius1;
		last.y = y - std::sin(angle_start) * radius2;

		for (a = angle_start + a_step; a < a_step + angle_stop; a += a_step) {
			next.x = x + std::cos(a) * radius1;
			next.y = y - std::sin(a) * radius2;

			SDL_RenderDrawLine(renderer, last, next);
			
			last = next;
		}
	}

	void SDL_RenderDrawArcInWindow(SDL_Renderer* renderer, const vec2& centre, float radius, const vec2& window, const Colour& colour) {
		// Gets approximate start and stop angle, then calls RenderDrawArc

		// This doesn't work if the whole circle is on screen
		Rect window_rect = Rect(VEC_NULL, window);
		
		// Get vector from centre to each window vertex
		//
		// a-----b
		// |     |
		// c-----d
		//

		std::vector<vec2> vectors;

		vectors.push_back(window_rect.topleft() - centre);
		vectors.push_back(window_rect.topright() - centre);
		vectors.push_back(window_rect.bottomleft() - centre);
		vectors.push_back(window_rect.bottomright() - centre);

		// Find minimum and maximum angles

		float min_angle = 2 * PI; // or just PI
		float max_angle = -2 * PI; // or just -PI

		for (const vec2& vector : vectors) {
			// Calculate the angle acw from the positive x axis
			// Negative sign makes it acw rather than cw (I think)
			float angle = -std::atan2(vector.y, vector.x);

			min_angle = std::min(min_angle, angle);
			max_angle = std::max(max_angle, angle);
		}

		// Draw the arc
		SDL_RenderDrawArc(renderer, centre, radius, min_angle, max_angle, colour);
	}

	//void SDL_RenderDrawCircleTEST(SDL_Renderer* renderer, const vec2& centre, float radius, const Colour& colour) {
	//	// Modified from https://github.com/pygame/pygame/blob/main/src_c/draw.c

	//	Rect window = Rect(0, 0, 1024, 768); // to change - make param?

	//	// Angle step (radians)
	//	float a_step = 0.0f;
	//	float actual_a_step = 0.0f;
	//	float a_step_offscreen = 0.05f;

	//	vec2 last, next;

	//	float x = centre.x;
	//	float y = centre.y;

	//	// Angle step in rad
	//	if (radius < 1.0e-4f) {
	//		actual_a_step = 1.0f;
	//	}
	//	else {
	//		actual_a_step = std::asin(2.0f / radius);
	//	}

	//	// float error = 0.33f;
	//	// float thing = 1.0f - error / radius;
	//	// a_step = std::acos(2 * thing * thing - 1.0f);

	//	//aStep = std::max(aStep, 0.05f);
	//	a_step = actual_a_step;
	//	//printf("a step: %f, %f\n", a_step, actual_a_step);
	//	//printf("offscreen: %f\n", a_step_offscreen);

	//	last.x = x + radius;
	//	last.y = y;

	//	for (float a = 0.0f; a < a_step + 2 * PI; a += a_step) {
	//		next.x = x + std::cos(a) * radius;
	//		next.y = y - std::sin(a) * radius;

	//		// If both are offscreen, take bigger jumps
	//		a_step = colliding(window, last) || colliding(window, next) ? actual_a_step : a_step_offscreen;

	//		SDL_RenderDrawLine(renderer, last, next, a_step == actual_a_step ? colour : Colour(255,50,50)); // test - red where not rendering high detail version

	//		last = next;
	//	}
	//}

	void SDL_RenderDrawArc(SDL_Renderer* renderer, const vec2& centre, float radius, float angle_start, float angle_stop, const Colour& colour) {
		SDL_RenderDrawArc(renderer, centre, radius, radius, angle_start, angle_stop, colour);
	}



	SDL_Rect get_sdl_rect(Rect rect) {
		SDL_Rect sdl_rect{ static_cast<int>(rect.position.x), static_cast<int>(rect.position.y), static_cast<int>(rect.size.x), static_cast<int>(rect.size.y) };
		return sdl_rect;
	}

	SDL_Point get_sdl_point(vec2 vec) {
		SDL_Point sdl_point{ static_cast<int>(vec.x), static_cast<int>(vec.y) };
		return sdl_point;
	}

	SDL_RendererFlip get_sdl_renderer_flip(ImageFlip flip) {
		uint8_t sdl_flip = SDL_FLIP_NONE;

		if (flip & ImageFlip::FLIP_HORIZONTAL) sdl_flip |= SDL_FLIP_HORIZONTAL;
		if (flip & ImageFlip::FLIP_VERTICAL) sdl_flip |= SDL_FLIP_HORIZONTAL;

		return static_cast<SDL_RendererFlip>(sdl_flip);
	}

	Rect get_bounds(const vec2& centre, float radius) {
		return Rect(centre - radius, radius * 2 * VEC_ONES);
	}

	Rect trim(const Rect& a, const Rect& b) {
		// If no intersection, we can't clip
		if (!colliding(a, b)) return RECT_NULL;

		Rect result; // note: broken?

		result.position.x = std::max(a.position.x, b.position.x);
		result.position.y = std::max(a.position.y, b.position.y);

		result.size.x = std::min(a.position.x + a.size.x, b.position.x + b.size.x) - result.position.x;
		result.size.y = std::min(a.position.y + a.size.y, b.position.y + b.size.y) - result.position.y;

		return result;
	}
}