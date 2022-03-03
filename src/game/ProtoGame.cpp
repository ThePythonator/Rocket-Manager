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

	// Set camera position

	// Just for testing
	map_camera.set_position(PhysicsEngine::to_fvec2({ GAME::SANDBOX::BODIES::APHELION_DISTANCES[3] * GAME::SANDBOX::UNIVERSE_SCALE, 0.0 }));
	map_camera.set_scale(1e-5f);

	// TODO
	sandbox_camera.set_position(PhysicsEngine::to_fvec2({ GAME::SANDBOX::BODIES::APHELION_DISTANCES[3] * GAME::SANDBOX::UNIVERSE_SCALE, -GAME::SANDBOX::BODIES::RADII[3] * GAME::SANDBOX::UNIVERSE_SCALE }));
	sandbox_camera.set_scale(1.0f);

	// Create Solar System
	create_solar_system(GAME::SANDBOX::UNIVERSE_SCALE);

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
	update_sandbox_scale(dt); // TESTING

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




void GameStage::create_solar_system(float scale) {

	// TODO: create method to load system from object positions
	float sun_radius = GAME::SANDBOX::BODIES::RADII[GAME::SANDBOX::BODIES::ID::SUN] * scale;
	float sun_area_density = volume_to_area_density(GAME::SANDBOX::BODIES::VOLUME_DENSITIES[GAME::SANDBOX::BODIES::ID::SUN], sun_radius);
	float sun_area = PhysicsEngine::PI * sun_radius * sun_radius;
	float sun_mass = sun_area * sun_area_density;

	// testnig
	//float r = 1e13f;
	//float m1 = 2e30f;
	//float m2 = 1e29f;
	//// need to use double in background otherwise overflow
	//float big = PhysicsEngine::gravitational_force(m1, m2, r * r);
	//printf("largest: %f\n", big);

	PhysicsEngine::dvec2 sun_position; // defaults to (0,0)

	for (uint8_t i = 0; i < GAME::SANDBOX::BODIES::ID::TOTAL; i++) {
		float radius = GAME::SANDBOX::BODIES::RADII[i] * scale;

		PhysicsEngine::Circle* circle_ptr = new PhysicsEngine::Circle(radius);
		physics_data.shapes.push_back(circle_ptr);

		float area_density = volume_to_area_density(GAME::SANDBOX::BODIES::VOLUME_DENSITIES[i], radius);

		PhysicsEngine::Material* material_ptr = new PhysicsEngine::Material(0.7f, 0.5f, 0.1f, area_density); // todo: get friction and restitution
		physics_data.materials.push_back(material_ptr);

		float aphelion = GAME::SANDBOX::BODIES::APHELION_DISTANCES[i] * scale;
		float perihelion = GAME::SANDBOX::BODIES::PERIHELION_DISTANCES[i] * scale;
		float semimajor_axis = find_semimajor_axis(aphelion, perihelion);

		// Todo: calculate actual position? or get position from save data?
		PhysicsEngine::dvec2 position = PhysicsEngine::dvec2{ aphelion, 0.0f };
		PhysicsEngine::dvec2 me_to_sun = sun_position - position;

		// Todo: get correct direction
		PhysicsEngine::dvec2 velocity_direction = PhysicsEngine::normalise(PhysicsEngine::perpendicular_acw(me_to_sun));

		PhysicsEngine::RigidBody object = PhysicsEngine::RigidBody(circle_ptr, material_ptr, position); //i==GAME::SANDBOX::BODIES::ID::SUN // Make sun immovable? - would break gravity calculation!
		
		float speed = find_velocity(sun_mass, PhysicsEngine::length(position), semimajor_axis);
		float TEST = 0.5f;
		if (sun_position != position) {
			object.velocity = speed * velocity_direction * TEST; // testing
		}

		// Set render ID
		object.render_id = i;

		// Set category ID
		object.category_id = GAME::SANDBOX::CATEGORIES::PLANET;
		if (i == 3)
		physics_manager.add_body(object);
	}
}

void GameStage::create_components() {
	PhysicsEngine::Polygon* poly_ptr = PhysicsEngine::create_rect(PhysicsEngine::to_dvec2(GAME::SANDBOX::COMPONENTS::SIZES[0])); // just a test
	physics_data.shapes.push_back(poly_ptr);

	Framework::vec2 position = sandbox_camera.get_position() - Framework::Vec(0.0f, GAME::SANDBOX::COMPONENTS::SIZES[0].y / 2); // testing

	PhysicsEngine::Material* material_ptr = new PhysicsEngine::Material(0.5f, 0.3f, 0.2f, 2000); // todo
	physics_data.materials.push_back(material_ptr);

	PhysicsEngine::RigidBody object = PhysicsEngine::RigidBody(poly_ptr, material_ptr, PhysicsEngine::to_dvec2(position));

	object.render_id = 0; // test

	// Set category ID
	object.category_id = GAME::SANDBOX::CATEGORIES::COMPONENT;

	// Keep moving at earth speed
	//object.velocity = physics_manager.get_bodies()[3].velocity;
	object.velocity = physics_manager.get_bodies()[0].velocity;

	physics_manager.add_body(object);
}

float GameStage::find_eccentricity(float aphelion, float perihelion) {
	return (aphelion - perihelion) / (aphelion + perihelion);
}

float GameStage::find_semimajor_axis(float aphelion, float perihelion) {
	return (aphelion + perihelion) / 2.0f;
}

float GameStage::find_velocity(float primary_mass, float radius, float semimajor_axis) {
	return std::sqrt(primary_mass * GAME::SANDBOX::GRAVITATIONAL_CONSTANT * (2.0f / radius - 1.0f / semimajor_axis));
}

float GameStage::volume_to_area_density(float volume_density, float radius) {
	return volume_density * 4.0f * radius / 3.0f;
}


//void GameStage::render_physics_objects() {
//	// todo?
//}

void GameStage::render_planet(const PhysicsEngine::RigidBody& planet, const Camera& camera, bool map) {
	PhysicsEngine::Circle* circle_ptr = static_cast<PhysicsEngine::Circle*>(planet.shape);
	// For now assume circle
	float radius = circle_ptr->radius * camera.get_scale();
	Framework::vec2 position = camera.get_render_position(PhysicsEngine::to_fvec2(planet.centre));

	// Get bounding rect to check if on screen
	Framework::Rect bounding_rect = Framework::SDLUtils::get_bounds(position, radius);
	Framework::Rect screen_rect = Framework::Rect(Framework::VEC_NULL, WINDOW::SIZE);

	// Check if on screen
	if (Framework::colliding(bounding_rect, screen_rect)) {
		// Render planet
		
		// If map is set, draw normal circle, otherwise draw arc in window
		if (map) {
			graphics_objects->graphics_ptr->render_circle(position, radius, COLOURS::PLANETS[planet.render_id]);
		}
		else {
			Framework::SDLUtils::SDL_RenderDrawArcInWindow(graphics_objects->graphics_ptr->get_renderer(), position, radius, WINDOW::SIZE, COLOURS::PLANETS[planet.render_id]);
		}

		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(STRINGS::GAME::PLANET_NAMES[planet.render_id], position, COLOURS::WHITE, FONTS::SCALE::PLANET_NAME_FONT);
	}
}

void GameStage::render_component(const PhysicsEngine::RigidBody& component, const Camera& camera, bool map) {
	// For now assume rect
	//PhysicsEngine::Polygon* poly_ptr = static_cast<PhysicsEngine::Polygon*>(component.shape);

	if (map) {
		// Draw small triangle representing componenet

		Framework::vec2 position = map_camera.get_render_position(PhysicsEngine::to_fvec2(component.centre));

		// TODO: represent rocket instead????
		std::vector<Framework::vec2> points{
			position + Framework::Vec(0, -2),
			position + Framework::Vec(-3, 4),
			position + Framework::Vec(3, 4)
		};

		graphics_objects->graphics_ptr->render_poly(points, COLOURS::WHITE);
	}
	else {
		Framework::vec2 size = GAME::SANDBOX::COMPONENTS::SIZES[component.render_id] * sandbox_camera.get_scale(); // to change: render from spritesheet?

		Framework::Rect render_rect = Framework::Rect(sandbox_camera.get_render_position(PhysicsEngine::to_fvec2(component.centre)) - size / 2, size);
		Framework::Rect screen_rect = Framework::Rect(Framework::VEC_NULL, WINDOW::SIZE);

		// Not sure if this check speeds it up or slows it down. TODO: test
		if (Framework::colliding(render_rect, screen_rect)) {
			graphics_objects->graphics_ptr->render_rect(render_rect, COLOURS::WHITE); // to improve
		}
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
		switch (body.category_id) {
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
		"Map Scale: " + Framework::trim_precision(map_camera.get_scale() * 1e6f, GAME::DEBUG::PRECISION::MAP_SCALE) + "e-6x",
		"Map Position: (" + Framework::trim_precision(map_camera.get_position().x, GAME::DEBUG::PRECISION::MAP_POSITION) + ", " + Framework::trim_precision(map_camera.get_position().y, GAME::DEBUG::PRECISION::MAP_POSITION) + ")",
		"Sandbox Scale: " + Framework::trim_precision(sandbox_camera.get_scale(), GAME::DEBUG::PRECISION::SANDBOX_SCALE) + "x",
		"Sandbox Position: (" + Framework::trim_precision(sandbox_camera.get_position().x, GAME::DEBUG::PRECISION::SANDBOX_POSITION) + ", " + Framework::trim_precision(sandbox_camera.get_position().y, GAME::DEBUG::PRECISION::SANDBOX_POSITION) + ")",
		"Time multiplier: " + std::to_string(GAME::SANDBOX::WARP_SPEEDS[time_warp_index]) + "x",
		"Paused: " + std::string(paused ? "yes" : "no"),
		"Map type: " + std::string(show_map ? "fullscreen" : "minimap")
	};

	for (uint8_t i = 0; i < debug_text.size(); i++) {
		float y = i * FONTS::SCALE::DEBUG_FONT * FONTS::SIZE::MAIN_FONT;
		graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT]->render_text(debug_text[i], Framework::Vec(0.0f, y), COLOURS::WHITE, FONTS::SCALE::DEBUG_FONT, Framework::Font::AnchorPosition::TOP_LEFT);
	}
}

void GameStage::render_sandbox() {
	graphics_objects->graphics_ptr->fill(COLOURS::BLACK);

	for (const PhysicsEngine::RigidBody& body : physics_manager.get_bodies()) {
		switch (body.category_id) {
		case GAME::SANDBOX::CATEGORIES::PLANET:
			render_planet(body, sandbox_camera, false);
			break;

		case GAME::SANDBOX::CATEGORIES::COMPONENT:
		{
			render_component(body, sandbox_camera, false);
			break;
		}
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

		Framework::vec2 new_position = map_camera.get_position() - input->get_mouse()->distance_dragged() / map_camera.get_scale();

		// Adjust map so that zooming always zooms towards the cursor
		Framework::vec2 offset = show_map ? WINDOW::SIZE_HALF : GAME::MAP::UI::MINIMAP::RECT.centre();
		new_position += actual_zoom * (input->get_mouse()->position() - offset) / map_camera.get_scale();

		map_camera.set_position(new_position);
	}
}


void GameStage::update_sandbox_scale(float dt) {
	// TEST

	// temp:
	//sandbox_camera.set_position(body.centre);
	//printf("%f,%f\n", body.centre.x, body.centre.y);
	for (const PhysicsEngine::RigidBody& body : physics_manager.get_bodies()) {
		if (body.category_id == GAME::SANDBOX::CATEGORIES::COMPONENT) {
			Framework::vec2 v = PhysicsEngine::to_fvec2(body.centre - sandbox_camera.get_position()) * 1.0f;
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
	// Old method: make jumps bigger:
	physics_manager.update(dt * GAME::SANDBOX::WARP_SPEEDS[time_warp_index]);

	// Other method: iterate 10 times on each frame
	// Means physics is good up to 10x speed
	/*float step = dt * GAME::SANDBOX::WARP_SPEEDS[time_warp_index] / 10.0f;
	for (uint8_t i = 0; i < 10; i++) {
		physics_manager.update(step);
	}*/

	/*
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
	*/
}

void GameStage::toggle_map() {
	show_map = !show_map;
	
	float zoom_amount = GAME::MAP::UI::MINIMAP::EXTRA_ZOOM;

	// Adjust zoom so that minimap fits similar amount on
	float zoom = show_map ? zoom_amount : 1.0f / zoom_amount;

	map_camera.set_scale(zoom * map_camera.get_scale());
}