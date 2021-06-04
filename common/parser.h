/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
/*
 * Estructura del parser
 */

#pragma once

#include <cstddef>

#define PARSER_TOKEN_MAX 1024
#define PARSER_VALUE_MAX 1024

struct DELIM {
	char accept;
	bool valid=true;

	DELIM& operator = (DELIM const* other)
	{
		if(other) {
			*this = *other;
		} else {
			valid = false;
		}
		return *this;
	}
};

struct PARSER {
	PARSER(char const *data, char eq=':', char sep=',')
		: PARSER{data, DELIM{eq}, DELIM{sep}}
	{
	}

	PARSER(char const *data, DELIM const& eq, DELIM const& sep)
		: data{data}, equals{eq}, separators{sep}
		, m_can_continue{bool(data) && (data[0] != 0)}, m_ok{m_can_continue}
	{
	}

	void reset(char const* retry)
	{
		data = retry;
		m_can_continue = true;
		m_ok = true;
	}

	char const* remainder() const
	{
		return data;
	}

	/// Try to parse the input text, and return if it was successful
	bool parse();

	/// Parse with the expectation that there will something remain
	bool parse_fragment()
	{
		return parse() && can_continue();
	}

	/// parse with the expectation that everything is captured
	bool parse_everything()
	{
		return parse() && !can_continue();
	}

	/** \brief parses the next token
	 *
	 * This function checks if the parser is in a continuable state, and tries
	 * to fetch the next token from the input.
	 *
	 * The parser state will be set to failed when there is no more input to
	 * consume.
	 *
	 * \return true, if there where no previous errors, and one token could be
	 *         extracted. false otherwise. */
	bool parse_token()
	{
		if(ok()) {
			if(parse()) {
				return true;
			} else {
				fail();
			}
		}
		return false;
	}

	/** Returns the parser extraction state.
	 *
	 * The extraction operators can set the parser to failed when they recognize
	 * that the input string did not match the syntax. This property can be
	 * queried with this method. */
	bool ok() const
	{
		return m_ok;
	}

	bool can_continue()const
	{
		return bool(data) && m_can_continue;
	}

	/** Returns if the parsing steps where succesfull and there are no more
	 *  tokens left in the input */
	bool finished() const
	{
		return ok() && !can_continue();
	}

	/** \brief Set the indication that a value could not be parsed.
	 *
	 * This method is meant to be called by the `>>` parsing operators. */
	void fail()
	{
		m_ok=false;
	}

	char token[PARSER_TOKEN_MAX];
	char value[PARSER_VALUE_MAX];

private:
	DELIM const equals;
	DELIM const separators;
	char const *data;

	bool m_can_continue;
	bool m_ok;
};

PARSER& operator >> (PARSER& source, int& value);

struct Limited {
	int &dest; ///< Variable to set
	int const min; ///< minimal acceptable value
	int const max; ///< maximal acceptable value

	/// value to set if the syntax of the input is wrong or out of bounds
	int const reset_to;
};

PARSER& operator >> (PARSER& source, Limited && limit);

struct CString {
	template <std::size_t sz> CString(char (&dest)[sz]) : dest{dest}, count{sz} {}
	char *const dest;
	std::size_t const count;
};

PARSER& operator >> (PARSER& source, CString && limit);
