#pragma once

#include "PhysicsEngineMath.hpp"
#include "RigidBody.hpp"

namespace PhysicsEngine {
	// These constraints are treated as having no mass or volume, and cannot collide with any objects
	class Constraint {
	public:
		Constraint();
		Constraint(RigidBody* _a, RigidBody* _b, dvec2 _offset_a = D_VEC_NULL, dvec2 _offset_b = D_VEC_NULL);

		virtual dvec2 calculate_force() = 0;
		void apply_force();

		bool is_broken();

		RigidBody* a = nullptr;
		RigidBody* b = nullptr;
		dvec2 offset_a;
		dvec2 offset_b;

	protected:
		bool broken = false;
	};

	class Spring : public Constraint {
	public:
		Spring(RigidBody* _a, RigidBody* _b, dvec2 _offset_a = D_VEC_NULL, dvec2 _offset_b = D_VEC_NULL, double _natural_length = 1.0, double _modulus_of_elasticity = 1.0, double _max_length_factor = 2.0);
		dvec2 calculate_force();

	protected:
		const double natural_length = 1.0;
		const double modulus_of_elasticity = 1.0;
		const double max_length = 2.0;
	};

	class String : public Spring {
	public:
		String(RigidBody* _a, RigidBody* _b, dvec2 _offset_a = D_VEC_NULL, dvec2 _offset_b = D_VEC_NULL, double _natural_length = 1.0, double _modulus_of_elasticity = 1.0, double _max_length_factor = 2.0);
		dvec2 calculate_force();
	};
}