#include <iostream>
#include <filesystem>
#include "usage.hpp"

int main(int argc, char **argv) {

	usage_t usage = {
		.args = { argc, argv },
		.info = {
			.name = "usage_cpp",
			.version = "1.4.0",
			.author = "Oskari Rauta",
			.copyright = "2024, Oskari Rauta",
			.description = "\nExample program for demonstrating command-line parser\n"
		},
		.options = {
			{ "name", { .key = "n", .word = "name", .desc = "user's name", .flag = usage_t::REQUIRED, .name = "name" }},
			{ "age", { .key = "a", .word = "age", .desc = "user's age", .flag = usage_t::REQUIRED, .name = "number", .type = usage_t::INT }},
			{ "optional", { .key = "o", .word = "optional", .desc = "optional arg", .flag = usage_t::OPTIONAL, .name = "value" }},
			{ "hex", { .key = "x", .word = "hex", .desc = "hex value", .flag = usage_t::REQUIRED, .name = "hex", .type = usage_t::HEX }},
			{ "sudo", { .key = "s", .word = "sudo", .desc = "user can sudo" }},
			{ "help", { .key = "h", .word = "help", .desc = "usage help" }},
			{ "version", { .key = "v", .word = "version", .desc = "show version" }}
		}
	};

	if ( usage["help"] ) {

		std::cout << usage << "\n" << usage.help() << "\n" << std::endl;
		return 0;

	} else if ( usage["version"] ) {

		std::cout << usage.version() << std::endl;
		return 0;

	} else if ( usage.args.empty()) {

		std::cout << usage << "\n\n" << usage.name() <<
			" needs some arguments, none was provided;\n  try calling it with --help argument\n" << std::endl;
		return 1;
	}

	std::cout << usage << "\n" << std::endl;

	if ( !usage.validated ) {

		auto errors = usage.errors();

		std::cout << "command-line errors found:\n" << errors << std::endl;

		if ( auto it = std::find_if(errors.begin(), errors.end(),
			[](usage_t::error_t e) {
				return e.error != usage_t::error_type::DUPLICATE && e.error != usage_t::error_type::UNKNOWN_OPTION;
			}); it != errors.end()) {

			std::cout << "\naborting, fatal errors occurred while parsing command-line arguments." << std::endl;
			return 1;
		}
	}

	if ( usage["name"] ) std::cout << "user's name is " << (std::string)usage["name"] << std::endl;
	if ( usage["age"] ) std::cout << "user's age is " << usage["age"].value << std::endl;
	if ( usage["optional"] ) {
		std::cout << "optional is enabled";
		if ( !usage["optional"].value.empty())
			std::cout << " with optional value: " << usage["optional"].value;
		std::cout << std::endl;
	}
	if ( usage["hex"] ){
		std::cout << "hex value is " << usage["hex"].value << " and if converted to int: " << usage["hex"].hexValue() << std::endl;
	}
	if ( usage["sudo"] ) std::cout << "and user can sudo" << std::endl;
	std::cout << std::endl;

	if ( !usage.remainder().empty()) {
		std::cout << "remaining arguments:";
		for ( const auto &s : usage.remainder())
			std::cout << " " << s;
		std::cout << "\n" << std::endl;
	}

	return 0;
}
