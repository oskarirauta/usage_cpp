#include <iostream>
#include <filesystem>
#include "usage.hpp"

usage_t* usage;

int main(int argc, char **argv) {

//	std::cout << usage << std::endl;

	usage = new usage_t {
		.args = { argc, argv },
		.name = "usage_cpp",
		.version = "1.0.0",
		.author = "Oskari Rauta",
		.copyright = "2024, Oskari Rauta",
		.options = {
			{ "name", { .key = "n", .word = "name", .desc = "user's name", true, "name" }},
			{ "age", { .key = "a", .word = "age", .desc = "user's age", true, "number" }},
			{ "sudo", { .key = "s", .word = "sudo", .desc = "user can sudo" }},
			{ "help", { .key = "h", .word = "help", .desc = "usage help" }}
		}
	};

	std::cout << usage << std::endl;

	if ( usage -> args.empty()) {

		std::cout << usage -> name << " needs some arguments, none was provided;\n  try calling it with --help argument\n" << std::endl;
		delete usage;
		return 1;
	}

	if ( (*usage)["help"] ) {

		std::cout << usage -> help() << "\n" << std::endl;
		delete usage;
		return 0;
	}

	try {
		usage -> validate();
	} catch ( const std::runtime_error& e ) {
		std::cout << "error: " << e.what() << std::endl;
		delete usage;
		return 1;
	}

	if ( (*usage)["name"] ) std::cout << "user's name is " << (std::string)(*usage)["name"] << std::endl;
	if ( (*usage)["age"] ) std::cout << "user's age is " << (std::string)(*usage)["age"] << std::endl;
	if ( (*usage)["sudo"] ) std::cout << "and user can sudo" << std::endl;
	std:: cout << std::endl;

	delete usage;
	return 0;
}
