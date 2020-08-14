#include "cppp.hpp"

namespace {
	class Vars {
		public:
			static int 	count;
			std::string type;
			std::string name;
			~Vars();
	};

	Vars::~Vars() { Vars::count = 0; }
	int Vars::count = 0;
}


namespace {
	void input_base_header(std::string base, std::string derive, \
	std::string& dline, std::istringstream& dss, std::ofstream& out)
	{
		std::string word = std::string("class ") + derive;
		get::sstream_with_target(dss, dline, word, out, true);
		out << "# include \"" << base << ".hpp" << "\"" << std::endl << std::endl;
		out << word << " : public " << base << std::endl;
	}

	void input_inherit_function(std::istringstream& bss, std::istringstream& dss, \
	std::string& bline, std::string& dline, std::ofstream& out)
	{
		get::sstream_with_target(dss, dline, "~", out, true);
		get::sstream_with_target(bss, bline, "~", out, false);
		bool first_inherit = true;

		out << dline << std::endl;
		while (std::getline(bss, bline))
		{
			if (bline[0] == '}')
				break ;
			if (bline.size() < 9 || bline.substr(2, 7) != "virtual")
				continue;
			if (first_inherit)
			{
				out << std::endl;
				out << "\t\t/* inherit overload function */" << std::endl;
				first_inherit = false;
			}
			if (bline.find(" = ") != std::string::npos)
			{
				int idx = bline.find("virtual");
				int lidx = bline.find(" = ");
				out << "\t\t";
				out << bline.substr(idx, lidx - idx);
				out << ";" << std::endl;
			}
			else
				out << "\t\t" << bline.substr(bline.find("virtual")) << std::endl;
		}
	}

	void inherit_header_file(std::string base, std::string derive, \
	std::string bstring, std::string dstring, std::ofstream& out)
	{
		std::istringstream bss(bstring);
		std::istringstream dss(dstring);
		std::string bline, dline;

		input_base_header(base, derive, dline, dss, out);
		if (bstring.find("public:") == std::string::npos)
			return ;
		input_inherit_function(bss, dss, bline, dline, out);
		get::sstream_with_target(dss, dline, "#endif", out, true);
		out << dline << std::endl;
	}

	void convert_variable(std::string line, Vars *vars)
	{
		std::string word;

		line = line.substr(line.find("(") + 1);
		line = line.substr(0, line.find(")"));
		while (!line.empty())
		{
			if (line.find(",") != std::string::npos)
			{
				int idx = line.find(",");
				word = line.substr(0, idx);
				line = line.substr(idx + 2);
			}
			else
			{
				word = line;
				line = "";
			}
			int idx = word.rfind(" ");
			if (word[idx + 1] == '*')
			{
				vars[Vars::count].type = word.substr(0, idx + 2);
				vars[Vars::count].name = word.substr(idx + 2);
			}
			else
			{
				vars[Vars::count].type = word.substr(0, idx);
				vars[Vars::count].name = word.substr(idx + 1);
			}
			Vars::count += 1;
		}
		return ;
	}

	Vars *get_vars_from_base_constructor(std::string bstring, std::string base)
	{
		std::istringstream ss(bstring);
		std::string line, bword, nword;
		std::ofstream dummy;
		Vars *vars = new Vars[20];

		if (bstring.find("public:") != std::string::npos)
			get::sstream_with_target(ss, line, "public:", dummy, false);
		else {
			delete[] vars;
			throw ("Failed to find constructor variables in public areas.");
		}
		bword = base + "()";
		nword = "const &" + base;
		while (std::getline(ss, line))
		{
			if (line.empty())
				continue;
			if (line[0] == '{')
				break ;
			if (line.substr(2, base.size()) != base)
				continue;
			if (line.find(nword) != std::string::npos)
				continue;
			if (line.substr(2, base.size() + 2) == bword)
				continue;
			convert_variable(line, vars);
			break ;
		}
		return (vars);
	}

	void update_constructor_initialize_list(std::string base, std::string& sstring, Vars *vars)
	{
		std::string find_string = "/* constructor initialize list */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param = base + "(";
		int count = 0;
		for (int i = 0; i < Vars::count; i++)
		{
			if (count != 0)
				new_param.append(", ");
			count += 1;
			new_param.append(vars[i].name);
		}
		new_param.append("), ");

		sstring.insert(idx, new_param);
		return ;
	}

	void update_copy_constructor_initialize_list(std::string base, std::string& sstring)
	{
		std::string find_string = "/* copy-constructor initialize list */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param = base + "(copy), ";
		sstring.insert(idx, new_param);
		return ;
	}

	void update_overload_operator(std::string base, std::string& sstring)
	{
		std::string find_string = "/* overload= code */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;
		idx -= 1;
		std::string new_param = "\tthis->" + base + "::operator=(obj);\n";
		sstring.insert(idx, new_param);
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

	void update_overload_function(std::string base, std::string& bstring, std::string& sstring)
	{
		std::istringstream ss(bstring);
		std::string find_string = "/* inherit overload function */";
		std::string line, arg, body, new_param;
		std::ofstream dummy;

		int idx = bstring.find(find_string);
		if (idx == -1)
			return ;
		get::sstream_with_target(ss, line, "/* inherit overload function */", dummy, false);
		while (std::getline(ss, line))
		{
			if (line.empty() || line[0] == '}')
				break ;
			if (line.find("/*") != std::string::npos)
				break ;
			line = line.substr(9, line.size() - 10);
			get_func_info(line, arg, body);
			new_param.push_back('\n');
			new_param.append(arg + "\n");
			new_param.append(base + "::" + body + " {\n");
			new_param.append("\t/* function body */\n");
			new_param.append("}\n");
		}
		idx = sstring.find("GETTER");
		if (idx != -1)
			sstring.insert(idx - 119, new_param);
		return ;
	}

	void inherit_source_file(std::string base, std::string bstring, \
	std::string hstring, std::string sstring, std::ofstream& out)
	{
		Vars *vars = get_vars_from_base_constructor(bstring, base);

		update_constructor_initialize_list(base, sstring, vars);
		update_copy_constructor_initialize_list(base, sstring);
		update_overload_operator(base, sstring);
		update_overload_function(base, hstring, sstring);

		out << sstring << std::endl;
		delete[] vars;
	}
}

namespace inherit {
	void	main(int ac, char *av[])
	{
		if (ac < 4)
			throw ("The inherit command requires base and derived class separately.");
		reset::backup(ac, av, CMD_INHERIT);
		std::string base = std::string(av[2]);
		std::string bstring = get::string_from_file(base, CMD_START, PATH_HEADER);
		for (int i = 3; i < ac; i++)
		{
			std::string derive = std::string(av[i]);
			std::string dstring = get::string_from_file(derive, CMD_START, PATH_HEADER);
			std::string sstring = get::string_from_file(derive, CMD_START, PATH_SOURCE);

			if (bstring.empty() || dstring.empty())
				throw ("The file is empty or did not open normally.");

			std::ofstream hout(get::path(derive, CMD_START, PATH_HEADER), std::ofstream::trunc);
			std::ofstream sout(get::path(derive, CMD_START, PATH_SOURCE), std::ofstream::trunc);
			inherit_header_file(base, derive, bstring, dstring, hout);
			hout.close();
			std::string hstring = get::string_from_file(derive, CMD_START, PATH_HEADER);
			inherit_source_file(base, bstring, hstring, sstring, sout);
			sout.close();
		}
	}
}
