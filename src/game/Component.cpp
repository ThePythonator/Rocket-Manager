#include "Component.hpp"

Component::Component() {

}

Component::Component(uint32_t component_id, std::vector<ComponentNode> nodes, Framework::BaseSprite* sprite) : _component_id(component_id), _nodes(nodes), _sprite(sprite) {

}

void Component::render() const {
	_sprite->render(_position);
}

void Component::set_position(Framework::vec2 position) {
	_position = position;
}



ComponentManager::ComponentManager() {

}

//ComponentManager::~ComponentManager() {
//
//}

void ComponentManager::add_component(const Component& component) {
	_components.push_back(component);
}

void ComponentManager::add_connection(const Connection& connection) {
	_connections.push_back(connection);
}


std::vector<Component> ComponentManager::get_components() {
	return _components;
}

std::vector<Connection> ComponentManager::get_connections() {
	return _connections;
}


void ComponentManager::render() {
	for (const Component& component : _components) {
		component.render();
	}
}