/* sigit.c -- an auto signature creating tool.. 

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

/* $Id: sigit.c,v 0.9 2001/08/13 16:43:00 redhead Exp $ */

#include "sigit.h" // #include "config.h"
#include "files.h"
#include "deamoninit.h"


int print_help(void)
{
 /* should we have nothing to give here, we better just try not to use the parameter */
  printf("\n");
  printf("The normal use of this program would be:\n");   
  printf(PACKAGE " [-a file] [-l file] [-r file] [-u ident] [-cdevV] \n");   
  printf("\t-a  <file>\n");
  printf("\t   This is used, when you want to add a\n");
  printf("\t   signature to your database.\n");
  printf("\t   The desired signature is stored in the\n");
  printf("\t   file parsed with this flag.\n");
  printf("\t-c\n");
  printf("\t   This is used, when the program is to be\n");
  printf("\t   run as a cron job.\n");
  printf("\t-d\n");
  printf("\t   This will launch " PACKAGE " into the background,\n");
  printf("\t   and thereby create a deamon, which will run even\n");
  printf("\t   after you are loged out.\n");
  printf("\t-e\n");
  printf("\t   This will retrieve all signatures from the\n");
  printf("\t   data file, and load them into your $EDITOR\n");
  printf("\t   where you can edit them all.\n");
  printf("\t   Upon exit the program will rebuild the data file\n");
  printf("\t   with the new updated signatures.\n");
  printf("\t-l  <file> \n");
  printf("\t   This will write any signatures in the data file\n");
  printf("\t   to the given file. If the file exist, it will\n");
  printf("\t   overwrite the file.\n");
  printf("\t-r  <file> \n");
  printf("\t   This is used to rebuild the sigs database\n");
  printf("\t   it will erase every former entry in the\n");
  printf("\t   data_file defined in .sigitrc and build a\n");
  printf("\t   new one with every signatures found in the\n");
  printf("\t   file parsed to the program.\n");
  printf("\t   If your using the globaly placed data file,\n");
  printf("\t   then you must be root to use this feature.\n");
  printf("\t-u  <ident> \n");
  printf("\t   This will make sure the signature, with the identifyer\n");
  printf("\t   matching ident, will be removed from the data file.\n");
  printf("\t-v\n");
  printf("\t   This is used, to produce verbose output.\n");
  printf("\t-vv\n");
  printf("\t   produces very verbose output, good for debugging.\n");
  printf("\t   Throw in a couple more -v's and you get a real treat.\n");
  printf("\t-V\n");
  printf("\t   This will print info about the program version.\n\n");
  return SUCCESS;
}


/* Lets fire this baby up.. */


int main(int argc, char* argv[])
{
  struct setting config;
  unsigned short int add = 0, cron = 0, edit = 0, rebuild = 0, deamon = 0;
  int opt; /* to determain which options we are given in argv */
  char edit_f[LINE_SIZE +1]; /* shoudl be enough for ht efile placed in home dir */
  char optarg_element[LINE_SIZE+1 ]; /* to take the sig file for adding.. */
 
  /*
   * to make sure theres no errors here.. 
   * due to any garbage placed in the memory from previus. 
   */
  config.sig_file[0] = '\0';
  config.data_file[0] = '\0';
  config.pritty[0] = '\0';
  config.t_line[0] = '\0';
  config.b_line[0] = '\0';
  config.optarg_element[0] = '\0';
  config.config_file[0] = '\0';
  config.delete_sig[0] = '\0';

  config.sig_n.sig[0] = '\0';
  config.sig_n.ident[0] = '\0';
  config.sig_n.count = htonl(0);

  config.cnt.ver[0] = '\0';
  config.cnt.count = htonl(0);
 
  config.delete = 0;
  config.list = 0;
  config.slash = 0;
  config.deamon_proc = 0;
  config.create_sig = 0;
  config.sys_log = 0;
  config.verbose = 0;
  config.c_num = 0;
  config.date = 0;  

  if(argc > 1)
    { /* if we were run with an argument, lets findout what that was.. */
      while( (opt = getopt(argc, argv, "a:cdehl:r:u:vV")) > 0)
	{
	  switch ((char)opt) 
	    {
	    case 'V':
	      printf(PACKAGE "-" VERSION " (" CALLSIGN ") ["
		     __DATE__ " " __TIME__ "]\n\n"
		     "Copyright (C) 2000-2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>\n"
		     "This is free software; see the source for copying conditions.\n"
		     "There is NO warranty; not even for MERCHANTABILITY or FITNESS\n"
		     "FOR A PARTICULAR PURPOSE, to the extent permitted by law.\n"
		     "Latest version can be obtained from: <" HOMEPAGE ">\n");
  	      return SUCCESS;
	    case 'h':
	      print_help();
	      return SUCCESS;
	    case 'a':
	      strcpy(config.optarg_element, optarg);
	      add = 1;
	      break;
	    case 'c':
	      cron = 1;
	      break;
	    case 'd':
	      deamon = 1;
	      break;
	    case 'l':
	      strcpy(config.optarg_element, optarg);
	      config.list = 1;
	    case 'e':
	      edit = 1;
	      break;
	    case 'r':
	      strcpy(config.optarg_element, optarg);
	      rebuild = 1;
	      break;
	    case 'u':
	      config.delete = 1;
	      strcpy(config.delete_sig, optarg);
	      edit = 1;
	      break;
	    case 'v':
	      config.verbose++;
	      break;
	    default:
	      print_help();
	      return FAILURE;	
	    } 
	}
    }
  if(config.verbose > 5) /* only loones want this much verbose */
    return(_shine_on_you_crazy_diamond_());
  /* lets read what is to be read. */
  if(read_config_file(&config))
    return FAILURE;
  if(!add && !edit && !rebuild && !deamon && cron)
    {
      if(!get_sig(&config))
	{
	  if(!pretty_print(&config))
	    {/* no need to unlink if there is a sigit running in the background */
	      return SUCCESS;
	    }
	  else
	    {
	      return FAILURE;
	    }
	}
      else
	{
	  return FAILURE;
	}
      return SUCCESS;
    } /* or perhaps they want to add a sig */
  if(add && !edit && !rebuild && !deamon && !cron)
    {
      if(access(config.optarg_element, F_OK))
	{
	  logging(1, config.verbose, 0, config.sys_log, 1, "[ERROR] :Cant locate file: %s\n", config.optarg_element);
	  return FAILURE;
	}
      if(add_sig(&config))
	{/* no need to unlink if there is a sigit running in the background */
	  return FAILURE;
	}
      return SUCCESS;
    } /* if not adding, could it be the editing part ? */
  if(!add && edit && !rebuild && !deamon && !cron)
    {
      if(!config.list)
	{
            /* just create the edit file from the normal signature database */
#ifdef LINUX
	  snprintf(config.optarg_element, LINE_SIZE, "/tmp/%s-%d", PACKAGE, getpid());
#else
	  sprintf(config.optarg_element, "/tmp/%s-%d", PACKAGE, getpid());
#endif
	}
      if(edit_data_file(&config))
	{/* no need to unlink if there is a sigit running in the background */
	  logging(1, config.verbose, 0, config.sys_log, 1,  "[ERROR] : There was an error trying to load your desired editor.\n");
	  logging(0, config.verbose, 0, config.sys_log, 1, "\tHave you set the correct $EDITOR environment. ?\n");	  logging(0, config.verbose, 0, config.sys_log, 1, "\tOr is it due to the fact, that [%s] is missing. ?\n", config.data_file);
	  if(!config.list)
	    unlink(config.optarg_element);
	  return FAILURE;
	}
      if(config.list)
	return SUCCESS;
      if(!access(config.data_file, R_OK)) /* atleast the edit file must be pressent.. */
	{
	  /* erase the previus database.. */
	  unlink(config.data_file);
	  /* check if it was removed correct.. */
	  if(!access(config.data_file, F_OK))
	    {
	      logging(1, config.verbose, 0, config.sys_log, 1, ": [ERROR] : The database file couldnt be removed..\n");
	      logging(0, config.verbose, 0, config.sys_log, 1, "\tCould this be due to the fact that you dont have write access to the file ?\n");
	      logging(0, config.verbose, 0, config.sys_log, 1, "\tOr that your a normal user, who is trying to remove the standard data file ?\n");
	      /* no need to unlink the fifo, when were just rebuilding the data file */
	      unlink(config.optarg_element);
	      return FAILURE;
	    }
	  if(!add_sig(&config))
	    {
	      unlink(config.optarg_element);
	      return SUCCESS;
	    }
	  else
	    {
	      unlink(edit_f);
	      return FAILURE;
	    }
	}
      else
	{
	  logging(1, config.verbose, 0, config.sys_log, 1, "[ERROR] : There was an error while updating the data file with\n");
	  logging(0, config.verbose, 0, config.sys_log, 1, "\the changes made, could this be because you havn't got write access to teh data file?\n");
	  unlink(config.optarg_element);
	  return FAILURE;
	}
      
    } /* or perhaps they are rebuilding it */
  if(!add && !edit && rebuild && !deamon && !cron)
    {
      if(!access(config.optarg_element, R_OK)) /* atleast the file must be pressent.. */
	{
	  /* erase the previus database.. */
	  unlink(config.data_file);
	  /* check if it was removed correct.. */
	  if(!access(config.data_file, F_OK))
	    {
	      logging(1, config.verbose, 0, config.sys_log, 1, "[ERROR] : The database file couldnt be removed..\n");
	      logging(0, config.verbose, 0, config.sys_log, 1, "\tCould this be due to the fact that you dont have write access to the file ?\n");
	      logging(0, config.verbose, 0, config.sys_log, 1, "\tOr that your a normal user, who is trying to remove the standard data file ?\n");
	      /* no need to unlink the fifo, when were just rebuilding the data file */
	      return FAILURE;
	    }
	  if(add_sig(&config))
	    return FAILURE;
	}
      else 
	{
	  logging(1, config.verbose, 0, config.sys_log, 1, "[ERROR] : The files needed to rebuild the database couldn't be found.\n");
	  logging(0, config.verbose, 0, config.sys_log, 1, "\tCheck if the '%s' file exist.\n", optarg_element);
	  return FAILURE;
	}
      return SUCCESS;
    }
  if(!add && !edit && !rebuild && !cron)
    {
      /* it must be the trivial case.. we just run as a normal process */
      
      /* initializing the fifo, 
       * it will lock till someone opens 
       * it from the other end for reading
       */

      init_fifo(&config);

      /* install signal handler */
      install_sig_handler(config.verbose, config.sys_log);
      if(deamon)
	{ /* at this point it better be the deamon version poeple want.. */
	  logging(0, config.verbose, 0, config.sys_log, 0, "\n\tLauching " PACKAGE " into the background."
		 "\n\tAll verbose output will be syslog'ed.\n\n");
	  config.sys_log = 1;
	  deamon_init(argv[0], 0);
	}
      create_pid_file(config.verbose, config.sys_log); /* lets hope its the right PID thats written to it.. */
      /*
	At this point we must have been called regularly, with no arguments. 
	Meaning we should just be run like a deamon..
      */
      while(1)
	{
	  if(!get_sig(&config))
	    {
	      if(pretty_print(&config))
		{
		  unlink_fifo(&config);
		  return FAILURE;
		}
	    }
	  else
	    {
	      unlink_fifo(&config);
	      return FAILURE;
	    }
	  /*
	   * no need to sleep the delay, 
	   * now that the fifo will lock the process. 
	   *
	   * sleep(config.delay);
	   *	  
	   * Instead we only sleep for a brief moment.	   
	   * to make sure the process reading the fifo will 
	   * empty it, befor we take hold of it again.
	   */
	  
	  sleep(1);
	  /* if the user has made changes to the configuration,
	   * while we were waiting for the fifo to be activated.
	   * we better reread it.
	   */
	  if(read_config_file(&config))
	    {
	            unlink_fifo(&config);
		    return FAILURE;
	    }
	}
      unlink_fifo(&config);
      return SUCCESS;
    } 
  else
    { /* there must have been a failure. And we better let folks know.. */
      logging(1, config.verbose, 0, config.sys_log, 1, "[ERROR] : The program can't do multiple things at the same time.\n");
      logging(1, config.verbose, 0, config.sys_log, 1, "[ERROR] : When adding, editing, rebuilding signatures, or running as a cron job.\n");
      return FAILURE;
    } 
  return SUCCESS;
}






