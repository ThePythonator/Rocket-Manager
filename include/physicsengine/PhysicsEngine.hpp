#pragma once

#include <vector>

#include "Collisions.hpp"
#include "Constraints.hpp"

namespace PhysicsEngine {
	// This struct can be used by the user to store the pointers.
	struct PhysicsData {
		PhysicsData();
		~PhysicsData();

		std::vector<Shape*> shapes;
		std::vector<Constraint*> constraints;
		std::vector<Material*> materials;
	};

	class PhysicsManager {
	public:
		struct Constants {
			// Number of iterations shouldn't really be used (i.e. keep as 1) since it doesn't recalculate collision data
			uint8_t iterations = 1; // increasing this may or may not make the sim work better
			// Bias factor scales the extra impulse used for positional correction. Don't set it much higher than 0.1, otherwise too much kinetic energy is added to the system
			double bias_factor = 0.1;
			// The amount of penetration we allow before applying the positional correction impulse
			double penetration_slop = 0.005;
		};

		PhysicsManager();
		void set_constants(Constants _constants);

		void update(double dt);
		void step(double dt);

		// Returns the index of the item in the vector
		uint16_t add_body(RigidBody body);
		uint16_t add_constraint(Constraint* constraint);
		//void clear_bodies();

		std::vector<RigidBody>& get_bodies();
		std::vector<Constraint*>& get_constraints();

	private:
		void update_velocity(RigidBody& body, double dt);
		void update_position(RigidBody& body, double dt);

		void add_impulse(RigidBody& body, const dvec2& impulse, const dvec2& vector_to_contact);

		void update_forces();
		void update_constraints();
		void update_velocities(double dt);
		void update_positions(double dt);
		void handle_collisions(double dt);

		CollisionInformation detect_collision(RigidBody& a, RigidBody& b);
		void resolve_collision(RigidBody& a, RigidBody& b, const CollisionInformation& collision_information, double dt);

		struct CollisionPacket {
			CollisionInformation collision_information;
			// Index of each body in bodies
			uint16_t index_a, index_b;
		};

		std::vector<RigidBody> bodies;
		std::vector<Constraint*> constraints;

		Constants constants;
	};
}