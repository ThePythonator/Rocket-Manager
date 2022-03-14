#pragma once

#include <cmath>
#include <limits>
#include <vector>
#include <map>
#include <string>

#include "linalg.h"

namespace Framework {
	typedef linalg::aliases::float2 vec2;
	typedef linalg::aliases::float2x2 mat22;
	//typedef vec2 Vec;
	//typedef mat22 Mat;

	using linalg::mul;
	using linalg::length;
	using linalg::dot;
	using linalg::cross;
	using linalg::inverse;
	using linalg::identity;

	extern const float PI;
	// extern const float EPSILON;

	extern const vec2 VEC_NULL;
	extern const vec2 VEC_ONES;

	class Rect {
	public:
		Rect(vec2 _position = VEC_NULL, vec2 _size = VEC_NULL);
		Rect(vec2 _position, int _size);
		Rect(float x, float y, float width = 0.0f, float height = 0.0f);
		Rect(int x, int y, int width = 0, int height = 0);

		vec2 centre() const;

		vec2 topleft() const;
		vec2 topright() const;
		vec2 bottomleft() const;
		vec2 bottomright() const;

		vec2 position;
		vec2 size;
	};

	extern const Rect RECT_NULL;

	Rect centred_rect(vec2 centre, vec2 size);
	Rect centred_rect(vec2 centre, float size);
	Rect centred_rect(float centre_x, float centre_y, float width, float height);

	bool colliding(Rect a, vec2 b);
	bool colliding(Rect a, Rect b);

	vec2 Vec(float x, float y);
	vec2 Vec(int x, int y);

	float length_squared(vec2 v);
	vec2 normalise(vec2 v);

	enum ImageFlip {
		FLIP_NONE		= 0b00,
		FLIP_HORIZONTAL = 0b01,
		FLIP_VERTICAL	= 0b10,

		FLIP_BOTH		= FLIP_HORIZONTAL | FLIP_VERTICAL
	};

	// Convert to/from radians and degrees
	float deg_to_rad(float degrees);
	float rad_to_deg(float radians);

	// angle is in radians
	mat22 rotation_matrix(float angle);

	// Get vector perpendicular to supplied vector
	vec2 perpendicular_acw(vec2 vector);
	vec2 perpendicular_cw(vec2 vector);

	float clamp(float x, float _min, float _max);

	/*
	* Generates a random float between 0 and 1 inclusive.
	* Resolution is 0.001f.
	*/
	float randf();

	float randf(float min, float max);

	/*
	* Trims all characters which are more than 'precision' chars after a full stop/decimal place.
	* 
	* Example usage:
	*		trim_precision(std::to_string(my_float), 3);
	*/
	std::string trim_precision(std::string string, uint8_t precision);

	/*
	* Overloaded version which automatically converts a float to a string before trimming.
	*/
	std::string trim_precision(float num, uint8_t precision);
	std::string trim_precision(double num, uint8_t precision);
}