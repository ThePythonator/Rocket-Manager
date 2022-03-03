#pragma once

#include <cmath>
#include <limits>
#include <vector>

#include "linalg.h"

namespace PhysicsEngine {
	typedef linalg::aliases::float2 fvec2;
	typedef linalg::aliases::double2 dvec2;
	//typedef linalg::aliases::float2x2 fmat22;
	typedef linalg::aliases::double2x2 dmat22;

	using linalg::mul;
	using linalg::length;
	using linalg::dot;
	using linalg::cross;
	using linalg::inverse;
	using linalg::identity;

	extern const double PI;
	extern const double G;
	// extern const float EPSILON;

	extern const dvec2 D_VEC_NULL;

	// Struct used by clip_edge_to_line
	// Keeps track of up to two points
	struct ClipResult {
		uint8_t points_count = 0;
		dvec2 points[2];
	};


	/*struct BoundingCircle {
		vec2 centre;
		float radius;
	};


	bool intersects(const BoundingCircle& a, const BoundingCircle& b);*/
	bool intersects(const dvec2& centre_a, double radius_a, const dvec2& centre_b, double radius_b);


	dvec2 to_dvec2(fvec2 v);
	fvec2 to_fvec2(dvec2 v);


	double length_squared(dvec2 v);
	dvec2 normalise(dvec2 v);

	// Convert to/from radians and degrees
	double deg_to_rad(double degrees);
	double rad_to_deg(double radians);

	// angle is in radians
	dmat22 rotation_matrix(double angle);

	// model_centre is the centre of the object in world space
	dvec2 to_model_space(dvec2 point, dvec2 model_centre, dmat22 rotation_matrix);
	dvec2 to_model_space(dvec2 point, dmat22 rotation_matrix);
	dvec2 to_world_space(dvec2 point, dvec2 model_centre, dmat22 rotation_matrix);
	dvec2 to_world_space(dvec2 point, dmat22 rotation_matrix);

	// Support point is the furthest point in a given direction
	dvec2 find_support_point(dvec2 direction, std::vector<dvec2>& points);

	// Get vector perpendicular to supplied vector
	dvec2 perpendicular_acw(dvec2 vector);
	dvec2 perpendicular_cw(dvec2 vector);

	// Clamp the point to the positive side of the line parallel to the normal, origin_distance from the origin (along the normal)
	// Origin_distance could also be thought as the closest point the line perpendicular to the normal passes to the origin
	ClipResult clip_edge_to_line(dvec2 edge[2], dvec2 line_normal, double origin_distance);


	//float gravitational_force(float mass_a, float mass_b, float distance_squared);
	
	// Use version with double behind the scene so that we don't overflow
	double gravitational_force(double mass_a, double mass_b, double distance_squared);
}