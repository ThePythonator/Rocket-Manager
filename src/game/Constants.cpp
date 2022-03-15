#include "Constants.hpp"

namespace WINDOW {
	const Framework::vec2 SIZE = Framework::vec2{ 1024, 768 };
	//const Framework::vec2 SIZE = Framework::vec2{ 1280, 720 };
	const Framework::vec2 SIZE_HALF = SIZE / 2;

	const char* TITLE = "CS NEA TO RENAME";

	const float TARGET_FPS = 60.0f;
	const float TARGET_DT = 1.0f / TARGET_FPS;

	const float MAX_DT = 0.05f;
}

namespace STRINGS {
	namespace BUTTONS {
		const std::vector<std::string> TITLE {
			"Play",
			"Editor",
			"Settings",
			"Credits",
			"Quit"
		};

		const std::vector<std::string> PLAY_OPTIONS {
			"Play",
			"Create",
			"Back"
		};

		const std::vector<std::string> SAVE_SELECT {
		};

		const std::vector<std::string> NEW_SAVE {
			"Save Name",
			"Create"
		};

		const std::vector<std::string> SETTINGS {
			"Game",
			"Graphics",
			"Sound",
			"Controls",
			"Back"
		};

		const std::vector<std::string> CREDITS {
			"Back"
		};

		const std::vector<std::string> PAUSED {
			"Resume",
			"Exit"
		};
	}


	namespace GAME {
		const std::vector<std::string> PLANET_NAMES {
			// Planets
			"Sun",
			"Mercury",
			"Venus",
			"Earth",
			"Mars",
			"Jupiter",
			"Saturn",
			"Uranus",
			"Neptune",

			// Moons
			"Moon"
		};
	}

	const std::vector<std::string> RANDOM_SAVE_ADJECTIVES {
		"Broken",
		"Vast",
		"Amazing",
		"Brilliant",
		"Purple",
		"Super",
		"Cosmic",
		"Strange",
		"Quirky",
		"Unusual",
		"Round",
		"Metallic",
		"Dysfunctional",
		"Corrupted",
		"Curious",
		"Effective",
		"Awesome"
	};

	const std::vector<std::string> RANDOM_SAVE_NOUNS {
		"System",
		"Station",
		"Universe",
		"Galaxy",
		"Supernova",
		"Star",
		"Giant",
		"Rocket",
		"Planet",
		"Moon",
		"Byte",
		"Computer",
		"Cluster",
		"Code",
		"Drone",
		"Alloy",
		"Storage",
		"Satellite",
		"Mineral",
		"Metal",
		"Robot",
		"Explosion",
		"Colony"
	};
}

namespace PATHS {
	const uint8_t DEPTH = 4;

	const std::string PARENT = "../";

	// Set at runtime
	std::string BASE_PATH;

	namespace IMAGES {
		const std::string LOCATION = "assets/images/";

		const std::string LOGO = "logo.png";
		const std::string MAIN_SPRITESHEET = "icons.png";
		const std::string FONT_SPRITESHEET = "font.png";
	}

	namespace DATA {
		const std::string LOCATION = "data/";

		const std::string SETTINGS = "settings.json";
	}

	namespace SANDBOX_SAVES {
		const std::string LOCATION = "data/saves/";

		const std::string EXTENSION = ".json";
	}

	namespace COMPONENTS {
		const std::string LOCATION = "assets/components/";

		const std::string EXTENSION = ".json";
	}

	namespace ROCKET_TEMPLATES {
		const std::string LOCATION = "data/templates/";

		const std::string EXTENSION = ".json";
	}
}

namespace FONTS {
	namespace SIZE {
		const uint8_t MAIN_FONT = 16;
	}

	namespace SCALE {
		const uint8_t MAIN_FONT = 3;
		const uint8_t MAP_OBJECT_FONT = 1;
		const uint8_t DEBUG_FONT = 1;
	}

	namespace SPACING {
		const uint8_t MAIN_FONT = 1;
	}
}

namespace COLOURS {
	const Framework::Colour BLACK{ 0x00, 0x00, 0x00 };
	const Framework::Colour YELLOW{ 0xFF, 0xF1, 0x5E };
	const Framework::Colour WHITE{ 0xFF, 0xFF, 0xFF };

	const Framework::Colour EDITOR_GREY{ 0x7A, 0xA2, 0xB0 };
	//const Framework::Colour EDITOR_GREY{ 0x4C, 0x65, 0x89 };
	const Framework::Colour EDITOR_PALETTE_GREY{ 0x60, 0x83, 0x9C };
	const Framework::Colour EDITOR_GRID_COLOUR{ 0x9C, 0xBB, 0xC1 };
	
	const Framework::Colour CURRENT_ROCKET_ICON{ 0xFF, 0xFF, 0xFF };
	const Framework::Colour OTHER_ROCKET_ICONS{ 0x9C, 0xBB, 0xC1 };

	const std::vector<Framework::Colour> PLANETS = {
		// Planets
		{ 0xe4, 0x9b, 0x35 },
		{ 0x9c, 0xbb, 0xc1 },
		{ 0xed, 0xd5, 0xa5 },
		{ 0x5B, 0xB0, 0x31 },// todo... (maybe later change anyways) fix this and below
		{ 0xB1, 0x49, 0x26 },
		{ 0xFF, 0x00, 0x00 },
		{ 0xFF, 0x00, 0x00 },
		{ 0x00, 0x00, 0xFF },
		{ 0x00, 0x00, 0xFF },

		// Moons
		{ 0x7F, 0x7F, 0x7F },
	};

	const std::vector<Framework::Colour> ATMOSPHERES = {
		// Planets
		{ 0xe4, 0x9b, 0x35 }, // todo?
		{ 0x00, 0x00, 0x00 },
		{ 0xed, 0xd5, 0xa5 }, // todo?
		{ 0x41, 0x9D, 0xD9 },
		{ 0xCC, 0x70, 0x2F },
		{ 0xFF, 0xFF, 0xFF }, // todo... (maybe later change anyways) fix this and below
		{ 0xFF, 0xFF, 0xFF },
		{ 0xFF, 0xFF, 0xFF },
		{ 0xFF, 0xFF, 0xFF },

		// Moons
		{ 0xFF, 0xFF, 0xFF },
	};
}

namespace TIMINGS {
	namespace INTRO {
		namespace DURATION {
			const float FADE_LENGTH = 0.1f;//2.0f;

			const float INITIAL_DELAY = 0.1f;// 1.0f;
			const float INTRO_DELAY = 0.1f;// 2.5f;
		}

		namespace CUMULATIVE {
			const float INITIAL_DELAY = DURATION::INITIAL_DELAY;
			const float FADE_IN = DURATION::FADE_LENGTH + INITIAL_DELAY;
			const float INTRO_DELAY = DURATION::INTRO_DELAY + FADE_IN;
			const float FADE_OUT = DURATION::FADE_LENGTH + INTRO_DELAY;
		}
	}
}

namespace TRANSITIONS {
	const float FADE_TIME = 0.5f;
}

namespace SPRITES {
	const uint8_t SIZE = 16;
	const uint8_t SIZE_HALF = SIZE / 2;
	const uint8_t SCALE = 4;
	const uint8_t UI_SCALE = 4;

	const uint8_t SCALED_SIZE = SIZE * SCALE;
	
	namespace INDEX {
		const uint8_t BIN = 0;
	}
}

namespace SCALES {
	const uint8_t LOGO = 8;
}

namespace BUTTONS {
	const Framework::vec2 SIZE = Framework::Vec(192, 56);
	const Framework::vec2 WIDE_SIZE = Framework::Vec(384, 56); //512?
	const Framework::vec2 EDITOR_SIZE = Framework::Vec(320, 56);

	const uint8_t SELECTED_ALPHA = 0x40;

	const uint8_t NONE = 255;
}

namespace MENU {
	const Framework::Rect BACKGROUND_RECT = Framework::Rect(WINDOW::SIZE.x * 0.1f, 0.0f, BUTTONS::SIZE.x, WINDOW::SIZE.y);
	extern const Framework::Rect SUBMENU_BACKGROUND_RECT = Framework::Rect(BACKGROUND_RECT.topright(), BACKGROUND_RECT.size);
	extern const Framework::Rect OVERLAY_RECT = Framework::centred_rect(WINDOW::SIZE_HALF, { BUTTONS::WIDE_SIZE.x, WINDOW::SIZE.y * 0.7f });

	const uint8_t BACKGROUND_ALPHA = 0x60;
	const uint8_t OVERLAY_ALPHA = 0x60;//0xA0;
	const uint8_t BORDER_ALPHA = 0x80;
	//const uint8_t OVERLAY_BACKGROUND_ALPHA = 0x40;
}

namespace GAME {
	namespace CONTROLS {
		const float ENGINE_POWER_INCREASE_RATE = 1.0f;

		const float ENGINE_FORCE = 2e7f;

		// In radians
		const float ENGINE_TURN_ANGLE = PhysicsEngine::deg_to_rad(15);
	}

	namespace MAP {
		namespace UI {
			const float SCROLL_ZOOM_RATE = 0.2f;
			const float MAXIMUM_ZOOM = 5e-5f / SANDBOX::UNIVERSE_SCALE;
			const float MINIMUM_ZOOM = 1e-11f / SANDBOX::UNIVERSE_SCALE;

			const float DEFAULT_CAMERA_SCALE = 1e-5f / GAME::SANDBOX::UNIVERSE_SCALE;

			namespace MINIMAP {
				const float SIZE = WINDOW::SIZE.y * 0.25f;
				const float PADDING = WINDOW::SIZE.y * 0.01f;

				const Framework::Rect RECT = Framework::Rect(WINDOW::SIZE - SIZE - PADDING, SIZE * Framework::VEC_ONES);

				const float EXTRA_ZOOM = WINDOW::SIZE.y / SIZE;
			}

			namespace ICONS {
				// Size in pixels
				const float COMMAND_MODULE_SIZE = 8;

				const std::vector<PhysicsEngine::phyvec> COMMAND_MODULE_VERTICES = PhysicsEngine::isosceles_vertices({ COMMAND_MODULE_SIZE, COMMAND_MODULE_SIZE });
			}
		}
	}

	namespace SANDBOX {
		// WARNING: if setting UNIVERSE_SCALE to something like 1, then rendering circles crashes due to floating point inaccuracies
		const double UNIVERSE_SCALE = 0.01;//0.0001; small world

		const double GRAVITATIONAL_CONSTANT = 6.674e-11 / (UNIVERSE_SCALE * UNIVERSE_SCALE);

		const std::vector<uint32_t> WARP_SPEEDS {
			1,
			2,
			5,
			10,
			50,
			100,
			1000,
			10000,
			100000,
			1000000,
			//10000000
		};

		// ~95% of atmosphere is below 3 scale heights
		extern const float SCALE_HEIGHT_TO_ATMOSPHERE_HEIGHT_FACTOR = 3.0f;

		namespace UI {
			const float SCROLL_ZOOM_RATE = 0.2f;
			const float MAXIMUM_ZOOM = 1e1f;
			const float MINIMUM_ZOOM = 1e-1f;
			//const float MAXIMUM_ZOOM = 1e0f / SANDBOX::UNIVERSE_SCALE;
			//const float MINIMUM_ZOOM = 1e-2f / SANDBOX::UNIVERSE_SCALE;

			const float DEFAULT_CAMERA_SCALE = 5.0f;
		}


		namespace CATEGORIES {

		}

		namespace DEFAULT_MATERIALS {
			// No const only because it gets messy when using ptrs to here
			std::vector<PhysicsEngine::Material> MATERIALS {
				{ 0.74f, 0.57f, 0.7f, 7850 },
				{ 0.5f, 0.25f, 0.6f, 710 },
				{ 0.35f, 0.3f, 0.5f, 940 },
				{ 0.94f, 0.4f, 0.5f, 2500 }
			};
		}

		namespace BODIES {
			// Radii are in metres, to 4sf
			const std::vector<double> RADII = {
				// Planets
				6963e5 * UNIVERSE_SCALE,
				2440e3 * UNIVERSE_SCALE,
				6052e3 * UNIVERSE_SCALE,
				6371e3 * UNIVERSE_SCALE,
				3390e3 * UNIVERSE_SCALE,
				6991e4 * UNIVERSE_SCALE,
				5823e4 * UNIVERSE_SCALE,
				2536e4 * UNIVERSE_SCALE,
				2462e4 * UNIVERSE_SCALE,

				// Moons
				1737e3 * UNIVERSE_SCALE
			};

			// Densities are in kg / m^3, to 3sf
			const std::vector<double> VOLUME_DENSITIES = {
				// Planets
				1.41e3 * UNIVERSE_SCALE,
				5.43e3 * UNIVERSE_SCALE,
				5.24e3 * UNIVERSE_SCALE,
				5.51e3 * UNIVERSE_SCALE,
				3.93e3 * UNIVERSE_SCALE,
				1.33e3 * UNIVERSE_SCALE,
				6.87e2 * UNIVERSE_SCALE,
				1.27e3 * UNIVERSE_SCALE,
				1.64e3 * UNIVERSE_SCALE,

				// Moons
				3.34e3 * UNIVERSE_SCALE
			};

			// Distances are in metres, to 4sf
			const std::vector<double> PERIAPSIS_DISTANCES = {
				// Planets
				0.0 * UNIVERSE_SCALE,
				4600e7 * UNIVERSE_SCALE,
				1075e8 * UNIVERSE_SCALE,
				1471e8 * UNIVERSE_SCALE,
				2067e8 * UNIVERSE_SCALE,
				7405e8 * UNIVERSE_SCALE,
				1353e9 * UNIVERSE_SCALE,
				2736e9 * UNIVERSE_SCALE,
				4460e9 * UNIVERSE_SCALE,

				// Moons
				3626e5 * UNIVERSE_SCALE
			};
			const std::vector<double> APOAPSIS_DISTANCES = {
				// Planets
				0.0 * UNIVERSE_SCALE,
				6982e7 * UNIVERSE_SCALE,
				1089e8 * UNIVERSE_SCALE,
				1521e8 * UNIVERSE_SCALE,
				2492e8 * UNIVERSE_SCALE,
				8166e8 * UNIVERSE_SCALE,
				1515e9 * UNIVERSE_SCALE,
				3006e9 * UNIVERSE_SCALE,
				4537e9 * UNIVERSE_SCALE,

				// Moons
				4054e5 * UNIVERSE_SCALE
			};

			const double SCALE_HEIGHT_SCALE = 3.0f;

			// Maybe don't scale by UNIVERSE_SCALE??
			const std::vector<double> SCALE_HEIGHTS = {
				// Planets
				0.0, 
				0.0, // Mercury has no atmosphere
				15.9e3 * SCALE_HEIGHT_SCALE,
				8.5e3  * SCALE_HEIGHT_SCALE,
				10.8e3 * SCALE_HEIGHT_SCALE,
				27e3   * SCALE_HEIGHT_SCALE,
				59.5e3 * SCALE_HEIGHT_SCALE,
				27.7e3 * SCALE_HEIGHT_SCALE,
				19.7e3 * SCALE_HEIGHT_SCALE,

				// Moons
				0.0 // Moon has no atmosphere
			};


			const std::vector<uint32_t> PARENT_BODIES {
				// Planets
				ID::NONE, // Sun

				ID::SUN, // Mercury
				ID::SUN, // Venus
				ID::SUN, // Earth
				ID::SUN, // Mars
				ID::SUN, // Jupiter
				ID::SUN, // Saturn
				ID::SUN, // Uranus
				ID::SUN, // Neptune

				// Moons
				ID::EARTH // Moon
			};
		}

		namespace CONNECTIONS {
			const float MAX_EXTENSION = 1.0f;
			const float MODULUS_OF_ELASTICITY = 2e5f;
		}
	}


	namespace DEBUG {
		namespace PRECISION {
			const uint8_t FPS = 1;

			const uint8_t MAP_SCALE = 4;
			const uint8_t MAP_POSITION = 0;

			const uint8_t SANDBOX_SCALE = 2;
			const uint8_t SANDBOX_POSITION = 0;

			const uint8_t ALTITUDE = 0;
			const uint8_t RELATIVE_VELOCITY = 0;

			const uint8_t ENGINE_POWER = 1;
		}
	}


	namespace PARTICLES {
		namespace SMOKE {
			const float MIN_SIZE = 1.0f;
			const float MAX_SIZE = 2.0f;

			const float SIZE_CHANGE = 1.0f;

			const float MIN_AGE = 2.0f;
			const float MAX_AGE = 5.0f; // TODO: change

			// Maximum variation in velocity
			const float VELOCITY_OFFSET_MAX = 5.0f;

			const uint8_t RGB_MIN = 0x70;
			const uint8_t RGB_MAX = 0x90;
		}
	}

	// Loaded at runtime
	namespace COMPONENTS {
		std::map<uint32_t, std::vector<PhysicsEngine::phyvec>> VERTICES;
		std::map<uint32_t, std::vector<PhysicsEngine::phyvec>> NODE_POSITIONS;
		std::map<uint32_t, PhysicsEngine::phyvec> CENTROIDS;

		std::map<uint32_t, uint32_t> MATERIALS;

		std::map<uint32_t, std::string> NAMES;
	}
}



namespace EDITOR {
	// Use -1 as way to get max value for uint32_t
	const uint32_t NO_COMPONENT_SELECTED = -1;

	namespace CAMERA {
		const float DEFAULT_SCALE = 10.0f;
	}

	namespace UI {
		const Framework::Rect PALETTE_RECT = Framework::Rect(0.0f, 0.0f, BUTTONS::EDITOR_SIZE.x, WINDOW::SIZE.y);
		const Framework::Rect BIN_RECT = Framework::Rect(WINDOW::SIZE.x - SPRITES::SCALED_SIZE, 0, SPRITES::SCALED_SIZE, SPRITES::SCALED_SIZE);

		const Framework::vec2 BUTTON_OFFSET = { -BUTTONS::EDITOR_SIZE.x * 0.47f, 0.0f };
		const Framework::vec2 BUTTON_ICON_OFFSET = { BUTTONS::EDITOR_SIZE.x * 0.9f, BUTTONS::EDITOR_SIZE.y / 2};


		const float NODE_SIZE = 5.0f;
	}
}