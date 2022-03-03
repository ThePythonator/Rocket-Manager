#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Colour.hpp"

#include "Maths.hpp"

namespace WINDOW {
	extern const Framework::vec2 SIZE;
	extern const Framework::vec2 SIZE_HALF;
	
	extern const char* TITLE;

	extern const float TARGET_FPS;
	extern const float TARGET_DT;

	extern const float MAX_DT;
}

namespace STRINGS {
	namespace BUTTONS {
		extern const std::vector<std::string> TITLE;
		extern const std::vector<std::string> PLAY_OPTIONS;
		extern const std::vector<std::string> SETTINGS;
		extern const std::vector<std::string> CREDITS;
		extern const std::vector<std::string> PAUSED;
	}

	namespace GAME {
		extern const std::vector<std::string> PLANET_NAMES;
	}
}

namespace PATHS {
	extern const uint8_t DEPTH;

	namespace IMAGES {
		extern const std::string LOCATION;

		extern const std::string LOGO;
		extern const std::string MAIN_SPRITESHEET;
		extern const std::string FONT_SPRITESHEET;
	}

	namespace SAVE_DATA {
		
	}
}

namespace GRAPHICS_OBJECTS {
	// enum inside its own namespace allows multiple repeated names in the enums, without needing enum class and all the casting
	namespace IMAGES {
		enum IMAGES {
			LOGO,
			MAIN_SPRITESHEET,
			FONT_SPRITESHEET,

			BUTTON_DEFAULT,
			BUTTON_HOVERED,
			BUTTON_SELECTED,

			TOTAL_IMAGES
		};
	}

	namespace BUTTON_IMAGE_GROUPS {
		enum BUTTON_IMAGE_GROUPS {
			DEFAULT,

			TOTAL_BUTTON_IMAGE_GROUPS
		};
	}

	namespace SPRITESHEETS {
		enum SPRITESHEETS {
			MAIN_SPRITESHEET,
			FONT_SPRITESHEET,

			TOTAL_SPRITESHEETS
		};
	}

	namespace FONTS {
		enum FONTS {
			MAIN_FONT,

			TOTAL_FONTS
		};
	}

	namespace TRANSITIONS {
		enum TRANSITIONS {
			FADE_TRANSITION,
			TOTAL_TRANSITIONS
		};
	}
}

namespace FONTS {
	namespace SIZE {
		extern const uint8_t MAIN_FONT;
	}

	namespace SCALE {
		extern const uint8_t MAIN_FONT;
		extern const uint8_t PLANET_NAME_FONT;
		extern const uint8_t DEBUG_FONT;
	}

	namespace SPACING {
		extern const uint8_t MAIN_FONT;
	}
}

namespace COLOURS {
	extern const Framework::Colour BLACK;
	extern const Framework::Colour WHITE;

	extern const std::vector<Framework::Colour> PLANETS;
}

namespace TIMINGS {
	namespace INTRO {
		namespace DURATION {
			extern const float FADE_LENGTH;

			extern const float INITIAL_DELAY;
			extern const float INTRO_DELAY;
		}

		namespace CUMULATIVE {
			extern const float INITIAL_DELAY;
			extern const float FADE_IN;
			extern const float INTRO_DELAY;
			extern const float FADE_OUT;
		}
	}
}

namespace TRANSITIONS {
	extern const float FADE_TIME;
}

namespace SPRITES {
	extern const uint8_t SIZE;
	extern const uint8_t SIZE_HALF;
	extern const uint8_t SCALE;
	extern const uint8_t UI_SCALE;

	namespace INDEX {
		
	}
}

namespace SCALES {
	extern const uint8_t LOGO;
}

namespace CURVES {
	namespace BEZIER {
		
	}
}

namespace BUTTONS {
	extern const Framework::vec2 SIZE;
	
	extern const uint8_t SELECTED_ALPHA;

	extern const uint8_t NONE;

	namespace TITLE {
		enum TITLE {
			PLAY,
			EDITOR,
			SETTINGS,
			CREDITS,
			QUIT,

			TOTAL
		};
	}

	namespace PLAY_OPTIONS {
		enum PLAY_OPTIONS {
			PLAY,
			CREATE,
			BACK,

			TOTAL
		};
	}

	namespace SETTINGS {
		enum SETTINGS {
			GAME,
			GRAPHICS,
			SOUND,
			CONTROLS,
			BACK,

			TOTAL
		};
	}

	namespace CREDITS {
		enum CREDITS {
			BACK,

			TOTAL
		};
	}

	namespace PAUSED {
		enum PAUSED {
			RESUME,
			EXIT,

			TOTAL
		};
	}
}

namespace MENU {
	extern const Framework::Rect BACKGROUND_RECT;
	extern const Framework::Rect SUBMENU_BACKGROUND_RECT;

	extern const uint8_t BACKGROUND_ALPHA;
}

namespace GAME {
	namespace MAP {
		namespace UI {
			extern const float SCROLL_ZOOM_RATE;
			extern const float MAXIMUM_ZOOM;
			extern const float MINIMUM_ZOOM;

			namespace MINIMAP {
				extern const float SIZE;
				extern const float PADDING;

				extern const Framework::Rect RECT;

				extern const float EXTRA_ZOOM;
			}
		}
	}

	namespace SANDBOX {
		extern const float UNIVERSE_SCALE;

		extern const float GRAVITATIONAL_CONSTANT;

		extern const std::vector<uint32_t> WARP_SPEEDS;

		namespace UI {
			extern const float SCROLL_ZOOM_RATE;
			extern const float MAXIMUM_ZOOM;
			extern const float MINIMUM_ZOOM;
		}

		namespace CATEGORIES {
			enum CATEGORIES {
				PLANET, // Can be planet, sun, moon etc
				COMPONENT,

				TOTAL
			};
		}

		namespace BODIES {
			extern const std::vector<float> RADII;
			extern const std::vector<float> VOLUME_DENSITIES;
			extern const std::vector<float> PERIHELION_DISTANCES;
			extern const std::vector<float> APHELION_DISTANCES;

			namespace ID {
				enum ID { // TODO: add moon(s)!
					SUN,
					MERCURY,
					VENUS,
					EARTH,
					MARS,
					JUPITER,
					SATURN,
					URANUS,
					NEPTUNE,

					TOTAL
				};
			}
		}

		namespace COMPONENTS {
			extern const std::vector<Framework::vec2> SIZES;
		}
	}

	namespace DEBUG {
		namespace PRECISION {
			extern const uint8_t FPS;

			extern const uint8_t MAP_SCALE;
			extern const uint8_t MAP_POSITION;

			extern const uint8_t SANDBOX_SCALE;
			extern const uint8_t SANDBOX_POSITION;
		}
	}
}