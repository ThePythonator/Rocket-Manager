#pragma once

#include "ExtraMaths.hpp"

struct ComponentNode {
	uint32_t component_id, node_id;
};

struct Connection {
	ComponentNode a, b;

	// If set to true, don't save this
	bool broken = false;
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

	uint32_t get_type() const;
	void set_type(uint32_t type);

	phyvec get_offset() const;
	void set_offset(phyvec offset);

	bool get_broken() const;
	void set_broken() const;

private:
	//uint32_t _component_id = 0;
	uint32_t _type = 0;
	phyvec _offset;
	mutable bool _broken = false; // probably shouldn't really be mutable :/
};


class ComponentManager {
public:
	ComponentManager();

	uint32_t add_component(const Component& component);
	void add_connection(const Connection& connection);

	void erase_component(uint32_t component_id);
	void erase_connection(uint32_t connection_id);

	const std::map<uint32_t, Component>& get_components() const;
	const std::vector<Connection>& get_connections() const;

	void set_components(std::map<uint32_t, Component> components);
	void set_connections(std::vector<Connection> connections);

	Component* get_component_ptr(uint32_t component_id);
	Connection* get_connection_ptr(uint32_t connection_id);

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