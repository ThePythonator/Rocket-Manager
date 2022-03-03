#pragma once

#include <vector>
#include <algorithm>

#include "PhysicsEngineMath.hpp"

namespace PhysicsEngine {

	class Shape {
	public:
		enum class ShapeType {
			CIRCLE,
			POLYGON,
			SHAPE_COUNT
		};

		Shape();

		virtual ShapeType get_type() = 0;
		virtual double get_area() = 0;
		virtual double get_moment_of_inertia(double density) = 0;
		virtual dvec2 get_centroid() = 0;
		virtual double get_bounding_radius() = 0;
	};

	class Circle : public Shape {
	public:
		Circle(double _radius = 1.0);
		
		ShapeType get_type();
		double get_area();
		double get_moment_of_inertia(double density);
		dvec2 get_centroid();
		double get_bounding_radius();

		double radius;
	};

	class Polygon : public Shape {
	public:
		Polygon(std::vector<dvec2> _vertices = { { 0.5, -0.5 }, { 0.5, 0.5 }, { -0.5, 0.5 }, { -0.5, -0.5 } });

		ShapeType get_type();
		double get_area();
		double get_moment_of_inertia(double density);
		dvec2 get_centroid();
		double get_bounding_radius();

		std::vector<dvec2> vertices;
		std::vector<dvec2> face_normals;

	private:
		void calculate_face_normals();
		void reorder_vertices();
	};

	Polygon* create_rect(dvec2 size);
}