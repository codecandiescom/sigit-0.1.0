/* config.h  --  The file holding every overall needed info for the entire program.. 

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


/* $Id: config.h,v 0.11 2001/08/13 16:43:00 redhead Exp $ */

#ifndef __CONFIG__H__
#define __CONFIG__H__

#include <fcntl.h>       // O_CREAT, O_WRONLY, O_NONBLOCK
#include <pwd.h>         // getpwuid(), getuid()
#include <signal.h>      // signalhandler
#include <stdarg.h>      // va_start(), va_list
#include <stdio.h>       // snprintf(), fopen(), feof(), fgets(), fclose(), fwrite()
#include <stdlib.h>      // getenv()
#include <string.h>      // strcmp(), strncpy(), strcpy()
#include <syslog.h>      // openlog(), syslog(), closelog(), LOG_PID
#include <time.h>        // localtime(), ctime(), 
#include <unistd.h>      // access(), getpid(), getppid(), fork()

/* no need for this just yet.. 
 *  #include <pthread.h>     // pthread_dreate(), pthread_join(), pthread_exit(), 
 *                           // pthread_mutex_lock(), pthread_mutex_unlock()
 *  #include <zlib.h>        // gzopen(), gzread(), gzwrite(), gzseek(), gzclose()
 */

#include <sys/stat.h>    // umask()
#include <sys/types.h>   // open()
#include <sys/wait.h>    // wait()
#include <sys/utsname.h> // uname()
#include <netinet/in.h>  // htonl(), ntohl()


/* 
   well i have had alot of thoughts about, how the signature struct should be.. 
   thought a while 'bout making one, where there would be a pointer to the next sig
   But i ended up with this one.. 
   If the index char is '\0' then i have reached the end of the file, and theres no more 
   signatures to read, else there must be atleast one more.
*/

/* first we need to make sure, what size of a sig we can accept.. */

#define LINE_SIZE 72 /* in order to presssent a fine prity print */
#define NUM_LINES 8 /* how many lines can the max signature hold. */
#define BUFF_SIZE (LINE_SIZE * NUM_LINES +1) /*making sure theres room for a signature */
#define MAXFD 64 /* max file descriptors used when shiftign to deamon.*/
#define HEXCOUNT 10 /* the max length, og the unique identifier every sig has */

/* where are the default files to use.. */

#define DEFAULT_EDITOR "vi"
#define DEFAULT_CONF_FILE "/etc/sigit.rc"
#define DEFAULT_DATA_FILE "/usr/share/sigit/sigit.data"
#define DEFAULT_SIG_FILE  ".plan"
#define DEFAULT_EDIT_FILE "signature.edit"
#define DEFAULT_SIG  "#### due to errors the intended signature has been replaced with ####\n\"The significant problems we face cannot be solved at the\n same level of thinking we were at when we created them.\"\n                -Albert Einstein"


#define PACKAGE   "Sigit"
#define VERSION   "0.1.0"  /* we better remember to change this for any upgrade */
#define CALLSIGN  "Eir"
#define HOMEPAGE  "http://www.redhead.dk."

/***************************************************
 *  WARNING: Do NOT change what's below this line  *
 ***************************************************/

#ifndef _PATH_BSHELL
# define _PATH_BSHELL "/bin/sh"
#endif

/*
 *   the error types that can be returned. 
 *
 *   NO_READ     <-- We are not allowed to read from a file, or an error happende while reading 
 *   COUNT_MISS  <-- The file was either not found, or found empty when reading from it
 *   EXCEEDED    <-- The size exceeded the buffer length
 *   CONF_MI     <-- when missing every sign of sigit.rc file 
 *   CONF_ER     <-- when the sigit.rc file in /etc has no read access
 *   CONF_HR     <-- when the .sigitrc file in the homedir has no read access
 *   SUCCESS     <-- Well we all know what that means..
 *   FAILURE     <-- That is prity much known too..
 *   FAIL_FO     <-- If theres not enough heap to use fork()
 *   FAIL_EX     <-- If something happened during execution of the child process
 *   SEEK_ERR    <-- If we cant seek to the desired point in the data file
 *   WRITE_ERR   <-- If at some point a write error happens once we start writing to the files
 *
*/

#define NO_READ    -13
#define COUNT_MISS -12
#define EXCEEDED   -11
#define CONF_MI     -3
#define CONF_ER     -2
#define CONF_HR     -1
#define SUCCESS      0
#define FAILURE      1
#define FAIL_FO      2 
#define FAIL_EX      3
#define SEEK_ERR    11
#define WRITE_ERR   12


/* here are the lists that will be used to deliver pre set arguments */

#ifdef __cplusplus
extern "C" {
#endif

  char sigfile[LINE_SIZE +1];

struct signature{
  char sig[NUM_LINES*LINE_SIZE  +1];
  unsigned long int count;
  char ident[HEXCOUNT +1];
};

struct counter{
  char ver[11];
  unsigned long int count;
};


  /* 
     its very strange to place this here.
     But it's due to the fact, that if its a verbose 
     output thats requested, then every part of the program
     must see it. 
  */


struct setting{
  char sig_file[LINE_SIZE +1];
  char data_file[LINE_SIZE +1];
  char pritty[NUM_LINES*LINE_SIZE +2];
  char t_line[LINE_SIZE +1];
  char b_line[LINE_SIZE +1];
  char optarg_element[LINE_SIZE +1];
  char config_file[LINE_SIZE +1];
  char delete_sig[LINE_SIZE +1];

  struct signature sig_n;
  struct counter cnt;

  unsigned short int list;
  unsigned short int delete;
  unsigned short int slash;
  unsigned short int deamon_proc;
  unsigned short int create_sig;
  unsigned short int sys_log;
  unsigned short int verbose;
  unsigned long int c_num; 
  time_t date;
};
  
#ifdef __cplusplus
};
#endif



int create_pid_file(unsigned short int verbose, unsigned short int sys_log);
int remove_pid_file(unsigned short int verbose, unsigned short int sys_log);


int logging(int need_package_info, 
	    unsigned short int verbose, int level, 
	    unsigned short int sys_log, int sys_ready, const char* fmt, ...);


/*
  well this should be an obvius function, it reads the config file,
   and fills in the list of prefered arguments..
*/

int read_config_file(struct setting *conf);


/* yeah, yeah.. guess what this one does.. :)) */
int _shine_on_you_crazy_diamond_(void);

/* 
   Last night I used some code that wasn't there, 
   This morning It wasn't there either.
   Oh, how I wish it would stay...
 */

int Mostly_Harmless(struct setting *conf);

#endif /* ifndef config */











