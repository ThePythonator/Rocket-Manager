#include "Component.hpp"

Component::Component() {

}
Component::Component(ComponentType type, std::vector<ComponentNode> nodes) : _type(type), _nodes(nodes) {//uint32_t component_id, //_component_id(component_id),

}
const std::vector<ComponentNode>& Component::get_nodes() const {
	return _nodes;
}

void Component::set_nodes(std::vector<ComponentNode> nodes) {
	_nodes = nodes;
}

void Component::set_type(ComponentType type) {
	_type = type;
}

Component::ComponentType Component::get_type() const {
	return _type;
}

phyvec Component::get_position() const {
	return _position;
}

void Component::set_position(phyvec position) {
	_position = position;
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

void ComponentManager::set_components(std::map<uint32_t, Component> components) {
	_components = components;
}

void ComponentManager::set_connections(std::vector<Connection> connections) {
	_connections = connections;
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
		{"node_id", n.node_id},
		{"offset", n.offset}
	};
}
void from_json(const json& j, ComponentNode& n) {
	j.at("component_id").get_to(n.component_id);
	j.at("node_id").get_to(n.node_id);
	j.at("offset").get_to(n.offset);
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
		{"nodes", c.get_nodes()},
		{"position", c.get_position()}
	};
}
void from_json(const json& j, Component& c) {
	c.set_type(static_cast<Component::ComponentType>(j.at("type").get<uint32_t>()));
	c.set_nodes(j.at("nodes").get<std::vector<ComponentNode>>());
	c.set_position(j.at("position").get<phyvec>());
}