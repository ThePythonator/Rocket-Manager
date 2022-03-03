#pragma once

#include "Maths.hpp"

class Camera {
public:
	Camera();
	Camera(Framework::vec2 window_size);

	void set_position(Framework::vec2 position);
	Framework::vec2 get_position() const;

	void set_scale(float scale);
	float get_scale() const;

	// Returns the location that position should be rendered at
	// Camera's position is the centre of the display
	Framework::vec2 get_render_position(Framework::vec2 position) const;

private:
	float _scale = 1.0f;

	Framework::vec2 _position, _window_size;
};