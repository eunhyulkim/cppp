#include "cppp.hpp"

namespace {

/*
** 	create header file functions
*/
	void input_default_start_in_header(std::ofstream& out, std::string name, \
	int cmd)
	{
		std::string guard = get::header_guard(name, cmd);
		out << "#ifndef " << guard << std::endl;
		out << "# define " << guard << std::endl << std::endl;
		out << "# include <string>" << std::endl;
		out << "# include <iostream>" << std::endl;
		out << std::endl;
	}

	void input_default_end_in_header(std::ofstream& out) {
		out << std::endl << "#endif" << std::endl;
	}

	void create_interface_header_file(std::ofstream& out, std::string name)
	{
		out << "class " << name << std::endl;
		out << "{" << std::endl;
		out << "\t" << "public:" << std::endl;
		out << "\t\t" << "virtual ~" << name << "(){}" << std::endl;
		out << "\t\t" << "virtual /* prototype */ = 0;" << std::endl;
		out << "};" << std::endl;
	}

	void create_class_header_file(std::ofstream& out, int file_type, \
	std::string name)
	{
		out << "class " << name << std::endl;
		out << "{" << std::endl;
		if (file_type == TYPE_ABSTRACT)
			out << "\t" << "protected:" << std::endl;
		else
			out << "\t" << "private:" << std::endl;
		out << "\t\t" << "/* declare member variable */" << std::endl;
		out << "\t" << "public:" << std::endl;
		out << "\t\t" << name << "();" << std::endl;
		out << "\t\t" << name << "(const std::string& /* parameter */, ...);" << std::endl;
		out << "\t\t" << name << "(const " << name << "& copy);" << std::endl;
		out << "\t\t" << name << "& operator=(const " << name << "& obj);" << std::endl;
		out << "\t\t" << "virtual ~" << name << "();\n" << std::endl;
		out << "\t\t" << "/* declare member function */" << std::endl;
		out << "};" << std::endl << std::endl;
		out << "/* global operator overload */" << std::endl;
		out << "std::ostream&\toperator<<(std::ostream& out, const ";
		out << name << "& ";
		out << static_cast<char>(tolower(name[0])) << name.substr(1);
		out << ");" << std::endl;
	}

	void create_header_file(int file_type, std::string name, int cmd)
	{
		std::string path = get::path(name, cmd, PATH_HEADER);
		std::ofstream out(path, std::ofstream::trunc);

		input_default_start_in_header(out, name, cmd);
		if (file_type == TYPE_INTERFACE)
			create_interface_header_file(out, name);
		else if (file_type == TYPE_ABSTRACT || file_type == TYPE_CLASS)
			create_class_header_file(out, file_type, name);
		input_default_end_in_header(out);
		out.close();
	}

/*
** 	create source file functions
*/

	void create_function_source_file(std::ofstream& out, std::string name)
	{
		out << "namespace {" << std::endl;
		out << "\t" << "/* declare anonymous function */" << std::endl;
		out << "}" << std::endl << std::endl;
		out << "namespace " << name << " {" << std::endl;
		out << "\t" << "/* declare " << name << " function */" << std::endl;
		out << "}" << std::endl;
	}

	void create_class_source_file(std::ofstream& out, std::string name, int cmd)
	{
		// include
		out << "#include \"" << name;
		if (cmd == CMD_START)
			out <<  ".hpp" << "\"" << std::endl << std::endl;
		else
			out << ".class.hpp" << "\"" << std::endl << std::endl;

		// static variable header
		out << "/* " << std::string(74, '*') << " */" << std::endl;
		out << "/* " << std::string(28, '-') << " STATIC VARIABLE ";
		out << std::string(28, '-') << " */" << std::endl;
		out << "/* " << std::string(74, '*') << " */" << std::endl << std::endl;
		out << "/* static variable code */" << std::endl << std::endl;

		// constructor header
		out << "/* " << std::string(74, '*') << " */" << std::endl;
		out << "/* " << std::string(30, '-') << " CONSTRUCTOR ";
		out << std::string(31, '-') << " */" << std::endl;
		out << "/* " << std::string(74, '*') << " */" << std::endl << std::endl;

		// constuctor
		out << name << "::" << name << "() {}" << std::endl;
		out << name << "::" << name << "(/* constructor parameter */)" << std::endl;
		out << ": " << "/* constructor initialize list */" << std::endl;
		out << "{" << std::endl;
		out << "\t/* constructor code */" << std::endl;
		out << "}" << std::endl << std::endl;

		// copy constructor
		out << name << "::" << name << "(const " << name << "& copy)" << std::endl;
		out << ": " << "/* copy-constructor initialize list */" << std::endl;
		out << "{" << std::endl;
		out << "\t/* copy-constructor code */" << std::endl;
		out << "}" << std::endl << std::endl;

		// destructor header
		out << "/* " << std::string(74, '*') << " */" << std::endl;
		out << "/* " << std::string(31, '-') << " DESTRUCTOR ";
		out << std::string(31, '-') << " */" << std::endl;
		out << "/* " << std::string(74, '*') << " */" << std::endl << std::endl;

		// destructor
		out << name << "::~" << name << "()" << std::endl;
		out << "{" << std::endl;
		out << "\t/* destructor code */" << std::endl;
		out << "}" << std::endl << std::endl;

		// overload header
		out << "/* " << std::string(74, '*') << " */" << std::endl;
		out << "/* " << std::string(32, '-') << " OVERLOAD ";
		out << std::string(32, '-') << " */" << std::endl;
		out << "/* " << std::string(74, '*') << " */" << std::endl << std::endl;

		//overload =
		out << name << "& " << name << "::operator=(const " << name << "& obj)" << std::endl;
		out << "{" << std::endl;
		out << "\tif (this == &obj)" << std::endl;
		out << "\t\treturn (*this);" << std::endl;
		out << "\t/* overload= code */" << std::endl;
		out << "\treturn (*this);" << std::endl;
		out << "}" << std::endl << std::endl;

		//overload <<
		out << "std::ostream&" << std::endl;
		out << "operator<<(std::ostream& out, const ";
		out << name << "& " << static_cast<char>(tolower(name[0])) << name.substr(1) << ")" << std::endl;
		out << "{" << std::endl;
		out << "\t/* ostream output overload code */" << std::endl;
		out << "\treturn (out);" << std::endl;
		out << "}" << std::endl << std::endl;

		// getter header
		out << "/* " << std::string(74, '*') << " */" << std::endl;
		out << "/* " << std::string(33, '-') << " GETTER ";
		out << std::string(33, '-') << " */" << std::endl;
		out << "/* " << std::string(74, '*') << " */" << std::endl << std::endl;
		out << "/* getter code */" << std::endl << std::endl;

		// setter header
		out << "/* " << std::string(74, '*') << " */" << std::endl;
		out << "/* " << std::string(33, '-') << " SETTER ";
		out << std::string(33, '-') << " */" << std::endl;
		out << "/* " << std::string(74, '*') << " */" << std::endl << std::endl;
		out << "/* setter code */" << std::endl << std::endl;

		// exception header
		out << "/* " << std::string(74, '*') << " */" << std::endl;
		out << "/* " << std::string(31, '-') << " EXCEPTION ";
		out << std::string(32, '-') << " */" << std::endl;
		out << "/* " << std::string(74, '*') << " */" << std::endl << std::endl;
		out << "/* exception code */" << std::endl << std::endl;

		// function header
		out << "/* " << std::string(74, '*') << " */" << std::endl;
		out << "/* " << std::string(28, '-') << " MEMBER FUNCTION ";
		out << std::string(29, '-') << " */" << std::endl;
		out << "/* " << std::string(74, '*') << " */" << std::endl;

	}

	void create_source_file(int file_type, std::string name, int cmd)
	{
		std::string path = get::path(name, CMD_START, PATH_SOURCE);
		std::ofstream out(path, std::ofstream::trunc);

		if (file_type == TYPE_SOURCE)
			create_function_source_file(out, name);
		else
			create_class_source_file(out, name, cmd);
		out.close();
	}
}

namespace start {
	void main(int ac, char *av[], int cmd)
	{
		int file_type;

		for (int i = 2; i < ac; i++)
		{
			if (!(file_type = get::file_type(av[i])))
				return ;
			if (file_type != TYPE_SOURCE)
				create_header_file(file_type, av[i], cmd);
			if (file_type != TYPE_INTERFACE)
				create_source_file(file_type, av[i], cmd);
		}
	}
}
