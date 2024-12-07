#include <sstream>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <algorithm>

#include "common.hpp"
#include "usage.hpp"

tsl::ordered_map<std::string, std::string> usage_t::values() const {
	return std::as_const(this -> validated.values);
}

std::vector<usage_t::error_t> usage_t::errors() const {
	return std::as_const(this -> validated.errors);
}

std::vector<std::string> usage_t::remainder() const {
	return std::as_const(this -> validated.remainder);
}

size_t usage_t::validator_t::size() const {
	return this -> values.size();
}

usage_t::validator_t::operator bool() const {
	return this -> errors.empty();
}

bool usage_t::contains(const std::string& name) const {
	return this -> validated.values.contains(name);
}

usage_t::validator_t::validator_t(usage_t *u) {

	bool waiting_arg = false;
	bool optional_arg = false;
	bool parsing = true;
	usage_t::arg_type arg_type = usage_t::arg_type::STRING;
	std::string o;

	for ( const auto& s : u -> args._vec ) {

		if ( !parsing ) {
			this -> remainder.push_back(s);
			continue;
		}

		std::string a(s);
		std::string alt_name;
		std::string alt_option;
		std::string alt_value;
		usage_t::arg_type alt_type = usage_t::arg_type::STRING;
		bool alt = false;

		if ( !waiting_arg && !optional_arg ) {

			a = common::trim_ws(a);

			while ( common::has_prefix(a, "--") && a != "--" )
				a.erase(0, 1);

			if ( a.empty())
				continue;

			if ( !common::has_prefix(a, "-") || a == "--" ) {
				parsing = false;
				waiting_arg = false;
				optional_arg = false;
				arg_type = usage_t::arg_type::STRING;
				o = "";
				if ( a != "--" )
					this -> remainder.push_back(a);
				continue;
			}

			a.erase(0,1);
			a = common::ltrim_ws(a);

			if ( a.empty())
				continue;

			if ( auto pos = a.find('='); pos != std::string::npos && a.at(0) != '0' ) {
				alt_option = a.substr(0, pos);
				alt_value = a.substr(pos, a.size() - pos);
				if ( alt_value.front() == '=' )
					alt_value.erase(0, 1);
			}

			bool valid = false;

			for ( auto& opt : u -> options ) {
				if ( a == opt.second.key || a == opt.second.word ) {
					valid = true;
					waiting_arg = opt.second.flag == usage_t::arg_flag::REQUIRED ? true : false;
					optional_arg = opt.second.flag == usage_t::arg_flag::OPTIONAL ? true : false;
					arg_type = opt.second.type;
					o = opt.first;
					break;
				}
			}

			if ( !valid && !alt_option.empty()) {

				for ( auto& opt : u -> options ) {

					if (( alt_option == opt.second.key || alt_option == opt.second.word ) && ( opt.second.flag != usage_t::arg_flag::NO )) {
						alt = true;
						alt_type = opt.second.type;
						alt_name = opt.first;
						break;
					}
				}
			}

			if ( !valid && !alt ) {
				this -> errors.push_back({ .name = ( alt_option.empty() ? a : alt_option ), .error = usage_t::error_type::UNKNOWN_OPTION });
				continue;
			}

			if ( !alt ) {

				if ( this -> values.contains(o))
					this -> errors.push_back({ .name = o, .error = usage_t::error_type::DUPLICATE, .type = arg_type });

				this -> values[o] = "";

				continue;
			}
		}

		if ( alt ) {

			if ( this -> values.contains(alt_name))
				this -> errors.push_back({ .name = alt_name, .error = usage_t::error_type::DUPLICATE, .type = alt_type });

			if ( alt_type != usage_t::arg_type::STRING )
				alt_value = common::trim_ws(alt_value);

			if ( alt_type == usage_t::arg_type::INT && !common::is_number(alt_value)) {
				this -> errors.push_back({ .name = alt_name, .error = usage_t::error_type::ARG_VALIDATION, .type = alt_type, .value = alt_value });
				if ( this -> values.contains(alt_name))
					this -> values.erase(alt_name);
				waiting_arg = false;
				optional_arg = false;
				arg_type = usage_t::arg_type::STRING;
				o = "";
				continue;
			} else if ( alt_type == usage_t::arg_type::FLOAT && !common::is_float(alt_value)) {
				this -> errors.push_back({ .name = alt_name, .error = usage_t::error_type::ARG_VALIDATION, .type = alt_type, .value = alt_value });
				if ( this -> values.contains(alt_name))
					this -> values.erase(alt_name);
				waiting_arg = false;
				optional_arg = false;
				arg_type = usage_t::arg_type::STRING;
				o = "";
				continue;
                        } else if ( alt_type == usage_t::arg_type::HEX ) {

                                std::string v(alt_value);

				if ( v.size() > 1 && v.front() == '#' && std::isdigit(v.at(1))) {
					v.erase(0, 1);
					v = "0x" + v;
				} else if ( v.size() > 1 && v.front() == 'x' )
					v = "x" + v;
				else if ( !common::has_prefix(v, "0x"))
					v = "0x" + v;

				if ( !common::is_hex(v)) {
					this -> errors.push_back({ .name = alt_name, .error = usage_t::error_type::ARG_VALIDATION, .type = alt_type, .value = alt_value });
					if ( this -> values.contains(alt_name))
						this -> values.erase(alt_name);
					waiting_arg = false;
					optional_arg = false;
					arg_type = usage_t::arg_type::STRING;
					o = "";
					continue;
				} else alt_value = v;


			} else if ( alt_type == usage_t::arg_type::BOOL ) {

				std::string v(common::to_lower(alt_value));
				if ( v == "0" || v == "no" || v == "disabled" ) v = "false";
				else if ( v == "1" || v == "yes" || v == "enabled" ) v = "true";

				if ( v != "false" && v != "true" ) {
					this -> errors.push_back({ .name = alt_name, .error = usage_t::error_type::ARG_VALIDATION, .type = alt_type, .value = alt_value });
					if ( this -> values.contains(alt_name))
						this -> values.erase(alt_name);
					waiting_arg = false;
					optional_arg = false;
					arg_type = usage_t::arg_type::STRING;
					o = "";
					continue;
				} else a = v;
			}

			this -> values[alt_name] = alt_value;
			waiting_arg = false;
			optional_arg = false;
			arg_type = usage_t::arg_type::STRING;
			o = "";
			continue;
		}

		if ( o.empty() || alt ) {

			waiting_arg = false;
			optional_arg = false;
			arg_type = usage_t::arg_type::STRING;
			continue;
		}

		if ( waiting_arg ) {

			if ( arg_type != usage_t::arg_type::STRING )
				a = common::trim_ws(a);

			if ( arg_type == usage_t::arg_type::INT && !common::is_number(a)) {
				this -> errors.push_back({ .name = o, .error = usage_t::error_type::ARG_VALIDATION, .type = arg_type, .value = a });
				this -> values.erase(o);
				waiting_arg = false;
				optional_arg = false;
				arg_type = usage_t::arg_type::STRING;
				o = "";
				continue;
			} else if ( arg_type == usage_t::arg_type::FLOAT && !common::is_float(a)) {
				this -> errors.push_back({ .name = o, .error = usage_t::error_type::ARG_VALIDATION, .type = arg_type, .value = a });
				this -> values.erase(o);
				waiting_arg = false;
				optional_arg = false;
				arg_type = usage_t::arg_type::STRING;
				o = "";
				continue;
			} else if ( arg_type == usage_t::arg_type::HEX ) {

				std::string v(a);

				if ( v.size() > 1 && v.front() == '#' && std::isdigit(v.at(1))) {
					v.erase(0, 1);
					v = "0x" + v;
				} else if ( v.size() > 1 && v.front() == 'x' )
					v = "x" + v;
				else if ( !common::has_prefix(v, "0x"))
					v = "0x" + v;

				if ( !common::is_hex(v)) {
					this -> errors.push_back({ .name = o, .error = usage_t::error_type::ARG_VALIDATION, .type = arg_type, .value = a });
					this -> values.erase(o);
					waiting_arg = false;
					optional_arg = false;
					arg_type = usage_t::arg_type::STRING;
					o = "";
					continue;
				} else a = v;
			} else if ( arg_type == usage_t::arg_type::BOOL ) {

				std::string v(common::to_lower(a));
				if ( v == "0" || v == "no" || v == "disabled" ) v = "false";
				else if ( v == "1" || v == "yes" || v == "enabled" ) v = "true";

				if ( v != "false" && v != "true" ) {
					this -> errors.push_back({ .name = o, .error = usage_t::error_type::ARG_VALIDATION, .type = arg_type, .value = a });
					this -> values.erase(o);
					waiting_arg = false;
					optional_arg = false;
					arg_type = usage_t::arg_type::STRING;
					o = "";
					continue;
				} else a = v;
			}

			this -> values[o] = a;

		} else if ( optional_arg ) {

			std::string v(common::trim_ws(a));

			while ( common::has_prefix(v, "--") && v != "--" )
				v.erase(0, 1);

			if ( !v.empty() && common::has_prefix(v, "-")) {

				v.erase(0,1);
				v = common::ltrim_ws(v);

				if ( !v.empty()) {

					bool valid = false;
					std::string _o = o;

					for ( auto& opt : u -> options ) {
						if ( v == opt.second.key || v == opt.second.word ) {
							valid = true;
							waiting_arg = opt.second.flag == usage_t::arg_flag::REQUIRED ? true : false;
							optional_arg = opt.second.flag == usage_t::arg_flag::OPTIONAL ? true : false;
							arg_type = opt.second.type;
							o = opt.first;
							break;
						}
					}

					if ( valid && o != _o ) {

						if ( this -> values.contains(o))
							this -> errors.push_back({ .name = o, .error = usage_t::error_type::DUPLICATE, .type = arg_type });

						this -> values[o] = "";
						continue;
					}
				}
			}

			if ( arg_type != usage_t::arg_type::STRING )
				a = common::trim_ws(a);

			if ( arg_type == usage_t::arg_type::INT && !common::is_number(a)) {
				this -> errors.push_back({ .name = o, .error = usage_t::error_type::ARG_VALIDATION, .type = arg_type, .value = a });
				this -> values.erase(o);
				waiting_arg = false;
				optional_arg = false;
				arg_type = usage_t::arg_type::STRING;
				o = "";
				continue;
			} else if ( arg_type == usage_t::arg_type::FLOAT && !common::is_float(a)) {
				this -> errors.push_back({ .name = o, .error = usage_t::error_type::ARG_VALIDATION, .type = arg_type, .value = a });
				this -> values.erase(o);
				waiting_arg = false;
				optional_arg = false;
				arg_type = usage_t::arg_type::STRING;
				o = "";
				continue;
			} else if ( arg_type == usage_t::arg_type::HEX ) {

				std::string v(a);

				if ( v.size() > 1 && v.front() == '#' && std::isdigit(v.at(1))) {
					v.erase(0, 1);
					v = "0x" + v;
				} else if ( v.size() > 1 && v.front() == 'x' )
					v = "x" + v;
				else if ( !common::has_prefix(v, "0x"))
					v = "0x" + v;

				if ( !common::is_hex(v)) {
					this -> errors.push_back({ .name = o, .error = usage_t::error_type::ARG_VALIDATION, .type = arg_type, .value = a });
					this -> values.erase(o);
					waiting_arg = false;
					optional_arg = false;
					arg_type = usage_t::arg_type::STRING;
					o = "";
					continue;
				} else a = v;
			} else if ( arg_type == usage_t::arg_type::BOOL ) {

				std::string v(common::to_lower(a));
				if ( v == "0" || v == "no" || v == "disabled" ) v = "false";
				else if ( v == "1" || v == "yes" || v == "enabled" ) v = "true";

				if ( v != "false" && v != "true" ) {
					this -> errors.push_back({ .name = o, .error = usage_t::error_type::ARG_VALIDATION, .type = arg_type, .value = a });
					this -> values.erase(o);
					waiting_arg = false;
					optional_arg = false;
					arg_type = usage_t::arg_type::STRING;
					o = "";
					continue;
				} else a = v;
			}

			this -> values[o] = a;
		}

		waiting_arg = false;
		optional_arg = false;
		arg_type = usage_t::arg_type::STRING;
		o = "";

	}

	if ( !o.empty() && waiting_arg ) {
		this -> errors.push_back({ .name = o, .error = usage_t::error_type::MISSING_ARG, .type = arg_type });
		this -> values.erase(o);
	}
}

std::ostream& operator <<(std::ostream& os, const std::vector<usage_t::error_t>& errors) {

	for ( auto &e : errors ) {

		os << " --" << e.name << ": ";

		if ( e.error == usage_t::error_type::DUPLICATE ) os << "duplicated option";
		else if ( e.error == usage_t::error_type::UNKNOWN_OPTION ) os << "unknown option";
		else if ( e.error == usage_t::error_type::MISSING_ARG ) os << "missing required arg";
		else if ( e.error == usage_t::error_type::ARG_VALIDATION ) {
			os << "validation failure, expected ";
			if ( e.type == usage_t::arg_type::STRING ) os << "string";
			else if ( e.type == usage_t::arg_type::INT ) os << "integer number";
			else if ( e.type == usage_t::arg_type::FLOAT ) os << "decimal number";
			else if ( e.type == usage_t::arg_type::BOOL ) os << "boolean";
			else if ( e.type == usage_t::arg_type::HEX ) os << "hexadecimal number";
			else os << "something else";
			if ( !e.value.empty())
				os << ", got " << e.value;
		}
	}

	return os;
}

std::ostream& operator <<(std::ostream& os, const std::vector<usage_t::error_t>* errors) {

	os << *errors;
        return os;
}
