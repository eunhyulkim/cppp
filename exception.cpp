#include "cppp.hpp"

namespace {

	class MyException {
		public:
			static int count;
			std::string name;
			~MyException();
	};

	int MyException::count = 0;
	MyException::~MyException() { MyException::count = 0; }

	MyException *get_exception(std::string& hstring)
	{
		MyException *vars = new MyException[10];
		std::istringstream ss(hstring);
		std::ostringstream os;
		std::string line;

		while (std::getline(ss, line))
		{
			bool is_continue = false;
			if (line.empty())
				is_continue = true;
			else if (line.size() < 3 || line[2] == '\t')
				is_continue = true;
			if (line.find("/*") != std::string::npos)
				is_continue = true;
			if (line.find("exception") == std::string::npos)
				is_continue = true;
			if (line.find(":") != std::string::npos)
				is_continue = true;
			if (is_continue == true)
			{
				os << line << std::endl;
				continue;
			}
			line = line.substr(line.find("exception") + 10);
			vars[MyException::count].name = line.substr(0, line.size() - 1);
			MyException::count++;
		}
		hstring = os.str();
		return (vars);
	}

	void exception_update_to_header(std::string& hstring, MyException *vars)
	{
		int idx = hstring.find("/* global operator overload */");
		if (idx == -1)
			idx = hstring.find("#endif");
		idx -= 4;

		std::string new_param;
		if (hstring.find("/* exception declare */") == std::string::npos)
			new_param = "\t\t/* exception declare */\n";
		for (int i = 0; i < MyException::count; i++)
		{
			std::string name = vars[i].name;
			if (i > 0)
				new_param.push_back('\n');
			new_param.append("\t\tclass " + name + " : public std::exception {\n");
			new_param.append("\t\t\tpublic:\n");
			new_param.append("\t\t\t\t" + name + "() throw ();\n");
			new_param.append("\t\t\t\t" + name + "(const " + name + "&) throw ();\n");
			new_param.append("\t\t\t\t" + name + "& operator= (const " + name + "&) throw ();\n");
			new_param.append("\t\t\t\tvirtual ~" + name + "() throw ();\n");
			new_param.append("\t\t\t\tvirtual const char* what() const throw ();\n");
			new_param.append("\t\t};\n");
		}

		hstring.insert(idx, new_param);
	}

	void exception_update_to_source(std::string& sstring, MyException *vars, std::string name)
	{
		int idx = sstring.find("/* exception code */");
		if (idx == -1)
			return ;

		std::string new_param;
		for (int i = 0; i < MyException::count; i++)
		{
			std::string except = vars[i].name;
			if (i > 0)
				new_param.push_back('\n');
			new_param.append(name + "::" + except + "::" + except + "() throw () : std::exception(){}\n");
			new_param.append(name + "::" + except + "::" + except + "(const " \
			+ except + "&) throw () : std::exception(){}\n");
			new_param.append(name + "::" + except + "& " + name + "::" + except \
			+ "::operator=(const " + name + "::" + except \
			+ "&) throw() { return (*this); }\n");
			new_param.append(name + "::" + except + "::~" + except + "() throw (){}\n");
			new_param.append("const char* " + name + "::" + except \
			+ "::what() const throw () { return (\"" + except + " error\"); }\n");
		}

		new_param.push_back('\n');
		sstring.insert(idx, new_param);
	}
}

namespace exception {
	void	main(int ac, char *av[])
	{
		for (int i = 2; i < ac; i++)
		{
			std::string name = av[i];
			std::string hstring = get::string_from_file(name, CMD_START, PATH_HEADER);
			std::string sstring = get::string_from_file(name, CMD_START, PATH_SOURCE);
			MyException *vars = get_exception(hstring);

			if (hstring.empty() || sstring.empty() || MyException::count == 0)
			{
				delete[] vars;
				throw ("The file is empty or did not open normally.");
			}
			exception_update_to_header(hstring, vars);
			std::ofstream hout(get::path(name, CMD_START, PATH_HEADER), std::ofstream::trunc);
			hout << hstring;

			exception_update_to_source(sstring, vars, name);
			std::ofstream sout(get::path(name, CMD_START, PATH_SOURCE), std::ofstream::trunc);
			sout << sstring;

			delete[] vars;
			hout.close();
			sout.close();
		}
		return ;
	}
}
