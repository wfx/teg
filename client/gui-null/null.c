/*
 * To port TEG to a new platform/toolkit/etc, all of these functions
 * must be done
 */

#include "../../common/common.h"

/* called with at the very beginning, with the originals argc & argv */
TEG_STATUS gui_init(int argc, char **argv)
{
	return TEG_STATUS_SUCCESS;
}

/* for some initialization */
TEG_STATUS gui_main(void)
{
	return TEG_STATUS_SUCCESS;
}

/* to view messages */
TEG_STATUS gui_textmsg(char *aString )
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_disconnect(void)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_habilitado(int numjug)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_connected()
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_status(void)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_start(void)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_fichas(int cant, int conts)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_turno(void)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_pais(int p)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_pais_select( int pais )
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_tropas(int src, int dst, int cant)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_tarjeta(int pais )
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_dados()
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_reagrupe( int src, int dst, int cant )
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_sensi()
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_ataque( int src, int dst )
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_canje()
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_lost(int numjug)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_surrender(int numjug)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_winner(int numjug, int objetivo)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_objetivo()
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_textplayermsg(char *n,int nj, char *m )
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_country(int country_number)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_country_select(int country_number)
{
	return TEG_STATUS_SUCCESS;
}
