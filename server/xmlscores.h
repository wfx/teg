/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
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
/**
 * @file xmlscores.h
 */
#ifndef __TEGS_XMLSCORES_H
#define __TEGS_XMLSCORES_H

/*! loads the scores from and xml file */
TEG_STATUS xmlscores_load( void );

/*! Save the current high scores into the home directory. */
void xmlscores_save(void);

/*! insert a score from a SPLAYER structure */
TEG_STATUS scores_insert_player( PSPLAYER pJ );

/*! dumps to top high scores in parseable protocol format */
TEG_STATUS scores_dump(char *strout, size_t buflen );

#endif /* __TEGS_XMLSCORES_H */
