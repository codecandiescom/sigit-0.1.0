
/* config.c  --  The file holding every overall needed info for the entire program.. 

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


/* $Id: config.c,v 0.9 2001/08/13 16:43:00 redhead Exp $ */


#include "config.h" // #include <every_fucking_thing>


int create_pid_file(unsigned short int verbose, unsigned short int sys_log)
{
  int pid = -1, ppid = -1;
  char lockfile[LINE_SIZE +1]; /* should be enough for the user and /tmp and PACKAGE */
  FILE* lockfp;
  /* First findout who the user is.. */
  uid_t  original_real_uid = getuid();
  struct passwd* pw = getpwuid(original_real_uid);
  if (!pw)
    {
      logging(1, verbose, 0, sys_log, 1, "You don't exist, go away!\n");
      exit(1);
    }
  /* then test if theres a file called PACKAGE-USER */
  /* if found look into it, and decide the pid of the running program */
  /* send an alert, that there allready is a copy of PACKAGE running as PID  */
  /* and kill it, then start normaly */
#ifdef HPUX
  sprintf(lockfile, "%s/.%s-%s.lock", getenv("HOME"), PACKAGE, pw->pw_name);
#else
  snprintf(lockfile, LINE_SIZE, "%s/.%s-%s.lock", getenv("HOME"), PACKAGE, pw->pw_name);
#endif
  logging(1, verbose, 0, sys_log, 1, "Creating [%s] as lock file.\n", lockfile);
  if((lockfp = fopen(lockfile, "r")) != NULL )
    {
      fscanf(lockfp,"%d %d", &pid, &ppid);
      if(!kill(pid, 9)) { /* should be a kill-sig thats accepted even if its a suspended process */
	fclose(lockfp);	/* not checking should be safe, file mode was "r" */
	logging(1, verbose, 0, sys_log, 1, "Removing stale lockfile.\n");
	pid = -1;
	ppid = -1;
	unlink(lockfile);
      }
      else
	{
	  logging(1, verbose, 0, sys_log, 1, "Couldn't kill allready running process\n");      
	  unlink(lockfile);
	}
    }
  /* else start the program, and create the PID file. */
  pid = getpid();
  ppid = getppid();
  if((lockfp = fopen(lockfile, "w")) != NULL)
    fprintf(lockfp, "%d %d", pid, ppid);
  fclose(lockfp);
  logging(1, verbose, 0, sys_log, 1, "   DONE\n");
  return SUCCESS;
}


int remove_pid_file(unsigned short int verbose, unsigned short int sys_log)
{
  char lockfile[LINE_SIZE +1]; /* should be enough for the user and /tmp and PACKAGE */
  FILE* lockfp;
  /* First findout who the user is.. */
  uid_t  original_real_uid = getuid();
  struct passwd* pw = getpwuid(original_real_uid);
  if (!pw)
    {
      logging(1, verbose, 0, sys_log, 1, "You don't exist, go away!\n");
      exit(1);
    }
  /* then test if theres a file called PACKAGE-USER */
#ifdef HPUX
  sprintf(lockfile, "%s/.%s-%s.lock", getenv("HOME"), PACKAGE, pw->pw_name);
#else
  snprintf(lockfile, LINE_SIZE, "%s/.%s-%s.lock", getenv("HOME"),PACKAGE, pw->pw_name);
#endif
  if((lockfp = fopen(lockfile, "r")) != NULL )
    {
      logging(1, verbose, 0, sys_log, 1, "Removing [%s] as lock file.\n", lockfile);
      fclose(lockfp);	/* not checking should be safe, file mode was "r" */
      unlink(lockfile);
    }                               
  logging(1, verbose, 0, sys_log, 1, "   DONE\n");
  return SUCCESS;
}




int logging(int need_package_info, 
	    unsigned short int verbose, int level, 
	    unsigned short int sys_log, int sys_ready, const char* fmt, ...)
{
  va_list ap;
  int n, size = 100;
  char *p;
  if(verbose > 4 && level < 4)
    return SUCCESS;
  if(verbose > level)
    {
      if(!sys_log)
	{
	  if(need_package_info >= 1)
	    {
	      if(need_package_info > 1)
		printf("\n");
	      printf(PACKAGE ": ");
	    }
	  fflush(stdout);
	  va_start(ap, fmt);
	  vprintf(fmt, ap);
	  fflush(stdout);
	  va_end(ap);
	  return SUCCESS;
	}
      else
	{
	  if(sys_ready)
	    {
	      if ((p = malloc (size)) == NULL)
		{
		  free(p);
		  return FAILURE;
		}
	      while (1)
		{
		  /* Try to print in the allocated space. */
		  va_start(ap, fmt);
#ifdef LINUX
		  n = vsnprintf (p, size, fmt, ap);
#else
		  n = vsprintf(p, fmt, ap);
#endif
		  va_end(ap);
		  /* If that worked, log the string. */
		  if (n > -1 && n < size)
		    {
		      syslog(5, p);
		      free(p);
		      return SUCCESS;
		    }
		  /* Else try again with more space. */
		  if (n > -1)    /* glibc 2.1+ */
		    size = n+1; /* precisely what is needed */
		  else           /* glibc 2.0 */
		    size *= 2;  /* twice the old size */
		  if ((p = realloc (p, size)) == NULL)
		    {		
		      free(p);
		      return FAILURE;
		    }
		}
	    }
	}
    }
  return SUCCESS;
}





/*
 * Get a config file entry. The file must be open, and space must be allocated.
 * Return 0 if an entry was found - or non-zero on error or EOF.
 */


/* 
 * This function shouldn't have any debug printing inside it.. 
 * its all taken care of in read_config_file()
 * If there should be one, it should be checking for verbose of 2 and greater
 */


int get_config_entry (char entry[], char value[], FILE* fp, 
		      unsigned short int verbose, unsigned short int sys_log)
{ 
  char* ptr;
  char buffer[BUFF_SIZE+1];
  char strbuff[NUM_LINES*LINE_SIZE+1] = "";
  while (1)
    {
      if (feof(fp))
	return FAILURE;
      fgets(buffer, BUFF_SIZE, fp);
   /* Sort out lines starting with # (comments) and blank lines */
      if ((buffer[0] == '#') || (buffer[0] == '\0'))
	continue;
      if (!strncmp(buffer, "@@", 2))
	{
	  fgets(buffer, BUFF_SIZE, fp);
	  while(strncmp(buffer, "\\@@", 3))
	    {
	      logging(0, verbose, 1, sys_log, 0, "++++");
	      
	      strcat(buffer, "\n");
	      strncat(strbuff, buffer, strlen(buffer));
	      fgets(buffer, BUFF_SIZE, fp);
	    }
	    strncpy (entry, "pritty", LINE_SIZE);
	    strncpy (value,  strbuff, NUM_LINES*LINE_SIZE);
	    return SUCCESS;
	}
      /* Now find the delimiter and split up in var/value-pairs */
      for (ptr = buffer; *ptr != '\0'; ptr++)
	if (*ptr == '=')
	  {
	    logging(0, verbose, 1, sys_log, 0, "|");
	    *ptr = '\0';
	    strncpy (entry, buffer, LINE_SIZE);
	    strncpy (value, ptr+1, LINE_SIZE);
	    return SUCCESS;
	  }
      /* Hmmm... An invalid line? Ignore it. */
    }
  return FAILURE;
}

/* 
 * to read the config file for all its entries..
 */

int read_config_file(struct setting *conf)
{
  FILE* infile;
  char value[NUM_LINES*LINE_SIZE +2];/* should be enough if the user has alot of spaces befor the '=' */
  char entry[LINE_SIZE +1]; /* should be enough if theres alot of things to say to the argument*/
  struct stat file_stat;
  int tmp;
  logging(1, conf->verbose, 0, conf->sys_log, 1, "Configuring the run of the program.\n");
  /* if this is the first run we better see what file to use */
  if(conf->config_file[0] == '\0')
    {
#ifdef LINUX
      snprintf(conf->config_file, LINE_SIZE -1, "%s/.sigitrc", getenv("HOME"));
#else
      sprintf(conf->config_file, "%s/.sigitrc", getenv("HOME"));
#endif
      stat(conf->config_file, &file_stat);
      conf->date = file_stat.st_ctime;
     
      if (!access(conf->config_file, F_OK))
	{ /* WOW the user had made his own.. */
	  logging(1, conf->verbose, 0, conf->sys_log, 1, "The user had made his own .sigitrc file.\n");
	  if (!access(conf->config_file, R_OK))
	    { /* Lets see if we have read access to it.. */
	      infile = fopen (conf->config_file, "r");
	      if(feof(infile))
		{
		  logging(1, conf->verbose, 0, conf->sys_log, 1, "But theres nothing to read from it.\n");
		  return CONF_ER;
		}
	    }	 
	  else
	    {
	      logging(1, conf->verbose, 0, conf->sys_log, 1, "But there's no read access to it.\n");
	      return CONF_HR;
	    }
	} 
      else
	{ /* DOH we have to read the lame default settings.. */
	      logging(1, conf->verbose, 0, conf->sys_log, 1, "The lUSER havn't made a configuration file.\n");
	  if (!access(DEFAULT_CONF_FILE, F_OK))
	    { /* well then we just use the standard one */
	      strncpy(conf->config_file, DEFAULT_CONF_FILE, LINE_SIZE -1);
	      logging(1, conf->verbose, 0, conf->sys_log, 1, "So we will have to read from the standard.\n");
	      
	      if (!access(DEFAULT_CONF_FILE, R_OK))
		{
		  /* Lets see if we have read access to it.. */
		  infile = fopen (DEFAULT_CONF_FILE, "r");
		  if(feof(infile))
		    {
		      logging(1, conf->verbose, 0, conf->sys_log, 1, "That lame SysAdmin havn't made a standard one.\n");
		      return CONF_ER;
		    }
		}
	      else
		{
		  logging(1, conf->verbose, 0, conf->sys_log, 1, "That lame SysAdmin havn't provided read access to the file.\n");
		  return CONF_ER;
		}
	    }
	  else /* if none of the files exist we must alarm */
	    {
	      logging(1, conf->verbose, 0, conf->sys_log, 1, "That lame SysAdmin havn't made a configuration file.\n");
	      return CONF_MI;
	    }
	}
    }
  else
    {
      /* ###### make som testing to see ####### */
      /* ###### if the .sigitrc file is ####### */
      /* ###### changed, might save the ####### */
      /* ###### time spend here with io ####### */
      stat(conf->config_file, &file_stat);
      /* lets hope st_ctime is the correct value for changes in the file */
      if(file_stat.st_ctime <= conf->date)
	{
	  logging(1, conf->verbose, 0, conf->sys_log, 1, "The config file hasn't changed since last.\n");
	  logging(1, conf->verbose, 0, conf->sys_log, 1, "   DONE\n");
	  return SUCCESS;
	}
      else
	{
	  logging(1, conf->verbose, 0, conf->sys_log, 1, "The config file has been altered since last.\n");
	  conf->date = file_stat.st_ctime;
	  infile = fopen (conf->config_file, "r");
	}
    }
  logging(1, conf->verbose, 0, conf->sys_log, 1, "Reading config.\n");
  logging(0, conf->verbose, 1, conf->sys_log, 0, "\t[+");
  while (SUCCESS == get_config_entry (entry, value, infile, conf->verbose, conf->sys_log))
    {
      /* Go through all configurable variables - sigh */
      if (!strcmp(entry, "sig_file"))
	{
	  if(strlen(value) <= 1)
	    {
#ifdef LINUX
	      snprintf(conf->sig_file, LINE_SIZE -1, "%s/%s", getenv("HOME"), DEFAULT_SIG_FILE);
#else
	      sprintf(conf->sig_file, "%s/%s", getenv("HOME"), DEFAULT_SIG_FILE);
#endif
	    }
	  else
	    {
#ifdef LINUX
	      snprintf(conf->sig_file, LINE_SIZE -1, "%s/%s", getenv("HOME"), value);
#else
	      sprintf(conf->sig_file, "%s/%s", getenv("HOME"), value);
#endif
	    }
	  logging(0, conf->verbose, 1, conf->sys_log, 0,"++++");
	  if(conf->sig_file[strlen(conf->sig_file) -1] == '\n')
	    conf->sig_file[strlen(conf->sig_file) -1] = '\0'; /* to make sure the \n is removed */
	  continue;
	}
      if (!strcmp(entry, "data_file"))
	{
	  if(strlen(value) <= 1)
	    strncpy(conf->data_file, DEFAULT_DATA_FILE, LINE_SIZE -1);
	  else
	    strncpy (conf->data_file, value, LINE_SIZE -1);
    	  logging(0, conf->verbose, 1, conf->sys_log, 0,"++++");
	  if(conf->data_file[strlen(value) -1] == '\n')
	    conf->data_file[strlen(value) -1] = '\0'; /* to make sure the \n is removed */
	  continue;
	}
      if (!strcmp(entry, "--"))
	{
	  tmp = atoi (value);
	  if (tmp > 0)
	    conf->slash = tmp;
	  logging(0, conf->verbose, 1, conf->sys_log, 0,"++++");
	  continue;
	}
      if (!strcmp(entry, "pritty"))
	{
	  if(strlen(value) <= 1)
	    conf->pritty[0] = '\0';
	  else
	    strncpy(conf->pritty, value, NUM_LINES*LINE_SIZE);
	  logging(0, conf->verbose, 1, conf->sys_log, 0,"++++");
	  continue;
	}
      if (!strcmp(entry, "t_line"))
	{
	  strncpy (conf->t_line, value, LINE_SIZE);	  
	  conf->t_line[strlen(value)] = '\0';
	  logging(0, conf->verbose, 1, conf->sys_log, 0,"++++");
	  continue;
	}
      if (!strcmp(entry, "b_line"))
	{
	  strncpy (conf->b_line, value, LINE_SIZE);
	  conf->b_line[strlen(value)]  = '\0';
	  logging(0, conf->verbose, 1, conf->sys_log, 0,"++++");
	  continue;
	}
      /* normaly you would be alarmed, when this happened */
      /* but i decided to continue since its not a big NONO */
      /* to have unknown entries in the configuration, could */
      /* be an error from a previus version, so we tell the user. */
      logging(2, conf->verbose, 0, conf->sys_log, 1, "There was an error while reading config file.\n");
      logging(1, conf->verbose, 0, conf->sys_log, 1, "The entry [%s] is unknown.\n", entry);
      logging(1, conf->verbose, 0, conf->sys_log, 1, "In order to satisfy our listeners, we continue.\n\t");
    }
  fclose(infile);
  logging(0, conf->verbose, 1, conf->sys_log, 0, "+]\n");
  logging(1, conf->verbose, 0, conf->sys_log, 1, "   DONE\n");
  return SUCCESS;
}

