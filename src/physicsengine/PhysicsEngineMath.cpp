#include "PhysicsEngineMath.hpp"

namespace PhysicsEngine {
	const double PI = 3.141592653589793;
	const double G = 6.674e-11;

	const dvec2 D_VEC_NULL{ 0.0, 0.0 };

	//const float EPSILON = 0.0001f; // todo: check if reasonable

	/*bool intersects(const BoundingCircle& a, const BoundingCircle& b) {
		float dist_squared = length_squared(a.centre - b.centre);

		float radii_sum = a.radius + b.radius;

		return dist_squared <= radii_sum * radii_sum;
	}*/

	bool intersects(const dvec2& centre_a, double radius_a, const dvec2& centre_b, double radius_b) {
		double dist_squared = length_squared(centre_a - centre_b);

		double radii_sum = radius_a + radius_b;

		return dist_squared <= radii_sum * radii_sum;
	}


	dvec2 to_dvec2(fvec2 v) {
		return dvec2{ v.x, v.y };
	}


	fvec2 to_fvec2(dvec2 v) {
		return fvec2{ static_cast<float>(v.x), static_cast<float>(v.y) };
	}



	double length_squared(dvec2 v) {
		return linalg::length2(v);
	}

	dvec2 normalise(dvec2 v) {
		return linalg::normalize(v);
	}

	double deg_to_rad(double degrees) {
		return PI * degrees / 180.0;
	}
	double rad_to_deg(double radians) {
		return 180.0 * radians / PI;
	}

	dmat22 rotation_matrix(double angle) {
		// Anticlockwise rotation
		double s = std::sin(angle);
		double c = std::cos(angle);
		return dmat22{ {c, s}, {-s, c} };
	}


	// Convert world space to model space (in model space, shape is as if angle = 0)
	dvec2 to_model_space(dvec2 point, dvec2 model_centre, dmat22 rotation_matrix) {
		return mul(inverse(rotation_matrix), point - model_centre);
	}

	dvec2 to_model_space(dvec2 point, dmat22 rotation_matrix) {
		return mul(inverse(rotation_matrix), point);
	}

	// Convert model space to world space (in world space, shape is at actual angle)
	dvec2 to_world_space(dvec2 point, dvec2 model_centre, dmat22 rotation_matrix) {
		return mul(rotation_matrix, point) + model_centre;
	}

	dvec2 to_world_space(dvec2 point, dmat22 rotation_matrix) {
		return mul(rotation_matrix, point);
	}


	dvec2 find_support_point(dvec2 direction, std::vector<dvec2>& points) {
		double maximum_distance = -DBL_MAX;
		uint16_t point_index = 0;

		for (uint16_t i = 0; i < points.size(); i++) {
			double distance = dot(direction, points[i]);

			if (distance > maximum_distance) {
				maximum_distance = distance;
				point_index = i;
			}
		}

		return points[point_index];
	}

	dvec2 perpendicular_acw(dvec2 vector) {
		return dvec2{ -vector.y, vector.x };
	}

	dvec2 perpendicular_cw(dvec2 vector) {
		return dvec2{ vector.y, -vector.x };
	}

	ClipResult clip_edge_to_line(dvec2 edge[2], dvec2 line_normal, double origin_distance) {
		ClipResult result;

		// Calculate signed distance along normal from line
		double signed_distance[2] = {
			dot(edge[0], line_normal) - origin_distance,
			dot(edge[1], line_normal) - origin_distance
		};

		if (signed_distance[0] <= 0.0) {
			result.points[result.points_count++] = edge[0];
		}

		if (signed_distance[1] <= 0.0) {
			result.points[result.points_count++] = edge[1];
		}

		// Is the sign of the distances different?
		// We also check that points_count is < 2 so that we don't accidentally add a third point (although it should be impossible)
		if (signed_distance[0] * signed_distance[1] < 0.0 && result.points_count < 2) {
			// Edge passes through the clamping line
			// One of the distances was negative, so we need to add the intersection point

			// Calculate intersection
			double ratio = signed_distance[0] / (signed_distance[0] + signed_distance[1]);
			result.points[result.points_count++] = edge[0] + ratio * (edge[1] - edge[0]);
		}

		return result;
	}

	/*float gravitational_force(float mass_a, float mass_b, float distance_squared) {
		return G * mass_a * mass_b / distance_squared;
	}*/

	double gravitational_force(double mass_a, double mass_b, double distance_squared) {
		return G * mass_a * mass_b / distance_squared;
	}
}