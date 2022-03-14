#include "EditorStage.hpp"

void EditorStage::init() {
	// Set transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);

	// Create camera
	camera = Camera(WINDOW::SIZE);
	camera.set_scale(EDITOR::CAMERA::DEFAULT_SCALE);

	// Create buttons
	float y = 0.0f;
	for (const std::pair<uint32_t, std::string>& p : GAME::COMPONENTS::NAMES) {
		Framework::Text text = Framework::Text(graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], p.second, COLOURS::WHITE, Framework::Font::CENTER_LEFT, EDITOR::UI::BUTTON_OFFSET);
		Framework::Button button = Framework::Button(Framework::Rect({ 0, y }, BUTTONS::EDITOR_SIZE), graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], text, p.first);

		buttons.push_back(button);

		y += BUTTONS::EDITOR_SIZE.y;

		// Initialise icon_scales and component_bounding_radii
		component_bounding_radii[p.first] = PhysicsEngine::find_bounding_radius(GAME::COMPONENTS::VERTICES[p.first]);
		icon_scales[p.first] = 0.5f * BUTTONS::EDITOR_SIZE.y / component_bounding_radii[p.first];
	}
}

void EditorStage::start() {
	// Reset rocket and add a COMMAND_MODULE
	rocket = Rocket();

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

			// Create module
			// TODO: check if ID is actually a module?

			// If component_selected is not none, remove currently selected component
			rocket.erase_component(component_selected);

			// Add new component
			component_selected = rocket.add_component({ button_selected });
		}
	}

	// Handle editing

	float line_size = camera.get_scale();

	phyvec converted_mouse_position = camera.get_position_from_render_position(PhysicsEngine::to_phyvec(input->get_mouse()->position()));

	// Set position of currently selected component to cursor position
	if (component_selected != EDITOR::NO_COMPONENT_SELECTED) {
		rocket.get_component_ptr(component_selected)->set_offset(converted_mouse_position);
	}

	if (input->just_down(Framework::MouseHandler::MouseButton::LEFT)) {
		// If colliding with any object then pick it up
		for (const std::pair<uint32_t, Component>& p : rocket.get_components()) {

			if (colliding(p.second.get_offset(), component_bounding_radii[p.second.get_type()], converted_mouse_position)) {
				component_selected = p.first;
				break;
			}
		}
	}
	else if (input->just_up(Framework::MouseHandler::MouseButton::LEFT)) {
		// If we dropped it onto the bin, remove it
		Component* component_ptr = rocket.get_component_ptr(component_selected);

		if (colliding(EDITOR::UI::BIN_RECT, PhysicsEngine::to_fvec(camera.get_render_position(component_ptr->get_offset())))) {
			rocket.erase_component(component_selected);
		}

		// Snap position to grid
		phyvec offset = component_ptr->get_offset();
		offset.x = std::round(offset.x);
		offset.y = std::round(offset.y);
		component_ptr->set_offset(offset);

		// TODO: issue: currently snapping to centroid. need to somehow store an 'origin' point which lies on grid, then snap that to grid (also shift by centroid when loading, so can unshift when snapping).
		
		// If currently selected something, drop it
		component_selected = EDITOR::NO_COMPONENT_SELECTED;
	}

	return true;
}

void EditorStage::render() {
	graphics_objects->graphics_ptr->fill(COLOURS::EDITOR_GREY);

	render_grid();

	render_ui();

	render_rocket(rocket);

	transition->render();
}


void EditorStage::render_grid() {
	// Draw a line every metre
	float line_size = camera.get_scale();

	// TODO: centre on camera

	for (float x = 0; x < WINDOW::SIZE.x; x += line_size) {
		// Render vertical line
		graphics_objects->graphics_ptr->render_line({ x, 0.0f }, { x, WINDOW::SIZE.y }, COLOURS::EDITOR_GRID_COLOUR);
	}
	for (float y = 0; y < WINDOW::SIZE.y; y += line_size) {
		// Render horizontal line
		graphics_objects->graphics_ptr->render_line({ 0.0f, y }, { WINDOW::SIZE.x, y }, COLOURS::EDITOR_GRID_COLOUR);
	}
}


void EditorStage::render_ui() {
	// Background for palette
	graphics_objects->graphics_ptr->fill(EDITOR::UI::PALETTE_RECT, COLOURS::EDITOR_PALETTE_GREY);

	// Buttons
	for (const Framework::Button& button : buttons) {
		button.render();

		// Render mini component icon over top

		std::vector<Framework::vec2> vertices;
		for (const phyvec& v : GAME::COMPONENTS::VERTICES[button.get_id()]) {
			vertices.push_back(PhysicsEngine::to_fvec(v * icon_scales[button.get_id()]) + button.position() + EDITOR::UI::BUTTON_ICON_OFFSET);
		}

		graphics_objects->graphics_ptr->render_poly(vertices, COLOURS::WHITE);
	}

	// Bin icon
	graphics_objects->spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::MAIN_SPRITESHEET]->sprite(SPRITES::INDEX::BIN, EDITOR::UI::BIN_RECT.position / SPRITES::SCALE);
}


void EditorStage::render_rocket(const Rocket& rocket) {
	for (const std::pair<uint32_t, Component>& c : rocket.get_components()) {
		render_component(c.second);
	}
}


void EditorStage::render_component(const Component& component) {
	uint32_t component_type = component.get_type();

	// TODO: rotate if needed later?

	// Render component
	std::vector<Framework::vec2> vertices;

	for (const phyvec& v : GAME::COMPONENTS::VERTICES[component_type]) {
		vertices.push_back(PhysicsEngine::to_fvec(camera.get_render_position(v + component.get_offset())));
	}

	graphics_objects->graphics_ptr->render_poly(vertices, COLOURS::WHITE);

	// Render nodes
	for (const phyvec& v : GAME::COMPONENTS::NODE_POSITIONS[component_type]) {
		graphics_objects->graphics_ptr->render_filled_rect(Framework::centred_rect(PhysicsEngine::to_fvec(camera.get_render_position(v + component.get_offset())), 5.0f), COLOURS::WHITE); // TODO : change size to constant and colour to change if in use
	}
}