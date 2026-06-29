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
			.version = "1.7.0",
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

options is an ordered list (`std::vector<std::pair<std::string, option_t>>`):
options are shown in usage/help in the exact order they were declared.

### <sub>Positional arguments</sub>

Arguments that are not options are collected, in order, into `usage.remainder()`.
Option parsing continues across positionals, so options may appear before, after
or in between positional arguments. This makes subcommand-style command lines
work naturally:

```
prog <command> <name> --option value
```

`<command>` and `<name>` end up in `remainder()`, while `--option` is parsed as
usual. To stop option parsing explicitly (so the rest is treated as positional
even if it looks like an option), use the `--` end-of-options marker:

```
prog --name value -- --not-an-option
```

### <sub>Subcommands</sub>

A `usage_t` may declare subcommands, each with its OWN `usage_t` - its own
options and, recursively, its own subcommands:

```
std::vector<std::pair<std::string, std::shared_ptr<usage_t>>> commands;
```

When the first positional argument matches a command name, the rest of the
command line is handed to that command's `usage_t` and parsed against ITS options
- exactly like running a separate program with `argv + 1`. Global options that
appear before the command are still parsed against the parent. A `nullptr` entry
is a **raw passthrough**: the command is recognised but its arguments are left
unparsed - useful when they are foreign flags (e.g. wrapping `docker run`).

```
usage_t prog = {
    .args = { argc, argv },
    .options = { /* global flags: -h, -v ... */ },
    .commands = {
        { "greet", std::make_shared<usage_t>(usage_t{
            .options = {{ "name", { .key = "n", .word = "name", .flag = usage_t::REQUIRED, .name = "name" }}}
        }) },
        { "raw", nullptr }    // raw passthrough -> prog.tail()
    }
};

if ( prog.subcommand() == "greet" )
    std::cout << "hello " << (std::string)(*prog.sub())["name"] << "\n";
else if ( prog.subcommand() == "raw" )
    for ( const auto& a : prog.tail()) std::cout << a << "\n";
```

Accessors:
 - `subcommand()` - the matched command name (`""` if none)
 - `sub()` - the command's `usage_t*` (`nullptr` for a raw or unmatched command)
 - `tail()` - the raw arguments that followed the command

Using `shared_ptr` keeps `usage_t` an aggregate (so `.args = { ... }` designated
initialisation still works) while still owning the subcommands: they are released
automatically with the parent, no destructor needed. Leaving `commands` empty
keeps the classic single-level behaviour unchanged.

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
