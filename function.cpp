#include "cppp.hpp"

namespace {
	std::string *get_function_lines(std::string& hstring, int& idx)
	{
		std::istringstream ss(hstring);
		std::ofstream dummy;
		std::string line;

		if (hstring.find("/* declare member function */") != std::string::npos)
			get::sstream_with_target(ss, line, "/* declare member function */", dummy, false);
		else
			return (nullptr);

		std::string *function_lines = new std::string[20];
		while (std::getline(ss, line))
		{
			if (line.empty() || line.find("/*") != std::string::npos)
				break;
			if (line.find("(") == std::string::npos)
				break ;
			if (line[0] == '}' || line.find("#") != std::string::npos)
				break ;
			if (line[line.size() - 1] == '\\')
			{
				std::string app_line;
				std::getline(ss, app_line);
				app_line.erase(0, 2);
				line.append("\n" + app_line);
			}
			if (line.find(";;") == std::string::npos)
				continue;
			if (line.find("virtual") != std::string::npos)
				line = line.substr(line.find("virtual") + 8);
			else
				line = line.substr(2);
			function_lines[idx] = line.substr(0, line.size() - 2);
			idx++;
		}

		return (function_lines);
	}

	void function_update_to_header(std::string& hstring)
	{
		while (hstring.find(";;") != std::string::npos)
			hstring.replace(hstring.find(";;"), 2, ";");
		return ;
	}

	void get_func_info(std::string& line, std::string& arg, std::string& body)
	{
		int sep = line.find(" ");
		int cidx = line.find("const");

		if (cidx != -1 && cidx < static_cast<int>(line.find("(")))
			sep += (line.substr(sep + 1).find(" ") + 1);
		if (line[sep] + 1 != '*')
		{
			arg = line.substr(0, sep);
			body = line.substr(sep + 1);
		}
		else
		{
			arg = line.substr(0, sep + 2);
			body = line.substr(sep + 2);
		}
		return ;
	}

	void function_update_to_source(std::string& sstring, \
	std::string *function_lines, int line_size, std::string name)
	{
		std::string new_param;
		for (int i = 0; i < line_size; i++)
		{
			new_param.push_back('\n');

			std::string line = function_lines[i];
			std::string arg, body;
			get_func_info(line, arg, body);

			new_param.append(arg + "\n");
			new_param.append(name + "::" + body + " {\n");
			new_param.append("\t/* function body */\n");
			new_param.append("}\n");
		}

		sstring.append(new_param);
	}
}

namespace function {
	void	main(int ac, char *av[])
	{
		for (int i = 2; i < ac; i++)
		{
			std::string name = av[i];
			std::string hstring = get::string_from_file(name, CMD_START, PATH_HEADER);
			std::string sstring = get::string_from_file(name, CMD_START, PATH_SOURCE);

			int line_size = 0;
			std::string *function_lines = get_function_lines(hstring, line_size);

			if (hstring.empty() || sstring.empty() || line_size == 0)
			{
				delete[] function_lines;
				throw ("The file is empty or did not open normally.");
			}

			function_update_to_header(hstring);
			std::ofstream hout(get::path(name, CMD_START, PATH_HEADER), std::ofstream::trunc);
			hout << hstring;

			function_update_to_source(sstring, function_lines, line_size, name);
			std::ofstream sout(get::path(name, CMD_START, PATH_SOURCE), std::ofstream::trunc);
			sout << sstring;

			delete[] function_lines;
		}
		return ;
	}
}
