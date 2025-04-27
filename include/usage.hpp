#pragma once
#include <string>
#include <ostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

class usage_t {

	public:
		struct error_t;

	private:

		class validator_t {

			friend class usage_t;

			private:
				validator_t(usage_t *u);

				std::unordered_map<std::string, std::string> values;
				std::vector<usage_t::error_t> errors;
				std::vector<std::string> remainder;
				size_t size() const;

			public:
				operator bool() const;
		};

	public:

		enum arg_flag {
			NO, REQUIRED, OPTIONAL
		};

		enum arg_type {
			STRING, INT, FLOAT, BOOL, HEX
		};

		enum error_type {
			UNKNOWN_OPTION, MISSING_ARG, ARG_VALIDATION, DUPLICATE
		};

		struct error_t {
			std::string name;
			usage_t::error_type error;
			usage_t::arg_type type = usage_t::arg_type::STRING;
			std::string value;
		};

		class args_t {
				friend class validator_t;

			private:
				std::string _cmd;
				std::vector<std::string> _vec;

			public:

				bool empty() const;
				size_t size() const;
				std::string cmd() const;
				std::string operator [](size_t index) const;

				args_t(const int argc, char **argv);
		};

		struct option_t {

			public:
				std::string key;
				std::string word;
				std::string desc;
				arg_flag flag = usage_t::arg_flag::NO;
				std::string name;
				arg_type type = usage_t::arg_type::STRING;
		};

		struct result_t {

			public:
				bool enabled = false;
				std::string value;

				operator bool() const;
				operator std::string() const;

				bool boolValue() const;
				std::string stringValue() const;
				long intValue() const;
				double doubleValue() const;
				double floatValue() const;
				long hexValue() const;
		};

		struct info_t {
			std::string name;
			std::string version_title = "\nversion ";
			std::string version;
			std::string author_title = "author ";
			std::string author;
			std::string copyright_title = "copyright ";
			std::string copyright;
			std::string usage_title = "\nusage:\n   ";
			std::string usage = "[options]";
			std::string options_title = "\nOptions:";
			std::string description;
			std::string footer;
		};

		args_t args;
		info_t info;

		std::unordered_map<std::string, option_t> options;

		validator_t validated = validator_t(this);

		result_t operator [](const std::string& name) const;
		result_t operator [](const char* name) const;

		operator std::string() const;

		std::string name() const;
		std::string cmd() const;
		std::string title() const;
		std::string version() const;
		std::string help() const;

		std::unordered_map<std::string, std::string> values() const;
		std::vector<usage_t::error_t> errors() const;
		std::vector<std::string> remainder() const;

		bool contains(const std::string& name) const;
		std::string value(const std::string& name) const;
};

std::ostream& operator <<(std::ostream& os, const usage_t::args_t& args);
std::ostream& operator <<(std::ostream& os, const usage_t& usage);
std::ostream& operator <<(std::ostream& os, const usage_t* usage);
std::ostream& operator <<(std::ostream& os, const std::vector<usage_t::error_t>& errors);
std::ostream& operator <<(std::ostream& os, const std::vector<usage_t::error_t>* errors);
