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

#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstdint>

#include <array>
#include <string>

constexpr unsigned maximum_player_count{6};
constexpr std::uint16_t default_server_port{2000};
constexpr unsigned max_playername_length{50};
constexpr std::size_t inet_addr_len	{sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"};
constexpr std::size_t maximum_servername_length{50};
constexpr std::size_t maximum_country_cards{5};
constexpr std::size_t sides_on_the_dice{6};


constexpr char const* rc_directory_name{".teg/"};

enum TEG_STATUS {
	TEG_STATUS_SUCCESS = 0,
	TEG_STATUS_ERROR = 1,
	TEG_STATUS_NOTFOUND = 2,
	TEG_STATUS_NOMEM = 3,
	TEG_STATUS_CONNCLOSED = 4,
	TEG_STATUS_TOKENNOTFOUND = 5,
	TEG_STATUS_TOKENNULL = 6,
	TEG_STATUS_PLAYERNOTFOUND = 7,
	TEG_STATUS_PARSEERROR = 8,
	TEG_STATUS_NOTCONNECTED = 9,
	TEG_STATUS_UNEXPECTED = 10,
	TEG_STATUS_FILENOTFOUND = 11,
	TEG_STATUS_GAMEOVER = 12,
	TEG_STATUS_THEMEERROR = 13,
};

enum ARMY {
	ARMY_ROJO = 0,
	ARMY_AMARILLO = 1,
	ARMY_AZUL = 2,
	ARMY_NEGRO = 3,
	ARMY_ROSA = 4,
	ARMY_VERDE = 5
};
extern char const *g_colores[];

inline int random_between(int min, int max)
{
	auto const random_fract{static_cast<double>(rand())/(RAND_MAX+1.0)};
	return min + (1+max-min)*random_fract;
}

enum {
	M_INF = 1,
	M_MSG = 2,
	M_IMP = 4,
	M_ERR = 8,
	M_ALL = (M_INF|M_MSG|M_IMP|M_ERR),
};


enum PLAYER_STATUS {
	PLAYER_STATUS_DESCONECTADO,	/**< not connected */
	PLAYER_STATUS_CONNECTED,	/**< connected */
	PLAYER_STATUS_GAMEOVER,		/**< game over */
	PLAYER_STATUS_HABILITADO,	/**< enabled with color */
	PLAYER_STATUS_START,		/**< started the gamej */
	PLAYER_STATUS_FICHAS,		/**< placing the initial armies */
	PLAYER_STATUS_POSTFICHAS,	/**< post */
	PLAYER_STATUS_FICHAS2,		/**< placing the 2nd initial armies */
	PLAYER_STATUS_POSTFICHAS2,	/**< post */
	PLAYER_STATUS_IDLE,		/**< waiting turn */
	PLAYER_STATUS_FICHASC,		/**< placing continets armies */
	PLAYER_STATUS_CANJE,		/**< exchaging cards */
	PLAYER_STATUS_POSTFICHASC,	/**< post */
	PLAYER_STATUS_PACTO,		/**< making a deal */
	PLAYER_STATUS_TURNOSTART,	/**< starting an attack */
	PLAYER_STATUS_ATAQUE,		/**< attacking */
	PLAYER_STATUS_TROPAS,		/**< moving armies to new country */
	PLAYER_STATUS_REAGRUPE,		/**< regrouping armies */
	PLAYER_STATUS_TARJETA,		/**< asking for a card */
	PLAYER_STATUS_TURNOEND,		/**< ending turn */

	PLAYER_STATUS_LAST		/**< unreacheble state */
};
extern char const *g_estados[];

struct LIST_ENTRY {
	struct LIST_ENTRY *Flink;
	struct LIST_ENTRY *Blink;
};

using PLIST_ENTRY = LIST_ENTRY*;


#define LENTRY_NULL {NULL,NULL}

#define LIST_NEXT(Entry) (((PLIST_ENTRY)Entry)->Flink)

/*
  void
  InitializeListHead(
      PLIST_ENTRY ListHead
      );
*/

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

/*
  int
  IsListEmpty(
      PLIST_ENTRY ListHead
      );
*/

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

/*
  PLIST_ENTRY
  RemoveHeadList(
      PLIST_ENTRY ListHead
      );
*/

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

/*
  void
  RemoveEntryList(
      PLIST_ENTRY Entry
      );
*/

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

/*
  void
  InsertTailList(
      PLIST_ENTRY ListHead,
      PLIST_ENTRY Entry
      );
*/

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

/// Read a random number from the linux/bsd kernel
int get_int_from_dev_random(void);

/**
 * \brief Calculates the number of additional armies after already \p excanges
 * had taken place.
 *
 * @return The number of additional armies.
 */
unsigned cards_for_this_exchange(unsigned exchanges);

/**
 * @brief Removes invalid characters from the string, for use with player names
 *        and remote addresses.
 *
 * @param[inout] n the string to strip
 */
void strip_invalid(char *n);

/**
 * @brief Removes invalid characters from the string, for use in network
 *        messages.
 *
 * @param[inout] n
 */
void strip_invalid_msg(char *n);

/**! like atoi, but returns -1 when s==NULL*/
int my_atoi(const char *s);

/** @brief Copies a string respecting the size limit of the destination buffer.
 *
 * This function copies up to \p destlen bytes from \p source to \p dest. When
 * the size limit is hit, it places a terminating zero byte at the end of the
 * target buffer (unless \p destlen is 0). */
void string_copy(char* dest, size_t destlen, char const* source);

using ContinentNames = std::array<std::string, 6>;
std::string replace_continents(std::string const& format,
                               ContinentNames const& country_names);
