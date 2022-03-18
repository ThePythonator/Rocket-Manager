#pragma once

// ProtoGame is to be used instead of GameStage.cpp/hpp
// This is just a test of features and ideas, will be moved to GameStage when ready


#include "PhysicsEngine.hpp"

#include "Stages.hpp"

#include "Camera.hpp"
#include "Rocket.hpp"
#include "Planet.hpp"
#include "Settings.hpp"
#include "StarField.hpp"
#include "Particles.hpp"

using namespace PhysicsEngine::phy_t;

class GameStage : public Framework::BaseStage {
public:
	GameStage();
	GameStage(std::string _save_name);

	void init();

	void start();
	void end();

	bool update(float dt);
	void render();

	void load_rocket(std::string rocket_name, uint8_t planet, uint8_t site);

private:
	void init_temporaries();

	void load_sandbox();
	void save_sandbox();

	void new_sandbox();
	void create_planets(const std::vector<Planet>& planets);

	//void create_components();

	void create_rocket(const Rocket& rocket);



	phyflt find_eccentricity(phyflt aphelion, phyflt perihelion);
	phyflt find_semimajor_axis(phyflt aphelion, phyflt perihelion);
	phyflt find_velocity(phyflt primary_mass, phyflt radius, phyflt semimajor_axis);
	phyflt volume_to_area_density(phyflt volume_density, phyflt radius);

	void render_planet(PhysicsEngine::RigidBody* planet, const Camera& camera, bool map);
	void render_component(PhysicsEngine::RigidBody* component, const Camera& camera, bool map);

	void render_atmosphere();
	void render_map();
	void render_debug();
	void render_sandbox();

	void update_temporaries(float dt);
	void update_game_state(float dt);

	void update_map(float dt);
	void update_sandbox(float dt);
	void update_physics(float dt);

	void toggle_map();

	std::vector<Framework::vec2> convert_poly_vertices(std::vector<phyvec> vertices, const phyvec& centre, const phymat& rotation_matrix, const Camera& camera);
	std::vector<Framework::vec2> convert_poly_vertices_retain_size(std::vector<phyvec> vertices, const phyvec& centre, const phymat& rotation_matrix, const Camera& camera);

	uint32_t get_next_rocket_index();

	PhysicsEngine::RigidBody* get_planet_from_rigidbodies(uint32_t id);

	phyvec get_launch_site_position(uint8_t planet, uint8_t site);

	bool command_module_is_connected(Rocket& rocket, uint32_t component_id);

	Component* breadth_first_component_search(Rocket& rocket, uint32_t start_id, uint32_t target_type);

	PhysicsEngine::PhysicsManager physics_manager;
	PhysicsEngine::PhysicsData physics_data;

	Camera map_camera, sandbox_camera;

	std::map<uint32_t, Rocket> rockets;

	Settings settings;

	StarField star_field;

	std::string save_name;

	Particles particles;

	// State variables
	struct {
		bool paused = false;
		bool show_map = false;
		uint8_t time_warp_index = 0;
	} game_state;

	// Temporaries
	struct {
		uint8_t nearest_planet = 0;
		phyflt distance_to_nearest_planet = 0.0f;
		phyvec nearest_planet_centre, nearest_planet_velocity;

		uint32_t current_rocket = GAME::SANDBOX::NO_ROCKET_SELECTED;
		phyvec cmd_mdl_centre, last_cmd_mdl_centre, cmd_mdl_velocity;
	} sandbox_temporaries;

	struct {
		// Used for controlling rockets
		int8_t direction = 0; // For now, just -1, 0, 1 (for left, none, right) turns
		float engine_power = 0.0f; // From 0 to 1?
	} rocket_controls;

	struct {
		float fps = 0.0f; // Used for debug info
	} debug_temporaries;
};


class PausedStage : public Framework::BaseStage {
public:
	PausedStage(GameStage* background_stage);

	void init();

	void start();

	bool update(float dt);
	void render();

	void load_rocket(std::string rocket_name, uint8_t planet, uint8_t site);

	GameStage* get_background_stage();

private:
	GameStage* _background_stage;
};

class LoadRocketStage : public Framework::BaseStage {
public:
	LoadRocketStage();
	LoadRocketStage(PausedStage* paused_stage);

	void init();

	void start();

	bool update(float dt);
	void render();

private:

	PausedStage* _paused_stage = nullptr;

	std::vector<std::string> rocket_names;
	uint8_t rocket_index = 0;
	uint8_t planet_index = 0;
	uint8_t site_index = 0;
};