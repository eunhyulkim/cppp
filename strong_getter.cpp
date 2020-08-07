#include "cppp.hpp"

namespace {
	class Vars {
		public:
			static int 	count;
			bool 		is_static;
			bool		has_underline;
			std::string type;
			std::string name;
			~Vars();
	};

	Vars::~Vars() { Vars::count = 0; }
	int Vars::count = 0;
}

namespace {
	void convert_variable(std::string line, Vars *vars)
	{
		int sep, ldx;

		if (line.find(" ") == std::string::npos || line.empty())
			return ;
		if (line.find("(") != std::string::npos || line.find("/*") != std::string::npos)
			return ;
		if ((!line[0] || !line[1] || line[0] != '\t') || line[1] != '\t')
			return ;
		line = line.substr(2);
		sep = 0;
		if (line.find("static") != std::string::npos)
		{
			line = line.erase(line.find("static"), 7);
			vars[Vars::count].is_static = true;
		}
		else
			vars[Vars::count].is_static = false;
		ldx = line.rfind(" ");
		if (line[ldx + 1] == '*')
		{
			vars[Vars::count].type = line.substr(0, ldx + 2);
			vars[Vars::count].name = line.substr(ldx + 2, line.size() - ldx - 3);
		}
		else
		{
			vars[Vars::count].type = line.substr(0, ldx);
			vars[Vars::count].name = line.substr(ldx + 1, line.size() - ldx - 2);
		}
		if (vars[Vars::count].name[0] == '_')
		{
			vars[Vars::count].has_underline = true;
			vars[Vars::count].name.erase(0, 1);
		}
		else
			vars[Vars::count].has_underline = false;
		std::cout << vars[Vars::count].type << std::endl;
		std::cout << vars[Vars::count].name << std::endl;
		Vars::count += 1;
		return ;
	}

	Vars *get_vars_from_header(std::string hstring)
	{
		std::istringstream ss(hstring);
		std::string line;
		std::ofstream dummy;
		Vars *vars = new Vars[20];

		if (hstring.find("private:") != std::string::npos)
			get::sstream_with_target(ss, line, "private", dummy, false);
		else if (hstring.find("protected:") != std::string::npos)
			get::sstream_with_target(ss, line, "protected", dummy, false);
		else {
			delete[] vars;
			throw ("Failed to find variables in private or protected areas to apply strong getter.");
		}
		while (std::getline(ss, line))
		{
			if (line.find("public") != std::string::npos || line[0] == '}')
				break ;
			convert_variable(line, vars);
		}
		return (vars);
	}
	
	void strong_getter_update_to_header(std::string hstring, std::ofstream& out, \
	Vars *vars)
	{
		std::istringstream ss(hstring);
		std::string line;

		get::sstream_with_target(ss, line, "~", out, true);
		out << line << std::endl;
		for (int i = 0; i < Vars::count; i++)
		{
			if (i == 0)
			{
				out << std::endl;
				out << "\t\t/* getter function */" << std::endl;
			}
			out << "\t\t" << vars[i].type << " get";
			out << static_cast<char>(toupper(vars[i].name[0]));
			out << vars[i].name.substr(1);
			out << "() const;" << std::endl;
		}
		get::sstream_with_target(ss, line, "#endif", out, true);
		out << line << std::endl;
	}

	void update_static_variable(std::string& sstring, Vars *vars, std::string name)
	{
		std::string find_string = "/* static variable code */\n";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		for (int i = 0; i < Vars::count; i++)
		{
			if (!vars[i].is_static)
				continue;
			new_param.append(vars[i].type);
			if (vars[i].type[vars[i].type.size() - 1] != '*')
				new_param.push_back(' ');
			new_param.append(name + "::");
			new_param.append(vars[i].name);
			new_param.append(" = ");
			if (vars[i].type[vars[i].type.size() - 1] == '*')
				new_param.append("nullptr;");
			else if (vars[i].type == "int")
				new_param.append("0;");
			else if (vars[i].type == "bool")
				new_param.append("false;");
			else if (vars[i].type == "std::string")
				new_param.append("\"\";");
			else
				new_param.append("/* declare initial value */");
			new_param.push_back('\n');
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_constructor_parameter(std::string& sstring, Vars *vars)
	{
		std::string find_string = "/* constructor parameter */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::count; i++)
		{
			if (vars[i].is_static)
				continue;
			if (count != 0)
				new_param.append(", ");
			count += 1;
			new_param.append(vars[i].type);
			if (vars[i].type[vars[i].type.size() - 1] != '*')
				new_param.push_back(' ');
			new_param.append(vars[i].name);
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_constructor_initialize_list(std::string& sstring, Vars *vars)
	{
		std::string find_string = "/* constructor initialize list */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::count; i++)
		{
			if (vars[i].is_static)
				continue;
			if (count != 0)
				new_param.append(", ");
			count += 1;
			if (vars[i].has_underline)
				new_param.push_back('_');
			new_param.append(vars[i].name);
			new_param.push_back('(');
			new_param.append(vars[i].name);
			new_param.push_back(')');
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_copy_constructor_initialize_list(std::string& sstring, Vars *vars)
	{
		std::string find_string = "/* copy-constructor initialize list */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::count; i++)
		{
			if (vars[i].is_static)
				continue;
			if (count != 0)
				new_param.append(", ");
			count += 1;
			if (vars[i].has_underline)
				new_param.push_back('_');
			new_param.append(vars[i].name);
			new_param.append("(copy.get");
			new_param.push_back(static_cast<char>(toupper(vars[i].name[0])));
			new_param.append(vars[i].name.substr(1));
			new_param.append("())");
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_overload_operator(std::string& sstring, Vars *vars)
	{
		std::string find_string = "/* overload= code */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;
		idx -= 1;

		int count = 0;
		std::string new_param;
		for (int i = 0; i < Vars::count; i++)
		{
			if (vars[i].is_static)
				continue;
			count += 1;
			new_param.append("\tthis->");
			if (vars[i].has_underline)
				new_param.push_back('_');
			new_param.append(vars[i].name);
			new_param.append(" = obj.get");
			new_param.push_back(static_cast<char>(toupper(vars[i].name[0])));
			new_param.append(vars[i].name.substr(1));
			new_param.append("();\n");
		}
		if (count == 0)
			return ;

		sstring.insert(idx, new_param);
		return ;
	}

	void update_getter_function_to_source_file(std::string& sstring, \
	Vars *vars, std::string name)
	{
		std::string find_string = "/* getter code */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		for (int i = 0; i < Vars::count; i++)
		{
			new_param.append(vars[i].type);
			if (vars[i].type[vars[i].type.size() - 1] != '*')
				new_param.push_back(' ');
			new_param.append(name);
			new_param.append("::");
			new_param.append("get");
			new_param.push_back(static_cast<char>(toupper(vars[i].name[0])));
			new_param.append(vars[i].name.substr(1));
			new_param.append("() const { return (this->");
			if (vars[i].has_underline)
				new_param.push_back('_');
			new_param.append(vars[i].name);
			new_param.append("); }");
			if (i < Vars::count - 1)
				new_param.push_back('\n');
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void strong_getter_update_to_source(std::string sstring, std::ofstream& out, \
	Vars *vars, std::string name)
	{
		std::istringstream ss(sstring);
		std::string line;

		update_static_variable(sstring, vars, name);
		update_constructor_parameter(sstring, vars);
		update_constructor_initialize_list(sstring, vars);
		update_copy_constructor_initialize_list(sstring, vars);
		update_overload_operator(sstring, vars);
		update_getter_function_to_source_file(sstring, vars, name);
		out << sstring;
	}
}

namespace strong_getter {
	void	main(int ac, char *av[])
	{
		for (int i = 2; i < ac; i++)
		{
			std::string name = av[i];
			std::string hstring = get::string_from_file(name, CMD_START, PATH_HEADER);
			std::string sstring = get::string_from_file(name, CMD_START, PATH_SOURCE);
			Vars *vars = get_vars_from_header(hstring);
			if (hstring.empty() || sstring.empty() || Vars::count == 0)
			{
				delete[] vars;
				throw ("The file is empty or did not open normally.");
			}
			std::ofstream hout(get::path(name, CMD_START, PATH_HEADER), std::ofstream::trunc);
			std::ofstream sout(get::path(name, CMD_START, PATH_SOURCE), std::ofstream::trunc);
			strong_getter_update_to_header(hstring, hout, vars);
			strong_getter_update_to_source(sstring, sout, vars, name);
			delete[] vars;
		}
		return ;
	}
}
