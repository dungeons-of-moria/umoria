
#ifdef USG

/*                                                                    -CJS-
// The following code is provided especially for systems which
// have no flock system call. It has never been tested.
*/

#define L_SET 0

#define LOCK_EX	1
#define LOCK_SH	2
#define LOCK_NB	4
#define LOCK_UN	8

/*
// An flock HACK. LOCK_SH and LOCK_EX are not distinguished. DO NOT release a
// lock which you failed to set! ALWAYS release a lock you set!
*/
STATIC flock(f, l)
int f, l;
{
  struct stat sbuf;
  char lockname[80];

  if(fstat(f,  &sbuf) < 0)
    return FALSE;
  (void) sprintf(lockname, "/tmp/moria.%d", sbuf.st_ino);
  if(l & LOCK_UN){
    return unlink(lockname);
    }
  while(open(lockname, O_WRONLY|O_CREAT|O_EXCL, 0) < 0){
    if(errno != EEXIST)
      return FALSE;
    if(stat(lockname, &sbuf) < 0)
      return FALSE;
/*
// Log Locks which last more than 10 seconds get deleted.
*/
    if((f == logfile_fd || f == lstfile_fd)
       && time((long *)0) - sbuf.st_mtime > 10) {
      if (unlink(lockname) < 0)
	return FALSE;
      }
    else if(l & LOCK_NB)
      return FALSE;
    else
      (void) sleep(1);
    }
  return TRUE;
}
#endif
