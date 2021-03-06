#include "cppp.hpp"

namespace {
	class Vars {
		public:
			static int 	modifier_count;
			static int 	constructor_count;
			bool 		is_static;
			bool		is_initialize;
			bool		has_prefix;
			std::string type;
			std::string name;
			Vars();
			~Vars();
	};

	Vars::Vars() { this->is_initialize = false; }
	Vars::~Vars() { Vars::modifier_count = 0; Vars::constructor_count = 0; }
	int Vars::modifier_count = 0;
	int Vars::constructor_count = 0;
}

namespace {
/*
** 	HEADER UPDATE
*/
	void convert_variable_from_accessModifer(std::string line, Vars *vars)
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
			vars[Vars::modifier_count].is_static = true;
		}
		else
			vars[Vars::modifier_count].is_static = false;
		ldx = line.rfind(" ");
		if (line[ldx + 1] == '*')
		{
			vars[Vars::modifier_count].type = line.substr(0, ldx + 2);
			vars[Vars::modifier_count].name = line.substr(ldx + 2, line.size() - ldx - 3);
		}
		else
		{
			vars[Vars::modifier_count].type = line.substr(0, ldx);
			vars[Vars::modifier_count].name = line.substr(ldx + 1, line.size() - ldx - 2);
		}
		if (vars[Vars::modifier_count].name.substr(0, 2) == "m_")
		{
			vars[Vars::modifier_count].has_prefix = true;
			vars[Vars::modifier_count].name.erase(0, 2);
		}
		else
			vars[Vars::modifier_count].has_prefix = false;
		Vars::modifier_count += 1;
		return ;
	}

	void convert_variable_from_constructor(std::string line, Vars *vars)
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
				vars[Vars::constructor_count].type = word.substr(0, idx + 2);
				vars[Vars::constructor_count].name = word.substr(idx + 2);
			}
			else
			{
				vars[Vars::constructor_count].type = word.substr(0, idx);
				vars[Vars::constructor_count].name = word.substr(idx + 1);
			}
			Vars::constructor_count += 1;
		}
		return ;
	}

	Vars *get_vars_from_accessModifier(std::string hstring)
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
			convert_variable_from_accessModifer(line, vars);
		}
		return (vars);
	}
	
	Vars *get_vars_from_constructor(std::string hstring, std::string base)
	{
		std::istringstream ss(hstring);
		std::string line, bword, nword;
		std::ofstream dummy;
		Vars *vars = new Vars[20];

		if (hstring.find("public:") != std::string::npos)
			get::sstream_with_target(ss, line, "public:", dummy, false);
		else {
			delete[] vars;
			throw ("Failed to find constructor variables in public areas.");
		}
		bword = base + "()";
		nword = "const " + base + "&";
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
			convert_variable_from_constructor(line, vars);
			break ;
		}
		return (vars);
	}

	void strong_getter_and_setter_update_to_header(std::string hstring, std::ofstream& out, \
	Vars *vars)
	{
		std::istringstream ss(hstring);
		std::string line;

		get::sstream_with_target(ss, line, "~", out, true);
		out << line << std::endl;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (i == 0)
			{
				out << std::endl;
				out << "\t\t/* getter function */" << std::endl;
			}
			out << "\t\t" << vars[i].type;
			if (vars[i].type[vars[i].type.size() - 1] != '*')
				out << " ";
			out << "get_";
			if (vars[i].has_prefix == true)
				out << "m_";			
			out << vars[i].name;
			out << "() const;" << std::endl;
		}
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (i == 0)
			{
				out << std::endl;
				out << "\t\t/* setter function */" << std::endl;
			}
			out << "\t\tvoid ";
			out << "set_";
			if (vars[i].has_prefix == true)
				out << "m_";			
			out << vars[i].name;
			out << "(";
			out << vars[i].type;
			if (vars[i].type[vars[i].type.size() - 1] != '*')
				out << " ";
			out << vars[i].name;
			out << ");" << std::endl;
		}
		get::sstream_with_target(ss, line, "#endif", out, true);
		out << line << std::endl;
	}

/*
** SOURCE UPDATE
*/

	Vars* get_constructor_var(Vars* constructor_vars, Vars& modifier_vars)
	{
		int i = -1;
		while (++i < Vars::constructor_count)
		{
			if (constructor_vars[i].name.find(modifier_vars.name) != std::string::npos)
			{
				modifier_vars.is_initialize = true;
				return (&constructor_vars[i]);
			}
		}
		return (nullptr);
	}

	std::string get_initial_value(std::string type)
	{
		if (type[type.size() - 1] == '*')
			return (" = nullptr;");
		else if (type.find("int") != std::string::npos)
			return (" = 0;");
		else if (type.find("float") != std::string::npos)
			return (" = 0.0;");
		else if (type.find("double") != std::string::npos)
			return (" = 0.0;");
		else if (type.find("bool") != std::string::npos)
			return (" = false;");
		else if (type.find("std::string") != std::string::npos)
			return (".clear();");
		else
			return (" = /* declare initial value */");
	}

	void update_static_variable(std::string& sstring, Vars *vars, std::string name)
	{
		if (Vars::modifier_count == 0)
			return ;

		std::string find_string = "/* static variable code */\n";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (!vars[i].is_static)
				continue;
			new_param.append(vars[i].type);
			if (vars[i].type[vars[i].type.size() - 1] != '*')
				new_param.push_back(' ');
			new_param.append(name + "::");
			new_param.append(vars[i].name);
			new_param.append(get_initial_value(vars[i].type));
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

		if (Vars::constructor_count == 0)
		{
			sstring.replace(idx, find_string.size(), std::string(""));
			return ;
		}

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::constructor_count; i++)
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

	void update_constructor_initialize_list(std::string& sstring, Vars *modifier_vars, Vars *constructor_vars)
	{
		std::string find_string = "/* constructor initialize list */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		if (Vars::modifier_count == 0)
		{
			sstring.replace(idx, find_string.size(), std::string(""));
			return ;
		}

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (modifier_vars[i].is_static)
				continue ;
			Vars *constructor_var = get_constructor_var(constructor_vars, modifier_vars[i]);
			if (constructor_var == nullptr)
				continue ;
			if (count != 0)
				new_param.append(", ");
			count += 1;
			if (modifier_vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(modifier_vars[i].name);
			new_param.push_back('(');
			new_param.append(constructor_var->name);
			new_param.push_back(')');
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_constructor_code(std::string& sstring, Vars *vars)
	{
		if (Vars::modifier_count == 0)
			return ;

		std::string find_string = "/* constructor code */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (vars[i].is_static)
				continue;
			if (vars[i].is_initialize)
				continue;
			if (count != 0)
				new_param.append("\n\t");
			count += 1;
			new_param.append("this->");
			if (vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(vars[i].name);
			new_param.append(get_initial_value(vars[i].type));
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_copy_constructor_initialize_list(std::string& sstring, Vars *modifier_vars, Vars *constructor_vars)
	{
		std::string find_string = "/* copy-constructor initialize list */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;
		
		if (Vars::modifier_count == 0)
		{
			sstring.replace(idx, find_string.size(), std::string(""));
			return ;
		}

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (modifier_vars[i].is_static)
				continue ;
			Vars *constructor_var = get_constructor_var(constructor_vars, modifier_vars[i]);
			if (constructor_var == nullptr)
				continue ;
			if (count != 0)
				new_param.append(", ");
			count += 1;
			if (modifier_vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(modifier_vars[i].name);
			new_param.append("(copy.get_");
			if (modifier_vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(modifier_vars[i].name);
			new_param.append("())");
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}
	
	void update_copy_constructor_code(std::string& sstring, Vars *vars)
	{
		if (Vars::modifier_count == 0)
			return ;

		std::string find_string = "/* copy-constructor code */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (vars[i].is_static)
				continue;
			if (vars[i].is_initialize)
				continue;
			if (count != 0)
				new_param.append("\n\t");
			count += 1;
			new_param.append("this->");
			if (vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(vars[i].name);
			new_param.append(get_initial_value(vars[i].type));
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_destructor_code(std::string& sstring, Vars *vars)
	{
		if (Vars::modifier_count == 0)
			return ;

		std::string find_string = "/* destructor code */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (vars[i].is_static)
				continue;
			if (count != 0)
				new_param.append("\n\t");
			count += 1;
			if (vars[i].type[vars[i].type.size() - 1] == '*')
				new_param.append("delete ");
			new_param.append("this->");
			if (vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(vars[i].name);
			if (vars[i].type[vars[i].type.size() - 1] == '*')
				new_param.push_back(';');
			else
				new_param.append(get_initial_value(vars[i].type));
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_overload_operator(std::string& sstring, Vars *modifier_vars)
	{
		std::string find_string = "/* overload= code */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;
		idx -= 1;

		int count = 0;
		std::string new_param;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (modifier_vars[i].is_static)
				continue;
			count += 1;
			new_param.append("\tthis->");
			if (modifier_vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(modifier_vars[i].name);
			new_param.append(" = obj.get_");
			if (modifier_vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(modifier_vars[i].name);
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
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			new_param.append(vars[i].type);
			if (vars[i].type[vars[i].type.size() - 1] != '*')
				new_param.push_back(' ');
			new_param.append(name);
			new_param.append("::");
			new_param.append("get_");
			if (vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(vars[i].name);
			new_param.append("() const { return (this->");
			if (vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(vars[i].name);
			new_param.append("); }");
			if (i < Vars::modifier_count - 1)
				new_param.push_back('\n');
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_setter_function_to_source_file(std::string& sstring, \
	Vars *vars, std::string name)
	{
		std::string find_string = "/* setter code */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			new_param.append("void ");
			new_param.append(name);
			new_param.append("::");
			new_param.append("set_");
			if (vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(vars[i].name);
			new_param.append("(");
			new_param.append(vars[i].type);
			if (vars[i].type[vars[i].type.size() - 1] != '*')
				new_param.append(" ");
			new_param.append(vars[i].name);
			new_param.append(") { this->");
			if (vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(vars[i].name);
			new_param.append(" = ");
			new_param.append(vars[i].name);
			new_param.append("; }");
			if (i < Vars::modifier_count - 1)
				new_param.push_back('\n');
		}

		sstring.replace(idx, find_string.size(), new_param);
		return ;
	}

	void update_overload_ostream(std::string& sstring, Vars *vars, std::string name)
	{
		std::string find_string = "/* ostream output overload code */";
		int idx = sstring.find(find_string);
		if (idx == -1)
			return ;

		std::string new_param;
		int count = 0;
		for (int i = 0; i < Vars::modifier_count; i++)
		{
			if (vars[i].is_static)
				continue;
			if (count != 0)
				new_param.append("\n\t");
			count += 1;
			new_param.append("out << \"\" << ");
			new_param.push_back(static_cast<char>(tolower(name[0])));
			new_param.append(name.substr(1) + ".");
			new_param.append("get_");
			if (vars[i].has_prefix)
				new_param.append("m_");
			new_param.append(vars[i].name);
			new_param.append("();");
		}
		if (count != 0)
			new_param.append("\n\tout << \"\" << std::endl;");
		sstring.replace(idx, find_string.size(), new_param);
	}

	void strong_getter_update_to_source(std::string sstring, std::ofstream& out, \
	Vars *modifier_vars, Vars *constructor_vars, std::string name)
	{
		std::istringstream ss(sstring);
		std::string line;

		update_static_variable(sstring, modifier_vars, name);
		update_constructor_parameter(sstring, constructor_vars);
		update_constructor_initialize_list(sstring, modifier_vars, constructor_vars);
		update_constructor_code(sstring, modifier_vars);
		update_copy_constructor_initialize_list(sstring, modifier_vars, constructor_vars);
		update_copy_constructor_code(sstring, modifier_vars);
		update_destructor_code(sstring, modifier_vars);
		update_overload_operator(sstring, modifier_vars);
		update_overload_ostream(sstring, modifier_vars, name);
		update_getter_function_to_source_file(sstring, modifier_vars, name);
		update_setter_function_to_source_file(sstring, modifier_vars, name);
		out << sstring;
	}
}

namespace strong_getter {
	void	main(int ac, char *av[])
	{
		reset::backup(ac, av, CMD_STRONG_GETTER);
		for (int i = 2; i < ac; i++)
		{
			std::string name = av[i];
			std::string hstring = get::string_from_file(name, CMD_START, PATH_HEADER);
			std::string sstring = get::string_from_file(name, CMD_START, PATH_SOURCE);
			if (hstring.empty() || sstring.empty())
				throw ("The file is empty or did not open normally.");
			std::ofstream hout(get::path(name, CMD_START, PATH_HEADER), std::ofstream::trunc);
			std::ofstream sout(get::path(name, CMD_START, PATH_SOURCE), std::ofstream::trunc);
			Vars *modifier_vars = get_vars_from_accessModifier(hstring);
			strong_getter_and_setter_update_to_header(hstring, hout, modifier_vars);
			Vars *constructor_vars = get_vars_from_constructor(hstring, name);
			strong_getter_update_to_source(sstring, sout, modifier_vars, constructor_vars, name);
			delete[] modifier_vars;
			delete[] constructor_vars;
			hout.close();
			sout.close();
		}
		return ;
	}
}
