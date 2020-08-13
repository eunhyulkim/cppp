#include "cppp.hpp"

namespace fs = std::filesystem;

namespace {
	std::string get_add_suffix_path(std::string suffix)
	{
		std::string path = std::string(std::getenv("PWD"));
		path.push_back('/');
		path.append(suffix);
		return (path);
	}

	void setup_folder()
	{
		std::string root_path = get_add_suffix_path(".cppgit");	
		if (!fs::is_directory(root_path))
			fs::create_directory(root_path);
	}

	std::string get_string_from_file_with_suffix(std::string suffix)
	{
		std::ifstream in(get_add_suffix_path(suffix));
		if (!in.is_open())
			return std::string("");
		std::string rstring;
		std::string line;
		while (std::getline(in, line))
			rstring.append(line).push_back('\n');
		in.close();
		return (rstring);
	}

	void cleanup_backup_folder(std::string root_path, std::string& status, int ac, char *av[])
	{
		int count = 0;

		for(auto& p: fs::directory_iterator(root_path))
		{
			count += 1;
			(void)p;
		}
		if (count == 0)
			fs::remove_all(root_path);
		else
		{
			status.append(fs::path(root_path).filename());
			status.append(std::string(2, ' '));
			for (int i = 1; i < ac; i++)
			{
				status.append(std::string(" "));
				status.append(std::string(av[i]));
				if (i == 1)
					status.append(std::string(10 - std::string(av[i]).size(), ' '));
			}
			status.push_back('\n');
		}
	}

	void cleanup_root_folder(std::string& status)
	{
		std::string root_path = get_add_suffix_path(".cppgit");	

		int count = 0;
		std::string first_path;

		for(auto& p: fs::directory_iterator(root_path))
		{
			if (fs::is_directory(p.path()))
			{
				count += 1;
				if (first_path.empty())
					first_path = p.path();
				else if (p.path() < first_path)
					first_path = p.path();
			}
		}
		if (count > MAX_BACKUP_FOLDER)
		{
			status = status.substr(status.find("\n") + 1);
			fs::remove_all(first_path);
		}
	}

	std::string	get_timestamp(bool has_bracket = false)
	{
		std::time_t	t = std::time(0);
		std::tm* now = std::localtime(&t);
		std::stringstream	ss;

		if (has_bracket)
			ss << "[";
		ss << (now->tm_year + 1900);
		ss << std::setw(2) << std::setfill('0') << (now->tm_mon + 1);
		ss << std::setw(2) << std::setfill('0') << now->tm_mday << "_";
		ss << std::setw(2) << std::setfill('0') << now->tm_hour;
		ss << std::setw(2) << std::setfill('0') << now->tm_min;
		ss << std::setw(2) << std::setfill('0') << now->tm_sec;
		if (has_bracket)
			ss << "]";
		return (ss.str());
	}

	std::string get_root_path()
	{
		std::string root_path = get_add_suffix_path(".cppgit/" + get_timestamp());
		fs::create_directory(root_path);
		return (root_path);
	}

	void backup_file(std::string root_path, std::string base, int opt)
	{
		std::string path;

		if (opt == BACKUP_HEADER || opt == BACKUP_ALL)
		{
			path = get_add_suffix_path(base + ".hpp");
			if (fs::is_regular_file(path))
				fs::copy_file(path, std::string(root_path) + "/" + base + ".hpp");
		}
		if (opt == BACKUP_SOURCE || opt == BACKUP_ALL)
		{
			path = get_add_suffix_path(base + ".cpp");
			if (fs::is_regular_file(path))
				fs::copy_file(path, std::string(root_path) + "/" + base + ".cpp");
		}
		if (opt == BACKUP_MAKEFILE)
		{
			path = get_add_suffix_path(base);
			if (fs::is_regular_file(path))
				fs::copy_file(path, std::string(root_path) + "/" + base);
		}
	}

	void arrange_status(std::string& status)
	{
		std::string path = get_add_suffix_path(".cppgit/.status");		
		std::ofstream out(path, std::ofstream::trunc);
		out << status;
		out.close();
	}

	std::string get_commit_path(std::string input)
	{
		std::string file = get_string_from_file_with_suffix(".cppgit/.status");
		if (file.empty())
			throw ("Invalid commit.");
		else if (file.find(input) == std::string::npos)
			throw ("Invalid commit.");
		int idx = file.find(input);
		while (idx >= 0 && (file.substr(idx, 2) != "20" || file[idx + 8] != '_'))
			idx--;
		if (idx < 0)
			throw ("There was an error in the Commit search.");
		std::string commit = file.substr(idx, 15);
		std::string path = get_add_suffix_path(".cppgit/" + commit);
		if (!fs::is_directory(path))
			throw ("Invalid commit.");
		return (path);
	}
}

namespace reset {
	int		backup(int ac, char *av[], int cmd)
	{
		setup_folder();
		std::string root_path = get_root_path();
		int idx = 2;

		std::string status = get_string_from_file_with_suffix(".cppgit/.status");
		if (cmd == CMD_MAIN) {
			backup_file(root_path, av[2], BACKUP_ALL);	
		} else if (cmd == CMD_MAKE) {
			backup_file(root_path, "Makefile", BACKUP_MAKEFILE);
		} else {
			while (idx < ac)
			{
				backup_file(root_path, av[idx], BACKUP_ALL);
				idx++;
			}
		}
		cleanup_backup_folder(root_path, status, ac, av);
		cleanup_root_folder(status);
		arrange_status(status);
		return (0);
	}

	void	status()
	{
		std::string status = get_string_from_file_with_suffix(".cppgit/.status");
		if (status.empty())
			return ;
		std::cout << "\033[1mcommit\033[0m" << std::string(9, ' ');
		std::cout << std::string(3, ' ') << "\033[1mcommand\033[0m";
		std::cout << std::string(4, ' ') << "\033[1marg\033[0m" << std::endl;
		std::cout << status.substr(0, status.size() - 1);
		std::cout << std::string(4, ' ') << "\033[1;33;40m<-- recent commit\033[0m" << std::endl;
		return ;
	}

	void	hard(std::string commit_path)
	{
		std::string status = get_string_from_file_with_suffix(".cppgit/.status");
		std::istringstream in(status);
		std::ofstream fout(get_add_suffix_path(".cppgit/.status"), std::ofstream::trunc);
		std::string line;

		get::sstream_with_target(in, line, fs::path(commit_path).filename(), fout, true);
		fout.close();

		std::string commit_list = line.substr(0, 15);
		while (std::getline(in, line))
		{
			if (line.empty())
				break ;
			commit_list.insert(0, line.substr(0, 15) + "\n");
		}
		std::istringstream newin(commit_list);
		while (std::getline(newin, line))
		{
			std::string path = get_add_suffix_path(".cppgit/" + line);
			reset::cherrypick(path);
			fs::remove_all(path);
		}
	}

	void	cherrypick(std::string commit_path)
	{
		std::string root_path = std::string(std::getenv("PWD"));

		for(auto& p: fs::directory_iterator(commit_path))
		{
			std::string copy_path = root_path + "/" + std::string(fs::path(p.path()).filename());
			if (fs::is_regular_file(p.path()))
				fs::copy_file(p.path(), copy_path, fs::copy_options::overwrite_existing);
		}
	}

	void	main(int ac, char *av[])
	{
		if (ac < 3)
			throw ("There shall be at least three arguments.");
		if (std::string(av[2]) == "status")
			reset::status();
		else
		{
			if (ac != 4)
				throw ("There must be four argument.");

			std::string commit_path = get_commit_path(av[3]);

			if (std::string(av[2]) == "cherrypick")
				reset::cherrypick(commit_path);
			else if (std::string(av[2]) == "hard")
				reset::hard(commit_path);
			else
				throw ("Invalid command.");
		}
		return ;
	}
}

