#include "ProtoGame.hpp"

// GameStage

//GameStage::~GameStage() {
//	// Tidy up
//	for (Framework::BaseSprite* planet : planets) {
//		delete planet;
//	}
//	planets.clear();
//}

void GameStage::init() {
	map_camera = Camera(WINDOW::SIZE);
	sandbox_camera = Camera(WINDOW::SIZE);

	PhysicsEngine::PhysicsManager::Constants constants = physics_manager.get_constants();
	constants.gravitational_constant = GAME::SANDBOX::GRAVITATIONAL_CONSTANT;
	physics_manager.set_constants(constants);

	// Set camera position

	// Just for testing
	//map_camera.set_position({ GAME::SANDBOX::BODIES::APHELION_DISTANCES[3] * GAME::SANDBOX::UNIVERSE_SCALE, 0.0f });
	map_camera.set_position({});
	map_camera.set_scale(1e-5f / GAME::SANDBOX::UNIVERSE_SCALE);

	// TODO
	//sandbox_camera.set_position({ GAME::SANDBOX::BODIES::APHELION_DISTANCES[3] * GAME::SANDBOX::UNIVERSE_SCALE,  -GAME::SANDBOX::BODIES::RADII[3] * GAME::SANDBOX::UNIVERSE_SCALE });
	sandbox_camera.set_position({ 0.0f ,  -GAME::SANDBOX::BODIES::RADII[3] });
	sandbox_camera.set_scale(1.0f);

	// Create Solar System
	create_solar_system();

	create_components(); // todo: load from save
}

void GameStage::start() {

}

bool GameStage::update(float dt) {
	//transition->update(dt);

	// Warping. To redo controls
	if (input->just_down(Framework::KeyHandler::Key::Z)) {
		time_warp_index++;
		if (time_warp_index >= GAME::SANDBOX::WARP_SPEEDS.size()) time_warp_index = 0;
	}


	if (input->just_down(Framework::KeyHandler::Key::SPACE)) paused = !paused;

	if (input->just_down(Framework::KeyHandler::Key::M)) toggle_map();


	// Note: slight issue where sun actually does slowly move
	// This seems to cause other orbit to go crazy after a little bit too
	if (!paused) update_physics(dt);

	update_map(dt);
	update_sandbox(dt); // TESTING

	fps = 1.0f / dt;

	return true;
}

void GameStage::render() {
	// Render background
	//graphics_objects->image_ptrs[GRAPHICS_OBJECTS::IMAGES::GAME_BACKGROUND]->render();
	graphics_objects->graphics_ptr->fill(COLOURS::BLACK);

	// Add atmosphere overlay, depending on height
	//if (near_current_planet && distance_to_planet_centre < planet_atmosphere_height_from_centre) {
		//graphics_objects->graphics_ptr->fill(COLOURS::ATMOSPHERES[current_planet], linear(0xFF, 0x00, some_function_which_stays_small_for_a_while(0, planet_atmosphere_height_from_centre));
	//}

	//render_physics_objects();

	if (!show_map) render_sandbox();

	render_map();

	// todo: check if debug has been set
	render_debug();
}




void GameStage::create_solar_system() {
#define JUST_EARTH
#ifdef JUST_EARTH
	uint8_t i = GAME::SANDBOX::BODIES::ID::EARTH;

	phyflt radius = GAME::SANDBOX::BODIES::RADII[i];

	PhysicsEngine::Circle* circle_ptr = new PhysicsEngine::Circle(radius);
	physics_data.shapes.push_back(circle_ptr);

	phyflt area_density = volume_to_area_density(GAME::SANDBOX::BODIES::VOLUME_DENSITIES[i], radius);

	// Add material_ptr to physics_data since it's not a heap allocated ptr
	PhysicsEngine::Material* material_ptr = new PhysicsEngine::Material(0.7f, 0.5f, 0.1f, area_density); // todo: get friction and restitution?
	physics_data.materials.push_back(material_ptr);

	PhysicsEngine::phyvec position = PhysicsEngine::PHYVEC_NULL;

	PhysicsEngine::RigidBody object = PhysicsEngine::RigidBody(circle_ptr, material_ptr, position); //i==GAME::SANDBOX::BODIES::ID::SUN // Make sun immovable? - would break gravity calculation!

	object.ids.assign(GAME::SANDBOX::RIGID_BODY_IDS::TOTAL, 0);

	// Set render ID
	object.ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] = i;

	// Set category ID
	object.ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] = GAME::SANDBOX::CATEGORIES::PLANET;

	physics_manager.add_body(object);
#else
	// TODO: create method to load system from object positions
	phyflt sun_radius = GAME::SANDBOX::BODIES::RADII[GAME::SANDBOX::BODIES::ID::SUN];
	phyflt sun_area_density = volume_to_area_density(GAME::SANDBOX::BODIES::VOLUME_DENSITIES[GAME::SANDBOX::BODIES::ID::SUN], sun_radius);
	phyflt sun_area = PhysicsEngine::PI * sun_radius * sun_radius;
	phyflt sun_mass = sun_area * sun_area_density;

	PhysicsEngine::phyvec sun_position; // defaults to (0,0)

	for (uint8_t i = 0; i < GAME::SANDBOX::BODIES::ID::TOTAL; i++) {
		phyflt radius = GAME::SANDBOX::BODIES::RADII[i];

		PhysicsEngine::Circle* circle_ptr = new PhysicsEngine::Circle(radius);
		physics_data.shapes.push_back(circle_ptr);

		phyflt area_density = volume_to_area_density(GAME::SANDBOX::BODIES::VOLUME_DENSITIES[i], radius);

		// Add material_ptr to physics_data since it's not a heap allocated ptr
		PhysicsEngine::Material* material_ptr = new PhysicsEngine::Material(0.7f, 0.5f, 0.1f, area_density); // todo: get friction and restitution?
		physics_data.materials.push_back(material_ptr);

		phyflt aphelion = GAME::SANDBOX::BODIES::APHELION_DISTANCES[i];
		phyflt perihelion = GAME::SANDBOX::BODIES::PERIHELION_DISTANCES[i];
		phyflt semimajor_axis = find_semimajor_axis(aphelion, perihelion);

		// Todo: calculate actual position? or get position from save data?
		PhysicsEngine::phyvec position = PhysicsEngine::phyvec{ aphelion, 0.0f };
		PhysicsEngine::phyvec me_to_sun = sun_position - position;

		// Todo: get correct direction
		PhysicsEngine::phyvec velocity_direction = PhysicsEngine::normalise(PhysicsEngine::perpendicular_acw(me_to_sun));

		PhysicsEngine::RigidBody object = PhysicsEngine::RigidBody(circle_ptr, material_ptr, position); //i==GAME::SANDBOX::BODIES::ID::SUN // Make sun immovable? - would break gravity calculation!

		object.ids.assign(GAME::SANDBOX::RIGID_BODY_IDS::TOTAL, 0);
		
		phyflt speed = find_velocity(sun_mass, PhysicsEngine::length(position), semimajor_axis);
		
		if (sun_position != position) {
			object.velocity = speed * velocity_direction;
		}

		// Set render ID
		object.ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] = i;

		// Set category ID
		object.ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] = GAME::SANDBOX::CATEGORIES::PLANET;

		physics_manager.add_body(object);
	}
#endif
}

void GameStage::create_components() {
	// Test component
	PhysicsEngine::Polygon* poly_ptr = new PhysicsEngine::Polygon(GAME::SANDBOX::COMPONENTS::VERTICES[0]); // just a test
	physics_data.shapes.push_back(poly_ptr);

	phyvec position = sandbox_camera.get_position() - phyvec{ 0.0f, 10.0f }; // testing

	PhysicsEngine::Material* material_ptr = &GAME::SANDBOX::DEFAULT_MATERIALS::STEEL; // TODO
	physics_data.materials.push_back(material_ptr);

	PhysicsEngine::RigidBody object = PhysicsEngine::RigidBody(poly_ptr, material_ptr, PhysicsEngine::to_phyvec(position));

	object.ids.assign(GAME::SANDBOX::RIGID_BODY_IDS::TOTAL, 0);


	object.ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE] = static_cast<uint32_t>(ComponentType::COMMAND_MODULE); // test

	// Set category ID
	object.ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] = GAME::SANDBOX::CATEGORIES::COMPONENT;

	// Keep moving at earth speed
	object.velocity = physics_manager.get_bodies()[3].velocity; // messy, TODO // FIXME
	//object.velocity = physics_manager.get_bodies()[0].velocity;

	physics_manager.add_body(object);
}


std::vector<PhysicsEngine::RigidBody> GameStage::create_rocket_rigidbodies(const Rocket& rocket) {
	// Create the physics RigidBodies to be added to the engine
	for (const std::pair<uint32_t, Component>& p : rocket.get_components()) {
		uint32_t index = p.first;
		const Component& component = p.second;

		uint32_t type = static_cast<uint32_t>(component.get_type());

		PhysicsEngine::Shape* shape_ptr = new PhysicsEngine::Polygon(GAME::SANDBOX::COMPONENTS::VERTICES[type]);
		physics_data.shapes.push_back(shape_ptr);

		// Don't add material_ptr to physics_data since it's not a heap allocated ptr
		PhysicsEngine::Material* material_ptr = GAME::SANDBOX::COMPONENTS::MATERIALS[type];

		//PhysicsEngine::RigidBody object = PhysicsEngine::RigidBody(shape_ptr, material_ptr, centre); // TODO

		// todo : set velocity? position?
	}


	return std::vector<PhysicsEngine::RigidBody>(); // TODO
}

std::vector<PhysicsEngine::Constraint> GameStage::create_rocket_constraints(const Rocket& rocket) {
	// TODO 
	return std::vector<PhysicsEngine::Constraint>(); // TODO
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

void GameStage::render_planet(const PhysicsEngine::RigidBody& planet, const Camera& camera, bool map) {
	PhysicsEngine::Circle* circle_ptr = static_cast<PhysicsEngine::Circle*>(planet.shape);
	// For now assume circle
	phyflt radius = circle_ptr->radius * camera.get_scale();
	phyvec position = camera.get_render_position(planet.centre);

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
			graphics_objects->graphics_ptr->render_circle(f_position, f_radius, COLOURS::PLANETS[planet.ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE]]);

			graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::GAME::PLANET_NAMES[planet.ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE]], f_position, COLOURS::WHITE, FONTS::SCALE::PLANET_NAME_FONT);
		}
		else {
			Framework::SDLUtils::SDL_RenderDrawArcInWindow(graphics_objects->graphics_ptr->get_renderer(), f_position, f_radius, WINDOW::SIZE, COLOURS::PLANETS[planet.ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE]]);
		}
	}
}

void GameStage::render_component(const PhysicsEngine::RigidBody& component, const Camera& camera, bool map) {
	uint32_t rocket_id = component.ids[GAME::SANDBOX::RIGID_BODY_IDS::GROUP];
	uint32_t component_type = component.ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE];

	if (map) {
		// Draw small triangle representing command module

		// Only render icon on map if it's a command module
		if (component_type == static_cast<uint32_t>(ComponentType::COMMAND_MODULE)) {

			phyvec position = map_camera.get_render_position(component.centre);

			// TODO: represent rocket instead????
			// TODO: rotate
			std::vector<Framework::vec2> points = convert_poly_vertices_retain_size(GAME::MAP::UI::ICONS::COMMAND_MODULE_VERTICES, component.centre, component.get_rotation_matrix(), map_camera);

			graphics_objects->graphics_ptr->render_poly(points, COLOURS::WHITE);
		}
	}
	else {
		//Framework::vec2 size = PhysicsEngine::to_fvec(GAME::SANDBOX::COMPONENTS::SIZES[component_type] * sandbox_camera.get_scale()); // to change: render from spritesheet?
		//printf("%f, %f\n", size.x, size.y);
		//Framework::Rect render_rect = Framework::Rect(sandbox_camera.get_render_position(PhysicsEngine::to_fvec(component.centre)) - size / 2, size);
		//Framework::Rect screen_rect = Framework::Rect(Framework::VEC_NULL, WINDOW::SIZE);

		//// Not sure if this check speeds it up or slows it down. TODO: test
		//if (Framework::colliding(render_rect, screen_rect)) {
		//	graphics_objects->graphics_ptr->render_rect(render_rect, COLOURS::WHITE); // to improve
		//}

		// Assume poly
		PhysicsEngine::Polygon* poly_ptr = static_cast<PhysicsEngine::Polygon*>(component.shape);
		
		std::vector<Framework::vec2> vertices = convert_poly_vertices(poly_ptr->vertices, component.centre, component.get_rotation_matrix(), sandbox_camera);
		
		graphics_objects->graphics_ptr->render_poly(vertices, COLOURS::WHITE);
	}
}


void GameStage::render_map() {
	Framework::Image map_image = Framework::Image(graphics_objects->graphics_ptr);

	map_image.load(WINDOW::SIZE);

	map_image.fill(COLOURS::BLACK);

	if (!show_map) {
		map_image.set_render_target();
	}

	for (const PhysicsEngine::RigidBody& body : physics_manager.get_bodies()) {
		switch (body.ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY]) {
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

	if (show_map) {
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
	std::vector<std::string> debug_text{
		"DEBUG INFORMATION:",
		"Frames per second: " + Framework::trim_precision(fps, GAME::DEBUG::PRECISION::FPS),
		"Paused: " + std::string(paused ? "yes" : "no"),
		"Time multiplier: " + std::to_string(GAME::SANDBOX::WARP_SPEEDS[time_warp_index]) + "x",
		"Map type: " + std::string(show_map ? "fullscreen" : "minimap"),
		"Map Scale: " + Framework::trim_precision(map_camera.get_scale() * 1e6f, GAME::DEBUG::PRECISION::MAP_SCALE) + "e-6x",
		"Map Position: (" + Framework::trim_precision(map_camera.get_position().x, GAME::DEBUG::PRECISION::MAP_POSITION) + ", " + Framework::trim_precision(map_camera.get_position().y, GAME::DEBUG::PRECISION::MAP_POSITION) + ")",
		"Sandbox Scale: " + Framework::trim_precision(sandbox_camera.get_scale(), GAME::DEBUG::PRECISION::SANDBOX_SCALE) + "x",
		"Sandbox Position: (" + Framework::trim_precision(sandbox_camera.get_position().x, GAME::DEBUG::PRECISION::SANDBOX_POSITION) + ", " + Framework::trim_precision(sandbox_camera.get_position().y, GAME::DEBUG::PRECISION::SANDBOX_POSITION) + ")",
		"Nearest planet: " + STRINGS::GAME::PLANET_NAMES[nearest_planet]
	};

	for (uint8_t i = 0; i < debug_text.size(); i++) {
		float y = i * FONTS::SCALE::DEBUG_FONT * FONTS::SIZE::MAIN_FONT;
		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(debug_text[i], Framework::Vec(0.0f, y), COLOURS::WHITE, FONTS::SCALE::DEBUG_FONT, Framework::Font::AnchorPosition::TOP_LEFT);
	}
}

void GameStage::render_sandbox() {
	graphics_objects->graphics_ptr->fill(COLOURS::BLACK);

	for (const PhysicsEngine::RigidBody& body : physics_manager.get_bodies()) {
		switch (body.ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY]) {
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

	// todo
}

void GameStage::update_map(float dt) {
	// Handle map controls

	// Only allow scrolling/dragging map if we're in fullscreen map mode, or if the mouse is over the minimap
	if (show_map || Framework::colliding(GAME::MAP::UI::MINIMAP::RECT, input->get_mouse()->position())) {
		float maximum_zoom = GAME::MAP::UI::MAXIMUM_ZOOM;
		if (!show_map) maximum_zoom /= GAME::MAP::UI::MINIMAP::EXTRA_ZOOM;

		float minimum_zoom = GAME::MAP::UI::MINIMUM_ZOOM;
		if (!show_map) minimum_zoom /= GAME::MAP::UI::MINIMAP::EXTRA_ZOOM;

		float zoom = input->get_mouse()->scroll_amount() * GAME::MAP::UI::SCROLL_ZOOM_RATE + 1.0f;

		float new_scale = map_camera.get_scale() * zoom;
		float clamped_new_scale = Framework::clamp(new_scale, minimum_zoom, maximum_zoom);

		float actual_zoom = clamped_new_scale / map_camera.get_scale() - 1.0f;

		map_camera.set_scale(clamped_new_scale);

		phyvec new_position = map_camera.get_position() - input->get_mouse()->distance_dragged() / map_camera.get_scale();

		// Adjust map so that zooming always zooms towards the cursor
		Framework::vec2 offset = show_map ? WINDOW::SIZE_HALF : GAME::MAP::UI::MINIMAP::RECT.centre();
		new_position += actual_zoom * (input->get_mouse()->position() - offset) / map_camera.get_scale();

		map_camera.set_position(new_position);
	}
}


void GameStage::update_sandbox(float dt) {
	// Find nearest planet
	phyflt nearest_planet_squared_distance = PHYFLT_MAX;

	for (const PhysicsEngine::RigidBody& body : physics_manager.get_bodies()) {
		if (body.ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] == GAME::SANDBOX::CATEGORIES::PLANET) {
			phyvec current_rocket_pos_to_rename; // TODO: change, get somewhere else
			phyflt squared_distance = PhysicsEngine::length_squared(body.centre - current_rocket_pos_to_rename);
			if (squared_distance < nearest_planet_squared_distance) {
				nearest_planet = body.ids[GAME::SANDBOX::RIGID_BODY_IDS::TYPE];

				nearest_planet_squared_distance = squared_distance;
			}
		}
		else if (body.ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] == GAME::SANDBOX::CATEGORIES::COMPONENT) {
			//Framework::vec2 v = PhysicsEngine::to_fvec(body.centre - sandbox_camera.get_position()) * 1.0f;
			//sandbox_camera.set_position(sandbox_camera.get_position() + v);

			phyvec v = body.centre; // TODO: not centering???
			//printf("v: %f, %f\n", v.x, v.y);
			sandbox_camera.set_position(v);
		}
	}

	// TEST

	// temp:
	//sandbox_camera.set_position(body.centre);
	//printf("%f,%f\n", body.centre.x, body.centre.y);
	for (const PhysicsEngine::RigidBody& body : physics_manager.get_bodies()) {
		if (body.ids[GAME::SANDBOX::RIGID_BODY_IDS::CATEGORY] == GAME::SANDBOX::CATEGORIES::COMPONENT) {
			Framework::vec2 v = PhysicsEngine::to_fvec(body.centre - sandbox_camera.get_position()) * 1.0f;
			sandbox_camera.set_position(sandbox_camera.get_position() + v);
		}
	}

	// TESTING ONLY
	// Only allow scrolling/--dragging-- sandbox if we're not in fullscreen map mode and not clicking on the minimap
	if (!show_map && !Framework::colliding(GAME::MAP::UI::MINIMAP::RECT, input->get_mouse()->position())) {
		float zoom = input->get_mouse()->scroll_amount() * GAME::SANDBOX::UI::SCROLL_ZOOM_RATE + 1.0f;

		float new_scale = sandbox_camera.get_scale() * zoom;
		float clamped_new_scale = Framework::clamp(new_scale, GAME::SANDBOX::UI::MINIMUM_ZOOM, GAME::SANDBOX::UI::MAXIMUM_ZOOM);

		//float actual_zoom = clamped_new_scale / sandbox_camera.get_scale() - 1.0f;

		sandbox_camera.set_scale(clamped_new_scale);

		// NO DRAGGING
		//Framework::vec2 new_position = sandbox_camera.get_position() - input->get_mouse()->distance_dragged() / sandbox_camera.get_scale();

		// Adjust map so that zooming always zooms towards the cursor
		//new_position += actual_zoom * (input->get_mouse()->position() - WINDOW::SIZE_HALF) / sandbox_camera.get_scale();

		//sandbox_camera.set_position(new_position);
	}
}


void GameStage::update_physics(float dt) {
	// Need to find a good method

	// Old method: make jumps bigger:
	/*
	dt = 0.01696f;
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
	uint32_t speed = GAME::SANDBOX::WARP_SPEEDS[time_warp_index];
	uint32_t multiplier = 1;

	while (speed > 10) {
		speed /= 10;
		multiplier *= 10;
	}

	float modified_dt = dt * multiplier;

	//printf("New reps: %u\n", speed);
	//printf("New fps: %f\n", 1.0f / modified_dt);

	for (uint32_t i = 0; i < speed; i++) {
		physics_manager.update(modified_dt);
	}
}

void GameStage::toggle_map() {
	show_map = !show_map;
	
	float zoom_amount = GAME::MAP::UI::MINIMAP::EXTRA_ZOOM;

	// Adjust zoom so that minimap fits similar amount on
	float zoom = show_map ? zoom_amount : 1.0f / zoom_amount;

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

// Allows for simulation of camera when you don't actually have a camera object
// Or allows for tweaking of specific camera attributes temporarily
//std::vector<Framework::vec2> GameStage::convert_poly_vertices(std::vector<phyvec> vertices, const phyvec& centre, const phymat& rotation_matrix, const phyvec& camera_position, phyflt camera_scale) {
//	std::vector<Framework::vec2> new_vertices;
//
//	for (const phyvec& v : vertices) {
//		phyvec a = centre + PhysicsEngine::to_world_space(v, rotation_matrix);
//		new_vertices.push_back(PhysicsEngine::to_fvec((a - camera_position) * camera_scale + WINDOW::SIZE_HALF));
//	}
//
//	return new_vertices;
//}

// Converts icon shapes by rotating and shifting, but keeping spacing of points the same
std::vector<Framework::vec2> GameStage::convert_poly_vertices_retain_size(std::vector<phyvec> vertices, const phyvec& centre, const phymat& rotation_matrix, const Camera& camera) {
	std::vector<Framework::vec2> new_vertices;

	for (const phyvec& v : vertices) {
		phyvec a = PhysicsEngine::to_world_space(v, rotation_matrix) + (centre - camera.get_position()) * camera.get_scale() + WINDOW::SIZE_HALF;
		new_vertices.push_back(PhysicsEngine::to_fvec(a));
	}

	return new_vertices;
}