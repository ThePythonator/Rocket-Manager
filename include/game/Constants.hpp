#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Colour.hpp"

#include "Maths.hpp"

#include "PhysicsEngine.hpp"

namespace WINDOW {
	extern const Framework::vec2 SIZE;
	extern const Framework::vec2 SIZE_HALF;
	
	extern const char* TITLE;

	extern const float TARGET_FPS;
	extern const float TARGET_DT;

	extern const float MAX_DT;
}

namespace STRINGS {
	namespace STATUS {
		extern const std::string ON;
		extern const std::string OFF;
		extern const std::string YES;
		extern const std::string NO;
	}

	namespace HUD {
		extern const std::string MAP;
		extern const std::string ALTITUDE;
		extern const std::string VELOCITY;
		extern const std::string PAUSED;
		extern const std::string TIME_WARP;
		extern const std::string TIMES;
		extern const std::string NEAREST_PLANET;
		extern const std::string CURRENT_ROCKET;

		extern const std::string DISTANCE_SUFFIX;
		extern const std::string VELOCITY_SUFFIX;
	}

	namespace BUTTONS {
		extern const std::vector<std::string> TITLE;
		extern const std::vector<std::string> PLAY_OPTIONS;
		extern const std::vector<std::string> SAVE_SELECT;
		extern const std::vector<std::string> NEW_SAVE;
		extern const std::vector<std::string> SETTINGS;
		extern const std::vector<std::string> GAME_SETTINGS;
		extern const std::vector<std::string> GRAPHICS_SETTINGS;
		extern const std::vector<std::string> CREDITS;
		extern const std::vector<std::string> PAUSED;
		extern const std::vector<std::string> LOAD_ROCKET;



		namespace LOAD_ROCKET_TEXT {
			extern const std::string SITE;
		}

		extern const std::vector<std::string> EDITOR;
	}

	namespace GAME {
		extern const std::vector<std::string> PLANET_NAMES;
	}

	extern const std::vector<std::pair<std::string, std::vector<std::string>>> CREDITS;

	extern const std::vector<std::string> RANDOM_SAVE_ADJECTIVES;
	extern const std::vector<std::string> RANDOM_SAVE_NOUNS;

	extern const std::vector<std::string> RANDOM_ROCKET_NAMES;

	extern const std::string ROCKET_NAME_SEPARATOR;

	extern const std::string GITHUB_URL;
	extern const std::string ITCHIO_URL;

	extern const std::string REPO_URL;
}

namespace PATHS {
	extern const uint8_t DEPTH;

	extern const std::string PARENT;

	// Set at runtime
	extern std::string BASE_PATH;

	namespace IMAGES {
		extern const std::string LOCATION;

		extern const std::string LOGO;
		extern const std::string MAIN_SPRITESHEET;
		extern const std::string FONT_SPRITESHEET;
	}

	namespace DATA {
		extern const std::string LOCATION;

		extern const std::string SETTINGS;
	}

	namespace SANDBOX_SAVES {
		extern const std::string LOCATION;

		extern const std::string EXTENSION;
	}

	namespace COMPONENTS {
		extern const std::string LOCATION;
		
		extern const std::string EXTENSION;
	}

	namespace ROCKET_TEMPLATES {
		extern const std::string LOCATION;

		extern const std::string EXTENSION;
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
		extern const uint8_t MAP_OBJECT_FONT;
		extern const uint8_t DEBUG_FONT;
		extern const uint8_t HUD_FONT;
	}

	namespace SPACING {
		extern const uint8_t MAIN_FONT;
	}
}

namespace COLOURS {
	extern const Framework::Colour BLACK;
	extern const Framework::Colour YELLOW;
	extern const Framework::Colour LIGHT_GREY;
	extern const Framework::Colour WHITE;

	extern const Framework::Colour EDITOR_GREY;
	extern const Framework::Colour EDITOR_PALETTE_GREY;
	extern const Framework::Colour EDITOR_GRID_COLOUR;

	extern const Framework::Colour CURRENT_ROCKET_ICON;
	extern const Framework::Colour OTHER_ROCKET_ICONS;

	extern const std::vector<Framework::Colour> PLANETS;
	extern const std::vector<Framework::Colour> ATMOSPHERES;
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

	extern const uint8_t SCALED_SIZE;

	namespace INDEX {
		extern const uint8_t BIN;
	}
}

namespace SCALES {
	extern const uint8_t LOGO;
}

namespace BUTTONS {
	extern const Framework::vec2 SIZE;
	extern const Framework::vec2 WIDE_SIZE;
	extern const Framework::vec2 VERY_WIDE_SIZE;
	extern const Framework::vec2 EDITOR_SIZE;

	extern const Framework::vec2 OFFSET;
	extern const float HUD_OFFSET;

	//extern const int INDENT;
	
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

	namespace SAVE_SELECT {
		enum SAVE_SELECT {
			TOTAL
		};
	}

	namespace NEW_SAVE {
		enum SAVE_SELECT {
			NAME,
			CREATE,

			TOTAL
		};
	}

	namespace EDITOR {
		enum EDITOR {
			NAME,
			SAVE,
			EXIT,

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

	namespace GAME_SETTINGS {
		enum GAME_SETTINGS {
			DEBUG_MODE,
			MAP_AUTO_SCROLL,

			TOTAL
		};
	}

	namespace GRAPHICS_SETTINGS {
		enum GRAPHICS_SETTINGS {
			FULLSCREEN,

			TOTAL
		};
	}

	namespace CREDITS {
		enum CREDITS {
			GITHUB,

			TOTAL
		};
	}

	namespace PAUSED {
		enum PAUSED {
			RESUME,
			LOAD_ROCKET,
			EXIT,

			TOTAL
		};
	}

	namespace LOAD_ROCKET {
		enum LOAD_ROCKET {
			FILE,
			PLANET,
			SITE,
			LAUNCH,

			TOTAL
		};
	}
}

namespace MENU {
	extern const Framework::Rect BACKGROUND_RECT;
	extern const Framework::Rect SUBMENU_BACKGROUND_RECT;
	extern const Framework::Rect OVERLAY_RECT;
	extern const Framework::Rect WIDE_OVERLAY_RECT;

	extern const uint8_t BACKGROUND_ALPHA;
	extern const uint8_t OVERLAY_ALPHA;
	extern const uint8_t BORDER_ALPHA;
	//extern const uint8_t OVERLAY_BACKGROUND_ALPHA;
}

namespace GAME {
	namespace CONTROLS {
		extern const float ENGINE_POWER_INCREASE_RATE;

		extern const float ENGINE_FORCE;

		extern const float ENGINE_TURN_ANGLE;
	}

	namespace MAP {
		namespace UI {
			extern const float SCROLL_ZOOM_RATE;
			extern const float MAXIMUM_ZOOM;
			extern const float MINIMUM_ZOOM;

			extern const float DEFAULT_CAMERA_SCALE;

			namespace MINIMAP {
				extern const float SIZE;
				extern const float PADDING;

				extern const Framework::Rect RECT;

				extern const float EXTRA_ZOOM;
			}

			namespace ICONS {
				extern const float COMMAND_MODULE_SIZE;

				extern const std::vector<PhysicsEngine::phyvec> COMMAND_MODULE_VERTICES;
			}
		}
	}

	namespace SANDBOX {
		extern const double UNIVERSE_SCALE;

		extern const double GRAVITATIONAL_CONSTANT;

		extern const std::vector<uint32_t> WARP_SPEEDS;
		
		extern const float SCALE_HEIGHT_TO_ATMOSPHERE_HEIGHT_FACTOR;

		extern const uint32_t NO_ROCKET_SELECTED;

		namespace UI {
			extern const float SCROLL_ZOOM_RATE;
			extern const float MAXIMUM_ZOOM;
			extern const float MINIMUM_ZOOM;

			extern const float DEFAULT_CAMERA_SCALE;
		}

		namespace RIGID_BODY_IDS {
			enum RIGID_BODY_IDS {
				CATEGORY, // is it a planet or component?
				GROUP, // components are grouped into rockets
				TYPE, // what type of object is it (e.g. Fuel Tank, Command Module), or what planet it is
				OBJECT, // which object (planet or component) is it (unique among rocket's components/planet types)
				
				TOTAL
			};
		}

		namespace CONSTRAINT_IDS {
			enum CONSTRAINT_IDS {
				ROCKET, // What rocket id
				CONSTRAINT, // What constraint id

				TOTAL
			};
		}

		namespace CATEGORIES {
			enum CATEGORIES {
				PLANET, // Can be planet, or sun, or moon
				COMPONENT,

				TOTAL
			};
		}

		namespace DEFAULT_MATERIALS {
			// No const only because it gets messy when using ptrs to here
			extern std::vector<PhysicsEngine::Material> MATERIALS;

			namespace TYPES {
				enum TYPES {
					STEEL,
					WOOD,
					PLASTIC,
					GLASS,

					TOTAL
				};
			}
		}

		namespace BODIES {
			extern const std::vector<double> RADII;
			extern const std::vector<double> VOLUME_DENSITIES;
			extern const std::vector<double> PERIAPSIS_DISTANCES;
			extern const std::vector<double> APOAPSIS_DISTANCES;

			extern const double SCALE_HEIGHT_SCALE;
			extern const std::vector<double> SCALE_HEIGHTS;

			extern const std::vector<uint32_t> PARENT_BODIES; // Used to identify what planet/moon orbits around what

			namespace ID {
				enum ID {
					// Planets
					SUN,
					MERCURY,
					VENUS,
					EARTH,
					MARS,
					JUPITER,
					SATURN,
					URANUS,
					NEPTUNE,

					// Moons
					MOON,

					TOTAL,

					NONE
				};
			}
		}

		namespace CONNECTIONS {
			extern const float MAX_EXTENSION;
			extern const float MODULUS_OF_ELASTICITY;
		}

		namespace LAUNCH_SITES {
			extern const uint8_t DEFAULT_LAUNCH_PLANET;
			extern const uint8_t DEFAULT_LAUNCH_SITE;

			extern const std::vector<std::vector<float>> SITES;
		}
	}

	namespace DEBUG {
		namespace PRECISION {
			extern const uint8_t FPS;

			extern const uint8_t MAP_SCALE;
			extern const uint8_t MAP_POSITION;

			extern const uint8_t SANDBOX_SCALE;
			extern const uint8_t SANDBOX_POSITION;

			extern const uint8_t ALTITUDE;
			extern const uint8_t RELATIVE_VELOCITY;

			extern const uint8_t ENGINE_POWER;
		}
	}

	namespace HUD {
		namespace PRECISION {
			extern const uint8_t ALTITUDE;
			extern const uint8_t RELATIVE_VELOCITY;

			extern const uint8_t ENGINE_POWER;
		}
	}

	namespace PARTICLES {
		namespace SMOKE {
			extern const float MIN_SIZE;
			extern const float MAX_SIZE;

			extern const float SIZE_CHANGE;

			extern const float MIN_AGE;
			extern const float MAX_AGE;

			extern const float VELOCITY_OFFSET_MAX;

			extern const uint8_t RGB_MIN;
			extern const uint8_t RGB_MAX;
		}
	}

	// These are loaded in at runtime
	namespace COMPONENTS {
		//extern const std::vector<PhysicsEngine::phyvec> SIZES;
		extern std::map<uint32_t, std::vector<PhysicsEngine::phyvec>> VERTICES;
		extern std::map<uint32_t, std::vector<PhysicsEngine::phyvec>> NODE_POSITIONS;
		extern std::map<uint32_t, PhysicsEngine::phyvec> CENTROIDS;

		extern std::map<uint32_t, uint32_t> MATERIALS;

		extern std::map<uint32_t, std::string> NAMES;

		// Only these are supported right now
		namespace COMPONENT_TYPE {
			enum COMPONENT_TYPE {
				COMMAND_MODULE,
				FUEL_TANK,
				ENGINE,

				TOTAL,

				NONE
			};
		}
	}
}

namespace EDITOR {
	extern const uint32_t NO_COMPONENT_SELECTED;

	namespace CAMERA {
		extern const float DEFAULT_SCALE;
	}

	namespace UI {
		extern const Framework::Rect PALETTE_RECT;
		extern const Framework::Rect BIN_RECT;

		extern const Framework::vec2 BUTTON_OFFSET;
		extern const Framework::vec2 BUTTON_ICON_OFFSET;

		extern const float NODE_SIZE;
	}
}