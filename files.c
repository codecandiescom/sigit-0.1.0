/* files.c  --  The file holding all i/o functions required by Sigit..
   
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

/* $Id: files.c,v 0.10 2001/08/13 16:43:00 redhead Exp $ */

#include "files.h" // #include "config.h"

/* just a small function used to create random numbers */

inline unsigned long int get_rand (unsigned long int max, unsigned short int verbose, unsigned short sys_log) 
{
  size_t res;
  time_t tim, c_start;
  if(max <= 0)
    return 0;
  logging(1, verbose, 4, sys_log, 0, "Who wants a Lesotho ?\n");
  c_start = clock();
  srandom(c_start);
  logging(2, verbose, 2, sys_log, 0, "[get_rand] Feeding srand() with bouderies, max: %d.\n", max);
  time(&tim);
  srandom(tim);
  logging(1, verbose, 2, sys_log, 0, "[get_rand] Charging srandom() to give us our random number.\n\t");
  res = random() % max;
  logging(0, verbose, 2, sys_log, 0, "*%d|%d*", (int) max, (int) res);
  logging(1, verbose, 3, sys_log, 0, "[get_rand] Returning with a normal inbound number.\n\t");
  return res;
}

/*
 * this is a function, which will take a integer, 
 * and from that make a unique hexa number. 
 * it is intentively to be called when you realy need a unique number
 * the buff should hold atleast 
 */
inline void create_hex(char buff[], unsigned short int verbose, unsigned short sys_log)
{
   /* creating a number which you cant
   * recreate in a million years or so.. 
   * unless the time on your computer 
   * is realy fucked.. 
   */
  static size_t counter = 0;
  size_t time_stamp = time(NULL);
  logging(2, verbose, 3, sys_log, 0, "Can time be NULL ??\n\t");
  counter++;
  logging(1, verbose, 2, sys_log, 0, "[create_hex] counter is: %d\n", counter);
  time_stamp += counter;
  logging(1, verbose, 2, sys_log, 0, "[create_hex] time_stamp is: %d\n\t", time_stamp);
  sprintf(buff, "%X", time_stamp);
  logging(0, verbose, 1, sys_log, 0, "-{%s}-",buff);
  logging(1, verbose, 4, sys_log, 0, "- ### Ceci nést pas un error. ### -\n");
}


/* a function to add all the signatures you need */
int add_sig(struct setting *conf)
{
  FILE* in_file;
  char buffer[LINE_SIZE +1];
  unsigned long int total = 0;
  unsigned int number, identifyer;
  conf->sig_n.ident[0] = '\0';
  logging(1, conf->verbose, 0, conf->sys_log, 1, ": Adding sig from file [%s].\n", conf->optarg_element);
  logging(0, conf->verbose, 1, conf->sys_log, 0, "\t[+");
  
  /*
    first we have the routine to read in the signature.. 
    (lets hope theres enough room for it in the NUM_LINES*LINE_SIZE)
     Lets hope fopen will return NULL if the file dosnt exist.
  */
  if( !(in_file = fopen(conf->optarg_element, "r")))
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Couldn't open file.\n");
      return CONF_MI;
    }
  /* else lets peek in the file, to see if it contains alot of signatures.. */
  if(!fgets(buffer, LINE_SIZE, in_file))
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : The file is empty.\n");
      return NO_READ;
    }
  if(buffer[0] == '@' && buffer[1] == '@')
    { 
      if(sscanf(buffer, "@@[Number: %d]--[Identifier: %X]@@\n", &number , &identifyer) == 2)
	sprintf(conf->sig_n.ident, "%X", identifyer);
      while(!feof(in_file))
	{
	  if(!fgets(buffer, LINE_SIZE, in_file))
	    {
	      logging(2, conf->verbose, 2, conf->sys_log, 0, "[add_sig] there was nothing more to read.\n");
	      continue; /* next time in the while this will get cought */
	    }
	  total += strlen(buffer); /* first time will allways have enough room */
	  logging(2, conf->verbose, 2, conf->sys_log, 0, "[add_sig] The total is: %d.\n", total);
	  strcpy(conf->sig_n.sig, buffer); /* we hope this is the first sig line */
	  logging(2, conf->verbose, 3, conf->sys_log, 0, "[add_sig] The buffer is containing {%s}.\n", buffer);
	  while(!feof(in_file))
	    { /* we recon theres gonna be no error once we made the first successfull read */
	      fgets(buffer, LINE_SIZE, in_file);
	      logging(0, conf->verbose, 1, conf->sys_log, 0, "+");
	      if(buffer[0] == '@' && buffer[1] == '@')
		{/* now we must assume, that the entire sig is gathered in sign */
		  if(set_sig(conf))
		    {
		      fclose(in_file);
		      return FAILURE;
		    }
		  conf->sig_n.sig[0] = '\0';
		  if(sscanf(buffer, "@@[Number: %d]--[Identifier: %X]@@\n", &number , &identifyer) == 2)
		    sprintf(conf->sig_n.ident, "%X", identifyer);
		  else
		    conf->sig_n.ident[0] = '\0';
		  total = 0;
		  continue; /* lets skip the rest of the loop */
		}
	      if((total += strlen(buffer)) < (NUM_LINES*LINE_SIZE -1))
		{
		  strncat(conf->sig_n.sig, buffer, strlen(buffer));
		}
	      else
		{
		  logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : The signature was longer than %d chars.\n", 
			NUM_LINES*LINE_SIZE);
		  fclose(in_file);
		  return EXCEEDED;
		}
	    }
	  total = 0;
	  logging(0, conf->verbose, 1, conf->sys_log, 0, "|");
	}
      logging(0, conf->verbose, 1, conf->sys_log, 0, "+]\n");
      logging(1, conf->verbose, 0, conf->sys_log, 1, "   DONE\n");
      fclose(in_file);
    }
  else
    { /* we only have one sig, so lets just put that in there.. */
      strncpy(conf->sig_n.sig, buffer, strlen(buffer));
      logging(2, conf->verbose, 3, conf->sys_log, 0, "[add_sig] The buffer is containing {%s}.\n", buffer);
      conf->sig_n.sig[strlen(buffer)] = '\0';
      while (fgets(buffer, LINE_SIZE, in_file) > 0)
	{
 	  logging(0, conf->verbose, 1, conf->sys_log, 0, "+");
 	  total += strlen(buffer);
	  logging(2, conf->verbose, 2, conf->sys_log, 0, "[add_sig] The total is: %d.\n", total);
	  if(total < NUM_LINES*LINE_SIZE)
	    {
	      strncat(conf->sig_n.sig, buffer, strlen(buffer));
	      continue;
	    }
	  else
	    {
	      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] :The signature was longer than %d chars.\n", 
		      NUM_LINES*LINE_SIZE);
	      fclose(in_file);
	      return EXCEEDED;
	    }
	  logging(0, conf->verbose, 1, conf->sys_log, 0, "|");
	}
      if(set_sig(conf))
	{
	  fclose(in_file);
	  return FAILURE;
	}
      logging(0, conf->verbose, 1, conf->sys_log, 0, "+]\n");
      logging(1, conf->verbose, 0, conf->sys_log, 1, "   DONE\n");
      fclose(in_file);
    }
  return SUCCESS;
}


/* well isnt this fun, lets get a signature by the number we now know.. */
int get_sig(struct setting *conf)
{
  FILE* out_file;

  logging(1, conf->verbose, 0, conf->sys_log, 1, "Getting sig from file: [%s]\n", conf->sig_file);
  logging(0, conf->verbose, 1, conf->sys_log, 0, "\t[+");

  if(!(out_file = fopen(conf->data_file, "rb")))
    { /* at this point we should be at the top of the file */
      /* some error happened */
      logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can not open file.\n");
      return CONF_MI;
    }
  if(!fread((void*) &(conf->cnt), sizeof(conf->cnt),1 ,out_file))
    {
      /* some error happened */
      logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : The file is empty.\n");
      fclose(out_file);
      return COUNT_MISS;
    }
  logging(0, conf->verbose, 1, conf->sys_log, 0, "+>%d<+", (int) ntohl(conf->cnt.count));
  /* now we know the max count, so lets get the random number for that */
  conf->c_num = get_rand(ntohl(conf->cnt.count) -2, conf->verbose, conf->sys_log);
  logging(0, conf->verbose, 1, conf->sys_log, 0, "+>%d<+", (int) conf->c_num);
  /* the lets search for that signature */
  if(fseek(out_file, conf->c_num*sizeof(conf->sig_n), SEEK_CUR) != 0)
    {
      logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't seek in file.\n");
      fclose(out_file);
      return SEEK_ERR;
    }
  /* lets read that signature we have now */
  if(!fread((void*) &(conf->sig_n), sizeof(conf->sig_n),1, out_file))
    {
      logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : The file is empty.\n");
      fclose(out_file);
      return NO_READ;
    }
  logging(0, conf->verbose, 1, conf->sys_log, 0, "+]\n");
  logging(1, conf->verbose, 0, conf->sys_log, 1, "   DONE\n");
  fclose(out_file);
  return SUCCESS;
}

/* 
 * this function is very simmular to the get_sig() 
 * it just writes in a loop what's in the database 
*/
	
int edit_data_file(struct setting *conf)
{
  char test_temp[NUM_LINES*LINE_SIZE +2], exec_line[LINE_SIZE +1], *editor; 
  size_t i = 1, pid, xpid;
  int waiter;
  FILE* in_file;
  FILE* out_file;
  logging(1, conf->verbose, 0, conf->sys_log, 1, "Getting sig from file: [%s].\n", conf->data_file);
  logging(1, conf->verbose, 0, conf->sys_log, 1, "Writing edit file  ");
  logging(0, conf->verbose, 1, conf->sys_log, 0, " [+");
  if(!(in_file = fopen(conf->data_file, "rb")))
    { /* at this point we should be at the top of the file */
      /* some error happened */
      logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can not open data file.\n");
      return CONF_MI;
    }
  if(!(out_file = fopen(conf->optarg_element, "w")))
    {
      /* at this point we should be at the top of the file */
      /* some error happened */
      logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can not open editing file: [%s]\n", conf->optarg_element);
      return CONF_MI;
    }
  if(!fread((void*) &(conf->cnt), sizeof(conf->cnt),1 ,in_file))
    {
      logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't read from file: [%s]\n", conf->data_file);
      /* some error happened */
      fclose(in_file);
      fclose(out_file);
      return COUNT_MISS;
    }
  logging(0, conf->verbose, 1, conf->sys_log, 0, "+>Total: %d<+", (int) ntohl(conf->cnt.count));
  /* now we know the max count, so lets get the random number for that */
  for(; i <= ntohl(conf->cnt.count) && !feof(in_file); i++)
    { 
      if(!fread((void*) &(conf->sig_n), sizeof(conf->sig_n),1, in_file))
	{
	  logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't read from file: [%s]\n", conf->data_file);
	  fclose(in_file);
	  fclose(out_file);
	  return NO_READ;
	}
      if(conf->delete && !strcasecmp(conf->sig_n.ident, conf->delete_sig))
	{
	  logging(2, conf->verbose, 0, conf->sys_log, 0, "Deleting Sig [%s]\n\t", conf->sig_n.ident);
	  if(conf->verbose > 4)
	    Mostly_Harmless(conf);
	  continue; /* hmm this was the signature we wanted to delete.. */
	}
      logging(0, conf->verbose, 1, conf->sys_log, 0, "+>%d<+", (int) i);
      if(0 < sprintf(test_temp,"%s", conf->sig_n.sig)) /* this is ugly.. totaly idiotic.. */
	{ /* but it is nesssary, if we want to have the correct number of signatures..*/
	  /* non the less.. it seems to remove the very last signature from the database */
	  if(!fprintf(out_file, "@@[Number: %d]--[Identifier: %s]@@\n", 
		      ntohl(conf->sig_n.count), conf->sig_n.ident))
	    {
	      logging(2, conf->verbose, 1, conf->sys_log, 0, "[ERROR] : Can't write to editing file.\n");
	      fclose(in_file);
	      fclose(out_file);
	      return FAILURE;
	    }
	  if(!fprintf(out_file, "%s", conf->sig_n.sig))
	    {
	      logging(2, conf->verbose, 1, conf->sys_log, 0, "[ERROR] : Can't write to editing file.\n");
	      fclose(in_file);
	      fclose(out_file);
	      return FAILURE;
	    }
	}
    }
  /* here we must be at the end of the out_put file.. Thus we create the last thing */
  if(!fprintf(out_file, "@@[O'lack atlast we've reached The End.]@@\n"))
    {
      logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't write to editing file.\n");
      fclose(in_file);
      fclose(out_file);
      return FAILURE;
    }
  logging(0, conf->verbose, 1, conf->sys_log, 0, "+]\n");
  logging(1, conf->verbose, 0, conf->sys_log, 1, "   DONE\n");
  fclose(in_file);
  fclose(out_file);
  if(conf->list || conf->delete)
    return SUCCESS;
  if ((!(editor = getenv("VISUAL")))
      && (!(editor = getenv("EDITOR")))
      ) {
    editor = DEFAULT_EDITOR;
  }
  
  switch (pid = fork()) {
  case -1:
    logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Creating fork()\n");
    return FAIL_FO;
  case 0:
    /* child */
    if (chdir("/tmp") < 0) {
      logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Couldn't change dir to /tmp\n");
      return FAILURE;
    } 
    if (strlen(editor) + strlen(conf->optarg_element) + 2 >= LINE_SIZE) {
 	  logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : The file length with the editor exceeded buffer length.\n");
	  return EXCEEDED;
    }
    sprintf(exec_line, "%s %s", editor, conf->optarg_element);
    execlp(_PATH_BSHELL, _PATH_BSHELL, "-c", exec_line, NULL);
    logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Couldn't execute editor.\n");
    logging(1, conf->verbose, 4, conf->sys_log, 0, "Powered by: cat - ^D\n\t");
    logging(0, conf->verbose, 4, conf->sys_log, 0, "\t- Because editors are for whimps.\n");
    return FAILURE;
    /*NOTREACHED*/
  default:
    /* parent */
    break;
  }
  
  /* parent */
  xpid = wait(&waiter);
  if (xpid != pid) {
    logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : wrong PID (%d != %d) from \"%s\"\n", (int) xpid, (int) pid, editor);
    return FAILURE;
  }
  if (WIFEXITED(waiter) && WEXITSTATUS(waiter)) {
    logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : \"%s\" exited with status %d\n",
	    editor, WEXITSTATUS(waiter));
    return FAILURE;
  }
  if (WIFSIGNALED(waiter)) {
    logging(2, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : \"%s\" killed; signal %d (%score dumped)\n",
	    editor, WTERMSIG(waiter),
	    WCOREDUMP(waiter) ?"" : "no ");
    return FAILURE;
  }
  return SUCCESS;
}


/* 
   set sig will open the sigdatabase and add the signature to it..
*/

int set_sig(struct setting *conf)
{
  char touch_cmd[LINE_SIZE];
  FILE* out_file;
  int was_there = 1;
  conf->cnt.count = htonl(0);
  strcpy(conf->cnt.ver, VERSION);
  logging(0, conf->verbose, 1, conf->sys_log, 0, "<");
  if(strlen(conf->sig_n.sig) > NUM_LINES*LINE_SIZE)
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] :The signature was longer than %d chars.\n", 
	      NUM_LINES*LINE_SIZE);
      return EXCEEDED;
    }
  if(conf->sig_n.ident[0] == '\0')
    create_hex(conf->sig_n.ident, conf->verbose, conf->sys_log);
  if(access(conf->data_file, F_OK))
    { /* it must be the very first signature.. hence no file */
      logging(2, conf->verbose, 3, conf->sys_log, 0, "[set_sig] The signature file never existed, creating one.\n");
      sprintf(touch_cmd, "touch %s", conf->data_file);
      system(touch_cmd);
      was_there = 0;
    }
  logging(0, conf->verbose, 1, conf->sys_log, 0, "-");
  
  /* if the file allready exists, then there must be atleast one sig in it */
  /* we now return to the start of the file */
   if(!(out_file = fopen(conf->data_file, "rwb+")))
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR : Cant open file {%s} for adding a sig.\n", conf->data_file);
      return FAILURE;
    }
  if(fseek(out_file, 0, SEEK_SET) != 0)
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Cant seek in file.\n");
      fclose(out_file);
      return SEEK_ERR;
    }
  if(was_there)
    if(!fread((void*) &(conf->cnt), sizeof(conf->cnt),1, out_file))
      {
	logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't read from file. {%s}\n", conf->data_file);
	fclose(out_file);
	return NO_READ;
      }
  logging(0, conf->verbose, 1, conf->sys_log, 0, "-]%d[-", ntohl(conf->cnt.count));
  
  /* then we increase that, and stores the new value to the file */
  if(fseek(out_file, 0, SEEK_SET) != 0)
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't seek in file.\n");
      fclose(out_file);
      return SEEK_ERR;
    }
  conf->cnt.count = htonl(ntohl(conf->cnt.count) + 1);
  logging(0, conf->verbose, 1, conf->sys_log, 0, "-[%d]-", ntohl(conf->cnt.count));
  if(!fwrite((void*) &(conf->cnt), sizeof(conf->cnt),1, out_file))
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't write count to file {%s}.\n", conf->data_file);
      fclose(out_file);
      return WRITE_ERR;
    }
  conf->sig_n.count = conf->cnt.count;
  /* now we can search to the end of the file, and store the new signature */
  if(fseek(out_file, 0, SEEK_END) != 0)
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't seek in file.\n");
      fclose(out_file);
      return SEEK_ERR;
    }
  logging(0, conf->verbose, 1, conf->sys_log, 0, "-");
  if(!fwrite((void*) &(conf->sig_n), sizeof(conf->sig_n),1, out_file))
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't write signature to file. {%s}\n", conf->data_file);
      fclose(out_file);
      return WRITE_ERR;
    }
  logging(0, conf->verbose, 1, conf->sys_log, 0, ">");
  fclose(out_file);
  return SUCCESS;
}



int buffer_magic(char ch, char buffer[], 
		 int sig_write, 
		 int *l_count, 
		 int *prity_sig_count, 
		 int *line_count, 
		 int *buffer_count)
{
  FILE  *fp;
#ifdef LINUX
  int upt = 0, temp = 0, days = 0, hours = 0, min = 0;
#endif
  int counter = 0;
  char uname_buffer[LINE_SIZE +1], tmp[10];
  struct utsname unamebuf;
  struct tm *tm_set;
  time_t tim, t;
  /* to make sure, the garbage in memory
     isnt causing any problems.*/
  uname_buffer[0] = '\0';
  tmp[0] = '\0';
  uname(&unamebuf);  
  switch(ch)
    {
    case('t'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      break;
    case ('H'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      strcpy(uname_buffer, HOMEPAGE);
      uname_buffer[strlen(HOMEPAGE)] = '\0';
      *line_count = *line_count + strlen(HOMEPAGE);
      break; 
    case ('M'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      sprintf(uname_buffer, "%s", unamebuf.machine);
      uname_buffer[strlen(unamebuf.machine)] = '\0';
      *line_count = *line_count + strlen(unamebuf.machine);
      break;
    case('N'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      sprintf(uname_buffer, "%s", unamebuf.nodename);
      uname_buffer[strlen(unamebuf.nodename)] = '\0';
      *line_count = *line_count + strlen(unamebuf.nodename);
      break;
    case ('P'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      strcpy(uname_buffer, PACKAGE);
      uname_buffer[strlen(PACKAGE)] = '\0';
      *line_count = *line_count + strlen(PACKAGE);
      break; 
    case ('R'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      sprintf(uname_buffer, "%s", unamebuf.release);
      uname_buffer[strlen(unamebuf.release)] = '\0';
      *line_count = *line_count + strlen(unamebuf.release);
      break; 
    case ('S'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      sprintf(uname_buffer, "%s", unamebuf.sysname);
      uname_buffer[strlen(unamebuf.sysname)] = '\0';
      *line_count = *line_count + strlen(unamebuf.sysname);
      break;
    case ('T'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      tim = time(&t);
      tm_set = localtime(&tim);
      sprintf(uname_buffer, "%.2d:%.2d", tm_set->tm_hour, tm_set->tm_min);
      *line_count = *line_count + strlen(uname_buffer);
      break;
    case ('U'):
      /* I know the HP_UX havn't gotten the same /proc structure as Linux
	 There might be other architechtures suffering of the same*/
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;

#ifdef LINUX
      fp = fopen("/proc/uptime", "r");
      fscanf(fp, "%d %d", &upt, &temp);
      fclose(fp);
      days = upt/86400;
      upt = upt%86400;
      hours = upt/3600;
      upt = upt%3600;
      min = upt/60;
      tim = time(&t);
      tm_set = localtime(&tim);
      if(days >0)
	sprintf(uname_buffer, "%.2d:%.2d up %.2d days, ", tm_set->tm_hour, tm_set->tm_min, days);
      else
	sprintf(uname_buffer, "%.2d:%.2d up ", tm_set->tm_hour, tm_set->tm_min);
      sprintf(tmp, "%.2d:%.2d", hours, min);
      strcat(uname_buffer, tmp);
      *line_count = *line_count + strlen(uname_buffer);
#endif

      break;
    case ('V'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      strcpy(uname_buffer, VERSION);
      uname_buffer[strlen(VERSION)] = '\0';
      *line_count = *line_count + strlen(VERSION);
      break; 
    case ('%'):
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      strcpy(uname_buffer, "%");
      uname_buffer[1] = '\0';
      *line_count = *line_count +1;
      break;
    default:
      if(sig_write == 0)
	*l_count = *l_count +1;
      else
	*prity_sig_count = *prity_sig_count +1;
      break;
    }
  for(counter = 0; 
      *buffer_count < *line_count && 
	*buffer_count < LINE_SIZE && 
	uname_buffer[counter] != '\0'; 
      *buffer_count = *buffer_count +1, counter++)
      buffer[*buffer_count] = uname_buffer[counter];
     
    
  return SUCCESS;
}
/* 
 * This is the pretty printer.. 
 *
 */

int pretty_print(struct setting *conf)
{
  FILE *out_file;
  char ch, line_buff[LINE_SIZE +3], buffer[LINE_SIZE +3];
  int line_count = 0, last_line = 0, sig_write = 0, buffer_count = 0, longest_line = 0;  
  int prity_count = 0, sig_count = 0, l_count = 0, one_two = 0, started = 0, spaces, i;
  
  logging(1, conf->verbose, 0, conf->sys_log, 1, "Opening the fifo: [%s] for writing.\n", conf->sig_file);
  logging(0, conf->verbose, 1, conf->sys_log, 0, "\t[+");
  /* make the normal required "-- \n" as the first line of a sig. */
  if(!(out_file = fopen(conf->sig_file, "w")))
    {
      logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't open file {%s} for writing.\n", conf->sig_file);
      /* some error happened */
      return CONF_MI;
    }
  if(conf->slash)
    { /* if the user told, that the system dosnt put it in */
      if(!fwrite("-- \n", 4, 1, out_file))
	{
	  logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't write [-- \\n] to file {%s}.\n", conf->sig_file);
	  fclose(out_file);
	  return WRITE_ERR;
	}
      logging(0, conf->verbose, 2, conf->sys_log, 0, "++W -- \\n W++");
    }
  logging(0, conf->verbose, 2, conf->sys_log, 0, "++W Signature W++");
  logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] Preparing to write signature number: %d.\n", conf->sig_n.count);
  if(strlen(conf->t_line) > 2)
    {
      logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] Preparing to write t_line {%s} if it will fit.\n", conf->t_line);
      strncpy(line_buff, conf->t_line, LINE_SIZE);
      /* #### very dangerous, counting to -1, but if we should allready #### */
      /* #### print one line, then the counter will be 0 for the first  #### */
      /* ####               run through the prity buffer.               #### */
      prity_count--;
    }
  else
    { /* we need to avoid the sig_count to be -1, so we have the started flag */
      logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] No t_line to write, altering started flag.\n");
      started = 1;
      sig_write = 1;
      one_two++;
    }
  for( ; sig_write != 4 ; )
    { 
      logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] Currently using sig_write setting {%d} in the process.\n", sig_write);
      
      /*
       * sig_write == 0  writing either top line or bottom line
       * sig_write == 1  writing presetting
       * sig_write == 2  writing signature
       * sig_write == 3  done writing signature
       * sig_write == 4  We've finished bottom line and its time to get out.
       */
      
      /* if its either the top line or the pre setting
	 we have the conversion chars to check */
      if(sig_write <= 1)
	{
	  logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] Checking conversion chars.\n");
	  if(sig_write == 0)
	    ch = line_buff[l_count];
	  else
	    ch = conf->pritty[prity_count];
 	  if(ch == '%')
 	    {
	      if(sig_write == 0)
		l_count++;
	      else
		prity_count++;
	      if(sig_write == 0)
		ch = line_buff[l_count];
	      else
		ch = conf->pritty[prity_count];
	      
	      logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] Calling buffer_magick() with [ch: %c, sig_write: %d, l_count: %d, prity_count: %d, line_count: %d, buffer_count: %d]\n",ch, sig_write, l_count, prity_count, line_count, buffer_count);
	      
	      buffer_magic(ch, buffer, 
			   sig_write, 
			   &l_count, 
			   &prity_count, 
			   &line_count, 
			   &buffer_count);
	      
	      logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] Returning from buffer_magick() with [ch: %c, sig_write: %d, l_count: %d, prity_count: %d, line_count: %d, buffer_count: %d]\n",ch, sig_write, l_count, prity_count, line_count, buffer_count);
	      
	    }
	  if(sig_write == 0)
	    {
	      if(line_buff[l_count] != '\0' && 
		 line_count < LINE_SIZE && 
		 buffer_count < LINE_SIZE)
		{
		  if(line_buff[l_count] == '\n')
		    {
		      l_count++;
		      sig_write = 4;
		    }	      
		  else
		    {
		      buffer[buffer_count++] = line_buff[l_count++];
		      line_count++;
		      if(line_count >= LINE_SIZE)
			sig_write = 4;
		    }
		}
	      else 
		sig_write = 4;
	    }
	  else
	    {
	      if(strlen(conf->pritty) > 1)
		{ /* as long as theres something in the pritty, we just copy it to our buffer */
		  if(conf->pritty[prity_count] != '\n' && 
		     conf->pritty[prity_count] != '\0' && 
		     line_count < LINE_SIZE) 
		    {
		      buffer[buffer_count++] = conf->pritty[prity_count++];
		      line_count++;
		    } /* if its a \n we've reached its time to write the signature */
		  if(conf->pritty[prity_count] == '\n')
		    {
		      buffer[buffer_count++] = ' ';
		      buffer[buffer_count++] = ' ';
		      prity_count++;
		      last_line = line_count;
		      sig_write = 2;
		    } 
		  /* if we've reached this part, and yet there was more signature to write
		     then we just fill blanks for the remaining preset, so there won't be
		     some odd mengling of the signature.. */
		  
		  logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] Filling the preset space with ' ' to accomplish smooth transformation.\n");
		  
		  if(conf->pritty[prity_count] == '\0')
		    {
		      line_count = 0;
		      for(i = 0; i <= last_line; i++)
			{ 
			  buffer[buffer_count++] = ' ';
			  line_count++;
			}
		      sig_write = 2;
		      /* making a slow ajustment to the normal line_size */
                      if(line_count <= 2)
                        last_line = 0;
                      else
			last_line = (line_count -2);
		    }
		} 
	      else
		sig_write = 2;
	    }  
	} /* now its time to write the signature part */
      if(sig_write == 2)
	{

	  if(conf->sig_n.sig[sig_count] != '\0' && 
	     line_count < LINE_SIZE)
	    { /* the '\n' will be added later */
	      if(conf->sig_n.sig[sig_count] == '\n')
		{
		  sig_count++;
		  sig_write = 3;
		}	      
	      else
		if(conf->sig_n.sig[sig_count] == '%' && conf->sig_n.sig[sig_count +1] == 'r')
		  {		  
		    if(!sig_count)
		      longest_line = LINE_SIZE -2;
		    else
		      if(conf->sig_n.sig[sig_count -1] != '\n')
			goto out;
		    sig_count += 2;
		    
		    /* removing any leading spaces in the signature */
		    while(conf->sig_n.sig[sig_count] == ' ' || conf->sig_n.sig[sig_count] == '\t')
		      sig_count++;
		    
		    for(spaces = 0; 
			(conf->sig_n.sig[sig_count + spaces] != '\n' &&
			 conf->sig_n.sig[sig_count + spaces] != '\0') &&
			  spaces < LINE_SIZE; spaces++)
		      /* just count what we need.. */ ;
		    
		    /* setting the spaces needed, to right place it */
		    if(longest_line - spaces > 0)
		      for(i = 0; i < longest_line - spaces - last_line && 
			    line_count + spaces < LINE_SIZE; i++)
			{
			  buffer[buffer_count++] = ' ';
			  line_count++;
			}
		    
		    
		  }
		else
		  { /* we just copy it into our buffer. The easiest way */
		  out:
		    buffer[buffer_count++] = conf->sig_n.sig[sig_count++];
		    line_count++;
		  }
	    }
	  else 
	    sig_write = 3;
	}/* thi is reached, if we have a full buffer to write.. */
      if(sig_write >= 3 || line_count >= LINE_SIZE)
	{
	  /* #### In order to correct the -1 in the prity_count #### */
	  if(prity_count == -1)
	    prity_count++;
	  else
	    {
	      if(conf->pritty[prity_count] != '\0')
		prity_count++;
	    } /* was it the bottom line we were writing ? */
	  if(sig_write == 4)
	    {
	      started = 1;
	      one_two++;
	      line_buff[0] = '\0';
	    }
	  else
	    {
	   logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] Finding the last white space, where we can split the line.\n");

	      /* it must have been part of the signature and preset, 
		 so if were in the middle of a word, we better go back to a white space */
	      if(conf->sig_n.sig[sig_count] != '\0' && 
		 conf->sig_n.sig[sig_count] != '\n' && 
		 conf->sig_n.sig[sig_count] != ' ' && 
		 conf->sig_n.sig[sig_count] != '\t' && 
		 started &&
		 conf->sig_n.sig[sig_count -1] != '\n')
		for(; conf->sig_n.sig[sig_count] != ' ' && 
		      conf->sig_n.sig[sig_count] !='\n' && 
		      conf->sig_n.sig[sig_count] !='\t' && 
		      conf->sig_n.sig[sig_count] != '\0'; sig_count--, buffer_count--)
		  /* lets just count.. */ line_count--;
	      else /* it was the first line, so its OK to start searching for white spaces */
		started = 1;
	    }/* if theres nothing in the buffer, then why bother filling more */	
	  if(!buffer_count &&
	      conf->sig_n.sig[sig_count] == '\0' && 
	      conf->pritty[prity_count] == '\0')
	    sig_write = 4;
	  else
	    {/* write what we got so far */
	      if(started && conf->sig_n.sig[sig_count] == '\n')
		{
		  sig_count++;
		  /* this might be where the buffer[buffer_count] = '\n'; should be */
		}
	      buffer[buffer_count++] = '\n';
	      buffer[buffer_count] = '\0';
	      buffer_count = 0;

	      logging(2, conf->verbose, 3, conf->sys_log, 0, "[pretty_print] Writing signature part {%s} to the fifo.\n", buffer);

	      if(!fwrite(buffer, strlen(buffer), 1, out_file))
		{
		  logging(1, conf->verbose, 0, conf->sys_log, 1, "[ERROR] : Can't write signature to file.\n");
		  fclose(out_file);
		  return WRITE_ERR;
		}/* prepare for the writing of presetting next */
	      sig_write = 1;
	      if(sig_count > 1 && longest_line < line_count)
		longest_line = line_count;
	      line_count = 0;  
	    }
	  logging(0, conf->verbose, 1, conf->sys_log, 0, "++");
	  /* hmmm are we done writing the signature and should we select bottom line now ? */
	  if(conf->pritty[prity_count] == '\0' && conf->sig_n.sig[sig_count] == '\0')
	    {
	      if(line_buff[0] == '\0' && one_two > 1)
		sig_write = 4; /* if we allready are done with the bottom line we exit */
	      if(one_two == 1)
		{ /* if there has been just one (indicating top line ) go on */
		  if(strlen(conf->b_line) > 1) 
		    {/* if theres somethign inteh bottom lie we can write it */
		      sig_write = 0;
#ifdef LINUX
		      strncpy(line_buff, conf->b_line, LINE_SIZE);
#else
		      strcpy(line_buff, conf->b_line);
#endif
		      l_count = 0;
		    }
		  else /* else we might aswell exit.. */
		    sig_write = 4;
		}
	    }	
	}
    }
  logging(0, conf->verbose, 1, conf->sys_log, 0, "+]\n");
  logging(1, conf->verbose, 0, conf->sys_log, 1, "   DONE\n");
  logging(1, conf->verbose, 4, conf->sys_log, 0, "So Long and Thanks for all the Fish.\n");
  fclose(out_file);
  return SUCCESS;
}


/* hmmm lets hope people won't read this far in the file.. */

int _shine_on_you_crazy_diamond_(void){return(printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",10,34,84,104,101,32,98,97,110,100,32,105,115,32,106,117,115,116,32,102,97,110,116,97,115,116,105,99,44,10,32,116,104,97,116,32,105,115,32,114,101,97,108,121,32,119,104,97,116,32,73,32,116,104,105,110,107,46,10,32,45,79,104,32,98,121,32,116,104,101,32,119,97,121,44,32,119,104,105,99,104,32,111,110,101,32,105,115,32,80,105,110,107,32,63,34,10,9,45,32,45,32,80,105,110,107,32,70,108,111,121,100,32,45,32,72,97,118,101,32,65,32,67,105,103,97,114,46,10,10));}

