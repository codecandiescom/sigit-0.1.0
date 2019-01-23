/* files.h  --  The file holding all i/o functions required by Sigit..

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

/* $Id: files.h,v 0.4 2001/06/17 22:23:50 redhead Exp $ */

#ifndef __FILES__H__
#define __FILES__H__

#include "config.h" // #include <every_fucking_thing>


/*
  the add_sig will read the file parsed to the function, and update the default datafile
  If the person wants to update another file, they will have to set the end file in the 
  configuration file. This feature is used like:
   sigit -a <srcfile>
  which will update the default data file.
*/

int add_sig(struct setting *conf);


/*
  The get_sig will read the signature from the file parsed to it, if theres 
  another setting in the config file this will be used instead.
 */

int edit_data_file(struct setting *conf);

int get_sig(struct setting *conf);


int set_sig(struct setting * conf);


int pretty_print(struct setting * conf);

#endif
