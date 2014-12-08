/*	$Id: share.c,v 1.5 2002/04/11 01:24:18 riq Exp $	*/
/********************************************************************** 
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/
/* Funciones sacadas del freeciv con algunas modificaciones */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "all.h"
#include "share.h"

/**************************************************************************
  return a char * to the parameter of the option or NULL.
  *i can be increased to get next string in the array argv[]. 
 **************************************************************************/
char * get_option(const char *option_name,char **argv,int *i,int argc)
{
	int len = strlen(option_name);

	if( !strcmp(option_name,argv[*i]) || (!strncmp(option_name,argv[*i],len) && argv[*i][len]=='=') ||
		!strncmp(option_name+1,argv[*i],2) ) {

		char *opt = argv[*i] + (argv[*i][1] != '-' ? 0 : len);

		if (*opt == '=') {
			opt++;
		} else {
			if (*i < argc - 1) {
				(*i)++;
				opt = argv[*i];
			} else {
				fprintf(stderr, _("Parameter is missing for \"%s\".\n"), option_name);
				exit(1);
			}
		}
		return opt;
	}
	return NULL;
}


int is_option(const char *option_name,char *option)
{
	if (!strcmp(option_name,option) || !strncmp(option_name+1,option,2))
		return 1;
	return 0;
}


void dont_run_as_root()
{
	if (getuid()==0 || geteuid()==0) {
		fprintf(stderr, _("Fatal error: You can't run this program as superuser\n"));
		fprintf(stderr, _("Try to run it as a different user\n"));
		exit(1);
	}
}

void init_nls()
{
#ifdef ENABLE_NLS
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif
}
