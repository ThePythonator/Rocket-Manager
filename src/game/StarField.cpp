#include "StarField.hpp"

StarField::StarField() {

}
StarField::StarField(Framework::Graphics* graphics, Framework::vec2 size) {
	_graphics = graphics;

	_size = size;

	// TODO: change
	make_stars(_constants.large_stars, _constants.large_star_size);
	make_stars(_constants.medium_stars, _constants.medium_star_size);
	make_stars(_constants.small_stars, _constants.small_star_size);
}

void StarField::set_constants(const StarFieldConstants& constants) {
	_constants = constants;
	// TODO reload stars since constants have changed?
}

void StarField::render() {
	for (const Star& star : _stars) {
		// Render star
		_graphics->render_filled_rect(Framework::Rect(star.position, star.size), star.colour);
	}
}


void StarField::move(Framework::vec2 distance) {
	Framework::vec2 offset = -distance * _constants.distance_scale;

	for (Star& star : _stars) {
		star.position += offset * star.size;

		// Has it gone off screen? If so, make new one at other side
		Framework::vec2 new_position = star.position;
		if (star.position.x + star.size < 0) {
			new_position.x = _size.x;
		}
		else if (star.position.x > _size.x) {
			new_position.x = -star.size;
		}
		if (star.position.y + star.size < 0) {
			new_position.y = _size.y;
		}
		else if (star.position.y > _size.y) {
			new_position.y = -star.size;
		}

		star.position = new_position;
	}
}


void StarField::make_stars(uint32_t count, uint8_t size) {
	for (uint32_t i = 0; i < count; i++) {
		Framework::vec2 position = { Framework::randf() * _size.x, Framework::randf() * _size.y };

		Framework::Colour colour = Framework::Colour(
			0xFF - Framework::randf() * _constants.colour_change_from_white,
			0xFF - Framework::randf() * _constants.colour_change_from_white,
			0xFF - Framework::randf() * _constants.colour_change_from_white
		);

		_stars.push_back(Star{ position, size, colour });
	}
}