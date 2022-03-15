#include "ExtraMaths.hpp"


// Conversion functions

// Must be in same namespace as type is defined

namespace linalg {

	/*void to_json(json& j, const phyvec& v) {
		j = json{
			{v.x, v.y}
		};
	}

	void from_json(const json& j, phyvec& v) {
		j.at(0).get_to(v.x);
		j.at(1).get_to(v.y);
	}*/
}

bool colliding(phyvec centre, phyflt radius, phyvec point) {
	phyflt distance_squared = PhysicsEngine::length_squared(point - centre);
	return distance_squared < radius * radius;
}

Framework::Rect circle_to_rect(phyvec centre, phyflt radius) {
	return circle_to_rect(PhysicsEngine::to_fvec(centre), PhysicsEngine::to_float(radius));
}

Framework::Rect circle_to_rect(Framework::vec2 centre, float radius) {
	return Framework::Rect(centre - radius, radius * 2);
}

Framework::Rect find_bounding_rect(const std::vector<phyvec>& vertices) {
	Framework::Rect rect;

	for (const phyvec& v : vertices) {
		if (v.x < rect.position.x) {
			rect.position.x = v.x;
		}
		else if (v.x > rect.position.x + rect.size.x) {
			rect.size.x = v.x - rect.position.x;
		}

		if (v.y < rect.position.y) {
			rect.position.y = v.y;
		}
		else if (v.y > rect.position.y + rect.size.y) {
			rect.size.y = v.y - rect.position.y;
		}
	}

	return rect;
}