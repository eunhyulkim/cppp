#include "cppp.hpp"
#include <experimental/filesystem>

namespace fs = std::filesystem;

namespace {
	void print_default_include(std::ofstream& out)
	{
		std::string includes[] = \
		{"string", "iostream", "fstream", "cstdlib", "iomanip"};

		for (int i = 0; (unsigned long)i < (sizeof(includes) / sizeof(includes[0])); i++)
			out << "#include <" << includes[i] << ">" << std::endl;
	}

	void print_custom_include(std::ofstream& out, std::string filename)
	{
		std::string root_path = std::string(std::getenv("PWD"));
		for(auto& p: fs::directory_iterator(root_path))
		{
			if (fs::is_directory(p.path()))
			{
				std::string dir_name = get::file_name(p.path());
				if (dir_name == "include" || dir_name == "includes")
				{
					for(auto& q: fs::directory_iterator(p.path()))
					{
 						if (get::is_right_extension_file(q.path(), ".hpp"))
						 {
							out << "#include \"" << get::file_name(q.path());
							out << "\"" << std::endl;
						 }
					}
				}
			}
			else if (get::is_right_extension_file(p.path(), ".hpp"))
			{
				std::string file_path = get::file_name(p.path());
				if (file_path == filename + ".hpp")
					continue;
				out << "#include \"" << file_path;
				out << "\"" << std::endl;
			}
		}
	}

	std::string *get_function_lines(std::string name, std::string& sstring, int& idx)
	{
		std::istringstream ss(sstring);
		std::ofstream dummy;
		std::string line;

		if (sstring.find("namespace " + name) == std::string::npos)
			return (nullptr);

		get::sstream_with_target(ss, line, "namespace " + name, dummy, false);
		std::string *function_lines = new std::string[20];

		while (std::getline(ss, line))
		{
			if (line.empty() || line.find("/*") != std::string::npos)
				continue;
			if (line[0] == '}' || line.find("#") != std::string::npos)
				break ;
			if (line[0] != '\t' || !line[1] || line[1] == '\t' || line[1] == '{' || line[1] == '}')
				continue;
			if (line.find("(") == std::string::npos)
				continue;
			if (line[line.size() - 1] == ';')
				continue;
			line = line.substr(1);
			if (line[line.size() - 1] == '}')
				line = line.substr(0, line.size() - 2);
			if (line[line.size() - 1] == ' ')
				line = line.substr(0, line.size() - 1);
			function_lines[idx] = line;
			if (line[line.size() - 1] == '\\')
			{
				std::getline(ss, line);
				function_lines[idx].append("\n" + line);
			}
			idx++;
		}
		if (idx == 0)
			delete[] function_lines;
		return (function_lines);
	}

	void input_functions(std::ofstream& out, std::string name, \
	std::string *function_lines, int line_size)
	{
		out << std::endl;
		out << "namespace " << name << " {" << std::endl;
		for (int i = 0; i < line_size; i++)
			out << "\t" << function_lines[i] << ";" << std::endl;
		out << "}" << std::endl;
		return ;
	}

	void print_namespace(std::ofstream& out, std::string file_name)
	{
		std::string name = file_name.substr(0, file_name.size() - 4);
		std::string sstring = get::string_from_file(name, CMD_START, PATH_SOURCE);
		if (sstring.empty())
			return ;

		int line_size = 0;
		std::string *function_lines = get_function_lines(name, sstring, line_size);
		if (line_size == 0)
			return ;

		input_functions(out, name, function_lines, line_size);

		delete[] function_lines;
		return ;
	}

	void print_namespaces(std::ofstream& out)
	{
		std::string root_path = std::string(std::getenv("PWD"));
		for(auto& p: fs::directory_iterator(root_path))
		{
			if (fs::is_directory(p.path()))
			{
				for(auto& q: fs::directory_iterator(p.path()))
				{
					std::string file_name = get::file_name(q.path());
					if (!islower(file_name[0]))
						continue;
					if (get::is_right_extension_file(q.path(), ".cpp"))
						print_namespace(out, file_name);
				}
			}
			else if (get::is_right_extension_file(p.path(), ".cpp"))
			{
				std::string file_name = get::file_name(p.path());
				if (islower(file_name[0]))
					print_namespace(out, file_name);
			}
		}
	}
}

namespace cmd_main {
	void	create_main_file(std::ofstream& out, std::string file_name)
	{
		out << "#include \"" << file_name << ".hpp\"" << std::endl;
		out << std::endl;
		out << "int\t\tmain(void)" << std::endl;
		out << "{" << std::endl;
		out << "\tint\ti;\n\n\t\n" << "\treturn (0);" << std::endl;
		out << "}" << std::endl;
	}

	void	create_header_file(std::ofstream& out, std::string filename)
	{
		print_default_include(out);
		out << std::endl;
		print_custom_include(out, filename);
		print_namespaces(out);
	}

	void	main(int ac, char *av[])
	{
		reset::backup(ac, av, CMD_MAIN);
		std::string file_name = av[2];

		std::string path = get::path(file_name, CMD_START, PATH_SOURCE);
		std::ofstream sout(path, std::ofstream::trunc);
		cmd_main::create_main_file(sout, file_name);

		path = get::path(file_name, CMD_START, PATH_HEADER);
		std::ofstream hout(path, std::ofstream::trunc);
		cmd_main::create_header_file(hout, file_name);
		hout.close();
		sout.close();
		return ;
	}
}
