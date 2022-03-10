#pragma once

//#include <random>

#include "Maths.hpp"

#include "GraphicsObjects.hpp"

struct Star {
	Framework::vec2 position;
	// Size also dictates speed of movement
	uint8_t size;
	Framework::Colour colour;
};

class StarField {
public:
	struct StarFieldConstants {
		uint32_t large_stars = 10;
		uint32_t medium_stars = 20;
		uint32_t small_stars = 40;

		uint8_t large_star_size = 4;
		uint8_t medium_star_size = 2;
		uint8_t small_star_size = 1;

		float distance_scale = 0.001f;

		uint8_t colour_change_from_white = 0x40;
	};

	StarField();
	StarField(Framework::Graphics* graphics, Framework::vec2 size);

	void set_constants(const StarFieldConstants& constants);

	void render();

	// Set the position of the centre of the StarField
	void move(Framework::vec2 distance);

private:
	void make_stars(uint32_t count, uint8_t size);

	Framework::Graphics* _graphics = nullptr;

	std::vector<Star> _stars;

	Framework::vec2 _size;

	StarFieldConstants _constants;
};