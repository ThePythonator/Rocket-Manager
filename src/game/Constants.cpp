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
		const uint8_t MAIN_FONT = 4;
	}

	namespace SPACING {
		const uint8_t MAIN_FONT = 1;
	}
}

namespace COLOURS {
	const Framework::Colour BLACK{ 0x00, 0x00, 0x00 };
	const Framework::Colour WHITE{ 0xFF, 0xFF, 0xFF };
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
	const float FADE_TIME = 0.7f;
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
	const uint8_t NONE = 255;
}

namespace MENU {
	const Framework::Rect BACKGROUND_RECT = Framework::Rect(WINDOW::SIZE.x * 0.1f, 0.0f, WINDOW::SIZE.x * 0.2f, WINDOW::SIZE.y);
	const uint8_t BACKGROUND_ALPHA = 0x7F;
}

namespace GAME {
}