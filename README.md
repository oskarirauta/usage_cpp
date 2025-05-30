[![License:MIT](https://img.shields.io/badge/License-MIT-blue?style=plastic)](LICENSE)
[![C++ CI build](../../actions/workflows/build.yml/badge.svg)](../../actions/workflows/build.yml)
### usage_cpp

C++ kickstarter for application; usage_cpp helps to easily create a usage/help parts of (console) applications
and validate/parse them. There is no support for parsing other kinds of variables, besides strings or any
kind of method built-in to validate them not being parameters.

Such as this:
```
-s -s
```

if there is a option with `-s` as it's keyword and it requires a argument, it's argument will be -s.
Some argument format validation also exists, available types are string, number, float, bool and hex.
Arguments of options can be either not used, optional or required.

Sample of usage definition:
```
        
	usage_t usage = {
		.args = { argc, argv },
		.info = {
			.name = "usage_cpp",
			.version = "1.5.0",
			.author = "Oskari Rauta",
			.copyright = "2024, Oskari Rauta"
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
```

To make usage_cpp work, it requires only .args and .options - and .name, .version, .author and .copyright
are optional, you can even use them partially and they are ignored if left empty.

options is std::unordered_map which is reverse iterated when composing usage/help.

### <sub>Note</sub>
usage_cpp replaces my previous library [cmdparser_cpp](https://github.com/oskarirauta/cmdparser_cpp) that has some similar
functionality. That repository is no longer updated and is now archived.


## <sub>Depends</sub>

usage_cpp does not have depencies

## <sub>Importing</sub>

 - clone usage_cpp as a submodule to usage
 - include usage_cpp's Makefile.incs in your Makefile
 - link with USAGE_OBJS

## <sub>Example</sub>

Example code is provided.
