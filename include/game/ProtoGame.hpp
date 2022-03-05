#pragma once

// ProtoGame is to be used instead of GameStage.cpp/hpp
// This is just a test of features and ideas, will be moved to GameStage when ready


#include "Stages.hpp"

#include "Camera.hpp"

#include "PhysicsEngine.hpp"

#include "Rocket.hpp"

using namespace PhysicsEngine::phy_t;

class GameStage : public Framework::BaseStage {
public:
	//~GameStage();

	void init();

	void start();

	bool update(float dt);
	void render();

private:
	void create_solar_system();
	void create_components();

	std::vector<PhysicsEngine::RigidBody> create_rocket_rigidbodies(const Rocket& rocket);
	std::vector<PhysicsEngine::Constraint> create_rocket_constraints(const Rocket& rocket);

	phyflt find_eccentricity(phyflt aphelion, phyflt perihelion);
	phyflt find_semimajor_axis(phyflt aphelion, phyflt perihelion);
	phyflt find_velocity(phyflt primary_mass, phyflt radius, phyflt semimajor_axis);
	phyflt volume_to_area_density(phyflt volume_density, phyflt radius);

	//void render_physics_objects();
	void render_planet(const PhysicsEngine::RigidBody& planet, const Camera& camera, bool map);
	void render_component(const PhysicsEngine::RigidBody& component, const Camera& camera, bool map);

	void render_map();
	void render_debug();
	void render_sandbox();

	void update_map(float dt);
	void update_sandbox(float dt);
	void update_physics(float dt);

	void toggle_map();

	std::vector<Framework::vec2> convert_poly_vertices(std::vector<phyvec> vertices, const phyvec& centre, const phymat& rotation_matrix, const Camera& camera);
	std::vector<Framework::vec2> convert_poly_vertices_retain_size(std::vector<phyvec> vertices, const phyvec& centre, const phymat& rotation_matrix, const Camera& camera);
	//std::vector<Framework::vec2> convert_poly_vertices(std::vector<phyvec> vertices, const phyvec& centre, const phymat& rotation_matrix, const phyvec& camera_position, phyflt camera_scale);

	PhysicsEngine::PhysicsManager physics_manager;
	PhysicsEngine::PhysicsData physics_data;

	Camera map_camera, sandbox_camera;

	std::map<uint32_t, Rocket> rockets;

	// Images for all physics objects??
	//std::vector<Framework::BaseSprite*> planets;
	//std::vector<Framework::Image> components; // do I need this, or can it be stored in game_objects???

	// State variables
	bool paused = true;
	bool show_map = false;
	uint8_t time_warp_index = 0;

	uint8_t nearest_planet = 0;

	float fps = 0.0f; // Used for debug info
};