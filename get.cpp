#include "cppp.hpp"

namespace get {
	int file_type(std::string filename)
	{
		if (filename == "main")
			return (error::msg("The main file must be made with a separate command."));
		else if (filename[0] == 'I' && filename[1] && isupper(filename[1]))
			return (TYPE_INTERFACE);
		else if (filename[0] == 'A' && filename[1] && isupper(filename[1]))
			return (TYPE_ABSTRACT);
		else if (isupper(filename[0]))
			return (TYPE_CLASS);
		else
			return (TYPE_SOURCE);
	}

	std::string file_name(std::string file_path)
	{
		return (file_path.substr(file_path.rfind("/") + 1));
	}

	std::string path(std::string name, int cmd, int path_type)
	{
		std::string path = std::string(std::getenv("PWD"));
		path.push_back('/');
		if (path_type == PATH_HEADER) {
			if (cmd != CMD_STARTP)
				path.append(name).append(".hpp");
			else
				path.append(name).append(".class.hpp");
		}
		else if (path_type == PATH_SOURCE)
		{
			if (cmd != CMD_STARTP)
				path.append(name).append(".cpp");
			else
				path.append(name).append(".class.cpp");
		} else {
			throw("Unable to create file path for some reason.");
		}
		return (path);
	}

	std::string header_guard(std::string name, int cmd)
	{
		unsigned int i = 0;
		std::string ret = "";
		while (i < name.length())
		{
			if (isupper(name[i]))
			{
				if (i != 0)
					ret.push_back('_');
				ret.push_back(name[i]);
			}
			else
				ret.push_back(static_cast<char>(toupper(name[i])));
			i++;
		}
		{
			std::string suffix;
			if (cmd == CMD_STARTP)
				suffix = ".class.hpp";
			else
				suffix = ".hpp";
			unsigned int i = 0;
			while (i < suffix.length())
			{
				if (suffix[i] != '.')
					ret.push_back(static_cast<char>(toupper(suffix[i])));
				else
					ret.push_back('_');
				i++;
			}
		}
		return (ret);
	}

	std::string string_from_file(std::string name, int cmd, int path_type)
	{
		std::ifstream in(get::path(name, cmd, path_type));
		if (!in.is_open())
			return std::string("");
		std::string rstring;
		std::string line;
		while (std::getline(in, line))
			rstring.append(line).push_back('\n');
		return (rstring);
	}

	bool sstream_with_target(std::istringstream& stream, std::string& line, \
	std::string target, std::ofstream& out, bool print_opt)
	{
		while (std::getline(stream, line))
		{
			if (line.find(target) != std::string::npos)
				return (true);
			if (print_opt)
				out << line << std::endl;
		}
		throw ("get::sstream_with_target error");
		return (false);
	}

	bool sstream_with_jump(std::istringstream& stream, std::string& line, \
	int jump_count, std::ofstream& out, bool print_opt)
	{
		for (int i = 0; i < jump_count; i++)
		{
			if (!std::getline(stream, line))
				throw("get::sstream_with_jump error");
			else if (print_opt && i < jump_count - 1)
				out << line << std::endl;
		}
		return (true);
	}

	bool is_right_extension_file(std::string path, std::string extension)
	{
		std::string filename = get::file_name(path);
		if (filename.size() < extension.size() + 1)
			return (false);
		if (filename.substr(filename.size() - extension.size()) == extension)
			return (true);
		return (false);
	}
}
