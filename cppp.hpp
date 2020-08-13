#ifndef CPPP_HPP
# define CPPP_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <cstdlib>
# include <string>
# include <ctime>
# include <iomanip>

# define CMD_HELP			1
# define CMD_START			2
# define CMD_STARTP			3
# define CMD_INHERIT 		4
# define CMD_FUNCTION		5
# define CMD_EXCEPTION		6
# define CMD_STRONG_GETTER	7
# define CMD_MAIN			8
# define CMD_MAKE			9
# define CMD_RESET			10

# define TYPE_INTERFACE		1
# define TYPE_ABSTRACT		2
# define TYPE_CLASS			3
# define TYPE_SOURCE		4

# define PATH_HEADER		1
# define PATH_SOURCE		2

# define MAX_BACKUP_FOLDER	5
# define BACKUP_HEADER		1
# define BACKUP_SOURCE		2
# define BACKUP_ALL			3
# define BACKUP_MAKEFILE	4

namespace start {
	void main(int ac, char *av[], int cmd);
}

namespace inherit {
	void main(int ac, char *av[]);
}

namespace strong_getter {
	void main(int ac, char *av[]);
}

namespace exception {
	void main(int ac, char *av[]);
}

namespace function {
	void main(int ac, char *av[]);
}

namespace cmd_main {
	void create_main_file(std::ofstream& out, std::string file_name);
	void create_header_file(std::ofstream& out);
	void main(int ac, char *av[]);
}

namespace make {
	void main(int ac, char *av[]);
}

namespace reset {
	int backup(int ac, char *av[], int cmd);
	void main(int ac, char *av[]);
	void cherrypick(std::string commit_path);
	void hard(std::string commit_path);
}

namespace help {
	void main();
}

namespace get {
	int file_type(std::string filename);
	std::string file_name(std::string file_path);
	std::string path(std::string name, int cmd, int path_type);
	std::string header_guard(std::string name, int cmd);
	std::string string_from_file(std::string name, int cmd, int path_type);
	bool sstream_with_target(std::istringstream& stream, std::string& line, \
	std::string target, std::ofstream& out, bool print_opt);
	bool sstream_with_jump(std::istringstream& stream, std::string& line, \
	int jump_count, std::ofstream& out, bool print_opt);
	bool is_right_extension_file(std::string path, std::string extension);
}

namespace error {
	int msg(std::string msg);
}

#endif
