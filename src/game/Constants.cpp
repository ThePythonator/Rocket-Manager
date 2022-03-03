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
			"Sun",
			"Mercury",
			"Venus",
			"Earth",
			"Mars",
			"Jupiter",
			"Saturn",
			"Uranus",
			"Neptune"
		};
	}
}

namespace PATHS {
	const uint8_t DEPTH = 4;
	namespace IMAGES {
		const std::string LOCATION = "images/";

		const std::string LOGO = "logo.png";
		const std::string MAIN_SPRITESHEET = "demo_spritesheet.png";
		const std::string FONT_SPRITESHEET = "font.png";
	}
}

namespace SAVE_DATA {

}

namespace FONTS {
	namespace SIZE {
		const uint8_t MAIN_FONT = 16;
	}

	namespace SCALE {
		const uint8_t MAIN_FONT = 3;
		const uint8_t PLANET_NAME_FONT = 1;
		const uint8_t DEBUG_FONT = 1;
	}

	namespace SPACING {
		const uint8_t MAIN_FONT = 1;
	}
}

namespace COLOURS {
	const Framework::Colour BLACK{ 0x00, 0x00, 0x00 };
	const Framework::Colour WHITE{ 0xFF, 0xFF, 0xFF };

	const std::vector<Framework::Colour> PLANETS = {
		{ 0xe4, 0x9b, 0x35 },
		{ 0x9c, 0xbb, 0xc1 },
		{ 0xed, 0xd5, 0xa5 },
		{ 0x00, 0xFF, 0x00 },// todo... (maybe later change anyways) fix this and below
		{ 0xFF, 0x00, 0x00 },
		{ 0xFF, 0x00, 0x00 },
		{ 0xFF, 0x00, 0x00 },
		{ 0x00, 0x00, 0xFF },
		{ 0x00, 0x00, 0xFF }
	};
}

namespace TIMINGS {
	namespace INTRO {
		namespace DURATION {
			const float FADE_LENGTH = 2.0f;

			const float INITIAL_DELAY = 1.0f;
			const float INTRO_DELAY = 2.5f;
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
	
	namespace INDEX {
	}
}

namespace SCALES {
	const uint8_t LOGO = 8;
}

namespace BUTTONS {
	const Framework::vec2 SIZE = Framework::Vec(192, 56);

	const uint8_t SELECTED_ALPHA = 0x40;

	const uint8_t NONE = 255;
}

namespace MENU {
	const Framework::Rect BACKGROUND_RECT = Framework::Rect(WINDOW::SIZE.x * 0.1f, 0.0f, BUTTONS::SIZE.x, WINDOW::SIZE.y);
	extern const Framework::Rect SUBMENU_BACKGROUND_RECT = Framework::Rect(BACKGROUND_RECT.topright(), BACKGROUND_RECT.size);

	const uint8_t BACKGROUND_ALPHA = 0x60;
}

namespace GAME {
	namespace MAP {
		namespace UI {
			const float SCROLL_ZOOM_RATE = 0.2f;
			const float MAXIMUM_ZOOM = 5e-4f;
			const float MINIMUM_ZOOM = 1e-10f;

			namespace MINIMAP {
				const float SIZE = WINDOW::SIZE.y * 0.25f;
				const float PADDING = WINDOW::SIZE.y * 0.01f;

				const Framework::Rect RECT = Framework::Rect(WINDOW::SIZE - SIZE - PADDING, SIZE * Framework::VEC_ONES);

				const float EXTRA_ZOOM = WINDOW::SIZE.y / SIZE;
			}
		}
	}

	namespace SANDBOX {
		const double UNIVERSE_SCALE = 0.1;

		const double GRAVITATIONAL_CONSTANT = 6.674e-11;

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
			10000000
		};

		namespace UI {
			const float SCROLL_ZOOM_RATE = 0.2f;
			const float MAXIMUM_ZOOM = 1e1f;
			const float MINIMUM_ZOOM = 1e-1f;
		}


		namespace CATEGORIES {

		}

		namespace BODIES {
			// Radii are in metres, to 4sf
			const std::vector<double> RADII = {
				6963e5,
				2440e3,
				6052e3,
				6371e3,
				3390e3,
				6991e4,
				5823e4,
				2536e4,
				2462e4
			};

			// Densities are in kg / m^3, to 3sf
			const std::vector<double> VOLUME_DENSITIES = {
				1.41e3,
				5.43e3,
				5.24e3,
				5.51e3,
				3.93e3,
				1.33e3,
				6.87e2,
				1.27e3,
				1.64e3
			};

			// Distances are in metres, to 4sf
			const std::vector<double> PERIHELION_DISTANCES = {
				0.0,
				4600e7,
				1075e8,
				1471e8,
				2067e8,
				7405e8,
				1353e9,
				2736e9,
				4460e9
			};
			const std::vector<double> APHELION_DISTANCES = {
				0.0,
				6982e7,
				1089e8,
				1521e8,
				2492e8,
				8166e8,
				1515e9,
				3006e9,
				4537e9
			};
		}

		namespace COMPONENTS {
			const std::vector<Framework::vec2> SIZES{
				{ 5, 20 }
			};
		}
	}


	namespace DEBUG {
		namespace PRECISION {
			const uint8_t FPS = 1;

			const uint8_t MAP_SCALE = 4;
			const uint8_t MAP_POSITION = 0;

			const uint8_t SANDBOX_SCALE = 2;
			const uint8_t SANDBOX_POSITION = 0;
		}
	}
}