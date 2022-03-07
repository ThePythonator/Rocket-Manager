#include "File.hpp"

namespace Framework {
	std::string get_directory_path(std::string filepath) {
		return filepath.substr(0, filepath.find_last_of("\\/"));
	}

	namespace JSONHandler {
		// Uses https://github.com/nlohmann/json

		json read(std::string filepath) {
			printf("Attempting to read from %s\n", filepath.c_str());

			// Read from file
			json data;
			try {
				std::ifstream file(filepath);
				file >> data;
			}
			catch (const parse_error& error) {
				printf("Couldn't find file!\n");
			}
			return data;
		}

		void write(std::string filepath, json data) {
			if (std::filesystem::create_directories(get_directory_path(filepath))) {
				printf("Created necessary directories.");
			}

			printf("Written to %s\n", filepath.c_str());
			// Write to the file
			std::ofstream file(filepath);
			file << data << std::endl;
			//file << std::setw(4) << data << std::endl;
		}
	}
}