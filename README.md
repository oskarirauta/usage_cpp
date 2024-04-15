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
All other kind of validation, such as format of argument - is not handled by this library - but
defining available options and querying them, is very simple and needs very little of coding.

Sample of usage definition:
```
        
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
			{ "help", { .key = "h", .word = "help", .desc = "usage help" }}
		}
	};
```

To make usage_cpp work, it requires only .args and .options - and .name, .version, .author and .copyright
are optional, you can even use them partially and they are ignored if left empty.

options is a [ordered_map](https://github.com/Tessil/ordered-map) by [Tessil](https://github.com/Tessil) which
I have merged to my [common_cpp](https://github.com/oskarirauta/common_cpp) library.

`tsl::ordered_map<std::string, { .key = string, .word = string, .desc = string, .needs_arg = bool, .arg_desc = string }>`

For full usage test, check provided example.

### <sub>Note</sub>
usage_cpp replaces my previous library [cmdparser_cpp](https://github.com/oskarirauta/cmdparser_cpp) that has some similar
functionality. That repository is no longer updated and is now archived.


## <sub>Depends</sub>

 - [common_cpp](https://github.com/oskarirauta/common_cpp)

## <sub>Importing</sub>

 - clone common_cpp as a submodule to common
 - clone usage_cpp as a submodule to usage
 - include common_cpp's and usage_cpp's Makefile.incs in your Makefile
 - link with COMMON_OBJS and USAGE_OBJS

## <sub>Example</sub>

Example code is provided.
