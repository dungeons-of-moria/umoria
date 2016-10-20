// src/standard_library.h: standard library imports - conditional compilation macros.
//
// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// This file is part of Umoria.
//
// Umoria is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Umoria is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Umoria.  If not, see <http://www.gnu.org/licenses/>.

#ifdef _WIN32
  #define _CRT_SECURE_NO_WARNINGS

  #include <ctype.h>
  #include <errno.h>
  #include <fcntl.h>
  // #include <pwd.h>
  #include <stdbool.h>
  #include <stdint.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
  // #include <unistd.h>

  // #include <sys/param.h>
  #include <sys/stat.h>

#elif __APPLE__

  #include <ctype.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <pwd.h>
  #include <stdbool.h>
  #include <stdint.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
  #include <unistd.h>

  #include <sys/param.h>
  #include <sys/stat.h>

#elif __linux__

  #include <ctype.h>     // islower(), isupper(), isalpha(), etc.
  #include <errno.h>
  #include <fcntl.h>
  #include <pwd.h>
  #include <stdbool.h>
  #include <stdint.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
  #include <unistd.h>    // getuid() and others

  #include <sys/param.h> // Defines the timeval structure / fd_set
  #include <sys/stat.h>  // Used only for chmod()

#else
#   error "Unknown compiler"
#endif
