#pragma once

#include "ExtraMaths.hpp"

struct ComponentNode {
	uint32_t component_id, node_id;
};

struct Connection {
	ComponentNode a, b;
};

// TODO: use something like this:

//struct ComponentNode {
//	uint32_t component_id, node_id;
//};

// Connection struct is the same

// Use ComponentNode for Connections, but use phyvec for Component (node offset)
// Then, node_id is just index of that offset in _nodes;
// Also, component_id is just the index of that component in the map in ComponentManager


class Component {
public:
	Component();
	Component(uint32_t type);

	// Node positions are the offsets from the centre (not centroid) of the component
	const std::vector<phyvec>& get_node_positions() const;
	void set_node_positions(std::vector<phyvec> node_positions);

	uint32_t get_type() const;
	void set_type(uint32_t type);

	phyvec get_position() const;
	void set_position(phyvec position);

private:
	//uint32_t _component_id = 0;
	uint32_t _type = 0;

	std::vector<phyvec> _node_positions;

	phyvec _position;
};


class ComponentManager {
public:
	ComponentManager();

	uint32_t add_component(const Component& component);
	void add_connection(const Connection& connection);

	void erase_component(uint32_t component_id);

	const std::map<uint32_t, Component>& get_components() const;
	const std::vector<Connection>& get_connections() const;

	void set_components(std::map<uint32_t, Component> components);
	void set_connections(std::vector<Connection> connections);

protected:
	std::map<uint32_t, Component> _components;
	std::vector<Connection> _connections;

private:
	uint32_t get_next_component_index() const;
};


// Conversion methods

void to_json(json& j, const ComponentNode& n);
void from_json(const json& j, ComponentNode& n);

void to_json(json& j, const Connection& c);
void from_json(const json& j, Connection& c);

void to_json(json& j, const Component& c);
void from_json(const json& j, Component& c);