#pragma once

#include "Maths.hpp"
#include "Spritesheet.hpp"

struct ComponentNode {
	uint32_t component_id, node_id;
};

struct Connection {
	ComponentNode a, b;
};

class Component {
public:
	Component();
	Component(uint32_t component_id, std::vector<ComponentNode> nodes, Framework::BaseSprite* sprite);

	void render() const;

	void set_position(Framework::vec2 position);

private:
	uint32_t _component_id = 0;

	std::vector<ComponentNode> _nodes;

	Framework::BaseSprite* _sprite = nullptr;

	Framework::vec2 _position;
};

class ComponentManager {
public:
	ComponentManager();
	//~ComponentManager();

	void add_component(const Component& component);
	void add_connection(const Connection& connection);

	std::vector<Component> get_components();
	std::vector<Connection> get_connections();


	void render();

private:
	std::vector<Component> _components;
	std::vector<Connection> _connections;
};