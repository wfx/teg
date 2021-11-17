// SPDX-License-Identifier: GPL-2.0-or-later

#include "execute.hpp"

#include <vector>
#include <cstdlib>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace teg
{

namespace
{

// Little RAII helper to not let any dangling pointers hang around
struct ArgsHolder {
	ArgsHolder(char const*const* cargs, std::size_t arg_count)
		: args{arg_count, nullptr}
	{
		for(std::size_t i=0; i<arg_count; i++) {
			if(nullptr != cargs[i]) {
				args[i] = strdup(cargs[i]);
			} else {
				args[i] = nullptr;
				break;
			}
		}
	}

	~ArgsHolder()
	{
		for(char *p: args) {
			free(p);
		}
	}

	std::vector<char*> args;
};

}

bool execute_program(const char * const *prog_and_args, std::size_t arg_count)
{
	ArgsHolder holder{prog_and_args, arg_count};
	auto *const exec_fn = (prog_and_args[0][0] == '/')
	                      ? execv
	                      : execvp;
	exec_fn(holder.args[0], holder.args.data());
	return false;
}

std::filesystem::path binary_path()
{
	return std::filesystem::read_symlink("/proc/self/exe");
}

std::filesystem::path program_directory()
{
	return binary_path().parent_path();
}

std::filesystem::path installation_directory()
{
	return binary_path().parent_path().parent_path();
}

}
