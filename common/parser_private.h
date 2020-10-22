// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "parser.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum CharClass {
	ccEnd, ///< end of data
	ccSeparators, ///< one of the separators
	ccEquals, ///< one of the equals
	ccData ///< none of the others
};

typedef enum {
	PARSER_FIN = ccEnd,
	PARSER_SEPARADOR = ccSeparators,
	PARSER_IGUAL = ccEquals,
	PARSER_DATA = ccData,
	PARSER_ERROR
} PARSER_VALUE, *PPARSER_VALUE;

bool parser_belongs_to_class(char ch, DELIM const* which);

/**
 * @brief Checks which "character class" the character to check falls into
 * @param a the character to test
 * @param equals possible equal signs
 * @param separators possible separators
 * @return the character class.
 */
enum CharClass parser_character_class(char a,
                                      DELIM const* equals,
                                      DELIM const* separators);

/// A delimiter descriptor which matches nothing.
extern DELIM const delim_null;

#ifdef __cplusplus
}
#endif
