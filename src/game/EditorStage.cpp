#include "EditorStage.hpp"

void EditorStage::init() {
	// Set transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);

	// Create camera
	camera = Camera(WINDOW::SIZE);
	camera.set_scale(EDITOR::CAMERA::DEFAULT_SCALE);

	Framework::vec2 position = EDITOR::UI::PALETTE_RECT.bottomleft() - Framework::vec2{ 0.0f, BUTTONS::EDITOR_SIZE.y * BUTTONS::EDITOR::TOTAL };

	for (uint8_t i = 0; i < BUTTONS::EDITOR::TOTAL; i++) {
		Framework::Text save_text = Framework::Text(graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], STRINGS::BUTTONS::EDITOR[i], COLOURS::WHITE);
		Framework::Button save_button = Framework::Button(Framework::Rect(position, BUTTONS::EDITOR_SIZE), graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], save_text, i);
		buttons.push_back(save_button);

		position.y += BUTTONS::EDITOR_SIZE.y;
	}

	

	// Create buttons
	float y = 0.0f;
	for (const std::pair<uint32_t, std::string>& p : GAME::COMPONENTS::NAMES) {
		Framework::Text text = Framework::Text(graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], p.second, COLOURS::WHITE, Framework::Font::CENTER_LEFT, EDITOR::UI::BUTTON_OFFSET);
		Framework::Button button = Framework::Button(Framework::Rect({ 0, y }, BUTTONS::EDITOR_SIZE), graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], text, BUTTONS::EDITOR::TOTAL + p.first);

		buttons.push_back(button);

		y += BUTTONS::EDITOR_SIZE.y;

		// Initialise icon_scales and component_bounding_radii and component_bounding_sizes
		component_bounding_radii[p.first] = PhysicsEngine::find_bounding_radius(GAME::COMPONENTS::VERTICES[p.first]);
		component_bounding_rects[p.first] = find_bounding_rect(GAME::COMPONENTS::VERTICES[p.first]);
		icon_scales[p.first] = 0.5f * BUTTONS::EDITOR_SIZE.y / component_bounding_radii[p.first];
	}
}

void EditorStage::start() {
	// Reset rocket and add a COMMAND_MODULE
	rocket = Rocket();

	rocket.set_is_template(true);

	Component command_module = Component(GAME::COMPONENTS::COMPONENT_TYPE::COMMAND_MODULE);

	rocket.add_component(command_module);

	// Open transition
	transition->open();
}

bool EditorStage::update(float dt) {
	transition->update(dt);

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();

			switch (button_selected) {
			case BUTTONS::EDITOR::SAVE:
				save_rocket();
				break;

			case BUTTONS::EDITOR::EXIT:
				transition->close();
				break;

			default:
				break;
			}


			if (button.get_id() >= BUTTONS::EDITOR::TOTAL) {
				// Assume it was a new-component button pressed

				// Create component
				// TODO: check if ID is actually a module?

				// If component_selected is not none, remove currently selected component
				rocket.erase_component(component_selected);

				// Remove all connections to it
				erase_connections(component_selected);

				uint8_t component_type = button_selected - BUTTONS::EDITOR::TOTAL;

				// Add new component
				component_selected = rocket.add_component({ component_type });
				click_offset = GAME::COMPONENTS::CENTROIDS[component_type];
			}
		}
	}

	// Handle editing

	float line_size = camera.get_scale();

	phyvec converted_mouse_position = camera.get_position_from_render_position(PhysicsEngine::to_phyvec(input->get_mouse()->position()));

	// Set position of currently selected component to cursor position
	if (component_selected != EDITOR::NO_COMPONENT_SELECTED) {
		rocket.get_component_ptr(component_selected)->set_offset(converted_mouse_position - click_offset);
	}

	if (input->just_down(Framework::MouseHandler::MouseButton::LEFT)) {
		// If colliding with any object then pick it up
		for (const std::pair<uint32_t, Component>& p : rocket.get_components()) {

			if (colliding(Framework::Rect(PhysicsEngine::to_fvec(p.second.get_offset() + GAME::COMPONENTS::CENTROIDS[p.second.get_type()] + component_bounding_rects[p.second.get_type()].position), component_bounding_rects[p.second.get_type()].size), PhysicsEngine::to_fvec(converted_mouse_position))) {
				component_selected = p.first;
				click_offset = converted_mouse_position - p.second.get_offset();

				// Remove all connections attached to this component
				erase_connections(component_selected);

				break;
			}
		}
	}
	else if (input->just_up(Framework::MouseHandler::MouseButton::LEFT)) {

		if (component_selected != EDITOR::NO_COMPONENT_SELECTED) {
			Component* component_ptr = rocket.get_component_ptr(component_selected);

			// These two checks only compare the rect to the centre of the component (i.e. allowing some overlap before deleting)
			// If we dropped it onto the bin, remove it
			if (colliding(EDITOR::UI::BIN_RECT, PhysicsEngine::to_fvec(camera.get_render_position(component_ptr->get_offset() + GAME::COMPONENTS::CENTROIDS[component_ptr->get_type()])))) {
				rocket.erase_component(component_selected);
			}
			// If we dragged it onto the palette, remove it
			if (colliding(EDITOR::UI::PALETTE_RECT, PhysicsEngine::to_fvec(camera.get_render_position(component_ptr->get_offset() + GAME::COMPONENTS::CENTROIDS[component_ptr->get_type()])))) {
				rocket.erase_component(component_selected);
			}

			// Snap position to grid
			phyvec offset = component_ptr->get_offset();

			offset.x = std::round(offset.x);
			offset.y = std::round(offset.y);

			component_ptr->set_offset(offset);


			// Add all connections from this component to nodes which overlap (i.e. rect intersect)
			for (const std::pair<uint32_t, Component>& p : rocket.get_components()) {
				// Check we're not connecting component to itself!
				if (p.first != component_selected) {

					uint8_t node_id_theirs = 0;
					for (const phyvec& v_theirs : GAME::COMPONENTS::NODE_POSITIONS[p.second.get_type()]) {

						uint8_t node_id_mine = 0;
						// Check if rect intersects with each of our nodes, if so then add connection
						for (const phyvec& v_mine : GAME::COMPONENTS::NODE_POSITIONS[component_ptr->get_type()]) {

							if (Framework::colliding(Framework::centred_rect(PhysicsEngine::to_fvec(v_mine + component_ptr->get_offset() + GAME::COMPONENTS::CENTROIDS[component_ptr->get_type()]), EDITOR::UI::NODE_SIZE / camera.get_scale()), Framework::centred_rect(PhysicsEngine::to_fvec(v_theirs + p.second.get_offset() + GAME::COMPONENTS::CENTROIDS[p.second.get_type()]), EDITOR::UI::NODE_SIZE / camera.get_scale()))) {
								rocket.add_connection({ { component_selected, node_id_mine }, { p.first, node_id_theirs } });
							}

							node_id_mine++;
						}

						node_id_theirs++;
					}
				}
			}


			// If currently selected something, drop it
			component_selected = EDITOR::NO_COMPONENT_SELECTED;
		}
	}

	if (transition->is_closed()) {
		switch (button_selected) {
		case BUTTONS::EDITOR::EXIT:
			finish(new TitleStage());
			break;

		default:
			break;
		}
	}

	return true;
}

void EditorStage::render() {
	graphics_objects->graphics_ptr->fill(COLOURS::EDITOR_GREY);

	render_grid();

	render_ui();

	render_rocket();

	transition->render();
}


void EditorStage::render_grid() {
	// Draw a line every metre
	float line_size = camera.get_scale();

	Framework::vec2 line_counts = WINDOW::SIZE / line_size;

	// TODO: centre on camera

	float i_have_made_this_code_so_bad_help = 5.0f;

	for (float x = i_have_made_this_code_so_bad_help; x < WINDOW::SIZE_HALF.x; x += line_size) {
		// Render vertical line
		graphics_objects->graphics_ptr->render_line({ WINDOW::SIZE_HALF.x + x, 0.0f }, { WINDOW::SIZE_HALF.x + x, WINDOW::SIZE.y }, COLOURS::EDITOR_GRID_COLOUR);
		graphics_objects->graphics_ptr->render_line({ WINDOW::SIZE_HALF.x - x, 0.0f }, { WINDOW::SIZE_HALF.x - x, WINDOW::SIZE.y }, COLOURS::EDITOR_GRID_COLOUR);
	}
	for (float y = 0.0f; y < WINDOW::SIZE_HALF.y; y += line_size) {
		// Render horizontal line
		graphics_objects->graphics_ptr->render_line({ 0.0f, WINDOW::SIZE_HALF.y + y }, { WINDOW::SIZE.x, WINDOW::SIZE_HALF.y + y }, COLOURS::EDITOR_GRID_COLOUR);
		graphics_objects->graphics_ptr->render_line({ 0.0f, WINDOW::SIZE_HALF.y - y }, { WINDOW::SIZE.x, WINDOW::SIZE_HALF.y - y }, COLOURS::EDITOR_GRID_COLOUR);
	}
}


void EditorStage::render_ui() {
	// Background for palette
	graphics_objects->graphics_ptr->fill(EDITOR::UI::PALETTE_RECT, COLOURS::EDITOR_PALETTE_GREY);

	// Buttons
	for (const Framework::Button& button : buttons) {
		button.render();

		// Render mini component icon over top

		if (button.get_id() >= BUTTONS::EDITOR::TOTAL) {
			uint8_t id = button.get_id() - BUTTONS::EDITOR::TOTAL;

			std::vector<Framework::vec2> vertices;
			for (const phyvec& v : GAME::COMPONENTS::VERTICES[id]) {
				vertices.push_back(PhysicsEngine::to_fvec(v * icon_scales[id]) + button.position() + EDITOR::UI::BUTTON_ICON_OFFSET);
			}

			graphics_objects->graphics_ptr->render_poly(vertices, COLOURS::WHITE);
		}
	}

	// Bin icon
	graphics_objects->spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::MAIN_SPRITESHEET]->sprite(SPRITES::INDEX::BIN, EDITOR::UI::BIN_RECT.position / SPRITES::SCALE);
}


void EditorStage::render_rocket() {
	for (const std::pair<uint32_t, Component>& c : rocket.get_components()) {
		render_component(c.second);
	}

	// Render connected nodes yellow
	for (const Connection& c : rocket.get_connections()) {
		Component* a = rocket.get_component_ptr(c.a.component_id);
		Component* b = rocket.get_component_ptr(c.b.component_id);

		const phyvec& v_a = GAME::COMPONENTS::NODE_POSITIONS[a->get_type()][c.a.node_id];
		const phyvec& v_b = GAME::COMPONENTS::NODE_POSITIONS[b->get_type()][c.b.node_id];

		const phyvec& centroid_a = GAME::COMPONENTS::CENTROIDS[a->get_type()];
		const phyvec& centroid_b = GAME::COMPONENTS::CENTROIDS[b->get_type()];

		// Rendering at probably same place so do I need both??
		graphics_objects->graphics_ptr->render_filled_rect(Framework::centred_rect(PhysicsEngine::to_fvec(camera.get_render_position(v_a + a->get_offset() + centroid_a)), EDITOR::UI::NODE_SIZE), COLOURS::YELLOW);
		graphics_objects->graphics_ptr->render_filled_rect(Framework::centred_rect(PhysicsEngine::to_fvec(camera.get_render_position(v_b + b->get_offset() + centroid_b)), EDITOR::UI::NODE_SIZE), COLOURS::YELLOW);
	}
}


void EditorStage::render_component(const Component& component) {
	uint32_t component_type = component.get_type();

	phyvec centroid = GAME::COMPONENTS::CENTROIDS[component_type];

	// TODO: rotate if needed later?

	// Render component
	std::vector<Framework::vec2> vertices;

	for (const phyvec& v : GAME::COMPONENTS::VERTICES[component_type]) {
		vertices.push_back(PhysicsEngine::to_fvec(camera.get_render_position(v + component.get_offset() + centroid)));
	}

	graphics_objects->graphics_ptr->render_poly(vertices, COLOURS::WHITE);

	// Render all nodes white
	for (const phyvec& v : GAME::COMPONENTS::NODE_POSITIONS[component_type]) {
		graphics_objects->graphics_ptr->render_filled_rect(Framework::centred_rect(PhysicsEngine::to_fvec(camera.get_render_position(v + component.get_offset() + centroid)), EDITOR::UI::NODE_SIZE), COLOURS::WHITE);
	}
}



void EditorStage::erase_connections(uint32_t component_id) {
	std::vector<uint32_t> to_erase;
	uint32_t connection_id = 0;
	for (const Connection& c : rocket.get_connections()) {
		if (c.a.component_id == component_id || c.b.component_id == component_id) {
			to_erase.push_back(connection_id);
		}

		connection_id++;
	}

	for (int i = to_erase.size() - 1; i >= 0; i--) {
		rocket.erase_connection(to_erase[i]);
	}
}


void EditorStage::save_rocket() {
	// TODO: set name, no pretty print
	Framework::JSONHandler::write(PATHS::BASE_PATH + PATHS::ROCKET_TEMPLATES::LOCATION + "a_test" + PATHS::ROCKET_TEMPLATES::EXTENSION, rocket, true);
}