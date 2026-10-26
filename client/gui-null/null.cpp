/*
 * To port TEG to a new platform/toolkit/etc, all of these functions
 * must be done
 */

#include "../../common/common.h"
#include "../globals.h"

namespace teg::client::callbacks
{

/* called with at the very beginning, with the originals argc & argv */
TEG_STATUS gui_init(ClientState& state, int argc, char **argv)
{
	return TEG_STATUS_SUCCESS;
}

/* for some initialization */
TEG_STATUS gui_main(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}

/* to view messages */
TEG_STATUS gui_textmsg(ClientState& state, char *aString)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_disconnect(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_habilitado(ClientState& state, int numjug)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_connected(ClientState& state, const int *c)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_reconnected(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_status(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_start(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_fichas(ClientState& state, int cant, int conts)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_turn(ClientState& state, PCPLAYER pJ)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_country(ClientState& state, int p)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_country_select(ClientState& state, int country)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_tropas(ClientState& state, int src, int dst, int cant)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_tarjeta(ClientState& state, int country)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_dados(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_reagrupe(ClientState& state, int src, int dst, int cant)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_sensi(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_attack(ClientState& state, int src, int dst)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_canje(ClientState& state, int cant, int p1, int p2, int p3)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_lost(ClientState& state, int numjug)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_surrender(ClientState& state, int numjug)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_winner(ClientState& state, int numjug, int objective)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_mission(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_textplayermsg(ClientState& state, char const *n, int nj, char const *m)
{
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS gui_scores(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}

}
