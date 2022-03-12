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

using namespace PhysicsEngine::phy_t;

class GameStage : public Framework::BaseStage {
public:
	void init();

	void start();
	void end();

	bool update(float dt);
	void render();

private:
	void init_temporaries();

	void load_settings();
	void save_settings();

	void load_sandbox(std::string filename);
	void save_sandbox(std::string filename);

	void create_solar_system();
	void create_planets(const std::vector<Planet>& planets);

	void create_components();
	void create_rocket(const Rocket& rocket);
	//void create_rocket(uint32_t rocket_id);

	/*std::vector<PhysicsEngine::RigidBody> create_rocket_rigidbodies(const Rocket& rocket, const phyvec& offset = {});
	std::vector<PhysicsEngine::Constraint*> create_rocket_constraints(const Rocket& rocket);*/

	phyflt find_eccentricity(phyflt aphelion, phyflt perihelion);
	phyflt find_semimajor_axis(phyflt aphelion, phyflt perihelion);
	phyflt find_velocity(phyflt primary_mass, phyflt radius, phyflt semimajor_axis);
	phyflt volume_to_area_density(phyflt volume_density, phyflt radius);

	//void render_physics_objects();
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

	PhysicsEngine::PhysicsManager physics_manager;
	PhysicsEngine::PhysicsData physics_data;

	Camera map_camera, sandbox_camera;

	std::map<uint32_t, Rocket> rockets;

	// Images for all physics objects??
	//std::vector<Framework::BaseSprite*> planets;
	//std::vector<Framework::Image> components; // do I need this, or can it be stored in game_objects???

	// State variables
	struct {
		bool paused = true;
		bool show_map = false;
		uint8_t time_warp_index = 0;
	} game_state;

	// Temporaries
	struct {
		uint8_t nearest_planet = 0;
		phyflt distance_to_nearest_planet = 0.0f;
		phyvec nearest_planet_centre, nearest_planet_velocity;

		uint32_t current_rocket = 0;
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

	Settings settings;

	StarField star_field;
};