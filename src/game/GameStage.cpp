#include "GameStage.hpp"

// GameStage


GameStage::GameStage() {

}
GameStage::GameStage(std::string _save_name) {
	save_name = _save_name;
}

void GameStage::init() {
	if (!game_state.paused) {
		map_camera = Camera(WINDOW::SIZE);
		sandbox_camera = Camera(WINDOW::SIZE);

		particles = Particles(graphics_objects->graphics_ptr);

		PhysicsEngine::PhysicsManager::Constants constants = physics_manager.get_constants();
		constants.gravitational_constant = GAME::SANDBOX::GRAVITATIONAL_CONSTANT;
		physics_manager.set_constants(constants);

		// Create star field
		star_field = StarField(graphics_objects->graphics_ptr, WINDOW::SIZE);

		// Set camera scales
		map_camera.set_scale(GAME::MAP::UI::DEFAULT_CAMERA_SCALE);
		sandbox_camera.set_scale(GAME::SANDBOX::UI::DEFAULT_CAMERA_SCALE);

		// Set transition
		set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
	}
}

void GameStage::start() {
	if (game_state.paused) {
		transition->set_open();
	}
	else {
		settings = load_settings();
		load_sandbox();

		init_temporaries();

		// Open transition
		transition->open();
	}

	game_state.paused = false;
}

void GameStage::end() {
	//save_settings();
	save_sandbox();
}

bool GameStage::update(float dt) {

	// Handle inputs etc
	update_game_state(dt);

	if (!game_state.paused) {
		transition->update(dt);

		// Note: slight issue where sun actually does slowly move
		// This seems to cause other orbit to go crazy after a little bit too
		update_physics(dt);

		// Temporary positions etc
		update_temporaries(dt);
	}


	update_map(dt);
	update_sandbox(dt);

	return true;
}

void GameStage::render() {
	// Render background
	graphics_objects->graphics_ptr->fill(COLOURS::BLACK);

	if (!game_state.show_map) render_sandbox();

	render_map();

	if (settings.show_debug) render_debug();
	
	render_hud();

	transition->render();
}


void GameStage::load_rocket(std::string rocket_name, uint8_t planet, uint8_t site) {
	printf("Loading rocket: %s on planet %s at site %u...\n", rocket_name.c_str(), STRINGS::GAME::PLANET_NAMES[planet].c_str(), site);

	Rocket rocket = Framework::JSONHandler::read(PATHS::BASE_PATH + PATHS::ROCKET_TEMPLATES::LOCATION + rocket_name + PATHS::ROCKET_TEMPLATES::EXTENSION);

	rocket.set_is_template(false);

	PhysicsEngine::RigidBody* planet_ptr = get_planet_from_rigidbodies(planet);

	phyvec position = get_launch_site_position(planet, site);
	phyvec velocity = planet_ptr->velocity;
	phyflt angle = GAME::SANDBOX::LAUNCH_SITES::SITES[planet][site];

	phyvec normalised_planet_to_me = PhysicsEngine::normalise(position - planet_ptr->centre);

	// Get a rough idea of the closest component to the planet
	phyvec closest_component_centre_to_planet; // 0, 0 : CMD module
	Framework::Rect component_size = find_bounding_rect(GAME::COMPONENTS::VERTICES[GAME::COMPONENTS::COMPONENT_TYPE::COMMAND_MODULE]);

	for (const std::pair<uint32_t, Component>& p : rocket.get_components()) {
		if (p.second.get_offset().y > closest_component_centre_to_planet.y) {
			closest_component_centre_to_planet = p.second.get_offset();
			component_size = find_bounding_rect(GAME::COMPONENTS::VERTICES[p.second.get_type()]);
		}
	}

	position += normalised_planet_to_me * (closest_component_centre_to_planet + phyvec{ 0.0f, component_size.size.y / 2 });
	phyvec a = position - get_launch_site_position(planet, site);

	Rocket::InitialData initial_data{ position, velocity, angle };
	
	rocket.set_initial_data(initial_data);

	create_rocket(rocket);
}



void GameStage::init_temporaries() {
	update_temporaries(WINDOW::TARGET_DT);

	sandbox_temporaries.last_cmd_mdl_centre = sandbox_temporaries.cmd_mdl_centre;

	// Set map camera to be centred on current rocket
	map_camera.set_position(sandbox_temporaries.cmd_mdl_centre);
}



void GameStage::load_sandbox() {
	try {
		json data = Framework::JSONHandler::read(PATHS::BASE_PATH + PATHS::SANDBOX_SAVES::LOCATION + save_name + PATHS::SANDBOX_SAVES::EXTENSION);

		// Get planets
		std::vector<Planet> planets;
		data.at("planets").get_to(planets);

		// Get rockets
		for (const std::pair<uint32_t, Rocket>& r : data.at("rockets").get<std::map<uint32_t, Rocket>>()) {
			create_rocket(r.second);
		}

		create_planets(planets);

		data.at("current_rocket").get_to(sandbox_temporaries.current_rocket);

	}
	catch (const json::exception& e) {
		// Ignore - we set defaults already so it's not the end of the world
		printf("Couldn't load save file: %s. Error: %s\n", save_name.c_str(), e.what());

		printf("Creating new sandbox instead...\n");

		new_sandbox();

		// For now, create basic rocket (handle loading later)
		//create_components();
	}
}

void GameStage::save_sandbox() {
	std::vector<Planet> planets;

	for (PhysicsEngine::RigidBody* body : physics_manager.get_bodies()) {
		switch (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY]) {
		case GAME::SANDBOX::CATEGORIES::PLANET:
			planets.push_back({body->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE], body->centre, body->velocity});
			break;

		case GAME::SANDBOX::CATEGORIES::COMPONENT:
			if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] == GAME::COMPONENTS::COMPONENT_TYPE::COMMAND_MODULE) {
				rockets[body->ids[GAME::SANDBOX::RIGID_BODY_IDS::OBJECT]].set_initial_data({body->centre, body->velocity, body->angle});
			}
			break;

		default:
			break;
		}
	}
	
	json data = json{
		{"planets", planets},
		{"rockets", rockets},
		{"current_rocket", sandbox_temporaries.current_rocket}
	};

	Framework::JSONHandler::write(PATHS::BASE_PATH + PATHS::SANDBOX_SAVES::LOCATION + save_name + PATHS::SANDBOX_SAVES::EXTENSION, data);
}

void GameStage::new_sandbox() {
	rockets.clear();
	physics_manager.clear();

	for (uint8_t i = 0; i < GAME::SANDBOX::BODIES::ID::TOTAL; i++) {
		phyflt radius = GAME::SANDBOX::BODIES::RADII[i];

		PhysicsEngine::Circle* circle_ptr = new PhysicsEngine::Circle(radius);
		physics_data.shapes.push_back(circle_ptr);

		phyflt area_density = volume_to_area_density(GAME::SANDBOX::BODIES::VOLUME_DENSITIES[i], radius);

		// Add material_ptr to physics_data since it's not a heap allocated ptr
		PhysicsEngine::Material* material_ptr = new PhysicsEngine::Material(0.7f, 0.5f, 0.0f, area_density); // todo: get friction and restitution?
		physics_data.materials.push_back(material_ptr);

		phyflt apoapsis = GAME::SANDBOX::BODIES::APOAPSIS_DISTANCES[i];
		phyflt periapsis = GAME::SANDBOX::BODIES::PERIAPSIS_DISTANCES[i];
		phyflt semimajor_axis = find_semimajor_axis(apoapsis, periapsis);

		// Position of body in sandbox
		PhysicsEngine::phyvec parent_position, parent_velocity;
		PhysicsEngine::phyflt parent_mass = 0.0f;
		if (GAME::SANDBOX::BODIES::PARENT_BODIES[i] != GAME::SANDBOX::BODIES::ID::NONE) {
			PhysicsEngine::RigidBody* parent_ptr = get_planet_from_rigidbodies(GAME::SANDBOX::BODIES::PARENT_BODIES[i]);
			parent_position = parent_ptr->centre;
			parent_velocity = parent_ptr->velocity;
			parent_mass = parent_ptr->mass;
		}

		PhysicsEngine::phyvec position = parent_position + PhysicsEngine::phyvec{ apoapsis, 0.0f };

		PhysicsEngine::phyvec me_to_parent = parent_position - position;

		PhysicsEngine::phyvec velocity_direction = PhysicsEngine::normalise(PhysicsEngine::perpendicular_acw(me_to_parent));

		PhysicsEngine::RigidBody* object_ptr = new PhysicsEngine::RigidBody(circle_ptr, material_ptr, position);
		physics_data.bodies.push_back(object_ptr);

		object_ptr->ids.assign(GAME::SANDBOX::RIGID_BODY_IDS::TOTAL, 0);
		
		phyflt speed = find_velocity(parent_mass, PhysicsEngine::length(me_to_parent), semimajor_axis);

		if (i != GAME::SANDBOX::BODIES::ID::SUN) {
			object_ptr->velocity = speed * velocity_direction + parent_velocity;
		}

		// Set planet ID
		object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] = i;

		// Set category ID
		object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] = GAME::SANDBOX::CATEGORIES::PLANET;

		//object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::GROUP] = idk;
		
		//object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::OBJECT] = i; //?

		physics_manager.add_body(object_ptr);
	}
}


void GameStage::create_planets(const std::vector<Planet>& planets) {
	for (const Planet& planet : planets) {
		phyflt radius = GAME::SANDBOX::BODIES::RADII[planet.id];

		PhysicsEngine::Circle* circle_ptr = new PhysicsEngine::Circle(radius);
		physics_data.shapes.push_back(circle_ptr);

		phyflt area_density = volume_to_area_density(GAME::SANDBOX::BODIES::VOLUME_DENSITIES[planet.id], radius);

		// Add material_ptr to physics_data since it's not a heap allocated ptr
		PhysicsEngine::Material* material_ptr = new PhysicsEngine::Material(0.7f, 0.5f, 0.0f, area_density); // todo: get friction and restitution?
		physics_data.materials.push_back(material_ptr);

		PhysicsEngine::RigidBody* object_ptr = new PhysicsEngine::RigidBody(circle_ptr, material_ptr, planet.position);
		physics_data.bodies.push_back(object_ptr);

		object_ptr->ids.assign(GAME::SANDBOX::RIGID_BODY_IDS::TOTAL, 0);

		object_ptr->velocity = planet.velocity;

		// Set planet ID
		object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] = planet.id;

		// Set category ID
		object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] = GAME::SANDBOX::CATEGORIES::PLANET;

		physics_manager.add_body(object_ptr);
	}
}

// TEST
/*void GameStage::create_components() {

	// Test component
	//PhysicsEngine::Polygon* poly_ptr = new PhysicsEngine::Polygon(GAME::COMPONENTS::VERTICES[0]); // just a test
	//physics_data.shapes.push_back(poly_ptr);

	//phyvec position = sandbox_camera.get_position() - phyvec{ 0.0f, 24.0f }; // testing
	//phyvec position = phyvec{0, -GAME::SANDBOX::BODIES::RADII[3]} - phyvec{ 0.0f, 25.0f };
	phyvec position = physics_manager.get_bodies()[3]->centre + phyvec{ 0, -GAME::SANDBOX::BODIES::RADII[3] } - phyvec{ 0.0f, 24.5f };

	//PhysicsEngine::Material* material_ptr = &GAME::SANDBOX::DEFAULT_MATERIALS::MATERIALS[; // TODO
	//physics_data.materials.push_back(material_ptr);

	//PhysicsEngine::RigidBody object = PhysicsEngine::RigidBody(poly_ptr, material_ptr, position);

	//object.ids.assign(GAME::SANDBOX::RIGID_BODY_IDS::TOTAL, 0);


	//object.ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] = Component::ComponentType::COMMAND_MODULE; // test

	// Set category ID
	//object.ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] = GAME::SANDBOX::CATEGORIES::COMPONENT;

	// Keep moving at earth speed
	//object.velocity = physics_manager.get_bodies()[3].velocity; // messy, TODO // FIXME
	//object.velocity = physics_manager.get_bodies()[0].velocity;

	//physics_manager.add_body(object);


	// Test rocket

	Rocket r;

	Component c = Component(GAME::COMPONENTS::COMPONENT_TYPE::COMMAND_MODULE);
	c.set_offset({ 0, 0 });
	r.add_component(c);

	c = Component(GAME::COMPONENTS::COMPONENT_TYPE::FUEL_TANK);
	c.set_offset({ 0, 11.2 });
	r.add_component(c);

	c = Component(GAME::COMPONENTS::COMPONENT_TYPE::ENGINE);
	c.set_offset({ 0, 23 });
	r.add_component(c);

	Connection conn{ ComponentNode{0, 1}, ComponentNode{1, 0} };
	r.add_connection(conn);

	conn = { ComponentNode{1, 1}, ComponentNode{2, 0} };
	r.add_connection(conn);

	conn = { ComponentNode{1, 2}, ComponentNode{2, 1} };
	r.add_connection(conn);

	r.set_name("Test Rocket");

	r.set_initial_data({ position, physics_manager.get_bodies()[3]->velocity, 0.0f });

	//r.set_name("Other");

	r.set_is_template(false);

	create_rocket(r);
	//create_rocket(1, object.centre + phyvec{20, 0});
}*/

// Creates RigidBodies and Constraints, using the positions stored in the Rocket instance identified by rocket_id, offsetting by the supplied offset
void GameStage::create_rocket(const Rocket& rocket) {
	uint32_t rocket_id = get_next_rocket_index();

	// Set current rocket
	sandbox_temporaries.current_rocket = rocket_id;

	// Set throttle to 0, direction to 0
	rocket_controls.direction = 0;
	rocket_controls.engine_power = 0.0f;

	// Add rocket to rockets
	rockets[rocket_id] = rocket;


	Rocket::InitialData initial_data = rocket.get_initial_data();

	// Create the physics RigidBodies to be added to the engine
	for (const std::pair<uint32_t, Component>& p : rocket.get_components()) {
		uint32_t component_id = p.first;
		const Component& component = p.second;

		uint32_t component_type = component.get_type();

		PhysicsEngine::Shape* shape_ptr = new PhysicsEngine::Polygon(GAME::COMPONENTS::VERTICES[component_type]);
		physics_data.shapes.push_back(shape_ptr);

		// Don't add material_ptr to physics_data since it's not a heap allocated ptr
		PhysicsEngine::Material* material_ptr = &GAME::SANDBOX::DEFAULT_MATERIALS::MATERIALS[GAME::COMPONENTS::MATERIALS[component_type]];

		// TODO: change so that angle isn't necessarily just same as CMD MODULE?
		PhysicsEngine::RigidBody* object_ptr = new PhysicsEngine::RigidBody(shape_ptr, material_ptr, PhysicsEngine::mul(PhysicsEngine::rotation_matrix(initial_data.angle), component.get_offset() + GAME::COMPONENTS::CENTROIDS[component_type]) + initial_data.position, initial_data.angle);
		physics_data.bodies.push_back(object_ptr);

		object_ptr->ids.assign(GAME::SANDBOX::RIGID_BODY_IDS::TOTAL, 0); // Set size of vector

		object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] = GAME::SANDBOX::CATEGORIES::COMPONENT; // It's a component
		object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::GROUP] = rocket_id; // Which rocket is it part of
		object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] = component_type; // What type of component is it
		object_ptr->ids[GAME::SANDBOX::RIGID_BODY_IDS::OBJECT] = component_id; // Component id is used to uniquely identify a component as part of a specific rocket

		// Set rocket's velocity
		// If loading from a save, initial_velocity is stored in the save data.
		// If loading from a rocket file (i.e. player just placed a new rocket in the world), then initial_velocity is set to the launch planet's current velocity.
		object_ptr->velocity = initial_data.velocity;

		physics_manager.add_body(object_ptr);
	}


	// Create constraints

	uint32_t connection_id = 0;
	for (const Connection& c : rocket.get_connections()) {
		PhysicsEngine::RigidBody* a = nullptr;
		PhysicsEngine::RigidBody* b = nullptr;

		// Find pointers to the right bodies
		for (PhysicsEngine::RigidBody* body : physics_manager.get_bodies()) {
			// Is it a component?
			if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] == GAME::SANDBOX::CATEGORIES::COMPONENT) {
				// Is it part of this rocket?
				if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::GROUP] == rocket_id) {
					// Do the component ids match? If so, set the ptr
					if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::OBJECT] == c.a.component_id) {
						a = body;
					}
					else if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::OBJECT] == c.b.component_id) {
						b = body;
					}
				}
			}
		}

		// Check that we actually found the bodies
		if (a && b) {
			// Natural length MUST be > 0
			std::map<uint32_t, Component> components = rockets[rocket_id].get_components();

			phyvec offset_a = GAME::COMPONENTS::NODE_POSITIONS[components[c.a.component_id].get_type()][c.a.node_id];
			phyvec offset_b = GAME::COMPONENTS::NODE_POSITIONS[components[c.b.component_id].get_type()][c.b.node_id];

			PhysicsEngine::Constraint* constraint_ptr = new PhysicsEngine::Spring(a, b, offset_a, offset_b, 0.01f, GAME::SANDBOX::CONNECTIONS::MODULUS_OF_ELASTICITY, GAME::SANDBOX::CONNECTIONS::MAX_EXTENSION);

			constraint_ptr->ids.assign(GAME::SANDBOX::CONSTRAINT_IDS::TOTAL, 0); // Set size of vector

			constraint_ptr->ids[GAME::SANDBOX::CONSTRAINT_IDS::ROCKET] = rocket_id; // Which rocket is it part of
			constraint_ptr->ids[GAME::SANDBOX::CONSTRAINT_IDS::CONSTRAINT] = connection_id; // What constraint id

			physics_data.constraints.push_back(constraint_ptr);

			physics_manager.add_constraint(constraint_ptr);
		}

		connection_id++;
	}
}


phyflt GameStage::find_eccentricity(phyflt aphelion, phyflt perihelion) {
	return (aphelion - perihelion) / (aphelion + perihelion);
}

phyflt GameStage::find_semimajor_axis(phyflt aphelion, phyflt perihelion) {
	return (aphelion + perihelion) / 2.0f;
}

phyflt GameStage::find_velocity(phyflt primary_mass, phyflt radius, phyflt semimajor_axis) {
	return std::sqrt(primary_mass * GAME::SANDBOX::GRAVITATIONAL_CONSTANT * (2.0f / radius - 1.0f / semimajor_axis));
}

phyflt GameStage::volume_to_area_density(phyflt volume_density, phyflt radius) {
	return volume_density * 4.0f * radius / 3.0f;
}


void GameStage::render_planet(PhysicsEngine::RigidBody* planet, const Camera& camera, bool map) {
	PhysicsEngine::Circle* circle_ptr = static_cast<PhysicsEngine::Circle*>(planet->shape);
	// For now assume circle
	phyflt radius = circle_ptr->radius * camera.get_scale();
	phyvec position = camera.get_render_position(planet->centre);

	float f_radius = PhysicsEngine::to_float(radius);
	Framework::vec2 f_position = PhysicsEngine::to_fvec(position);

	// Get bounding rect to check if on screen
	Framework::Rect bounding_rect = Framework::SDLUtils::get_bounds(f_position, f_radius);
	Framework::Rect screen_rect = Framework::Rect(Framework::VEC_NULL, WINDOW::SIZE);

	// Check if on screen
	if (Framework::colliding(bounding_rect, screen_rect)) {
		// Render planet
		
		// If map is set, draw normal circle, otherwise draw arc in window
		if (map) {
			graphics_objects->graphics_ptr->render_circle(f_position, f_radius, COLOURS::PLANETS[planet->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE]]);

			graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::GAME::PLANET_NAMES[planet->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE]], f_position, COLOURS::WHITE, FONTS::SCALE::MAP_OBJECT_FONT);
		}
		else {
			Framework::SDLUtils::SDL_RenderDrawArcInWindow(graphics_objects->graphics_ptr->get_renderer(), f_position, f_radius, WINDOW::SIZE, COLOURS::PLANETS[planet->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE]]);
		}
	}
}

void GameStage::render_component(PhysicsEngine::RigidBody* component, const Camera& camera, bool map) {
	uint32_t rocket_id = component->ids[GAME::SANDBOX::RIGID_BODY_IDS::GROUP];
	uint32_t component_type = component->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE];

	if (map) {
		// Draw small triangle representing command module

		// Only render icon on map if it's a command module
		if (component_type == GAME::COMPONENTS::COMPONENT_TYPE::COMMAND_MODULE) {

			Framework::vec2 text_position = PhysicsEngine::to_fvec(map_camera.get_render_position(component->centre) + phyvec{0, 1 + GAME::MAP::UI::ICONS::COMMAND_MODULE_SIZE / 2});

			Framework::Colour c = rocket_id == sandbox_temporaries.current_rocket ? COLOURS::CURRENT_ROCKET_ICON : COLOURS::OTHER_ROCKET_ICONS;
			
			// Rotate and translate icon points
			std::vector<Framework::vec2> points = convert_poly_vertices_retain_size(GAME::MAP::UI::ICONS::COMMAND_MODULE_VERTICES, component->centre, component->get_rotation_matrix(), map_camera);

			// Render icon. Use a different colour for the current_rocket's command module.
			graphics_objects->graphics_ptr->render_poly(points, c);

			// Render rocket name
			graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(rockets[rocket_id].get_name(), text_position, c, FONTS::SCALE::MAP_OBJECT_FONT, Framework::Font::AnchorPosition::TOP_CENTER);
		}
	}
	else {
		// Assume poly
		PhysicsEngine::Polygon* poly_ptr = static_cast<PhysicsEngine::Polygon*>(component->shape);

		// TODO: Consider rendering only if on screen (using component.bounding_radius?)
		
		std::vector<Framework::vec2> vertices = convert_poly_vertices(poly_ptr->vertices, component->centre, component->get_rotation_matrix(), sandbox_camera);
		
		graphics_objects->graphics_ptr->render_poly(vertices, COLOURS::WHITE);
	}
}


void GameStage::render_map() {
	Framework::Image map_image = Framework::Image(graphics_objects->graphics_ptr);

	map_image.load(WINDOW::SIZE);

	map_image.fill(COLOURS::BLACK);

	if (!game_state.show_map) {
		map_image.set_render_target();
	}

	// TODO: render planets first, then components!!! (also filled in triangle rather than outline?)
	// TODO: render rocket name with command module icon?

	for (PhysicsEngine::RigidBody* body : physics_manager.get_bodies()) {
		switch (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY]) {
		case GAME::SANDBOX::CATEGORIES::PLANET:
			render_planet(body, map_camera, true);
			break;

		case GAME::SANDBOX::CATEGORIES::COMPONENT:
			render_component(body, map_camera, true);
			break;
		default:
			break;
		}
	}

	if (game_state.show_map) {
		// Fullscreen map
		// Title text
		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::HUD::MAP, Framework::Vec(WINDOW::SIZE_HALF.x, 0.0f), COLOURS::WHITE, FONTS::SCALE::MAIN_FONT, Framework::Font::TOP_CENTER);
	}
	else {
		// Minimap
		// Stop rendering to map_image
		map_image.unset_render_target();

		// Render minimap image
		map_image.render(Framework::Rect(WINDOW::SIZE_HALF - GAME::MAP::UI::MINIMAP::RECT.size / 2, GAME::MAP::UI::MINIMAP::RECT.size), GAME::MAP::UI::MINIMAP::RECT);

		// Render white border
		graphics_objects->graphics_ptr->render_rect(GAME::MAP::UI::MINIMAP::RECT, COLOURS::WHITE);

		// Title text
		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::HUD::MAP, Framework::Vec(GAME::MAP::UI::MINIMAP::RECT.centre().x, GAME::MAP::UI::MINIMAP::RECT.position.y), COLOURS::WHITE, 1, Framework::Font::BOTTOM_CENTER);
	}
}

void GameStage::render_debug() {
	phyvec relative_velocity = sandbox_temporaries.nearest_planet_velocity - sandbox_temporaries.cmd_mdl_velocity;
	phyflt relative_speed = PhysicsEngine::length(relative_velocity);

	std::vector<std::string> debug_text{
		"DEBUG INFORMATION:",
		"Frames per second: " + Framework::trim_precision(debug_temporaries.fps, GAME::DEBUG::PRECISION::FPS),
		"Paused: " + std::string(game_state.paused ? "yes" : "no"),
		"Time multiplier: " + std::to_string(GAME::SANDBOX::WARP_SPEEDS[game_state.time_warp_index]) + "x",
		"Map type: " + std::string(game_state.show_map ? "fullscreen" : "minimap"),
		"Map scale: " + Framework::trim_precision(map_camera.get_scale() * 1e6f, GAME::DEBUG::PRECISION::MAP_SCALE) + "e-6x",
		"Map position: (" + Framework::trim_precision(map_camera.get_position().x, GAME::DEBUG::PRECISION::MAP_POSITION) + ", " + Framework::trim_precision(map_camera.get_position().y, GAME::DEBUG::PRECISION::MAP_POSITION) + ")",
		"Sandbox scale: " + Framework::trim_precision(sandbox_camera.get_scale(), GAME::DEBUG::PRECISION::SANDBOX_SCALE) + "x",
		"Sandbox position: (" + Framework::trim_precision(sandbox_camera.get_position().x, GAME::DEBUG::PRECISION::SANDBOX_POSITION) + ", " + Framework::trim_precision(sandbox_camera.get_position().y, GAME::DEBUG::PRECISION::SANDBOX_POSITION) + ")",
		"Nearest planet: " + STRINGS::GAME::PLANET_NAMES[sandbox_temporaries.nearest_planet],
		"Altitude: " + Framework::trim_precision(sandbox_temporaries.distance_to_nearest_planet - GAME::SANDBOX::BODIES::RADII[sandbox_temporaries.nearest_planet], GAME::DEBUG::PRECISION::ALTITUDE),
		"Relative velocity: " + Framework::trim_precision(relative_speed, GAME::DEBUG::PRECISION::RELATIVE_VELOCITY),
		"Direction: " + std::to_string(rocket_controls.direction),
		"Engine power: " + Framework::trim_precision(rocket_controls.engine_power, GAME::DEBUG::PRECISION::ENGINE_POWER),
		"Current rocket: " + std::to_string(sandbox_temporaries.current_rocket)
	};

	for (uint8_t i = 0; i < debug_text.size(); i++) {
		float y = i * FONTS::SCALE::DEBUG_FONT * FONTS::SIZE::MAIN_FONT;
		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(debug_text[i], Framework::Vec(0.0f, y), COLOURS::WHITE, FONTS::SCALE::DEBUG_FONT, Framework::Font::AnchorPosition::TOP_LEFT);
	}
}

void GameStage::render_hud() {
	std::string altitude = Framework::normalise_magnitude(sandbox_temporaries.distance_to_nearest_planet - GAME::SANDBOX::BODIES::RADII[sandbox_temporaries.nearest_planet], GAME::HUD::PRECISION::ALTITUDE, STRINGS::HUD::DISTANCE_SUFFIX);
	std::string velocity = Framework::normalise_magnitude(PhysicsEngine::length(sandbox_temporaries.nearest_planet_velocity - sandbox_temporaries.cmd_mdl_velocity), GAME::HUD::PRECISION::ALTITUDE, STRINGS::HUD::VELOCITY_SUFFIX);
	std::string nearest_planet = STRINGS::GAME::PLANET_NAMES[sandbox_temporaries.nearest_planet];
	std::string current_rocket = rockets[sandbox_temporaries.current_rocket].get_name();

	graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::HUD::ALTITUDE + altitude, Framework::vec2(BUTTONS::HUD_OFFSET, 0.0f), COLOURS::WHITE, FONTS::SCALE::HUD_FONT, Framework::Font::AnchorPosition::TOP_LEFT);
	graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::HUD::VELOCITY + velocity, Framework::vec2(BUTTONS::HUD_OFFSET, BUTTONS::SIZE.y / 2), COLOURS::WHITE, FONTS::SCALE::HUD_FONT, Framework::Font::AnchorPosition::TOP_LEFT);

	graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::HUD::NEAREST_PLANET + nearest_planet, Framework::vec2(WINDOW::SIZE.x - BUTTONS::HUD_OFFSET, 0.0f), COLOURS::WHITE, FONTS::SCALE::HUD_FONT, Framework::Font::AnchorPosition::TOP_RIGHT);

	graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::HUD::CURRENT_ROCKET + current_rocket, Framework::vec2(BUTTONS::HUD_OFFSET, WINDOW::SIZE.y), COLOURS::WHITE, FONTS::SCALE::HUD_FONT, Framework::Font::AnchorPosition::BOTTOM_LEFT);

	if (game_state.time_warp_index && !game_state.paused) {
		// Isn't 1x speed
		std::string warp_rate = std::to_string(GAME::SANDBOX::WARP_SPEEDS[game_state.time_warp_index]) + STRINGS::HUD::TIMES;

		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::HUD::TIME_WARP + warp_rate, Framework::vec2(WINDOW::SIZE_HALF.x, 0.0f), COLOURS::WHITE, FONTS::SCALE::HUD_FONT, Framework::Font::AnchorPosition::TOP_CENTER);
	}
}

void GameStage::render_atmosphere() {
	phyflt planet_radius = GAME::SANDBOX::BODIES::RADII[sandbox_temporaries.nearest_planet];
	phyflt height_above_surface = PhysicsEngine::length(sandbox_temporaries.nearest_planet_centre - sandbox_temporaries.cmd_mdl_centre) - planet_radius;

	uint8_t alpha = atmosphere_alpha(height_above_surface, GAME::SANDBOX::BODIES::SCALE_HEIGHTS[sandbox_temporaries.nearest_planet]);

	graphics_objects->graphics_ptr->fill(COLOURS::ATMOSPHERES[sandbox_temporaries.nearest_planet], alpha);
}

void GameStage::render_sandbox() {
	graphics_objects->graphics_ptr->fill(COLOURS::BLACK);

	// Draw stars
	star_field.render();

	// Add atmosphere overlay, depending on height
	render_atmosphere();

	// Draw particles
	particles.render(sandbox_camera);

	for (PhysicsEngine::RigidBody* body : physics_manager.get_bodies()) {
		switch (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY]) {
		case GAME::SANDBOX::CATEGORIES::PLANET:
			render_planet(body, sandbox_camera, false);
			break;

		case GAME::SANDBOX::CATEGORIES::COMPONENT:
			render_component(body, sandbox_camera, false);
			break;

		default:
			break;
		}
	}

#if 0
	// Testing: render constraints
	for (PhysicsEngine::Constraint* c : physics_manager.get_constraints()) {
		graphics_objects->graphics_ptr->render_line(PhysicsEngine::to_fvec(sandbox_camera.get_render_position(c->a->centre + PhysicsEngine::to_world_space(c->offset_a, c->a->get_rotation_matrix()))), PhysicsEngine::to_fvec(sandbox_camera.get_render_position(c->b->centre + PhysicsEngine::to_world_space(c->offset_b, c->b->get_rotation_matrix()))), c->is_broken() ? COLOURS::PLANETS[0] : COLOURS::WHITE);
	}
#endif
}


void GameStage::update_temporaries(float dt) {
	// Sandbox temporaries

	sandbox_temporaries.last_cmd_mdl_centre = sandbox_temporaries.cmd_mdl_centre;
	
	// Find position of current_rocket's command module
	for (PhysicsEngine::RigidBody* body : physics_manager.get_bodies()) {
		if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] == GAME::SANDBOX::CATEGORIES::COMPONENT) {
			// Does this component belong to the current rocket?
			if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::GROUP] == sandbox_temporaries.current_rocket) {
				// Is the component a command module?
				if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] == GAME::COMPONENTS::COMPONENT_TYPE::COMMAND_MODULE) {
					sandbox_temporaries.cmd_mdl_centre = body->centre;
					sandbox_temporaries.cmd_mdl_velocity = body->velocity;
				}
			}
		}
	}

	if (sandbox_temporaries.current_rocket == GAME::SANDBOX::NO_ROCKET_SELECTED) {
		// No rocket selected so default to Earth launch pad
		sandbox_temporaries.cmd_mdl_centre = get_launch_site_position(GAME::SANDBOX::LAUNCH_SITES::DEFAULT_LAUNCH_PLANET, GAME::SANDBOX::LAUNCH_SITES::DEFAULT_LAUNCH_SITE);
	}

	// Find nearest planet
	phyflt nearest_planet_squared_distance = PHYFLT_MAX;

	for (PhysicsEngine::RigidBody* body : physics_manager.get_bodies()) {
		if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] == GAME::SANDBOX::CATEGORIES::PLANET) {
			phyflt squared_distance = PhysicsEngine::length_squared(body->centre - sandbox_temporaries.cmd_mdl_centre);

			if (squared_distance < nearest_planet_squared_distance) {
				nearest_planet_squared_distance = squared_distance;

				// Set nearest planet
				sandbox_temporaries.nearest_planet = body->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE];

				sandbox_temporaries.nearest_planet_centre = body->centre;
				sandbox_temporaries.nearest_planet_velocity = body->velocity;
			}
		}
	}

	sandbox_temporaries.distance_to_nearest_planet = std::sqrt(nearest_planet_squared_distance);

	// Debug temporaries

	// Calculate fps
	debug_temporaries.fps = 1.0f / dt;
}

void GameStage::update_game_state(float dt) {
	// Warping. To redo controls
	if (!game_state.paused) {
		if (input->just_down(Framework::KeyHandler::Key::X)) {
			game_state.time_warp_index++;
			if (game_state.time_warp_index >= GAME::SANDBOX::WARP_SPEEDS.size()) game_state.time_warp_index = 0;
		}
		if (input->just_down(Framework::KeyHandler::Key::Z)) {
			if (game_state.time_warp_index == 0) {
				game_state.time_warp_index = GAME::SANDBOX::WARP_SPEEDS.size() - 1;
			}
			else {
				game_state.time_warp_index--;
			}
		}
	}

	// Compare distance above surface to "atmosphere" height
	if (sandbox_temporaries.distance_to_nearest_planet - GAME::SANDBOX::BODIES::RADII[sandbox_temporaries.nearest_planet] < GAME::SANDBOX::BODIES::SCALE_HEIGHTS[sandbox_temporaries.nearest_planet] * GAME::SANDBOX::SCALE_HEIGHT_TO_ATMOSPHERE_HEIGHT_FACTOR) {
		// Within "atmosphere", so can't warp!
		game_state.time_warp_index = 0;

		// TODO: show message saying about why can't warp
	}


	if (!game_state.paused) {
		if (input->just_down(Framework::KeyHandler::Key::ESCAPE) || input->just_down(Framework::KeyHandler::Key::SPACE)) {
			// Go to paused menu
			game_state.paused = true;
			finish(new PausedStage(this), false);
		}
	}

	if (input->just_down(Framework::KeyHandler::Key::M)) toggle_map();


	// Handle changing of current rocket selected
	if (input->just_down(Framework::KeyHandler::Key::Q)) {
		// Wrap round if necessary
		if (sandbox_temporaries.current_rocket == rockets.begin()->first) {
			sandbox_temporaries.current_rocket = rockets.rbegin()->first;
		}
		else {
			sandbox_temporaries.current_rocket--;
		}

		while (!rockets.count(sandbox_temporaries.current_rocket)) {
			// Rocket id doesn't exist so decrement it
			// Wrap round if necessary
			if (sandbox_temporaries.current_rocket == rockets.begin()->first) {
				sandbox_temporaries.current_rocket = rockets.rbegin()->first;
			}
			else {
				sandbox_temporaries.current_rocket--;
			}
		}
	}

	if (input->just_down(Framework::KeyHandler::Key::E)) {
		// Wrap round if necessary
		if (sandbox_temporaries.current_rocket == rockets.rbegin()->first) {
			sandbox_temporaries.current_rocket = rockets.begin()->first;
		}
		else {
			sandbox_temporaries.current_rocket++;
		}

		while (!rockets.count(sandbox_temporaries.current_rocket)) {
			// Rocket id doesn't exist so decrement it
			// Wrap round if necessary
			if (sandbox_temporaries.current_rocket == rockets.rbegin()->first) {
				sandbox_temporaries.current_rocket = rockets.begin()->first;
			}
			else {
				sandbox_temporaries.current_rocket++;
			}
		}
	}


	// Controls
	if (!game_state.paused) {

		// Positive is anticlockwise
		rocket_controls.direction = 0;
		if (input->is_down(Framework::KeyHandler::Key::LEFT)) rocket_controls.direction++;
		if (input->is_down(Framework::KeyHandler::Key::RIGHT)) rocket_controls.direction--;

		if (input->is_down(Framework::KeyHandler::Key::UP)) rocket_controls.engine_power += GAME::CONTROLS::ENGINE_POWER_INCREASE_RATE * dt;
		if (input->is_down(Framework::KeyHandler::Key::DOWN)) rocket_controls.engine_power -= GAME::CONTROLS::ENGINE_POWER_INCREASE_RATE * dt;

		rocket_controls.engine_power = Framework::clamp(rocket_controls.engine_power, 0.0f, 1.0f);
	}
}


void GameStage::update_map(float dt) {
	// Handle map controls
	
	// Move camera by distance the command module has moved
	if (settings.auto_move_map) {
		phyvec distance = sandbox_temporaries.cmd_mdl_centre - sandbox_temporaries.last_cmd_mdl_centre;
		map_camera.set_position(map_camera.get_position() + distance);
	}

	// Only allow scrolling/dragging map if we're in fullscreen map mode, or if the mouse is over the minimap
	if (game_state.show_map || Framework::colliding(GAME::MAP::UI::MINIMAP::RECT, input->get_mouse()->position())) {
		phyflt maximum_zoom = GAME::MAP::UI::MAXIMUM_ZOOM;
		if (!game_state.show_map) maximum_zoom /= GAME::MAP::UI::MINIMAP::EXTRA_ZOOM;

		phyflt minimum_zoom = GAME::MAP::UI::MINIMUM_ZOOM;
		if (!game_state.show_map) minimum_zoom /= GAME::MAP::UI::MINIMAP::EXTRA_ZOOM;

		phyflt zoom = input->get_mouse()->scroll_amount() * GAME::MAP::UI::SCROLL_ZOOM_RATE + 1.0f;

		phyflt new_scale = map_camera.get_scale() * zoom;
		phyflt clamped_new_scale = Framework::clamp(new_scale, minimum_zoom, maximum_zoom);

		phyflt actual_zoom = clamped_new_scale / map_camera.get_scale() - 1.0f;

		map_camera.set_scale(clamped_new_scale);

		phyvec new_position = map_camera.get_position() - input->get_mouse()->distance_dragged() / map_camera.get_scale();

		// Adjust map so that zooming always zooms towards the cursor
		Framework::vec2 offset = game_state.show_map ? WINDOW::SIZE_HALF : GAME::MAP::UI::MINIMAP::RECT.centre();
		new_position += actual_zoom * (input->get_mouse()->position() - offset) / map_camera.get_scale();

		map_camera.set_position(new_position);
	}
}


void GameStage::update_sandbox(float dt) {
	// Move camera to centre of command module of current_rocket
	sandbox_camera.set_position(sandbox_temporaries.cmd_mdl_centre);


	// TESTING ONLY
	// Only allow scrolling/--dragging-- sandbox if we're not in fullscreen map mode and not clicking on the minimap
	if (!game_state.show_map && !Framework::colliding(GAME::MAP::UI::MINIMAP::RECT, input->get_mouse()->position())) {
		phyflt zoom = input->get_mouse()->scroll_amount() * GAME::SANDBOX::UI::SCROLL_ZOOM_RATE + 1.0f;

		phyflt new_scale = sandbox_camera.get_scale() * zoom;
		phyflt clamped_new_scale = Framework::clamp(new_scale, GAME::SANDBOX::UI::MINIMUM_ZOOM, GAME::SANDBOX::UI::MAXIMUM_ZOOM);

		//float actual_zoom = clamped_new_scale / sandbox_camera.get_scale() - 1.0f;

		sandbox_camera.set_scale(clamped_new_scale);

		// NO DRAGGING
		//Framework::vec2 new_position = sandbox_camera.get_position() - input->get_mouse()->distance_dragged() / sandbox_camera.get_scale();

		// Adjust map so that zooming always zooms towards the cursor
		//new_position += actual_zoom * (input->get_mouse()->position() - WINDOW::SIZE_HALF) / sandbox_camera.get_scale();

		//sandbox_camera.set_position(new_position);
	}


	if (!game_state.paused) {

		// Move stars
		star_field.move(PhysicsEngine::to_fvec(sandbox_temporaries.cmd_mdl_centre - sandbox_temporaries.last_cmd_mdl_centre));

		// Update particles
		particles.update(dt);

		// Control rocket

		for (PhysicsEngine::RigidBody* body : physics_manager.get_bodies()) {
			if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] == GAME::SANDBOX::CATEGORIES::COMPONENT) {
				// Does this component belong to the current rocket?
				if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::GROUP] == sandbox_temporaries.current_rocket) {

					// Is the component an engine?
					if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] == GAME::COMPONENTS::COMPONENT_TYPE::ENGINE) {

						// Is the component connected to the command module?
						Rocket& rocket = rockets[body->ids[GAME::SANDBOX::RIGID_BODY_IDS::GROUP]];
						Component* component_ptr = rocket.get_component_ptr(body->ids[GAME::SANDBOX::RIGID_BODY_IDS::OBJECT]);

						if (!component_ptr->get_broken()) {

							// Add force
							phyflt force_magnitude = GAME::CONTROLS::ENGINE_FORCE * rocket_controls.engine_power;

							phyvec force = phyvec{ 0, -force_magnitude }; // Force up

							phyflt direction_angle = GAME::CONTROLS::ENGINE_TURN_ANGLE * rocket_controls.direction;

							force = PhysicsEngine::mul(PhysicsEngine::rotation_matrix(direction_angle + body->angle), force);

							body->apply_force(force);

							// Smoke particles

							// Create more particles if engine throttle is higher
							// NOTE: currently not framerate independent code!!
							if (rocket_controls.engine_power > Framework::randf()) {

								phyflt size = Framework::randf(GAME::PARTICLES::SMOKE::MIN_SIZE, GAME::PARTICLES::SMOKE::MAX_SIZE);
								phyflt size_change = GAME::PARTICLES::SMOKE::SIZE_CHANGE;

								phyvec position = body->centre;

								phyvec velocity = body->velocity;
								velocity += PhysicsEngine::mul(PhysicsEngine::rotation_matrix(Framework::randf(0.0, 2 * PhysicsEngine::PI)), { 1, 0 }) * Framework::randf() * GAME::PARTICLES::SMOKE::VELOCITY_OFFSET_MAX;

								// Average between nearest planet's velocity and current velocity?
								velocity += sandbox_temporaries.nearest_planet_velocity;
								velocity /= 2;

								phyflt age = Framework::randf(GAME::PARTICLES::SMOKE::MIN_AGE, GAME::PARTICLES::SMOKE::MAX_AGE);

								uint8_t a = Framework::randf(GAME::PARTICLES::SMOKE::RGB_MIN, GAME::PARTICLES::SMOKE::RGB_MAX);
								Framework::Colour colour = Framework::Colour(a, a, a);

								phyflt alpha_change = -0xFF / age;

								particles.add_particle({ size, size_change, position, velocity, age, colour, alpha_change });
							}
						}
					}
				}
			}
		}

		// Handle broken constraints and components

		// TODO: remove from PhysicsEngine?

		for (PhysicsEngine::Constraint* constraint : physics_manager.get_constraints()) {
			// Is it broken?
			if (constraint->is_broken()) {

				uint32_t rocket_id = constraint->ids[GAME::SANDBOX::CONSTRAINT_IDS::ROCKET];
				uint32_t connection_id = constraint->ids[GAME::SANDBOX::CONSTRAINT_IDS::CONSTRAINT];

				Rocket& rocket = rockets[rocket_id];

				Connection* connection = rocket.get_connection_ptr(connection_id);


				// If connection is broken then we don't need to set other components to broken
				if (!connection->broken) {

					// Remove connection
					connection->broken = true;

					// Check each component to see if it's still connected or not
					for (const std::pair<uint32_t, Component>& p : rocket.get_components()) {
						if (!command_module_is_connected(rocket, p.first)) {
							// We have to get a ptr to the component to be able to set broken (otherwise we set broken on a copy)
							rocket.get_component_ptr(p.first)->set_broken();
						}
					}
				}
			}
		}
	}
}


void GameStage::update_physics(float dt) {
	// Need to find a good method

	// Old method: make jumps bigger:
	/*
	printf("dt: %f, %f\n", dt, 1.0f / 60.0f);*/

	// several issues
	// 1. setting dt to 1/60 or .016 results in rect going down at constantish rate, ignore collisions for a bit
	// 2. setting dt to .017 results in rect going up at constantish rate before falling in a bit

	//physics_manager.update(dt * GAME::SANDBOX::WARP_SPEEDS[time_warp_index]);

	// Other method: iterate 10 times on each frame
	// Means physics is good up to 10x speed
	/*float step = dt * GAME::SANDBOX::WARP_SPEEDS[time_warp_index] / 10.0f;
	for (uint8_t i = 0; i < 10; i++) {
		physics_manager.update(step);
	}*/

	
	// New method: call it lots of times, but don't let the number of repetitions be huge
	uint32_t speed = GAME::SANDBOX::WARP_SPEEDS[game_state.time_warp_index];
	uint32_t multiplier = 1;

	while (speed > 10) {
		speed /= 10;
		multiplier *= 10;
	}

	float modified_dt = dt * multiplier;

	for (uint32_t i = 0; i < speed; i++) {
		physics_manager.update(modified_dt);
	}
}

void GameStage::toggle_map() {
	game_state.show_map = !game_state.show_map;
	
	float zoom_amount = GAME::MAP::UI::MINIMAP::EXTRA_ZOOM;

	// Adjust zoom so that minimap fits similar amount on
	float zoom = game_state.show_map ? zoom_amount : 1.0f / zoom_amount;

	map_camera.set_scale(zoom * map_camera.get_scale());
}



std::vector<Framework::vec2> GameStage::convert_poly_vertices(std::vector<phyvec> vertices, const phyvec& centre, const phymat& rotation_matrix, const Camera& camera) {
	std::vector<Framework::vec2> new_vertices;

	for (const phyvec& v : vertices) {
		phyvec a = centre + PhysicsEngine::to_world_space(v, rotation_matrix);
		new_vertices.push_back(PhysicsEngine::to_fvec(camera.get_render_position(a)));
	}

	return new_vertices;
}

// Converts icon shapes by rotating and shifting, but keeping spacing of points the same
std::vector<Framework::vec2> GameStage::convert_poly_vertices_retain_size(std::vector<phyvec> vertices, const phyvec& centre, const phymat& rotation_matrix, const Camera& camera) {
	std::vector<Framework::vec2> new_vertices;

	for (const phyvec& v : vertices) {
		phyvec a = PhysicsEngine::to_world_space(v, rotation_matrix) + (centre - camera.get_position()) * camera.get_scale() + WINDOW::SIZE_HALF;
		new_vertices.push_back(PhysicsEngine::to_fvec(a));
	}

	return new_vertices;
}

uint32_t GameStage::get_next_rocket_index() {
	// Get next free index in rockets
	uint32_t i = 0;
	while (rockets.count(i)) {
		i++;
	}
	return i;
}


PhysicsEngine::RigidBody* GameStage::get_planet_from_rigidbodies(uint32_t id) {
	for (PhysicsEngine::RigidBody* body : physics_manager.get_bodies()) {
		if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] == GAME::SANDBOX::CATEGORIES::PLANET) {
			if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] == id) {
				return body;
			}
		}
	}

	// Couldn't find it
	return nullptr;
}


phyvec GameStage::get_launch_site_position(uint8_t planet, uint8_t site) {
	PhysicsEngine::RigidBody* planet_ptr = get_planet_from_rigidbodies(planet);

	phyvec position = planet_ptr->centre;
	position += PhysicsEngine::mul(PhysicsEngine::rotation_matrix(PhysicsEngine::deg_to_rad(GAME::SANDBOX::LAUNCH_SITES::SITES[planet][site])), { 0.0f, -1.0f }) * GAME::SANDBOX::BODIES::RADII[planet];

	return position;
}


bool GameStage::command_module_is_connected(Rocket& rocket, uint32_t component_id) {
	return breadth_first_component_search(rocket, component_id, GAME::COMPONENTS::COMPONENT_TYPE::COMMAND_MODULE) != nullptr;
}


Component* GameStage::breadth_first_component_search(Rocket& rocket, uint32_t start_id, uint32_t target_type) {
	// Inspired by https://en.wikipedia.org/wiki/Breadth-first_search

	std::vector<uint32_t> explored;
	std::queue<uint32_t> queue;

	explored.push_back(start_id);
	queue.push(start_id);

	while (!queue.empty()) {
		uint32_t component_id = queue.front();
		queue.pop();

		Component* component_ptr = rocket.get_component_ptr(component_id);

		if (component_ptr->get_type() == target_type) {
			return component_ptr;
		}

		for (const Connection& c : rocket.get_connections()) {
			// Is connection not broken?
			if (!c.broken) {
				// Is component is linked to it?
				if (c.a.component_id == component_id || c.b.component_id == component_id) {
					uint32_t other_id = component_id == c.a.component_id ? c.b.component_id : c.a.component_id;

					if (!std::count(explored.begin(), explored.end(), other_id)) {
						// Not already explored
						explored.push_back(other_id);
						queue.push(other_id);
					}
				}
			}
		}
	}

	return nullptr;
}





// PausedStage

PausedStage::PausedStage(GameStage* background_stage) : BaseStage() {
	// Save the background stage so we can still render it, and then go back to it when done
	_background_stage = background_stage;
}

void PausedStage::init() {
	// Create buttons for selecting save file
	buttons = create_menu_from_constants(graphics_objects, STRINGS::BUTTONS::PAUSED);

	// Create transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void PausedStage::start() {
	transition->set_open();
}

bool PausedStage::update(float dt) {
	transition->update(dt);

	_background_stage->update(dt);

	// Un-pause if ESCAPE pressed
	if (input->just_down(Framework::KeyHandler::Key::ESCAPE) || input->just_down(Framework::KeyHandler::Key::SPACE)) {
		// Return to game (exit pause)
		finish(_background_stage);
	}

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();

			switch (button_selected) {
			case BUTTONS::PAUSED::RESUME:
				// Return to game (exit pause)
				finish(_background_stage);
				break;

			case BUTTONS::PAUSED::LOAD_ROCKET:
				button.reset_state();
				finish(new LoadRocketStage(this), false);
				break;

			case BUTTONS::PAUSED::EXIT:
				transition->close();
				break;

			default:
				break;
			}
		}
	}

	if (transition->is_closed()) {
		if (button_selected == BUTTONS::PAUSED::EXIT) {
			_background_stage->end();
			//delete _background_stage; // MEM LEAK!?

			finish(new TitleStage());
		}
	}

	return true;
}

void PausedStage::render() {
	// Render background stage
	if (_background_stage) _background_stage->render();

	graphics_objects->graphics_ptr->fill(MENU::BACKGROUND_RECT, COLOURS::EDITOR_GREY, 0x40); // Add non-black colour as base just to allow viewing of bar when background is black
	graphics_objects->graphics_ptr->fill(MENU::BACKGROUND_RECT, COLOURS::BLACK, MENU::BACKGROUND_ALPHA);

	graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::HUD::PAUSED, Framework::vec2(WINDOW::SIZE_HALF.x, 0.0f), COLOURS::WHITE, Framework::Font::TOP_CENTER);

	// Render pause menu
	for (const Framework::Button& button : buttons) button.render();

	transition->render();
}


void PausedStage::load_rocket(std::string rocket_name, uint8_t planet, uint8_t site) {
	_background_stage->load_rocket(rocket_name, planet, site);
}

GameStage* PausedStage::get_background_stage() {
	return _background_stage;
}

// PlayOptionsStage

LoadRocketStage::LoadRocketStage() { }
LoadRocketStage::LoadRocketStage(PausedStage* paused_stage) : _paused_stage(paused_stage) { }

void LoadRocketStage::init() {
	// Find valid rocket files:
	std::vector<std::string> filepaths = find_files_with_extension(PATHS::BASE_PATH + PATHS::ROCKET_TEMPLATES::LOCATION, PATHS::ROCKET_TEMPLATES::EXTENSION);

	for (std::string path : filepaths) {
		rocket_names.push_back(trim_extension(get_filename(path)));
	}

	// Create buttons for selecting save file
	buttons = create_menu_buttons(MENU::OVERLAY_RECT, BUTTONS::WIDE_SIZE, graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], STRINGS::BUTTONS::LOAD_ROCKET, graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], COLOURS::WHITE, MENU::OVERLAY_RECT.position.y);

	buttons[BUTTONS::LOAD_ROCKET::LAUNCH].set_position(MENU::OVERLAY_RECT.bottomleft() - Framework::vec2{ 0, BUTTONS::WIDE_SIZE.y });

	// Create transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void LoadRocketStage::start() {
	transition->set_open();

	// Set default text:
	for (uint8_t i = 0; i < BUTTONS::LOAD_ROCKET::TOTAL; i++) {
		buttons[i].set_text(STRINGS::BUTTONS::LOAD_ROCKET[i]);
	}

	// Set text and selected items:
	rocket_index = 0;
	planet_index = 0;
	site_index = 0;

	if (rocket_names.size() > 0) {
		buttons[BUTTONS::LOAD_ROCKET::FILE].set_text(rocket_names[rocket_index]);
	}
	if (GAME::SANDBOX::LAUNCH_SITES::SITES.size() > 0) {
		buttons[BUTTONS::LOAD_ROCKET::PLANET].set_text(STRINGS::GAME::PLANET_NAMES[planet_index]);

		if (GAME::SANDBOX::LAUNCH_SITES::SITES[planet_index].size() > 0) {
			buttons[BUTTONS::LOAD_ROCKET::PLANET].set_text(STRINGS::BUTTONS::LOAD_ROCKET_TEXT::SITE + std::to_string(site_index));
		}
	}
}

bool LoadRocketStage::update(float dt) {
	transition->update(dt);

	// If user clicked outside menu overlay, then go back
	if (input->just_down(Framework::MouseHandler::MouseButton::LEFT) && !Framework::colliding(MENU::OVERLAY_RECT, input->get_mouse()->position())) {
		finish(_paused_stage);
	}

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();


			switch (button_selected) {
			case BUTTONS::LOAD_ROCKET::FILE:
				if (rocket_names.size() > 0) {
					rocket_index++;
					rocket_index %= rocket_names.size();

					// Set text
					button.set_text(rocket_names[rocket_index]);
				}
				break;

			case BUTTONS::LOAD_ROCKET::PLANET:
				if (GAME::SANDBOX::LAUNCH_SITES::SITES.size() > 0) {
					planet_index++;
					planet_index %= GAME::SANDBOX::LAUNCH_SITES::SITES.size();

					// Set text
					button.set_text(STRINGS::GAME::PLANET_NAMES[planet_index]);

					site_index = 0;

					if (GAME::SANDBOX::LAUNCH_SITES::SITES[planet_index].size() > 0) {

						// Set text
						buttons[BUTTONS::LOAD_ROCKET::SITE].set_text(STRINGS::BUTTONS::LOAD_ROCKET_TEXT::SITE + std::to_string(site_index));
					}
					else {
						buttons[BUTTONS::LOAD_ROCKET::SITE].set_text(STRINGS::BUTTONS::LOAD_ROCKET[BUTTONS::LOAD_ROCKET::SITE]);
					}
				}
				break;

			case BUTTONS::LOAD_ROCKET::SITE:
				if (GAME::SANDBOX::LAUNCH_SITES::SITES.size() > 0 && GAME::SANDBOX::LAUNCH_SITES::SITES[planet_index].size() > 0) {
					site_index++;
					site_index %= GAME::SANDBOX::LAUNCH_SITES::SITES[planet_index].size();

					// Set text
					button.set_text(STRINGS::BUTTONS::LOAD_ROCKET_TEXT::SITE + std::to_string(site_index));
				}
				break;

			case BUTTONS::LOAD_ROCKET::LAUNCH:
				// Load rocket
				if (rocket_index < rocket_names.size() && planet_index < GAME::SANDBOX::LAUNCH_SITES::SITES.size() && site_index < GAME::SANDBOX::LAUNCH_SITES::SITES[planet_index].size()) {
					_paused_stage->load_rocket(rocket_names[rocket_index], planet_index, site_index);

					// Slightly hacky way to unpause the game
					finish(_paused_stage->get_background_stage());

					_paused_stage->end();
					//delete _paused_stage; // MEM LEAK!?
				}
				break;

			default:
				break;
			}
		}
	}

	return true;
}

void LoadRocketStage::render() {
	// Render play options stage
	_paused_stage->render();

	// Menu overlay
	graphics_objects->graphics_ptr->fill(MENU::OVERLAY_RECT, COLOURS::WHITE, 0x60);
	graphics_objects->graphics_ptr->fill(MENU::OVERLAY_RECT, COLOURS::ATMOSPHERES[GAME::SANDBOX::BODIES::ID::EARTH], 0x80); // TODO change
	graphics_objects->graphics_ptr->fill(MENU::OVERLAY_RECT, COLOURS::BLACK, MENU::OVERLAY_ALPHA);
	graphics_objects->graphics_ptr->render_rect(MENU::OVERLAY_RECT, Framework::Colour(COLOURS::WHITE, MENU::BORDER_ALPHA));

	// Buttons
	for (const Framework::Button& button : buttons) button.render();

	transition->render();
}