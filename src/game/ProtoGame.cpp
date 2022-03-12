#include "ProtoGame.hpp"

// GameStage


GameStage::GameStage() {

}
GameStage::GameStage(std::string _save_name) {
	save_name = _save_name;
}

void GameStage::init() {
	map_camera = Camera(WINDOW::SIZE);
	sandbox_camera = Camera(WINDOW::SIZE);

	PhysicsEngine::PhysicsManager::Constants constants = physics_manager.get_constants();
	constants.gravitational_constant = GAME::SANDBOX::GRAVITATIONAL_CONSTANT;
	physics_manager.set_constants(constants);

	// Create star field
	star_field = StarField(graphics_objects->graphics_ptr, WINDOW::SIZE);

	// Set camera scales
	map_camera.set_scale(GAME::MAP::UI::DEFAULT_CAMERA_SCALE);
	sandbox_camera.set_scale(GAME::SANDBOX::UI::DEFAULT_CAMERA_SCALE);
}

void GameStage::start() {
	load_settings();
	load_sandbox();

	init_temporaries();
}

void GameStage::end() {
	save_settings();
	save_sandbox();
}

bool GameStage::update(float dt) {
	//transition->update(dt);

	// Handle inputs etc
	update_game_state(dt);

	// Note: slight issue where sun actually does slowly move
	// This seems to cause other orbit to go crazy after a little bit too
	if (!game_state.paused) update_physics(dt);

	// Temporary positions etc
	update_temporaries(dt);

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
}




void GameStage::init_temporaries() {
	update_temporaries(WINDOW::TARGET_DT);

	sandbox_temporaries.last_cmd_mdl_centre = sandbox_temporaries.cmd_mdl_centre;

	// Set map camera to be centred on current rocket
	map_camera.set_position(sandbox_temporaries.cmd_mdl_centre);
}

void GameStage::load_settings() {
	// Set defaults
	settings = Settings();

	try {
		// Read json
		json j = Framework::JSONHandler::read(PATHS::BASE_PATH + PATHS::DATA::LOCATION + PATHS::DATA::SETTINGS);

		// Fill settings struct
		j.get_to(settings);
	}
	catch (const json::exception& e) {
		// Ignore - we set defaults already so it's not the end of the world
		printf("Couldn't load settings file. Error: %s\n", e.what());
	}
}
void GameStage::save_settings() {
	Framework::JSONHandler::write(PATHS::BASE_PATH + PATHS::DATA::LOCATION + PATHS::DATA::SETTINGS, settings);
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
	}
	catch (const json::exception& e) {
		// Ignore - we set defaults already so it's not the end of the world
		printf("Couldn't load save file: %s. Error: %s\n", save_name.c_str(), e.what());

		printf("Creating new sandbox instead...\n");

		new_sandbox();

		// For now, create basic rocket (handle loading later)
		create_components();
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
		{"rockets", rockets}
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
	// Find planet which is sun:
	phyvec sun_position;
	phyflt sun_radius = GAME::SANDBOX::BODIES::RADII[GAME::SANDBOX::BODIES::ID::SUN];
	phyflt sun_area_density = volume_to_area_density(GAME::SANDBOX::BODIES::VOLUME_DENSITIES[GAME::SANDBOX::BODIES::ID::SUN], sun_radius);
	phyflt sun_area = PhysicsEngine::PI * sun_radius * sun_radius;
	phyflt sun_mass = sun_area * sun_area_density;
	for (const Planet& planet : planets) {
		if (planet.id == GAME::SANDBOX::BODIES::ID::SUN) {
			// Get stats
			sun_position = planet.position;
		}
	}


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
void GameStage::create_components() {

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
}

// Creates RigidBodies and Constraints, using the positions stored in the Rocket instance identified by rocket_id, offsetting by the supplied offset
void GameStage::create_rocket(const Rocket& rocket) {
	uint32_t rocket_id = get_next_rocket_index();

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
		PhysicsEngine::RigidBody* object_ptr = new PhysicsEngine::RigidBody(shape_ptr, material_ptr, PhysicsEngine::mul(PhysicsEngine::rotation_matrix(initial_data.angle), component.get_offset()) + initial_data.position, initial_data.angle);
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

			PhysicsEngine::Constraint* constraint = new PhysicsEngine::Spring(a, b, offset_a, offset_b, 0.01f, GAME::SANDBOX::CONNECTIONS::MODULUS_OF_ELASTICITY, GAME::SANDBOX::CONNECTIONS::MAX_EXTENSION);
			physics_data.constraints.push_back(constraint);

			physics_manager.add_constraint(constraint); // NOTE: ISSUE WITH THIS?? constraints seem to do right force, but objects don't move under their infludence
		}
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


//void GameStage::render_physics_objects() {
//	// todo?
//}

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
		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text("Map", Framework::Vec(WINDOW::SIZE_HALF.x, 0.0f), COLOURS::WHITE, FONTS::SCALE::MAIN_FONT, Framework::Font::TOP_CENTER);
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
		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text("Map", Framework::Vec(GAME::MAP::UI::MINIMAP::RECT.centre().x, GAME::MAP::UI::MINIMAP::RECT.position.y), COLOURS::WHITE, 1, Framework::Font::BOTTOM_CENTER);
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
		"Map Scale: " + Framework::trim_precision(map_camera.get_scale() * 1e6f, GAME::DEBUG::PRECISION::MAP_SCALE) + "e-6x",
		"Map Position: (" + Framework::trim_precision(map_camera.get_position().x, GAME::DEBUG::PRECISION::MAP_POSITION) + ", " + Framework::trim_precision(map_camera.get_position().y, GAME::DEBUG::PRECISION::MAP_POSITION) + ")",
		"Sandbox Scale: " + Framework::trim_precision(sandbox_camera.get_scale(), GAME::DEBUG::PRECISION::SANDBOX_SCALE) + "x",
		"Sandbox Position: (" + Framework::trim_precision(sandbox_camera.get_position().x, GAME::DEBUG::PRECISION::SANDBOX_POSITION) + ", " + Framework::trim_precision(sandbox_camera.get_position().y, GAME::DEBUG::PRECISION::SANDBOX_POSITION) + ")",
		"Nearest planet: " + STRINGS::GAME::PLANET_NAMES[sandbox_temporaries.nearest_planet],
		"Altitude: " + Framework::trim_precision(sandbox_temporaries.distance_to_nearest_planet - GAME::SANDBOX::BODIES::RADII[sandbox_temporaries.nearest_planet], GAME::DEBUG::PRECISION::ALTITUDE),
		"Relative velocity: " + Framework::trim_precision(relative_speed, GAME::DEBUG::PRECISION::RELATIVE_VELOCITY),
		"Direction: " + std::to_string(rocket_controls.direction),
		"Engine power: " + Framework::trim_precision(rocket_controls.engine_power, GAME::DEBUG::PRECISION::ENGINE_POWER)
	};

	for (uint8_t i = 0; i < debug_text.size(); i++) {
		float y = i * FONTS::SCALE::DEBUG_FONT * FONTS::SIZE::MAIN_FONT;
		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(debug_text[i], Framework::Vec(0.0f, y), COLOURS::WHITE, FONTS::SCALE::DEBUG_FONT, Framework::Font::AnchorPosition::TOP_LEFT);
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
	if (input->just_down(Framework::KeyHandler::Key::Z)) {
		game_state.time_warp_index++;
		if (game_state.time_warp_index >= GAME::SANDBOX::WARP_SPEEDS.size()) game_state.time_warp_index = 0;
	}


	if (input->just_down(Framework::KeyHandler::Key::SPACE)) game_state.paused = !game_state.paused;

	if (input->just_down(Framework::KeyHandler::Key::M)) toggle_map();



	// Controls
	// Positive is anticlockwise
	rocket_controls.direction = 0;
	if (input->is_down(Framework::KeyHandler::Key::LEFT)) rocket_controls.direction++;
	if (input->is_down(Framework::KeyHandler::Key::RIGHT)) rocket_controls.direction--;

	if (input->is_down(Framework::KeyHandler::Key::UP)) rocket_controls.engine_power += GAME::CONTROLS::ENGINE_POWER_INCREASE_RATE * dt;
	if (input->is_down(Framework::KeyHandler::Key::DOWN)) rocket_controls.engine_power -= GAME::CONTROLS::ENGINE_POWER_INCREASE_RATE * dt;

	rocket_controls.engine_power = Framework::clamp(rocket_controls.engine_power, 0.0f, 1.0f);
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

	// Move stars
	star_field.move(PhysicsEngine::to_fvec(sandbox_temporaries.cmd_mdl_centre - sandbox_temporaries.last_cmd_mdl_centre));

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

	// Control rocket

	for (PhysicsEngine::RigidBody* body : physics_manager.get_bodies()) {
		if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] == GAME::SANDBOX::CATEGORIES::COMPONENT) {
			// Does this component belong to the current rocket?
			if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::GROUP] == sandbox_temporaries.current_rocket) {
				// Is the component an engine?
				if (body->ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] == GAME::COMPONENTS::COMPONENT_TYPE::ENGINE) {
					static const phyflt ENGINE_FORCE = 2e7f;
					// Add force
					phyflt force_magnitude = ENGINE_FORCE * rocket_controls.engine_power;

					phyvec force = phyvec{ 0, -force_magnitude }; // Force up

					static const phyflt TURN_AMOUNT = PhysicsEngine::deg_to_rad(15); //radians

					phyflt direction_angle = TURN_AMOUNT * rocket_controls.direction;
					
					force = PhysicsEngine::mul(PhysicsEngine::rotation_matrix(direction_angle + body->angle), force);

					body->apply_force(force);
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