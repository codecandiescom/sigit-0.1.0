Name: sigit
Version: 0.1.0
Release: 1
Summary: a dynamic signature generator for e-mail and news
Copyright: GPL
Group: Amusements
URL: http://www.redhead.dk
Source: http://redhead.dyndns.dk/download/stable/%{name}-%{version}.tar.gz
BuildRoot: /tmp/%{name}-%{version}
Packager: Kenneth 'Redhead' Nielsen <redhead@diku.dk>
Vendor: Kenneth 'Redhead' Nielsen <redhead@diku.dk>

#
# $Id: sigit.spec,v 0.8 2001/08/13 16:43:00 redhead Exp $
#
# If I ever find out a way, to combine the changelog with the log info submitted
# when you use CVS, then I would skip this next part, and just let CVS take controle
# of it.

%changelog

* Sat Sep 15 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- This is the final 0.1.0 release.
- Made the last changes to pretty_printer().
- Now the debugging info's are uptodate, but be careful with using too many -v
  It will produce an awfull lot of output.
- There still seems to be a problem with empty signatures from time to time, 
  but not on as regular basis as last time. But now I think it's not going to 
  affect anyone, so I'll just let it stay in, as a small reminder.


* Mon Aug 13 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- The latest pre version in teh 0.1.0 line, counting pre2 at the moment.
- Added a few more verbose outputs, to acommondate better debugging
- Made a small change to the 'sigit -e' routine, hopefully it will save time, and
  be less space consuming.
- Played with the seg_fault catching function, hope it isn't causing any unsuspected 
  problems.


* Thu Jul 12 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Getting prity close to the 0.1 final.
- Hopefuly got rid of the "empty signature" bug which has been haunting moffe.
- Fixed a bug, also discovered by moffe, which gave a strange char at the end of the
  signature, if you were using exactly 72 chars in b_line and t_line.
 

* Tue May 29 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Starting slowly on version 0.1 with this pre1.
- Fixed a suspected bug, that Moffe has been having with the b_line and t_line setting.
- Started creating a function, which will catch any segmentation faults the program
  might have, and hopefully print out, what caused the error.
- Fixed a few missplaced logging() calls.


* Fri Mar 16 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- This is going to be the real 0.0.4 version.
- Fixed a small bug, which caused the program to segmentation fault, when you 
  specified your own sig_file setting in the configuration file.
- Made some changes to where the lock file is beeing created, instead of creating 
  it in /tmp, it will now be created in $HOME and named .Sigit-<USER>.lock. 
  This change was made, since I can't control what machine the program is started 
  on at UNI, and since the only thing thats shared throughout the whole system there 
  is $HOME, then I found this to be the only way, I could make sure the program gets 
  killed correctly.
- Got the program to work on HP-UX too.. It was a big/little endian problem, but a call 
  to htonl() and ntohl() solved it.


* Sun Mar 11 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Added a '-l <file>' flag, if you wanted the contence of the data file in a strict 
  text file.
- Added a '-u ident' flag, which will remove the signature that matches the ident 
  from the data file.


* Sat Mar 10 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- This is the pre2 release of version 0.0.4 made some changes to the prity_printer() 
  added a %T in order to have a time stamp in the preset or top and bottom line.
  changed the outcome of %U to be '<time stamp> up xdays hh:mm'.
- Added a %r to the signatures, if a line in the signature starts with it, it will be 
  right aligned according to the longest line found in the signature at that point, 
  if its the very first line, it will be places according to LINE_SIZE.
- Finished the logging() function to work as intended, it turned out there was an error
  in my version of vfprintf() and vsprintf(), don't know if it has something to do 
  with the version of glibc i'm running, made a fix to avoid it.
- Got the fifo part working like a charm. It turned out, that it would be easier to have
  it reread the configuration file on every run, since the user could have made changes 
  to it, while the program was locked by the fifo.
  Since the fifo is now working, the delay sleep() time isn't beeing used any more.
- Chaged the writing/reading of the data file, to avoid any big/little endian problems. 
  (Let's hope this will make it work on HP-UX systems)
- Tested the program with a data file containing 12.500 signatures for 3 hours, with a 
  new signature written every 3. sec. Looks to be running smooth, gave one hell of a 
  logfile tho. 


* Tue Mar 06 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Made Sigit into a CVS controled program, which caused a few problems.
  Had to make a different file for the original signatures, since CVS likes
  to keep info in the <programname>.orig file, so all of a suddent it was erased.


* Mon Mar 05 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Hmm trying to prepare the 0.0.4 version to be strict ANSI C.
  And later on use it with some gzip support. But this alpha version of 0.0.4pre1 
  is giving me a hard time. Some of the arguments parsed to the functions will 
  look to be OK during every debugging info, yet no function calls with them seems 
  to work.. 
- This is merely a test version, not intended to be used practical in any way.
  It can't even run properly, so dont even try it, without having a copy of 
  0.0.3 around.


* Sun Feb 25 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- It is time to call this the official 0.0.3 stable release.
- Fixed a bug, which would cause the program only to write one line to the
  signature file, when a line in the desired signture was exactly 72chars
  the fix results in, signatures having intentionaly blank lines, where these 
  blanks won't be written to the fifo.
- Fixed a bug which would cause the program to abort without a warning, if
  you used: -avv where the 'vv' is mistaken to be the filename.


* Sat Feb 24 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Wondering if this should be the stable 0.0.3, but for now its 0.0.3pre4
  I'm not that sure about how stable it is, so it needs a bit more testing.
- Found a few debug info I hadn't removed befor releasing 0.0.3pre3
- Changed the verbose output, so if the program is run as a deamon
  it will syslog the output, instead of just denying output.
- Took a closer look at the prity_printer, and decided that it needs some more
  thinking, but the changes in that will have to wait till 0.0.3


* Fri Feb 23 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- The 0.0.3pre3 is on its way.. not long befor the 0.0.3 is final.
- Fixed a bug, which caused the program to give and endless loop if b_line
  was exactly 72 chars, and t_line was above 72 chars the fix results in 
  even lines above those length will be handlet with grace.. 
- Added a few more conversion chars, so it will take %H, %P and %V for 
  Homepage, Programname, and Version.
- Fixed a bug, which caused it to _not_ remove the temp file, if there was 
  an error 
  while loading the editor for editing the signatures.


* Wed Feb 21 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- 0.0.3pre2 is finaly ready, I must admit 0.0.3pre1 was out too fast, the 
  rewriting of the prity_printer gave more bugs than was needed.
- Solved a few mismatch about the linking and unlinking of the fifo
  when an error occur.
- Brought the man pages up to date with the reason changes.
- Took care of the *required*?? Author fault in the spec file, I thought rpm-4 
  could handle all the settings which rpm-3 can, guess I was wrong. 
- suplied the prity_printer with some comments, in 2 month I cant remember 
  why it does what anyway.. 
- I finaly came up with a description in the spec file.
- Found a small bug in the add_sig function, which made it deny to write the
  very first sig to the data file, yet build the file correct.
- Made a few changes to the format, so I now have a chance to balance a data file
  if the size of the sig structure has changed, and the program is using an older 
  data file.. However this will require every one rebuild their data file from either 
  a sigit -e from the old version, or from the sigit.orig file.. or what ever file
  they are usign now.. 


* Tue Feb 20 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- The pre release 2 is on its way..
- Solved the bug in the new prity_printer, which caused it to partly 
  give segmentation fault, write error to the file, and wind up in an endless loop.
- After i realized, that most users might wanted to use the %M, %N, ...
  in the top line or the bottom line, so i added those into the part, 
  which checks for occurencies of those.


* Mon Feb 19 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Still working on the pre release of version 0.0.3
- changed the order in which the program decides what to do on the arguments 
  and flags parsed to the program.
- Fixed a small bug in the rutine to create the pid file and check for allready 
  running processes.
- Finished writing the man pages.
- Made the '-e' flag in the program start your $EDITOR in order to interactive 
  edit the signatures.
- Changed the '-r' flag to rebuild the database from a filename parsed with the flag.
  only thing is, this file must have the exact same structure, as the 'sigit.orig' file.
- changed the prity_printer, so it will work better with the presettings and _much_ 
  cleaner code.  It now takes %U in the preset, in order to display the uptime
  But for some odd reason it cant take the rest of the arguments to it, without 
  resulting in a segmentation fault. I think it has somethign todo with the utsname struct.


* Fri Feb 16 2001 Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Starting on vesion 0.0.3 still in it's pre release. 
- After recieving a patch for correcting any spelling errors, partialy in 
  the documentation aswell as in the signatures located in the sigit.orig 
  file I kinda got a jump start on this version. 
  Thanks Moffe <moffespam (at) amagerkollegiet (dot) dk>
     - Hope you finished your assignment, instead of using all your time
       to patch my program. 
- After some consideration I have placed the sigit executable in the
  '/usr/bin' dir instead of the previus '/usr/local/bin' to any users 
  of the previus versions, please remove the '/usr/local/bin/sigit' file
  manualy, since I didn't make an option in the uninstall settings to 
  correct this change
- Made a few changes to correct a Big/Little endian problem, when using 
  the program on the HP-UX architechture. (this needs some work too) 
- Added an extra man page, since i felt the documentation on howto create
  the sigitrc file should be kept for it self.
- Fixed a small bug in the unlinking of the fifo file.


* Mon Feb 5 2001  Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Huge upgrade from 0.0.1 to 0.0.2
- Finished the verbose output.
- Made the [-r] and [-e] flags work.
- Changed the programs main function, so it will run as a regular process
  if no arguments are given to the program call. 
  Else use [-d] to make it a deamon.
- Reduced the memory use by 33%  -- which resulted in having to build a 
  new data file. 
- Added a pid file checker.
- Made the spec file work.


* Wed Dec 20 2000  Kenneth 'Redhead' Nielsen <redhead@diku.dk>
- Made the first fragile version 0.0.1
- All the arguments havn't been implemented yet.
- There's no verbose output what so ever.
- The order, in which the program handle the arguments might change, 
  even the number of arguments the program can handle.


%description
Getting tired of allways having the same signature attached to your emails. ?
Having trouble comming up with new ones all the time. ?
Problems with having to fit your email and contact info into your signature ?

Does any of these questions sound familiar, then Sigit might be the thing your looking for.
It will work with your own configuration file, or with the one globaly placed on the system
And you can customise it to fit your every need.

Theres plenty of signature changers out there, you might want to ask 'why I am 
making my own ?'. But Sigit is not just your simple signature changer, 
where you give it the signatures file to pick one from, and tell it what file 
to write it to.. Sigit is more, it has an advanced way of integrating your favourit
editor with the signatures it's allready using, for easy editing. You can start it
as a deamon, where you never have to restart it, since its running in the background 
as a system process.


%prep
%setup

%build
rm -rf $RPM_BUILD_ROOT
CFLAGS="$RPM_OPT_FLAGS" LDFLAGS="-s"
make

%install
make PREFIX=$RPM_BUILD_ROOT/usr/share/sigit MANDIR=$RPM_BUILD_ROOT%{_mandir} INSTALLDIR=$RPM_BUILD_ROOT/usr/bin RCDIR=$RPM_BUILD_ROOT/etc install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS COPYING INSTALL README TODO
%attr(555,root,root)/usr/bin/sigit
%attr(664,root,root)/etc/sigit.rc
%attr(664,root,root)/usr/share/sigit/sigit.data
%attr(664,root,root)/usr/share/sigit/sigit.sig
%attr(444,root,root)%{_mandir}/man1/sigit.1*
%attr(444,root,root)%{_mandir}/man1/sigit.rc.1*


