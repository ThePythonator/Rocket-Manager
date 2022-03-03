#include "PhysicsEngineMath.hpp"

namespace PhysicsEngine {
	const float PI = 3.14159265f;
	const float G = 6.674e-11f;
	//const float EPSILON = 0.0001f; // todo: check if reasonable

	/*bool intersects(const BoundingCircle& a, const BoundingCircle& b) {
		float dist_squared = length_squared(a.centre - b.centre);

		float radii_sum = a.radius + b.radius;

		return dist_squared <= radii_sum * radii_sum;
	}*/

	bool intersects(const vec2& centre_a, float radius_a, const vec2& centre_b, float radius_b) {
		float dist_squared = length_squared(centre_a - centre_b);

		float radii_sum = radius_a + radius_b;

		return dist_squared <= radii_sum * radii_sum;
	}


	float length_squared(vec2 v) {
		return linalg::length2(v);
	}

	vec2 normalise(vec2 v) {
		return linalg::normalize(v);
	}

	float deg_to_rad(float degrees) {
		return PI * degrees / 180.0f;
	}
	float rad_to_deg(float radians) {
		return 180.0f * radians / PI;
	}

	mat22 rotation_matrix(float angle) {
		// Anticlockwise rotation
		float s = std::sin(angle);
		float c = std::cos(angle);
		return mat22{ {c, s}, {-s, c} };
	}


	// Convert world space to model space (in model space, shape is as if angle = 0)
	vec2 to_model_space(vec2 point, vec2 model_centre, mat22 rotation_matrix) {
		return mul(inverse(rotation_matrix), point - model_centre);
	}

	vec2 to_model_space(vec2 point, mat22 rotation_matrix) {
		return mul(inverse(rotation_matrix), point);
	}

	// Convert model space to world space (in world space, shape is at actual angle)
	vec2 to_world_space(vec2 point, vec2 model_centre, mat22 rotation_matrix) {
		return mul(rotation_matrix, point) + model_centre;
	}

	vec2 to_world_space(vec2 point, mat22 rotation_matrix) {
		return mul(rotation_matrix, point);
	}


	vec2 find_support_point(vec2 direction, std::vector<vec2>& points) {
		float maximum_distance = -FLT_MAX;
		uint16_t point_index = 0;

		for (uint16_t i = 0; i < points.size(); i++) {
			float distance = dot(direction, points[i]);

			if (distance > maximum_distance) {
				maximum_distance = distance;
				point_index = i;
			}
		}

		return points[point_index];
	}

	vec2 perpendicular_acw(vec2 vector) {
		return vec2{ -vector.y, vector.x };
	}

	vec2 perpendicular_cw(vec2 vector) {
		return vec2{ vector.y, -vector.x };
	}

	ClipResult clip_edge_to_line(vec2 edge[2], vec2 line_normal, float origin_distance) {
		ClipResult result;

		// Calculate signed distance along normal from line
		float signed_distance[2] = {
			dot(edge[0], line_normal) - origin_distance,
			dot(edge[1], line_normal) - origin_distance
		};

		if (signed_distance[0] <= 0.0f) {
			result.points[result.points_count++] = edge[0];
		}

		if (signed_distance[1] <= 0.0f) {
			result.points[result.points_count++] = edge[1];
		}

		// Is the sign of the distances different?
		// We also check that points_count is < 2 so that we don't accidentally add a third point (although it should be impossible)
		if (signed_distance[0] * signed_distance[1] < 0.0f && result.points_count < 2) {
			// Edge passes through the clamping line
			// One of the distances was negative, so we need to add the intersection point

			// Calculate intersection
			float ratio = signed_distance[0] / (signed_distance[0] + signed_distance[1]);
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