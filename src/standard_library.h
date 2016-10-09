/*
 * This is not 1994 anymore, so let's include all standard libraries in one file!
 */

#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h> // defines NULL
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // rototype for execl

#include <sys/ioctl.h> // defines CTRL
#include <sys/stat.h> // defines chmod
#include <sys/wait.h>

//
// system dependent includes
//

#ifdef DEBIAN_LINUX
  #include <termios.h>
#endif

#ifdef USG
  #include <fcntl.h>
  #include <memory.h>
  #include <string.h>
#else // else USG
  #include <strings.h>

  /* only needed for Berkeley UNIX */
  #include <sys/file.h>
  #include <sys/param.h>
  #include <sys/resource.h>
  #include <sys/types.h>
#endif // end USG

#ifdef M_XENIX
  #include <sys/select.h>
  #include <sys/types.h>
#endif
