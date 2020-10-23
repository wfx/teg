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

#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


#define TEG_MAX_PLAYERS 6
#define TEG_MAX_CONNECTIONS 15
#define TEG_DEFAULT_PORT	2000
#define PLAYERNAME_MAX_LEN	50
#define PLAYERADDR_MAX_LEN	(sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")
#define SERVER_NAMELEN 50
#define TEG_MAX_TARJETAS 5
#define DICES_CANT	(6)


#define TEG_DIRRC	".teg/"

#ifndef TRUE 
#define TRUE 1
#define FALSE 0
#endif /* !TRUE */

#define TEG_SOCKET "/tmp/tegsocket"

typedef int BOOLEAN, *PBOOLEAN;

typedef enum {
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
} TEG_STATUS, *PTEG_STATUS;

/* hacer sync con common.c */
enum {
	TEG_RULES_TEG,			/**< reglas tipo TEG */
	TEG_RULES_RISK,			/**< reglas tipo RISK */
	TEG_RULES_1914,			/**< reglas tipo 1914 */
	TEG_RULES_MISC,			/**< reglas tipo otracosa */
};
extern char *g_reglas[];

typedef enum {
	ARMY_ROJO = 0,
	ARMY_AMARILLO = 1,
	ARMY_AZUL = 2,
	ARMY_NEGRO = 3,
	ARMY_ROSA = 4,
	ARMY_VERDE = 5
} ARMY, *PARMY;
extern char *g_colores[];

#define RANDOM_MAX(_min,_max) (_min+(int)(((float)(1+_max-_min))*rand() / (RAND_MAX+1.0)))

#define RANDOM_DEVICE "/dev/random"

#define MODALIDAD_READONLY	0
#define MODALIDAD_PLAYER	1

enum {
	M_INF = 1,
	M_MSG = 2,
	M_IMP = 4,
	M_ERR = 8,
	M_ALL = (M_INF|M_MSG|M_IMP|M_ERR),
};


/* XXX: must be synced with the one in common.c */
typedef enum {
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
} PLAYER_STATUS, *PPLAYER_STATUS;
extern char *g_estados[];

#ifndef MAX
#define MAX(a,b)	(((a) > (b)) ? (a): (b))
#endif

#ifndef MIN
#define MIN(a,b)	(((a) < (b)) ? (a): (b))
#endif


typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY *Flink;
	struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;


#define LENTRY_NULL {NULL,NULL}

#define LIST_NEXT(Entry) (((PLIST_ENTRY)Entry)->Flink)
#define LIST_PREV(Entry) (((PLIST_ENTRY)Entry)->Blink)

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
  PLIST_ENTRY
  RemoveTailList(
      PLIST_ENTRY ListHead
      );
*/

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}

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

/*
  void InsertHeadList(
      PLIST_ENTRY ListHead,
      PLIST_ENTRY Entry
      );
*/

#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }


/* prototipos de funciones */
int get_int_from_dev_random( void );
int cuantos_x_canje( int c );
TEG_STATUS strip_invalid( char *n );
TEG_STATUS strip_invalid_msg( char *n );

/**! like atoi, but if ! s, returns -1 */
int my_atoi( char * s);

/** @brief Copies a string respecting the size limit of the destination buffer.
 *
 * This function copies up to \p destlen bytes from \p source to \p dest. When
 * the size limit is hit, it places a terminating zero byte at the end of the
 * target buffer (unless \p destlen is 0). */
void string_copy(char* dest, size_t destlen, char const* source);

#ifdef __cplusplus
}
#endif
