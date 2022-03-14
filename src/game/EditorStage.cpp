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

		// Initialise icon_scales
		icon_scales[p.first] = 0.5f * BUTTONS::EDITOR_SIZE.y / PhysicsEngine::find_bounding_radius(GAME::COMPONENTS::VERTICES[p.first]);
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

			
		}
	}

	return true;
}

void EditorStage::render() {
	graphics_objects->graphics_ptr->fill(COLOURS::EDITOR_GREY);

	render_palette();

	render_rocket(rocket);

	transition->render();
}


void EditorStage::render_palette() {
	// Background
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

	// Stuff
	//for (uint8_t i = 0; i < GAME::COMPONENTS::NAMES.size(); i++) {
	//	Component c = Component(i);
	//	c.set_offset({10.0f, 10.0f + i * 10.0f}); // todo

	//	render_component(c);
	//}
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