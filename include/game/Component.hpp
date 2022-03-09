#pragma once

#include "ExtraMaths.hpp"

struct ComponentNode {
	uint32_t component_id, node_id;

	// Offset from centroid of component
	phyvec offset;
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

	// Note: the order of this matters since it's used in Constants.hpp/cpp
	// Really there shouldn't be two separate things which both need to be the same, but I can't think of a better way of doing this right now.
	enum ComponentType {
		COMMAND_MODULE,
		FUEL_TANK,
		ENGINE,

		TOTAL,
		NONE
	};

	Component();
	Component(ComponentType type, std::vector<ComponentNode> nodes);//uint32_t component_id, 

	const std::vector<ComponentNode>& get_nodes() const;
	void set_nodes(std::vector<ComponentNode> nodes); // TODO: maybe change to use just node positions

	ComponentType get_type() const;
	void set_type(ComponentType type);

	phyvec get_position() const;
	void set_position(phyvec position);

private:
	//uint32_t _component_id = 0;
	ComponentType _type = ComponentType::NONE;

	std::vector<ComponentNode> _nodes;

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