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

#include "SDL.h"
#include "SDL_image.h"

struct _paises {
  char *nombre;
  SDL_Surface *grafico;
  SDL_Rect pos;
};

void init_SDL();
void init_screen();
void load_countries();
void draw_board();
Uint32 getpixel(SDL_Surface *surface, int x, int y);
