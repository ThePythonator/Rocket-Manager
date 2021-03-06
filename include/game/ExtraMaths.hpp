#pragma once

#include "Maths.hpp"

#include "PhysicsEngineMath.hpp"

#include "File.hpp"

using nlohmann::json;

using namespace PhysicsEngine::phy_t;

// Conversion functions

// Must be in same namespace as type is defined

namespace linalg {

	//void to_json(json& j, const phyvec& v);
	//void from_json(const json& j, phyvec& v);

	// Must define body of templates in the header otherwise the necessary specializations aren't created.
	template<class T>
	void to_json(json& j, const linalg::vec<T, 2>& v) {
		j = json{
			v.x,
			v.y
		};
	}

	template<class T>
	void from_json(const json& j, linalg::vec<T, 2>& v) {
		j.at(0).get_to(v.x);
		j.at(1).get_to(v.y);
	}

}

// Circle to point collisions
bool colliding(phyvec centre, phyflt radius, phyvec point);

Framework::Rect circle_to_rect(phyvec centre, phyflt radius);
Framework::Rect circle_to_rect(Framework::vec2 centre, float radius);

Framework::Rect find_bounding_rect(const std::vector<phyvec>& vertices);


template<typename T>
T pop_front(std::vector<T>& v);