#include "Game.hpp"

Game::Game() : BaseGame() {

}

void Game::start() {
	stage = new IntroStage();
}

void Game::end() {

}

void Game::load_data() {
	PATHS::BASE_PATH = Framework::SDLUtils::find_assets_path(PATHS::IMAGES::LOCATION + PATHS::IMAGES::MAIN_SPRITESHEET, PATHS::DEPTH);

	// Base path is two above images path
	std::string IMAGES_PATH = PATHS::BASE_PATH + PATHS::IMAGES::LOCATION;

	// Load logo image
	graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::LOGO] = Framework::create_image(graphics_objects.graphics_ptr, IMAGES_PATH + PATHS::IMAGES::LOGO, Framework::Image::Flags::SDL_TEXTURE);

	// Load spritesheet image
	graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::MAIN_SPRITESHEET] = Framework::create_image(graphics_objects.graphics_ptr, IMAGES_PATH + PATHS::IMAGES::MAIN_SPRITESHEET, Framework::Image::Flags::SDL_TEXTURE);
	
	// Load font image
	// Note: we *need* to add SURFACE flags because Font uses the surface bit
	graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::FONT_SPRITESHEET] = Framework::create_image(graphics_objects.graphics_ptr, IMAGES_PATH + PATHS::IMAGES::FONT_SPRITESHEET, Framework::Image::Flags::SDL_TEXTURE | Framework::Image::Flags::SDL_SURFACE);
	
	graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::BUTTON_DEFAULT] = nullptr;
	graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::BUTTON_HOVERED] = Framework::create_image(graphics_objects.graphics_ptr, BUTTONS::SIZE, Framework::Colour(COLOURS::BLACK, BUTTONS::SELECTED_ALPHA), true);
	graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::BUTTON_SELECTED] = Framework::create_image(graphics_objects.graphics_ptr, BUTTONS::SIZE, COLOURS::BLACK);
	
	// Create spritesheet from spritesheet image
	graphics_objects.spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::MAIN_SPRITESHEET] = new Framework::Spritesheet(graphics_objects.graphics_ptr, graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::MAIN_SPRITESHEET], SPRITES::SIZE, SPRITES::SCALE);

	// Create spritesheet from font image
	graphics_objects.spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::FONT_SPRITESHEET] = new Framework::Spritesheet(graphics_objects.graphics_ptr, graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::FONT_SPRITESHEET], FONTS::SIZE::MAIN_FONT, FONTS::SCALE::MAIN_FONT);

	// Create font from font spritesheet
	graphics_objects.font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT] = new Framework::Font(graphics_objects.graphics_ptr, graphics_objects.spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::FONT_SPRITESHEET], FONTS::SPACING::MAIN_FONT);
	
	// Create transitions
	graphics_objects.transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION] = new Framework::FadeTransition(graphics_objects.graphics_ptr, COLOURS::BLACK, TRANSITIONS::FADE_TIME);


	// Create button_images
	graphics_objects.button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT] = {
		graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::BUTTON_DEFAULT],
		graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::BUTTON_HOVERED],
		graphics_objects.image_ptrs[GRAPHICS_OBJECTS::IMAGES::BUTTON_SELECTED]
	};

	// Load component data: find all json files in component directory and try parsing them
	for (const std::string& filepath : find_files_with_extension(PATHS::BASE_PATH + PATHS::COMPONENTS::LOCATION, PATHS::COMPONENTS::EXTENSION)) {
		try {
			// Read file
			json j = Framework::JSONHandler::read(filepath);

			// Get values
			uint32_t i = j.at("id").get<uint32_t>();
			uint32_t material = j.value<uint32_t, 0>("material", 0);
			std::vector<phyvec> vertices = j.at("vertices").get<std::vector<phyvec>>();
			std::vector<phyvec> nodes = j.at("nodes").get<std::vector<phyvec>>();
			std::string name = j.value("name", "unnamed_component");

			// Shift so that centroid is centre
			phyvec centroid = PhysicsEngine::find_centroid(vertices);

			// Store as runtime constants
			GAME::COMPONENTS::MATERIALS[i] = material;
			GAME::COMPONENTS::VERTICES[i] = PhysicsEngine::translate(vertices, -centroid);
			GAME::COMPONENTS::NODE_POSITIONS[i] = PhysicsEngine::translate(nodes, -centroid);
			GAME::COMPONENTS::CENTROIDS[i] = centroid;

			GAME::COMPONENTS::NAMES[i] = name;

			printf("Loaded component: %s\n", name.c_str());
		}
		catch (const json::exception& e) {
			printf("Failed to load component. Error: %s\n", e.what());
		}
	}
}

void Game::clear_data() {
	// Don't need to clear up graphics objects items - it's done for us in BaseGame

	// Clear up anything else we need to
}