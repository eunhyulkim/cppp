#include "cppp.hpp"

namespace fs = std::filesystem;

namespace {
	std::string get_makefile_path()
	{
		std::string path = std::string(std::getenv("PWD"));
		path.append("/Makefile");
		return (path);
	}

	std::string get_root(std::string &arg)
	{
		if (arg != "." && arg[arg.size() - 1] != '/')
			arg.push_back('/');
		return (arg);
	}

	std::string get_root_path(std::string root)
	{
		std::string path = std::string(std::getenv("PWD"));
		if (root == ".")
			return (path);
		path.append(root);
		return (path);
	}

	void print_default(std::ofstream& out, std::string& program_name)
	{
		out << "NAME = " << program_name << std::endl;
		out << "CC = clang++" << std::endl;
		out << "RM = rm -rf" << std::endl;
		out << "INCLUDES = -I./includes/" << std::endl;
		out << "CFLAGS = -Wall -Wextra -Werror" << std::endl << std::endl;
	}

	void print_srcs_files(std::ofstream& out, std::string path, int& main_count, int& char_count)
	{
		std::string filename = get::file_name(path);
		int last_idx = filename.size() - 4;
		if (char_count + last_idx >= 80)
		{
			out << std::endl;
			for (int i = 0; i < ((main_count + 1) / 4); i++)
				out << "\t";
			for (int i = 0; i < ((main_count + 1) % 4); i++)
				out << " ";
			char_count = main_count + 1 + last_idx;
			out << filename.substr(0, last_idx);
		}
		else
		{
			out << " " << filename.substr(0, last_idx);
			char_count += last_idx + 1;
		}
	}

	void print_main_files(std::ofstream& out, std::string root)
	{
		int main_occur = true;
		int main_count = 12;
		int char_count = 12;

		std::string root_path = get_root_path(root);
		for(auto& p: fs::directory_iterator(root_path))
		{
			if (get::is_right_extension_file(p.path(), ".cpp"))
			{
				if (main_occur == true)
				{
					out << "MAIN_FILES =";
					main_occur = false;
				}
				print_srcs_files(out, p.path(), main_count, char_count);
			}
		}
		if (main_occur == false)
			out << std::endl;
	}

	void print_sub_files(std::ofstream& out, std::string path, int& main_count)
	{
		int char_count = main_count;

		for(auto& p: fs::directory_iterator(path))
		{
			if (get::is_right_extension_file(p.path(), ".cpp"))
			{
				print_srcs_files(out, p.path(), main_count, char_count);
			}
		}
		out << std::endl;
	}

	void print_directory_files(std::ofstream& out, std::string root)
	{
		int main_count = 0;

		std::string root_path = get_root_path(root);
		for(auto& p: fs::directory_iterator(root_path))
		{
			if (fs::is_directory(p.path()))
			{
				std::string dir_name = get::file_name(p.path());
				main_count = 8 + dir_name.size();
				for (int i = 0; i < (int)dir_name.size(); i++)
					out << static_cast<char>(toupper(dir_name[i]));
				out << "_FILES =";
				print_sub_files(out, p.path(), main_count);
			}
		}
	}

	void set_srcs(std::ofstream& out, std::string root)
	{
		out << std::endl;
		if (root == ".")
			out << "SRCS = $(addsuffix .cpp, $(MAIN_FILES))" << std::endl;
		else
		{
			out << "SRCS = $(addprefix " << root
			<< ", $(addsuffix .cpp, $(MAIN_FILES)))"
			<< std::endl;
		}
		std::string root_path = get_root_path(root);
		for(auto& p: fs::directory_iterator(root_path))
		{
			if (fs::is_directory(p.path()))
			{
				if (root == ".")
				{
					out << "SRCS += $(addsuffix .cpp, $(";
					std::string dir_name = get::file_name(p.path());
					for (int i = 0; i < (int)dir_name.size(); i++)
						out << static_cast<char>(toupper(dir_name[i]));
					out << "_FILES";
					out << "))" << std::endl;
				}
				else
				{
					out << "SRCS += $(addprefix " << root
					<< ", $(addsuffix .cpp, $(";
					std::string dir_name = get::file_name(p.path());
					for (int i = 0; i < (int)dir_name.size(); i++)
						out << static_cast<char>(toupper(dir_name[i]));
					out << "_FILES";
					out << ")))" << std::endl;
				}
			}
		}
		out << std::endl;
	}

	void set_objs(std::ofstream& out, std::string root)
	{
		out << "OBJS = $(addsuffix .o, $(MAIN_FILES))" << std::endl;
		std::string root_path = get_root_path(root);
		for(auto& p: fs::directory_iterator(root_path))
		{
			if (fs::is_directory(p.path()))
			{
				out << "OBJS += $(addsuffix .o, $(";
				std::string dir_name = get::file_name(p.path());
				for (int i = 0; i < (int)dir_name.size(); i++)
					out << static_cast<char>(toupper(dir_name[i]));
				out << "_FILES))" << std::endl;
			}
		}
		out << std::endl;
	}

	void set_color(std::ofstream& out)
	{
		out << "GREEN = \\033[0;32m" << std::endl;
		out << "RED = \\033[0;31m" << std::endl;
		out << "RESET = \\033[0m" << std::endl;
		out << std::endl;
	}

	void set_run(std::ofstream& out)
	{
		out << "all: $(NAME)" << std::endl << std::endl;

		out << "${NAME}: ${OBJS}" << std::endl;
		out << "\t@echo " << "\"$(GREEN)$(NAME) START$(RESET)\"" << std::endl;
		out << "\t@echo " << "\"$(RESET)Compiling ${NAME} to root.\"" << std::endl;
		out << "\t@${CC} ${CFLAGS} ${INCLUDES} ${OBJS} -o ${NAME}" << std::endl;
		out << "\t@echo " << "\"$(GREEN)DONE$(RESET)\"" << std::endl << std::endl;

		out << "${OBJS}: ${SRCS}" << std::endl;
		out << "\t@${CC} ${CFLAGS} ${INCLUDES} -c ${SRCS}" << std::endl << std::endl;
	}

	void set_clean(std::ofstream& out)
	{
		out << "clean:" << std::endl;
		out << "\t@echo " << "\"$(RESET)Cleaning your .o files$(RESET)\"" << std::endl;
		out << "\t@${RM} $(OBJS)" << std::endl;
		out << "\t@echo " << "\"$(RED)clean DONE$(RESET)\"" << std::endl << std::endl;

		out << "fclean: clean" << std::endl;
		out << "\t@echo " << "\"$(RESET)Removing $(NAME)\"" << std::endl;
		out << "\t@${RM} $(NAME)" << std::endl;
		out << "\t@echo " << "\"$(RED)fclean DONE$(RESET)\"" << std::endl << std::endl;
	}

	void set_re_and_phony(std::ofstream& out)
	{
		out << "re: fclean all" << std::endl << std::endl;
		out << ".PHONY:	all clean fclean re bonus" << std::endl;
	}
}

namespace make {
	void	main(int ac, char *av[])
	{
		if (ac < 4)
			throw ("The make command requires program_name and root directory");
		reset::backup(ac, av, CMD_MAKE);
		std::string path = get_makefile_path();
		std::ofstream out(path, std::ofstream::trunc);
		if (!out.is_open())
			return ;

		std::string program_name = av[2];
		std::string root = av[3];
		root = get_root(root);

		print_default(out, program_name);
		print_main_files(out, root);
		print_directory_files(out, root);
		set_srcs(out, root);
		set_objs(out, root);
		set_color(out);
		set_run(out);
		set_clean(out);
		set_re_and_phony(out);
		out.close();
	}
}
