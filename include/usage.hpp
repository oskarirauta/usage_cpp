#pragma once
#include <string>
#include <ostream>
#include <vector>
#include "tsl/ordered_map.h"

struct args_t {

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
		bool needs_arg = false;
		std::string arg_name;
};

class usage_t {

	public:

		struct result_t {

			public:
				bool enabled = false;
				std::string value;

				operator bool();
				operator std::string();
		};

		args_t args;
		std::string name;
		std::string version_title = "\nVersion ";
		std::string version;
		std::string author_title = "Author ";
		std::string author;
		std::string copyright_title = "Copyright ";
		std::string copyright;
		tsl::ordered_map<std::string, option_t> options;

		result_t operator [](const std::string& name) const;
		result_t operator [](const char* name) const;

		operator std::string() const;

		std::string cmd() const;
		std::string title() const;
		std::string help() const;
		void validate() const;
};

std::ostream& operator <<(std::ostream& os, const args_t& args);
std::ostream& operator <<(std::ostream& os, const usage_t& usage);
std::ostream& operator <<(std::ostream& os, const usage_t* usage);