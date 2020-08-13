#include "cppp.hpp"

//tesr 
namespace {
	std::string command_help[] = {"-H", "-h", "-help", "-HELP", "-Help"};
	std::string command_start[] = {"-S", "-s", "-start", "-START", "-Start"};
	std::string command_startp[] = {"-S+", "-s+", "-start+", "-START+", "-Start+"};
	std::string command_inherit[] = {"-I", "-i", "-inherit", "-INHERIT", "-Inherit"};
	std::string command_strong_getter[] = {"-G", "-g", "-getter", "-GETTER", "-Getter"};
	std::string command_exception[] = {"-E", "-e", "-exception", "-EXCEPTION", "-Exception"};
	std::string command_function[] = {"-F", "-f", "-function", "-FUNCTION", "-Function"};
	std::string command_main[] = {"-M", "-m", "-main", "-MAIN", "-Main"};
	std::string command_make[] = {"-M+", "-m+", "-make", "-MAKE", "-Make"};
	std::string command_reset[] = {"-R", "-r", "-reset", "-RESET", "-Reset"};


	int get_command(std::string command_arg)
	{
		if (command_arg[0] != '-')
			return (error::msg("A command starting with the '-' character must exist."));
		for (int i = 0; i < 5; i++)
		{
			if (command_arg == command_help[i])
				return (CMD_HELP);
			else if (command_arg == command_start[i])
				return (CMD_START);
			else if (command_arg == command_startp[i])
				return (CMD_STARTP);
			else if (command_arg == command_inherit[i])
				return (CMD_INHERIT);
			else if (command_arg == command_strong_getter[i])
				return (CMD_STRONG_GETTER);
			else if (command_arg == command_exception[i])
				return (CMD_EXCEPTION);
			else if (command_arg == command_function[i])
				return (CMD_FUNCTION);
			else if (command_arg == command_main[i])
				return (CMD_MAIN);
			else if (command_arg == command_make[i])
				return (CMD_MAKE);
			else if (command_arg == command_reset[i])
				return (CMD_RESET);
		}
		return (error::msg("No supported commands were found."));
	}
}

int		main(int ac, char *av[])
{
	int cmd;

	if (ac < 2)
		return (error::msg("Enter the command and arguments."));
	if (!(cmd = get_command(av[1])))
		return (0);
	try {
		if (cmd == CMD_HELP)
			help::main();
		if (ac < 3)
			return (error::msg("Enter the command and arguments."));
		if (cmd == CMD_START || cmd == CMD_STARTP)
			start::main(ac, av, cmd);
		else if (cmd == CMD_INHERIT)
			inherit::main(ac, av);
		else if (cmd == CMD_STRONG_GETTER)
			strong_getter::main(ac, av);
		else if (cmd == CMD_EXCEPTION)
			exception::main(ac, av);
		else if (cmd == CMD_FUNCTION)
			function::main(ac, av);
		else if (cmd == CMD_MAIN)
			cmd_main::main(ac, av);
		else if (cmd == CMD_MAKE)
			make::main(ac, av);
		else if (cmd == CMD_RESET)
			reset::main(ac, av);
		else
			return (error::msg("It is a supported command, but the implementation is not yet complete."));
	} catch (char const *msg) {
		std::cout << "ERROR: " << msg << std::endl;
		return (0);
	} catch (...) {
		std::cout << "ERROR: UNEXPECTED ERROR" << std::endl;
		return (0);
	}
	return (0);
}
