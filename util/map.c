/* this used to be in files.c, this is not a working program */

/* Output dungeon section sizes					*/
#define OUTPAGE_HEIGHT 44     /* 44 lines of dungeon per section       */
#define OUTPAGE_WIDTH  99     /* 100 columns of dungeon per section    */

/* deleted in favor of improved Map and Where commands */
/* Prints dungeon map to external file			-RAK-	 */
print_map()
{
  register int i, j, m, n;
  register k, l;
  register i7, i8;
  char dun_line[MAX_WIDTH+1];
  char *dun_ptr;
  static vtype filename1 = "MORIAMAP.DAT";
  vtype filename2;
  char tmp_str[80];
  FILE *file1;
  static int page_width = OUTPAGE_WIDTH;
  static int page_height = OUTPAGE_HEIGHT;

  /* this allows us to strcat each character in the inner loop,
     instead of using the expensive sprintf */
  (void) sprintf (tmp_str, "File name [%s]: ", filename1);
  prt(tmp_str, 0, 0);
  if (get_string(filename2, 0, strlen(tmp_str), 64))
    {
      if (strlen(filename2) > 0)
	(void) strcpy(filename1, filename2);
      if ((file1 = fopen(filename1, "w")) == NULL)
	{
	  (void) sprintf(dun_line, "Cannot open file %s", filename1);
	  prt(dun_line, 0, 0);
	  return;
	}
      (void) sprintf(tmp_str, "section width (default = %d char):",
		     page_width);
      prt(tmp_str, 0, 0);
      (void) get_string(tmp_str, 0, strlen(tmp_str), 10);
      page_width = atoi(tmp_str);
      if (page_width < 10)
	page_width = 10;

      (void) sprintf(tmp_str, "section height (default = %d lines):",
		     page_height);
      prt(tmp_str, 0, 0);
      (void) get_string(tmp_str, 0, strlen(tmp_str), 10);
      page_height = atoi(tmp_str);
      if (page_height < 10)
	page_height = 10;

      prt("Writing Moria Dungeon Map...", 0, 0);
      put_qio();

      i = 0;
      i7 = 0;
      do
	{
	  j = 0;
	  k = i + page_height - 1;
	  if (k >= cur_height)
	    k = cur_height - 1;
	  i7++;
	  i8 = 0;
	  do
	    {
	      l = j + page_width - 1;
	      if (l >= cur_width)
		l = cur_width - 1;
	      i8++;
	      (void) fprintf(file1, "%c\n", CTRL('L'));
	      (void) fprintf(file1, "Section[%d,%d];     ", i7, i8);
	      (void) fprintf(file1, "Depth : %d (feet)\n\n   ",
			     (dun_level * 50));
	      for (m = j; m <= l; m++)
		{
		  n = (m / 100);
		  (void) fprintf(file1, "%d", n);
		}
	      (void) fputs("\n   ", file1);
	      for (m = j; m <= l; m++)
		{
		  n = (m / 10) - (m / 100) * 10;
		  (void) fprintf(file1, "%d", n);
		}
	      (void) fputs("\n   ", file1);
	      for (m = j; m <= l; m++)
		{
		  n = m - (m / 10) * 10;
		  (void) fprintf(file1, "%d", n);
		}
	      (void) fprintf(file1, "\n");
	      for (m = i; m <= k; m++)
		{
		  (void) sprintf(dun_line, "%2d ", m);
		  dun_ptr = &dun_line[3];
		  for (n = j; n <= l; n++)
		    *dun_ptr++ = loc_symbol(m, n);
		  *dun_ptr++ = '\n';
		  *dun_ptr++ = '\0';
		  (void) fputs(dun_line, file1);
		}
	      j += page_width;
	    }
	  while (j < cur_width);
	  i += page_height;
	}
      while (i < cur_height);
      (void) fclose(file1);
      prt("Completed.", 0, 0);
    }
}
