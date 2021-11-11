// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <filesystem>
#include <cstddef>
#include <optional>

namespace teg
{

/**
 * Runs the program \p prog_and_args with its arguments. The total count
 * (including the program) is signaled with \p arg_count.
 *
 * The newly created process will have the program name set as argv[0].
 *
 * On success this function does not return. On error this function returns
 * false.
 */
bool execute_program(char const*const* prog_and_args, std::size_t arg_count);

/**
 * Helper template to call execute_program with the correct arg_count parameter
 */
template <std::size_t N>
inline bool execute_program(char const*const(&prog_and_args)[N])
{
	return execute_program(prog_and_args, N);
}

/**
 * Returns the directory of the currently running program.
 *
 * This function throws a std::filesystem::filesystem_error when the path can't
 * be determined.
 */
std::filesystem::path program_directory();

/**
 * Return the installation base directory where the software is installed.
 *
 * \internal The assumption is, that the binary is installed in
 *           `installroot/bin`.
 *
 * A std::filesystem::filesystem_error is raised when the directory can't be
 * determined.
 */
std::filesystem::path installation_directory();
}
