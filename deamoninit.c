/* deamoninit.c -- a way to run a desired process as a deamon.. 

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

/* $Id: deamoninit.c,v 0.8 2001/08/13 16:43:00 redhead Exp $ */

#include "deamoninit.h" // include "config.h"


void deamon_init(const char* pname, int facility)
{
  int i;
  unsigned long pid;
  if((pid=fork())!=0)
    exit(0);
  setsid();
  signal(SIGHUP, SIG_IGN);
  if((pid=fork())!=0)
    exit(0);
  chdir("/");
  umask(0);
  for(i=0; i < MAXFD; i++)
    close(i);
  openlog(PACKAGE, LOG_PID, facility);
}


/* ignore SIGPIPE */
void ignore_sigpipe(int sig)
{
  /* 
     I havn't yet found out how to solve the SIGPIPE
     send from pine, when using default settings with
     .signature as the file.. 
     a strace revealed, that pine first says, 
     Oh it's the default setting, so it must be a stationary file, 
     then it tries to open it as such, where it chokes, but sigit 
     gets the open, and tries to write to it, but meanwhile pine
     has allready closed it, and send a fstat() request, in order 
     to decide what type of file it is.. 
  */

  logging(1, 1, 0, 0, 0, "Caught a SIGPIPE. Using Pine by any chance?\n");
 
 }


/* to handle any signals recieved.. */

void caught_signal(int sig)
{
  struct setting cnf;

  cnf.config_file[0] = '\0';

  cnf.sys_log = 0;
  cnf.verbose = 0;
  cnf.date = 0;  

  read_config_file(&cnf);
  logging(1, 1, 0, 0, 0, "Caught signal %d, Cleaning up..\n", sig);
  /* hmmm should be a check here, about the syslog, 
     but we hope it errors out, if the syslog wasnt opened. */ 
  closelog();
  /* some what of a hack since most run will end here. 
     But I'm not sure, if unlinking the fifo is needed when exiting. */
  unlink_fifo(&cnf);
  exit(SUCCESS);
}


/* 
   This is an experimental function.. It's main goal is to solve problems, if
   people experience segmentation faults, then this should print a nice
   message explaining what happened.. like the bug-trace in the kernel..
*/


void caught_sigsegv(int id, siginfo_t *sig, void *ptr)
{
  struct setting cnf;

  cnf.config_file[0] = '\0';

  cnf.sys_log = 0;
  cnf.verbose = 0;
  cnf.date = 0;  
  
  //              siginfo_t {
  //                int      si_code;   /* Signal code */
  //                clock_t  si_utime;  /* User time consumed */
  //                clock_t  si_stime;  /* System time consumed */
  //                sigval_t si_value;  /* Signal value */
  //                int      si_int;    /* POSIX.1b signal */
  //                void *   si_ptr;    /* POSIX.1b signal */
  //            }

  read_config_file(&cnf);
  logging(2, 1, 0, 0, 1, "[ERROR] : Segfault bugtrace started at address: 0x%.8X\n", &ptr);
  logging(1, 1, 0, 0, 1, "Running as pid: %d, with uid: %d, recieving status: %d\n", 
	  &sig->si_pid, &sig->si_uid, &sig->si_status);
  logging(1, 1, 0, 0, 1, "Consuming %d User time, and %d System time\n", 
	  &sig->si_utime , &sig->si_stime);
  logging(1, 1, 0, 0, 1, "Recovering from error code: %d\n", &sig->si_errno);
  logging(1, 1, 0, 0, 1, "Caused at address: 0x%.8X\n", &sig->si_addr);
  logging(1, 1, 0, 0, 1, "While mengeling with fd: %d\n", &sig->si_fd);
  /* hope this 'si_band' can be used here.. */
  logging(1, 1, 0, 0, 1, "(%score dumped)\n", WCOREDUMP(&sig->si_band) ?"" : "no ");
  /*logging(1, 1, 0, 0, 1, "");*/

  /* hmmm should be a check here, about the syslog, 
     but we hope it errors out, if the syslog wasn't opened. */ 
  closelog();
  /* some what of a hack since most run will end here. 
     But I'm not sure, if unlinking the fifo is needed when exiting. */
  unlink_fifo(&cnf);
  exit(SUCCESS);
}


/* to start the signal handler */
void install_sig_handler(unsigned short int verbose, unsigned short int sys_log)
{
  struct sigaction act;
  logging(1, verbose, 0, sys_log, 1,"Starting signal handler.\n");
  /* install signal handler */
  act.sa_handler = caught_signal;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGINT, &act, 0);
  sigaction(SIGHUP, &act, 0);
  sigaction(SIGTERM, &act, 0);
  act.sa_handler = ignore_sigpipe;
  sigemptyset(&act.sa_mask);
  sigaction(SIGPIPE, &act, 0);
  /* this is the seecret part, we're just like
     the kernel, If we happen to seg-faulte
     we better report what might caused the problem. */
  act.sa_sigaction = caught_sigsegv;
  sigemptyset(&act.sa_mask);
  sigaction(SIGSEGV, &act, 0);
  logging(1, verbose, 0, sys_log, 1, "   DONE\n");
}


/* to create the fifo */

int init_fifo(struct setting *conf)
{
  logging(1, conf->verbose, 0, conf->sys_log, 1,"Creating [%s] as fifo file.\n", conf->sig_file);
  if (!access(conf->sig_file, F_OK)) {
    /* FIFO already exists */
    if (unlink(conf->sig_file) == -1) {
      logging(1, conf->verbose, 0, conf->sys_log, 1,"Couldn't unlink the old [%s] fifo.\n", conf->sig_file);
      return FAILURE;
    }
  }
  if (mkfifo(conf->sig_file, 0644) == -1) {
    logging(1, conf->verbose, 0, conf->sys_log, 1,"Couldn't create [%s] as fifo.\n", conf->sig_file);
    return FAILURE;
  }
  logging(1, conf->verbose, 0, conf->sys_log, 1, "   DONE\n");
  return SUCCESS;
}

/* to remove the fifo.. */

int unlink_fifo(struct setting *conf)
{
  logging(1, conf->verbose, 0, conf->sys_log, 1,"Unlinking fifo file.\n");
  unlink(conf->sig_file);
  remove_pid_file(conf->verbose, conf->sys_log);
  logging(1, conf->verbose, 0, conf->sys_log, 1, "   DONE\n");
  return SUCCESS;
}


int Mostly_Harmless(struct setting *conf)
{
  goto one;
  zero:
  logging(1, conf->verbose, 4, conf->sys_log, 0," - Getting a little slippery here?\n");
  sleep(2);
  goto two;
 one:
  logging(1, conf->verbose, 4, conf->sys_log, 0,"Anything that happens, happens.\n");
  sleep(2);
  logging(1, conf->verbose, 4, conf->sys_log, 0,"Anything that, in happening, causes something else to happen,\n\tcauses something else to happen.\n");
  sleep(2);
  goto zero;
 two:
  logging(1, conf->verbose, 4, conf->sys_log, 0,"Anything that, in happening, causes something else to happen,\n\tcauses something else to happen.\n");
  sleep(2);
  goto four;
 three:
  logging(1, conf->verbose, 4, conf->sys_log, 0,"Real programmers aren't afraid of using GOTOs.\n");
  goto five;
  four:
  logging(1, conf->verbose, 4, conf->sys_log, 0,"It doesn't necessarily do it in chronological order, though..\n");
  sleep(2);
  goto three;
 five:
    return SUCCESS;
}



