#pragma once

#include "PhysicsEngineMath.hpp"

namespace PhysicsEngine {
	// Used for positioning objects for rendering
	struct Rect {
		dvec2 position, size;
	};

	// No longer used
	// Used to link images to a RigidBody instance
	struct ImageRenderData {
		// Rect from spritesheet
		Rect source_rect;

		// Relative to centre of shape (also shifted when centroid is used to shift polygon points)
		dvec2 destination;
	};
}