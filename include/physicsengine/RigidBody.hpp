#pragma once

#include "Shape.hpp"

// Maybe rename file
#include "GraphicsData.hpp"

namespace PhysicsEngine {
	// Information about a material
	struct Material {
		Material(double _static_friction = 0.5, double _dynamic_friction = 0.3, double _restitution = 0.5, double _density = 1.0);

		double static_friction, dynamic_friction;
		double restitution;
		double density;
	};

	// Structure containing information about a body
	class RigidBody {
	public:
		RigidBody();
		RigidBody(Shape* _shape, Material* _material, dvec2 _centre, double _angle = 0.0, bool infinite_mass = false);

		void set_layers(uint32_t layers);
		uint32_t get_layers();

		void apply_force(const dvec2& _force, const dvec2& vector_to_contact = dvec2{ 0.0, 0.0 });

		dmat22 get_rotation_matrix();

		Shape* shape = nullptr;
		Material* material = nullptr;

		double bounding_radius = 0.0;

		dvec2 centre, velocity, force;

		// angle is in radians
		double angle = 0.0;
		double angular_velocity = 0.0;
		double torque = 0.0;

		double mass = 0.0;
		double inverse_mass = 0.0;

		double moment_of_inertia = 0.0;
		double inverse_moment_of_inertia = 0.0;

		// Render ID can be used by other code to look up what objects and corresponding offsets to render at
		uint16_t render_id = 0;
		// Category ID can be used to group certain rigidbodies when searching
		uint16_t category_id = 0;

	private:
		// Defaults to do nothing (i.e. angle = 0.0)
		dmat22 cached_rotation_matrix = identity;

		double last_angle = 0.0;

		uint32_t _layers = 1; // Each bit corresponds to a layer
	};
}