/* sigit.h -- an auto signature creating tool.. 

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

/* $Id: sigit.h,v 0.5 2001/03/10 04:53:05 redhead Exp $ */

#ifndef __SIGIT__H__
#define __SIGIT__H__

#include "config.h" // #include <every_fucking_thing>


/*
 * This is some demanding function..
 * while the process is running it owns a pid. 
 * thus it will create a /tmp/PACKAGE-USER file
 * this will contain the pid of the running program. 
 * when the program executes it will check if theres a pid in the file, 
 * if there is, it will kill the runnign program.
 * (this happens if the program is started as a deamon), and exit.
 *
 * Else it will place its own PID in the file, and run as a regulary program.
 * upon exit the program will remove any remaining /tmp/PACKAGE-USER file.
 */
/* These functions have been moved to config.h to consort in the flow of the program.
  
   int create_pid_file(unsigned short int verbose, unsigned short int sys_log);
   
   int remove_pid_file(unsigned short int verbose, unsigned short int sys_log);
*/

int main(int argc, char* argv[]);


#endif
