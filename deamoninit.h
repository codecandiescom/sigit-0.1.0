/* deamoninit.h -- a way to run a desired process as a deamon.. 

   Copyright (C) Kenneth 'Redhead' Nielsen <redhead@diku.dk>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* $Id: deamoninit.h,v 0.5 2001/03/15 23:06:57 redhead Exp $ */


#ifndef __DEAMONINIT__
#define __DEAMONINIT__


#include "config.h" // #include <every_fucking_thing>


void deamon_init(const char* pname, int facility);


int init_fifo(struct setting *conf);

int unlink_fifo(struct setting *conf);

void ignore_sigpipe(int sig);

void caught_signal(int sig);

void install_sig_handler(unsigned short int verbose, unsigned short int sys_log);


#endif
