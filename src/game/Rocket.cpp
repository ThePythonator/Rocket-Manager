#include "Rocket.hpp"

Rocket::Rocket() {

}
Rocket::Rocket(std::string name) {
	_name = name;
}

std::string Rocket::name() const {
	return _name;
}