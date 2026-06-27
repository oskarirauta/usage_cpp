#include <sstream>
#include <vector>
#include <type_traits>
#include <stdexcept>

#include "usage.hpp"

static const std::string whitespace = " \t\n\r\f\v";

static std::string line_feed(const std::stringstream& ss) {

	if ( ss.str().empty())
		return "";
	return "\n";
}

static std::string to_lower(const std::string& str) {

        std::string _str(str);

        for ( auto& ch : _str )
                if ( std::isupper(ch))
                        ch ^= 32;
        return _str;
}

static std::string trim(const std::string& str) {

	std::string _str(str);
	_str.erase(_str.find_last_not_of(whitespace) + 1);
	_str.erase(0, _str.find_first_not_of(whitespace));
	return _str;
}

template <typename K, typename V>
static bool map_contains(const std::unordered_map<K, V>& _m, K val) {

	return std::find_if(_m.begin(), _m.end(), [&val](const auto& p) { return p.first == val; }) != _m.end();
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
	return to_lower(trim(this -> value)) == "true" ? true : false;
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

			// space the name from what follows, unless that already starts on a
			// new line (version_title is what immediately follows the name)
			if ( ( this -> info.version_title.empty() ? this -> info.version.front()
			                                          : this -> info.version_title.front()) != '\n' )
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

			// space the name from what follows, unless that already starts on a
			// new line (version_title is what immediately follows the name)
			if ( ( this -> info.version_title.empty() ? this -> info.version.front()
			                                          : this -> info.version_title.front()) != '\n' )
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
	if ( map_contains(values, name))
		return { .enabled = true, .value = values[name] };
	else return { .enabled = false };
}

usage_t::result_t usage_t::operator [](const char* name) const {
	return this -> operator [](std::string(name));
}

std::string usage_t::value(const std::string& name) const {
	auto values = this -> values();
	return map_contains(values, name) ? values[name] : "";
}

std::string usage_t::help() const {

	if ( this -> options.empty())
		return "No options available for this program";

	size_t ks = 0;
	std::string s;

	if ( !this -> info.usage_title.empty() || !this -> info.usage.empty()) {

		if ( !this -> info.usage_title.empty())
			s += this -> info.usage_title;

		s += this -> args.cmd();

		if ( !this -> info.usage.empty())
			s += " " + this -> info.usage;

		s += "\n";
	}

	if ( !this -> info.description.empty())
		s += this -> info.description;

	if ( !this -> info.options_title.empty())
		s += this -> info.options_title;

	// find size of shortest key
	for ( auto it = this -> options.begin(); it != this -> options.end(); it++ ) {

		if ( it -> first.empty() || ( it -> second.key.empty() && it -> second.word.empty()))
			continue;

		if ( !it -> second.key.empty() && ks < ( it -> second.key.size() + 1 ))
			ks = it -> second.key.size() + 1;
	}

	// Build each option's left part (everything up to the description), measure
	// the widest, then align every description to a common column. (Computing a
	// width up front mis-counted the key-column padding, so long options ran into
	// their descriptions.)
	std::vector<std::pair<std::string, std::string>> rows;   // { left part, description }
	size_t widest = 0;

	for ( auto it = this -> options.begin(); it != this -> options.end(); it++ ) {

		if ( it -> first.empty() || ( it -> second.key.empty() && it -> second.word.empty()))
			continue;

		std::string row = "   ";

		if ( !it -> second.key.empty()) {

			row += "-" + it -> second.key;
			if ( !it -> second.word.empty())
				row += ", ";

			while ( row.size() < ks + 5 )
				row += " ";

		} else while ( row.size() < ks + 5 )
			row += ' ';

		if ( !it -> second.word.empty())
			row += "--" + it -> second.word;

		if ( it -> second.flag == usage_t::arg_flag::REQUIRED )
			row += " <" + ( it -> second.name.empty() ? "arg" : it -> second.name ) + ">";
		else if ( it -> second.flag == usage_t::arg_flag::OPTIONAL )
			row += " [" + ( it -> second.name.empty() ? "arg" : it -> second.name ) + "]";

		if ( row.size() > widest )
			widest = row.size();

		rows.push_back({ row, it -> second.desc });
	}

	for ( const auto& r : rows ) {

		std::string row = r.first;

		if ( !r.second.empty()) {
			while ( row.size() < widest )
				row += ' ';
			row += "   " + r.second;   // gap between the option column and its description
		}

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
