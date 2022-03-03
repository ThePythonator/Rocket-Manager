#pragma once

// ProtoGame is to be used instead of GameStage.cpp/hpp
// This is just a test of features and ideas, will be moved to GameStage when ready


#include "Stages.hpp"

#include "Camera.hpp"

#include "PhysicsEngine.hpp"

class GameStage : public Framework::BaseStage {
public:
	//~GameStage();

	void init();

	void start();

	bool update(float dt);
	void render();

private:
	void create_solar_system(float scale = 1.0f);
	void create_components();

	float find_eccentricity(float aphelion, float perihelion);
	float find_semimajor_axis(float aphelion, float perihelion);
	float find_velocity(float primary_mass, float radius, float semimajor_axis);
	float volume_to_area_density(float volume_density, float radius);

	//void render_physics_objects();
	void render_planet(const PhysicsEngine::RigidBody& planet, const Camera& camera, bool map);
	void render_component(const PhysicsEngine::RigidBody& component, const Camera& camera, bool map);

	void render_map();
	void render_debug();
	void render_sandbox();

	void update_map(float dt);
	void update_sandbox_scale(float dt);
	void update_physics(float dt);

	void toggle_map();

	PhysicsEngine::PhysicsManager physics_manager;
	PhysicsEngine::PhysicsData physics_data;

	Camera map_camera, sandbox_camera;

	// Images for all physics objects??
	//std::vector<Framework::BaseSprite*> planets;
	//std::vector<Framework::Image> components; // do I need this, or can it be stored in game_objects???

	// State variables
	bool paused = true;
	bool show_map = false;
	uint8_t time_warp_index = 0;

	float fps = 0.0f; // Used for debug info
};