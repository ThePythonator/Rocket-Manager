#pragma once

#include "Maths.hpp"
#include "Spritesheet.hpp"

struct ComponentNode {
	uint32_t component_id, node_id;

	// Offset from centre of component (NOT CENTROID!!!!) or should it be centroid??
	//Framework::vec2 offset;
};

struct Connection {
	ComponentNode a, b;
};


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

	ComponentType get_type() const;

	Framework::vec2 get_position() const;
	void set_position(Framework::vec2 position);

private:
	//uint32_t _component_id = 0;
	ComponentType _type = ComponentType::NONE;

	std::vector<ComponentNode> _nodes;

	Framework::vec2 _position;
};

class ComponentManager {
public:
	ComponentManager();

	uint32_t add_component(const Component& component);
	void add_connection(const Connection& connection);

	void erase_component(uint32_t component_id);

	const std::map<uint32_t, Component>& get_components() const;
	const std::vector<Connection>& get_connections() const;

protected:
	std::map<uint32_t, Component> _components;
	std::vector<Connection> _connections;

private:
	uint32_t get_next_component_index() const;
};