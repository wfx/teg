/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 * C SDL Client Author: Ernesto Domato <ernesto@domato.com.ar>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License
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

#include "client.h"
#include "gui.h"
#include "themes.h"
#include <string.h>

TTheme gui_theme;
struct _paises paises[COUNTRIES_CANT];
SDL_Surface *screen, *board;

/* called with at the very beginning, with the originals argc & argv */
TEG_STATUS gui_init(int argc, char **argv)
{
  char *filename;
  
  init_nls();
  atexit(SDL_Quit);

  init_SDL();

  init_screen();
  SDL_ShowCursor(SDL_ENABLE);

  strncpy(g_game.theme, "m2", sizeof(g_game.theme)); // tema por defecto
  theme_load(g_game.theme);
  if (theme_giveme_theme(&gui_theme) != TEG_STATUS_SUCCESS)
  {
    fprintf(stderr, "Error loading theme!\n");
    return TEG_STATUS_ERROR;
  }
  filename = theme_load_file(gui_theme.board);
  board = IMG_Load(filename);
  if (board == NULL)
  {
    fprintf(stderr, "Error loading board!\n");
    SDL_Quit();
    return TEG_STATUS_ERROR;
  }

 
  load_countries();

  draw_board();

  int ok = 1, i;
  Uint32 color;
  SDL_Event eventos;
  while (ok)
  {
    while (SDL_PollEvent(&eventos))
    {
      switch (eventos.type)
      {
	case SDL_QUIT:
	  ok = 0;
	  break;
	case SDL_MOUSEMOTION:
	  for (i = 0; i < COUNTRIES_CANT; i++)
	  {
	    if (paises[i].pos.x < eventos.motion.x && paises[i].pos.x+paises[i].pos.w > eventos.motion.x && paises[i].pos.y < eventos.motion.y && paises[i].pos.y+paises[i].pos.h > eventos.motion.y)
	    {
	      color = getpixel(paises[i].grafico, eventos.motion.x-paises[i].pos.x, eventos.motion.y-paises[i].pos.y);
	      if (color)
	      {
		printf("%s\n", paises[i].nombre);
		break;
	      }
	    }
	  }
	  break;
      }
    }
  }
		  
  return TEG_STATUS_SUCCESS;
}

/* for some initialization */
TEG_STATUS gui_main(void)
{
	return TEG_STATUS_SUCCESS;
}

/* to free the resource */
TEG_STATUS gui_exit(char *str)
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
TEG_STATUS gui_connected( char *c )
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
TEG_STATUS gui_country_select(int country)
{
  	return TEG_STATUS_SUCCESS;
}

void init_SDL()
{
  if ((SDL_Init(SDL_INIT_VIDEO) == -1))
  {
    fprintf(stderr, "Error initiating SDL!\n");
    exit(1);
  }
}

void init_screen()
{
  screen = SDL_SetVideoMode(640, 480, 8, SDL_SWSURFACE);
  if (screen == NULL)
  {
    fprintf(stderr, "Error creating main screen!\n");
    SDL_Quit();
    exit(1);
  }
}

void load_countries()
{
  int i;
  char *filename;
  TCountry country;
  TContinent continent;
  for (i = 0; i < COUNTRIES_CANT; i++)
  {
    int c = g_countries[i].continente;
    int sum, j, p;
    sum = 0;
    for (j = 0; j < c; j++)
      sum += g_conts[j].cant_countries;
    p = i - sum;
    theme_giveme_country(&country, c, p);
    theme_giveme_continent(&continent, c);
    paises[i].nombre = country.name;
    filename = theme_load_file(country.file);
    paises[i].grafico = IMG_Load(filename);
    paises[i].pos.x = country.pos_x + continent.pos_x;
    paises[i].pos.y = country.pos_y + continent.pos_y;
    paises[i].pos.w = paises[i].grafico->w;
    paises[i].pos.h = paises[i].grafico->h;
    if (paises[i].grafico == NULL)
    {
      fprintf(stderr, "Error loading country!\n");
      exit(1);
    }
  }
}

void draw_board()
{
  SDL_BlitSurface(board, NULL, screen, NULL);
  int i;
  for(i = 0; i < COUNTRIES_CANT; i++) {
    SDL_BlitSurface(paises[i].grafico, NULL, screen, &paises[i].pos);
  }
  SDL_Flip(screen);
}

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
    case 1:
      return *p;

    case 2:
      return *(Uint16 *)p;


    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
	return p[0] << 16 | p[1] << 8 | p[2];
      else
	return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
      return *(Uint32 *)p;

    default:
      return 0;       /* shouldn't happen, but avoids warnings */
  }
}

