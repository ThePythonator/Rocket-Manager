#pragma once

#include "Maths.hpp"

#include "PhysicsEngineMath.hpp"
using namespace PhysicsEngine::phy_t;

class Camera {
public:
	Camera();
	Camera(Framework::vec2 window_size);

	void set_position(phyvec position);
	phyvec get_position() const;

	void set_scale(phyflt scale);
	phyflt get_scale() const;

	// Returns the location that position should be rendered at
	// Camera's position is the centre of the display
	//Framework::vec2 get_render_position(const Framework::vec2& position) const;
	phyvec get_render_position(const phyvec& position) const;

private:
	phyflt _scale = 1.0f;

	phyvec _position, _window_size;
};