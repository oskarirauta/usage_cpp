#include <iostream>
#include <filesystem>
#include "usage.hpp"

int main(int argc, char **argv) {

	usage_t usage = {
		.args = { argc, argv },
		.name = "usage_cpp",
		.version = "1.0.0",
		.author = "Oskari Rauta",
		.copyright = "2024, Oskari Rauta",
		.options = {
			{ "name", { .key = "n", .word = "name", .desc = "user's name", true, "name" }},
			{ "age", { .key = "a", .word = "age", .desc = "user's age", true, "number" }},
			{ "sudo", { .key = "s", .word = "sudo", .desc = "user can sudo" }},
			{ "help", { .key = "h", .word = "help", .desc = "usage help" }},
			{ "version", { .key = "v", .word = "version", .desc = "show version" }}
		}
	};

	if ( usage["help"] ) {

		std::cout << usage << "\n" << usage.help() << "\n" << std::endl;
		return 0;

	} else if (  usage["version"] ) {

		std::cout << usage.version_info() << std::endl;
		return 0;

	} else if ( usage.args.empty()) {

		std::cout << usage << "\n\n" << usage.name <<
			" needs some arguments, none was provided;\n  try calling it with --help argument\n" << std::endl;
		return 1;
	}

	std::cout << usage << "\n" << std::endl;

	try {
		usage.validate();
	} catch ( const std::runtime_error& e ) {

		std::cout << "error: " << e.what() << std::endl;
		return 1;
	}

	if ( usage["name"] ) std::cout << "user's name is " << (std::string)usage["name"] << std::endl;
	if ( usage["age"] ) std::cout << "user's age is " << usage["age"].value << std::endl;
	if ( usage["sudo"] ) std::cout << "and user can sudo" << std::endl;
	std:: cout << std::endl;

	return 0;
}
