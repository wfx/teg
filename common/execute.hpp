// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstddef>

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

}
