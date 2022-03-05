#include "Component.hpp"

Component::Component() {

}
Component::Component(ComponentType type, std::vector<ComponentNode> nodes) : _type(type), _nodes(nodes) {//uint32_t component_id, //_component_id(component_id),

}
const std::vector<ComponentNode>& Component::get_nodes() const {
	return _nodes;
}

ComponentType Component::get_type() const {
	return _type;
}



ComponentManager::ComponentManager() {

}

uint32_t ComponentManager::add_component(const Component& component) {
	uint32_t index = get_next_component_index();
	_components[index] = component;
	return index;
}

void ComponentManager::add_connection(const Connection& connection) {
	_connections.push_back(connection);
}

void ComponentManager::erase_component(uint32_t component_id) {
	_components.erase(component_id);
}

const std::map<uint32_t, Component>& ComponentManager::get_components() const {
	return _components;
}

const std::vector<Connection>& ComponentManager::get_connections() const {
	return _connections;
}

uint32_t ComponentManager::get_next_component_index() const {
	// Get next free index in map
	uint32_t i = 0;
	while (!_components.count(i)) {
		i++;
	}
	return i;
}