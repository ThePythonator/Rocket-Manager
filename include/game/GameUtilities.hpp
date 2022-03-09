#pragma once

#include "GraphicsObjects.hpp"

#include "Constants.hpp"

std::vector<Framework::Button> create_menu_buttons(const Framework::Rect& button_area, const Framework::vec2& button_size, const Framework::Button::ButtonImages& button_images, const std::vector<std::string>& button_titles, Framework::Font* font, const Framework::Colour& text_colour, float top_button_y);
std::vector<Framework::Button> create_menu_buttons_centralised(const Framework::Rect& button_area, const Framework::vec2& button_size, const Framework::Button::ButtonImages& button_images, const std::vector<std::string>& button_titles, Framework::Font* font, const Framework::Colour& text_colour);


std::vector<Framework::Button> create_submenu_from_constants(Framework::GraphicsObjects* graphics_objects, const std::vector<std::string>& button_titles, float top_button_y);
std::vector<Framework::Button> create_menu_from_constants(Framework::GraphicsObjects* graphics_objects, const std::vector<std::string>& button_titles);


//void render_trimmed_circle_to_window(Framework::Graphics* graphics, const Framework::vec2& centre, float radius, const Framework::Colour& colour);

// Scales percent so that return (alpha) value stays close to 255 for a while (when percent is low) before dropping to 0 (when percent is high)
uint8_t atmosphere_alpha(PhysicsEngine::phyflt height_above_surface, PhysicsEngine::phyflt scale_height);