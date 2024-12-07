#include <sstream>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <algorithm>
#include "common.hpp"

#include "usage.hpp"

static std::string line_feed(const std::stringstream& ss) {

	if ( ss.str().empty())
		return "";
	return "\n";
}

usage_t::args_t::args_t(const int argc, char **argv) {

	this -> _cmd = argv[0];
	for ( int i = 1; i < argc; i++ )
		this -> _vec.push_back(argv[i]);
}

size_t usage_t::args_t::size() const {

	return this -> _vec.size();
}

bool usage_t::args_t::empty() const {

	return this -> _vec.empty();
}

std::string usage_t::args_t::cmd() const {

	return this -> _cmd;
}

std::string usage_t::args_t::operator [](size_t index) const {

	if ( index >= this -> _vec.size())
		throw std::runtime_error("argument " + std::to_string(index) + " is out of bounds(" + std::to_string(this -> _vec.size()) + ")");

	return this -> _vec[index];
}

usage_t::result_t::operator bool() const {
	return this -> enabled;
}

usage_t::result_t::operator std::string() const {
	return this -> value;
}

bool usage_t::result_t::boolValue() const {
	return common::to_lower(common::trim_ws(this -> value)) == "true" ? true : false;
}

std::string usage_t::result_t::stringValue() const {
	return this -> value;
}

long usage_t::result_t::intValue() const {

	long res = 0;

	try {
		res = std::stol(this -> value);
	} catch (...) {
		res = 0;
	}
	return res;
}

double usage_t::result_t::doubleValue() const {

	double res = 0;

	try {
		res = std::stod(this -> value);
	} catch (...) {
		res = 0;
	}
	return res;
}

double usage_t::result_t::floatValue() const {
	return this -> doubleValue();
}

long usage_t::result_t::hexValue() const {

	long res = 0;

	try {
		res = std::stol(this -> value, nullptr, 0);
	} catch (...) {
		res = 0;
	}
	return res;
}

std::string usage_t::name() const {

	return this -> info.name;
}

std::string usage_t::cmd() const {

	return this -> args.cmd();
}

std::string usage_t::title() const {

	std::stringstream ss;

	if ( !this -> info.name.empty()) {

		ss << this -> info.name;
		if ( !this -> info.version.empty()) {

			if ( this -> info.version.front() != '\n' )
				ss << ' ';

			if ( !this -> info.version_title.empty())
				ss << this -> info.version_title << this -> info.version;
			else ss << this -> info.version;

		}

	} else if ( !this -> info.version.empty()) {

		std::string t = this -> info.version_title;
		while ( !t.empty() && ( std::isspace(t.front()) || t.front() == '\n' || t.front() == '\r' ))
			t.erase(0, 1);

		if ( !t.empty())
			ss << t << this -> info.version;
		else ss << this -> info.version;
	}

	if ( !this -> info.author.empty()) ss << line_feed(ss) << this -> info.author_title << this -> info.author;
	if ( !this -> info.copyright.empty()) ss << line_feed(ss) << this -> info.copyright_title << this -> info.copyright;

	return ss.str();
}

std::string usage_t::version() const {

	std::stringstream ss;

	if ( !this -> info.name.empty()) {

		ss << this -> info.name;
		if ( !this -> info.version.empty()) {

			if ( this -> info.version.front() != '\n' )
				ss << ' ';

			if ( !this -> info.version_title.empty())
				ss << this -> info.version_title << this -> info.version;
			else ss << this -> info.version;

		}

	} else if ( !this -> info.version.empty()) {

		std::string t = this -> info.version_title;
		while ( !t.empty() && ( std::isspace(t.front()) || t.front() == '\n' || t.front() == '\r' ))
			t.erase(0, 1);

		if ( !t.empty())
			ss << t << this -> info.version;
		else ss << this -> info.version;
	}

	return ss.str();
}

usage_t::result_t usage_t::operator [](const std::string& name) const {

	auto values = this -> values();
	if ( values.contains(name))
		return { .enabled = true, .value = values[name] };
	else return { .enabled = false };
}

usage_t::result_t usage_t::operator [](const char* name) const {
	return this -> operator [](std::string(name));
}

std::string usage_t::value(const std::string& name) const {
	auto values = this -> values();
	return values.contains(name) ? values[name] : "";
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
		//if ( it -> second.needs_arg ) i += 1 + 2 + ( it -> second.arg_name.empty() ? 3 : it -> second.arg_name.size());
		if ( it -> second.flag != usage_t::arg_flag::NO ) i += 1 + 2 + ( it -> second.name.empty() ? 3 : it -> second.name.size());

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
		} else while ( row.size() < ks + 5 )
			row += ' ';

		if ( !it -> second.word.empty())
			row += "--" + it -> second.word;

		if ( it -> second.flag == usage_t::arg_flag::REQUIRED )
			row += " <" + ( it -> second.name.empty() ? "arg" : it -> second.name ) + ">";
		else if ( it -> second.flag == usage_t::arg_flag::OPTIONAL )
			row += " [" + ( it -> second.name.empty() ? "arg" : it -> second.name ) + "]";

		while ( row.size() < ms )
			row += ' ';

		if ( !it -> second.desc.empty())
			row += it -> second.desc;

		if ( !s.empty())
			s += '\n';
		s += row;
	}

	if ( !this -> info.footer.empty())
		s += "\n" + this -> info.footer;

	return s;
}

usage_t::operator std::string() const {

	return this -> title();
}

std::ostream& operator <<(std::ostream& os, const usage_t::args_t& args) {

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
