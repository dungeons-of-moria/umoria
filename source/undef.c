/* undef.c: undefined routines */

#include "constant.h"
#include "config.h"
#include "types.h"

extern short log_index;

void init_scorefile()
{
}

void init_files()
{
}

void display_scores(region, final)
int region, final;
#ifdef MAC
	#pragma unused(region, final)
#endif
{
  msg_print("Scoring not implemented yet.");
}

_new_log()
{
  /* set log_index so that save code works correctly */
  log_index = 0;
  return 1;
}
