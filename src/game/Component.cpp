#include "Component.hpp"

Component::Component() {

}
Component::Component(uint32_t type) : _type(type) {

}

void Component::set_type(uint32_t type) {
	_type = type;
}

uint32_t Component::get_type() const {
	return _type;
}

phyvec Component::get_offset() const {
	return _offset;
}

void Component::set_offset(phyvec offset) {
	_offset = offset;
}

bool Component::get_broken() const {
	return _broken;
}
void Component::set_broken() const {
	_broken = true;
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

void ComponentManager::erase_connection(uint32_t connection_id) {
	_connections.erase(_connections.begin() + connection_id);
}

const std::map<uint32_t, Component>& ComponentManager::get_components() const {
	return _components;
}

const std::vector<Connection>& ComponentManager::get_connections() const {
	return _connections;
}

void ComponentManager::set_components(std::map<uint32_t, Component> components) {
	_components = components;
}

void ComponentManager::set_connections(std::vector<Connection> connections) {
	_connections = connections;
}

Component* ComponentManager::get_component_ptr(uint32_t component_id) {
	return &_components[component_id];
}

Connection* ComponentManager::get_connection_ptr(uint32_t connection_id) {
	return &_connections[connection_id];
}

uint32_t ComponentManager::get_next_component_index() const {
	// Get next free index in map
	uint32_t i = 0;
	while (_components.count(i)) {
		i++;
	}
	return i;
}

// Conversion methods

void to_json(json& j, const ComponentNode& n) {
	j = json{
		{"component_id", n.component_id},
		{"node_id", n.node_id}
	};
}
void from_json(const json& j, ComponentNode& n) {
	j.at("component_id").get_to(n.component_id);
	j.at("node_id").get_to(n.node_id);
}

void to_json(json& j, const Connection& c) {
	j = json{
		{"a", c.a},
		{"b", c.b}
	};
}
void from_json(const json& j, Connection& c) {
	j.at("a").get_to(c.a);
	j.at("b").get_to(c.b);
}

void to_json(json& j, const Component& c) {
	j = json{
		{"type", c.get_type()},
		{"offset", c.get_offset()}
	};
}
void from_json(const json& j, Component& c) {
	c.set_type(j.at("type").get<uint32_t>());
	c.set_offset(j.at("offset").get<phyvec>());
}