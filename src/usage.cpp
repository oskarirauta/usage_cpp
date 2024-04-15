#include <sstream>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <algorithm>

#include "usage.hpp"

static std::string line_feed(const std::stringstream& ss) {

	if ( ss.str().empty())
		return "";
	return "\n";
}

args_t::args_t(const int argc, char **argv) {

	this -> _cmd = argv[0];
	for ( int i = 1; i < argc; i++ )
		this -> _vec.push_back(argv[i]);
}

size_t args_t::size() const {

	return this -> _vec.size();
}

bool args_t::empty() const {

	return this -> _vec.empty();
}

std::string args_t::cmd() const {

	return this -> _cmd;
}

std::string args_t::operator [](size_t index) const {

	if ( index >= this -> _vec.size())
		throw std::runtime_error("argument " + std::to_string(index) + " is out of bounds(" + std::to_string(this -> _vec.size()) + ")");

	return this -> _vec[index];
}

usage_t::result_t::operator bool() {
	return this -> enabled;
}

usage_t::result_t::operator std::string() {
	return this -> value;
}

std::string usage_t::cmd() const {

	return this -> args.cmd();
}

std::string usage_t::title() const {

	std::stringstream ss;

	if ( !this -> name.empty()) {

		ss << this -> name;
		if ( !this -> version.empty()) {

			if ( this -> version.front() != '\n' )
				ss << ' ';

			if ( !this -> version_title.empty())
				ss << this -> version_title << this -> version;
			else ss << this -> version;

		}

	} else if ( !this -> version.empty()) {

		std::string t = this -> version_title;
		while ( !t.empty() && ( std::isspace(t.front()) || t.front() == '\n' || t.front() == '\r' ))
			t.erase(0, 1);

		if ( !t.empty())
			ss << t << this -> version;
		else ss << this -> version;
	}

	if ( !this -> author.empty()) ss << line_feed(ss) << this -> author_title << this -> author;
	if ( !this -> copyright.empty()) ss << line_feed(ss) << this -> copyright_title << this -> copyright;
	ss << line_feed(ss);

	return ss.str();
}

usage_t::result_t usage_t::operator [](const std::string& name) const {

	result_t res;
	auto it = this -> options.find(name);

	if ( name.empty() || it == this -> options.end())
		return res;

	for ( size_t i = 0; i < args.size(); i++ ) {

		std::string arg = this -> args[i];

		if ( arg.front() == '-' )
			arg.erase(0, arg.substr(0, 2) == "--" ? 2 : 1 );

		if ( arg == it -> second.key || arg == it -> second.word ) {

			res.enabled = true;

			if ( it -> second.needs_arg && i < args.size() - 1 )
				res.value = args[i + 1];

			return res;
		}
	}

	return res;
}

usage_t::result_t usage_t::operator [](const char* name) const {

	return this -> operator[](std::string(name));
}

void usage_t::validate() const {

	if ( args.empty())
		return;

	std::vector<std::string> args_found;

	size_t i = 0;
	while ( i < args.size()) {

		std::string key = "";
		std::string arg = this -> args[i];

		if ( arg.front() == '-' )
			arg.erase(0, arg.substr(0, 2) == "--" ? 2 : 1 );

		if ( arg.empty())
			throw std::runtime_error("invalid option " + this -> args[i]);

		auto it = this -> options.end();

		for ( auto _it = this -> options.begin(); _it != this -> options.end(); _it++ ) {

			if ( arg == _it -> second.key || arg == _it -> second.word ) {
				it =_it;
				break;
			}
		}

		if ( it == options.end())
			throw std::runtime_error("unknown option " + this -> args[i]);
		else if ( it -> first.empty()) continue;

		if ( std::find(args_found.begin(), args_found.end(), it -> first) != args_found.end())
			throw std::runtime_error("duplicate " + this -> args[i] + " argument");
		else args_found.push_back(it -> first);

		if ( it -> second.needs_arg ) {

			i++;
			if ( i >= args.size())
				throw std::runtime_error("option " + this -> args[this -> args.size() - 1] + " requires argument");
		}

		i++;
	}
}

std::string usage_t::help() const {

	if ( this -> options.empty())
		return "No options available for this program";

	size_t ks = 0;
	size_t ms = 0;
	std::string s;

	// find size of shortest key
	for ( auto it = this -> options.begin(); it != this -> options.end(); it++ ) {

		if ( it -> first.empty() || ( it -> second.key.empty() && it -> second.word.empty()))
			continue;

		if ( !it -> second.key.empty() && ks < ( it -> second.key.size() + 1 ))
			ks = it -> second.key.size() + 1;
	}

	// find longest key + word
	for ( auto it = this -> options.begin(); it != this -> options.end(); it++ ) {

		if ( it -> first.empty() || ( it -> second.key.empty() && it -> second.word.empty()))
			continue;

		size_t i = 0;
		if ( !it -> second.key.empty()) i += 1 + it -> second.key.size();
		if ( !it -> second.key.empty() && !it -> second.word.empty()) i += 2;
		if ( !it -> second.word.empty()) i += 2 + it -> second.word.size();
		if ( it -> second.needs_arg ) i += 1 + 2 + ( it -> second.arg_name.empty() ? 3 : it -> second.arg_name.size());

		if ( ms < i ) ms = i;
	}

	ms += 7; // prefix(3) + "tab"(4);

	for ( auto it = this -> options.begin(); it != this -> options.end(); it++ ) {

		if ( it -> first.empty() || ( it -> second.key.empty() && it -> second.word.empty()))
			continue;

		std::string row = "   ";

		if ( !it -> second.key.empty()) {

			row += "-" + it -> second.key;
			if ( !it -> second.word.empty())
				row += ", ";
		} else while ( row.size() < ks + 2 )
			row += ' ';

		if ( !it -> second.word.empty())
			row += "--" + it -> second.word;

		if ( it -> second.needs_arg )
			row += " <" + ( it -> second.arg_name.empty() ? "arg" : it -> second.arg_name ) + ">";

		while ( row.size() < ms )
			row += ' ';

		if ( !it -> second.desc.empty())
			row += it -> second.desc;

		if ( !s.empty())
			s += '\n';
		s += row;
	}

	return s;
}

usage_t::operator std::string() const {

	return this -> title();
}

std::ostream& operator <<(std::ostream& os, const args_t& args) {

	os << args.cmd();

	for ( size_t i = 0; i < args.size(); i++ )
		os << " " << args[i];
	return os;
}

std::ostream& operator <<(std::ostream& os, const usage_t& usage) {

	os << usage.operator std::string();
	return os;
}

std::ostream& operator <<(std::ostream& os, const usage_t* usage) {

	os << usage -> operator std::string();
	return os;
}
