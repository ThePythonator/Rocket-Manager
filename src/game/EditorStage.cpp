#include "EditorStage.hpp"

void EditorStage::start() {
	// Reset rocket and add a COMMAND_MODULE
	rocket = Rocket();

	//command_module = Component(Component::ComponentType::COMMAND_MODULE, GAME::SANDBOX::COMPONENTS::NODES);

	//rocket.add_component(command_module);
}

bool EditorStage::update(float dt) {
	return true;
}

void EditorStage::render() {
	render_rocket(rocket);
}


void EditorStage::render_rocket(const Rocket& r) {
	for (const std::pair<uint32_t, Component>& c : rocket.get_components()) {
		render_component(c);
	}
}


void EditorStage::render_component(const std::pair<uint32_t, Component>& c) {
	uint32_t component_id = c.first;
	const Component& component = c.second;

	// Assume poly
	//PhysicsEngine::Polygon* poly_ptr = static_cast<PhysicsEngine::Polygon*>(component.shape);

	// TODO: rotate if needed later?

	std::vector<Framework::vec2> vertices;

	for (const phyvec& v : GAME::COMPONENTS::VERTICES[component_id]) {
		vertices.push_back(PhysicsEngine::to_fvec(camera.get_render_position(component.get_position())));
	}

	graphics_objects->graphics_ptr->render_poly(vertices, COLOURS::WHITE);
}