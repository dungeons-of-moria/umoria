#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

typedef struct coords {
  int x, y;
} coords;

coords doorstk[100];
int doorptr;


/* Always picks a correct direction		*/
correct_dir(rdir, cdir, y1, x1, y2, x2)
int *rdir, *cdir;
register int y1, x1, y2, x2;
{
  if (y1 < y2)
    *rdir =  1;
  else if (y1 == y2)
    *rdir =  0;
  else
    *rdir = -1;
  if (x1 < x2)
    *cdir =  1;
  else if (x1 == x2)
    *cdir =  0;
  else
    *cdir = -1;
  if ((*rdir != 0) && (*cdir != 0))
    switch(randint(2))
      {
      case 1:
	*rdir = 0;
	break;
      case 2:
	*cdir = 0;
	break;
      }
}


/* Chance of wandering direction			*/
rand_dir(rdir, cdir, y1, x1, y2, x2, chance)
int *rdir, *cdir;
int y1, x1, y2, x2;
int chance;
{
  switch(randint(chance))
    {
    case 1:
      *rdir = -1;
      *cdir =  0;
      break;
    case 2:
      *rdir =  1;
      *cdir =  0;
      break;
    case 3:
      *rdir =  0;
      *cdir = -1;
      break;
    case 4:
      *rdir =  0;
      *cdir =  1;
      break;
    default:
      correct_dir(rdir, cdir, y1, x1, y2, x2);
      break;
    }
}


/* Blanks out entire cave				-RAK-	*/
blank_cave()
{
  register int i, j;
  register cave_type *c_ptr;

  c_ptr = &cave[0][0];
  for (i = 0; i < MAX_HEIGHT; i++)
    for (j = 0; j < MAX_WIDTH; j++)
      *c_ptr++ = blank_floor;
}


/* Fills in empty spots with desired rock		-RAK-	*/
/* Note: 9 is a temporary value.				*/
fill_cave(fill)
floor_type fill;
{
  register int i, j;
  register cave_type *c_ptr;

  for (i = 1; i < cur_height-1; i++)
    for (j = 1; j < cur_width-1; j++)
      {
	c_ptr = &cave[i][j];
	if ((c_ptr->fval == 0) || (c_ptr->fval == 8) || (c_ptr->fval == 9))
	  {
	    c_ptr->fval = fill.ftval;
	    c_ptr->fopen = fill.ftopen;
	  }
      }
}


/* Places indestructible rock around edges of dungeon	-RAK-	*/
place_boundary()
{
  register int i;

  for (i = 0; i < cur_height; i++)
    {
      cave[i][0].fval            = boundary_wall.ftval;
      cave[i][0].fopen           = boundary_wall.ftopen;
      cave[i][cur_width-1].fval  = boundary_wall.ftval;
      cave[i][cur_width-1].fopen = boundary_wall.ftopen;
    }
  for (i = 0; i < cur_width; i++)
    {
      cave[0][i].fval             = boundary_wall.ftval;
      cave[0][i].fopen            = boundary_wall.ftopen;
      cave[cur_height-1][i].fval  = boundary_wall.ftval;
      cave[cur_height-1][i].fopen = boundary_wall.ftopen;
    }
}


/* Places "streamers" of rock through dungeon		-RAK-	*/
place_streamer(rock, treas_chance)
floor_type rock;
int treas_chance;
{
  register int i, tx, ty;
  int y, x, dir, t1, t2;
  int flag;
  register cave_type *c_ptr;

  /* Choose starting point and direction		*/
  y = (cur_height/2.0) + 11 - randint(23);
  x = (cur_width/2.0)  + 16 - randint(33);

  dir = randint(8);	/* Number 1-4, 6-9	*/
  if (dir > 4)
    dir = dir + 1;

  /* Place streamer into dungeon			*/
  flag = FALSE;	/* Set to true when y, x are out-of-bounds*/
  t1 = 2*DUN_STR_RNG + 1;	/* Constants	*/
  t2 =   DUN_STR_RNG + 1;
  do
    {
      for (i = 0; i < DUN_STR_DEN; i++)
	{
	  ty = y + randint(t1) - t2;
	  tx = x + randint(t1) - t2;
	  if (in_bounds(ty, tx))
	    {
	      c_ptr = &cave[ty][tx];
	      if (c_ptr->fval == rock_wall1.ftval)
		{
		  c_ptr->fval = rock.ftval;
		  c_ptr->fopen = rock.ftopen;
		  if (randint(treas_chance) == 1)
		    place_gold(ty, tx);
		}
	    }
	}
      if (!move(dir, &y, &x))
	flag = TRUE;
    }
  while (!flag);
}


/* Place a trap with a given displacement of point	-RAK-	*/
vault_trap(y, x, yd, xd, num)
int y, x, yd, xd, num;
{
  register int count, y1, x1;
  int i, flag;
  register cave_type *c_ptr;

  for (i = 0; i < num; i++)
    {
      flag = FALSE;
      count = 0;
      do
	{
	  y1 = y - yd - 1 + randint(2*yd+1);
	  x1 = x - xd - 1 + randint(2*xd+1);
	  c_ptr = &cave[y1][x1];
	  if ((c_ptr->fval > 0) && (c_ptr->fval < 8) && (c_ptr->fval != 3))
	    if (c_ptr->tptr == 0)
	      {
		place_trap(y1, x1, 1, randint(MAX_TRAPA)-1);
		flag = TRUE;
	      }
	  count++;
	}
      while ((!flag) && (count <= 5));
    }
}


/* Place a trap with a given displacement of point	-RAK-	*/
vault_monster(y, x, num)
int y, x, num;
{
  register int i;
  int y1, x1;

  for (i = 0; i < num; i++)
    {
      y1 = y;
      x1 = x;
      (void) summon_monster(&y1, &x1, TRUE);
    }
}


/* Builds a room at a row, column coordinate		-RAK-	*/
build_room(yval, xval)
int yval, xval;
{
  register int i, j;
  register int y_height, y_depth;
  register int x_left, x_right;
  floor_type cur_floor;

  if (dun_level <= randint(25))
    cur_floor = lopen_floor;	/* Floor with light	*/
  else
    cur_floor = dopen_floor;	/* Dark floor		*/
  y_height = yval - randint(4);
  y_depth  = yval + randint(3);
  x_left   = xval - randint(11);
  x_right  = xval + randint(11);
  for (i = y_height; i <= y_depth; i++)
    for (j = x_left; j <= x_right; j++)
      {
	cave[i][j].fval  = cur_floor.ftval;
	cave[i][j].fopen = cur_floor.ftopen;
      }
  for (i = (y_height - 1); i <= (y_depth + 1); i++)
    {
      cave[i][x_left-1].fval   = rock_wall1.ftval;
      cave[i][x_left-1].fopen  = rock_wall1.ftopen;
      cave[i][x_right+1].fval  = rock_wall1.ftval;
      cave[i][x_right+1].fopen = rock_wall1.ftopen;
    }
  for (i = x_left; i <= x_right; i++)
    {
      cave[y_height-1][i].fval  = rock_wall1.ftval;
      cave[y_height-1][i].fopen = rock_wall1.ftopen;
      cave[y_depth+1][i].fval   = rock_wall1.ftval;
      cave[y_depth+1][i].fopen  = rock_wall1.ftopen;
    }
}


/* Builds a room at a row, column coordinate		-RAK-	*/
/* Type 1 unusual rooms are several overlapping rectangular ones	*/
build_type1(yval, xval)
int yval, xval;
{
  int y_height, y_depth;
  int x_left, x_right;
  register int i0, i, j;
  floor_type cur_floor;
  register cave_type *c_ptr;

  if (dun_level <= randint(25))
    cur_floor = lopen_floor;	/* Floor with light	*/
  else
    cur_floor = dopen_floor;	/* Dark floor		*/
  for (i0 = 0; i0 < (1 + randint(2)); i0++)
    {
      y_height = yval - randint(4);
      y_depth  = yval + randint(3);
      x_left   = xval - randint(11);
      x_right  = xval + randint(11);
      for (i = y_height; i <= y_depth; i++)
	for (j = x_left; j <= x_right; j++)
	  {
	    cave[i][j].fval  = cur_floor.ftval;
	    cave[i][j].fopen = cur_floor.ftopen;
	  }
      for (i = (y_height - 1); i <= (y_depth + 1); i++)
	{
	  c_ptr = &cave[i][x_left-1];
	  if (c_ptr->fval != cur_floor.ftval)
	    {
	      c_ptr->fval  = rock_wall1.ftval;
	      c_ptr->fopen = rock_wall1.ftopen;
	    }
	  c_ptr = &cave[i][x_right+1];
	    if (c_ptr->fval != cur_floor.ftval)
	      {
		c_ptr->fval  = rock_wall1.ftval;
		c_ptr->fopen = rock_wall1.ftopen;
	      }
	}
      for (i = x_left; i <= x_right; i++)
	{
	  c_ptr = &cave[y_height-1][i];
	  if (c_ptr->fval != cur_floor.ftval)
	    {
	      c_ptr->fval  = rock_wall1.ftval;
	      c_ptr->fopen = rock_wall1.ftopen;
	    }
	  c_ptr = &cave[y_depth+1][i];
	    if (c_ptr->fval != cur_floor.ftval)
	      {
		c_ptr->fval  = rock_wall1.ftval;
		c_ptr->fopen = rock_wall1.ftopen;
	      }
	}
    }
}


/* Builds an unusual room at a row, column coordinate	-RAK-	*/
/* Type 2 unusual rooms all have an inner room:			*/
/*   1 - Just an inner room with one door			*/
/*   2 - An inner room within an inner room			*/
/*   3 - An inner room with pillar(s)				*/
/*   4 - Inner room has a maze					*/
/*   5 - A set of four inner rooms				*/
build_type2(yval, xval)
int yval, xval;
{
  register int i, j;
  register int y_height, y_depth;
  register int x_left, x_right;
  floor_type cur_floor;

  if (dun_level <= randint(30))
    cur_floor = lopen_floor;	/* Floor with light	*/
  else
    cur_floor = dopen_floor;	/* Dark floor		*/
  y_height = yval - 4;
  y_depth  = yval + 4;
  x_left   = xval - 11;
  x_right  = xval + 11;
  for (i = y_height; i <= y_depth; i++)
    for (j = x_left; j <= x_right; j++)
      {
	cave[i][j].fval  = cur_floor.ftval;
	cave[i][j].fopen = cur_floor.ftopen;
      }
  for (i = (y_height - 1); i <= (y_depth + 1); i++)
    {
      cave[i][x_left-1].fval   = rock_wall1.ftval;
      cave[i][x_left-1].fopen  = rock_wall1.ftopen;
      cave[i][x_right+1].fval  = rock_wall1.ftval;
      cave[i][x_right+1].fopen = rock_wall1.ftopen;
    }
  for (i = x_left; i <= x_right; i++)
    {
      cave[y_height-1][i].fval  = rock_wall1.ftval;
      cave[y_height-1][i].fopen = rock_wall1.ftopen;
      cave[y_depth+1][i].fval   = rock_wall1.ftval;
      cave[y_depth+1][i].fopen  = rock_wall1.ftopen;
    }
  /* The inner room		*/
  y_height = y_height + 2;
  y_depth  = y_depth  - 2;
  x_left   = x_left   + 2;
  x_right  = x_right  - 2;
  for (i = (y_height - 1); i <= (y_depth + 1); i++)
    {
      cave[i][x_left-1].fval   = 8;
      cave[i][x_right+1].fval  = 8;
    }
  for (i = x_left; i <= x_right; i++)
    {
      cave[y_height-1][i].fval  = 8;
      cave[y_depth+1][i].fval   = 8;
    }
  /* Inner room variations		*/
  switch(randint(5))
    {
    case 1:	/* Just an inner room...	*/
      switch(randint(4)) 	/* Place a door	*/
	{
	case 1:	place_secret_door(y_height-1, xval); break;
	case 2:	place_secret_door(y_depth+1, xval); break;
	case 3:	place_secret_door(yval, x_left-1); break;
	case 4:	place_secret_door(yval, x_right+1); break;
	}
      vault_monster(yval, xval, 1);
      break;
    case 2:	/* Treasure Vault	*/
      switch(randint(4))	/* Place a door	*/
	{
	case 1:	place_secret_door(y_height-1, xval); break;
	case 2:	place_secret_door(y_depth+1, xval); break;
	case 3:	place_secret_door(yval, x_left-1); break;
	case 4:	place_secret_door(yval, x_right+1); break;
	}
      for (i = yval-1; i <= yval+1; i++)
	{
	  cave[i][xval-1].fval   = 8;
	  cave[i][xval+1].fval   = 8;
	}
      cave[yval-1][xval].fval  = 8;
      cave[yval+1][xval].fval  = 8;
      switch(randint(4))	/* Place a door	*/
	{
	case 1:	place_locked_door(yval-1, xval); break;
	case 2:	place_locked_door(yval+1, xval); break;
	case 3:	place_locked_door(yval, xval-1); break;
	case 4:	place_locked_door(yval, xval+1); break;
	}
      /* Place an object in the treasure vault	*/
      switch(randint(10))
	{
	case 1: place_up_stairs(yval, xval); break;
	case 2: place_down_stairs(yval, xval); break;
	default: place_object(yval, xval); break;
	}
      /* Guard the treasure well		*/
      vault_monster(yval, xval, 2+randint(3));
      /* If the monsters don't get 'em...	*/
      vault_trap(yval, xval, 4, 10, 2+randint(3));
      break;
    case 3:	/* Inner pillar(s)...	*/
      switch(randint(4))  /* Place a door	*/
	{
	case 1:	place_secret_door(y_height-1, xval); break;
	case 2:	place_secret_door(y_depth+1, xval); break;
	case 3:	place_secret_door(yval, x_left-1); break;
	case 4:	place_secret_door(yval, x_right+1); break;
	}
      for (i = yval-1; i <= yval+1; i++)
	for (j = xval-1; j <= xval+1; j++)
	  cave[i][j].fval   = 8;
      if (randint(2) == 1)
	{
	  switch(randint(2))
	    {
	    case 1:
	      for (i = yval-1; i <= yval+1; i++)
		for (j = xval-6; j <= xval-4; j++)
		  cave[i][j].fval   = 8;
	      for (i = yval-1; i <= yval+1; i++)
		for (j = xval+4; j <= xval+6; j++)
		  cave[i][j].fval   = 8;
	      break;
	    case 2:
	      for (i = yval-1; i <= yval+1; i++)
		for (j = xval-7; j <= xval-5; j++)
		  cave[i][j].fval   = 8;
	      for (i = yval-1; i <= yval+1; i++)
		for (j = xval+5; j <= xval+7; j++)
		  cave[i][j].fval   = 8;
	      break;
	    }
	}
      if (randint(3) == 1) 	/* Inner rooms	*/
	{
	  for (i = xval-5; i <= xval+5; i++)
	    {
	      cave[yval-1][i].fval = 8;
	      cave[yval+1][i].fval = 8;
	    }
	  cave[yval][xval-5].fval = 8;
	  cave[yval][xval+5].fval = 8;
	  switch(randint(2))
	    {
	    case 1: place_secret_door(yval+1, xval-3); break;
	    case 2: place_secret_door(yval-1, xval-3); break;
	    }
	  switch(randint(2))
	    {
	    case 1: place_secret_door(yval+1, xval+3); break;
	    case 2: place_secret_door(yval-1, xval+3); break;
	    }
	  if (randint(3) == 1)  place_object(yval, xval-2);
	  if (randint(3) == 1)  place_object(yval, xval+2);
	  vault_monster(yval, xval-2, randint(2));
	  vault_monster(yval, xval+2, randint(2));
	}
      break;
    case 4:	/* Maze inside...	*/
      switch(randint(4))	/* Place a door	*/
	{
	case 1:	place_secret_door(y_height-1, xval); break;
	case 2:	place_secret_door(y_depth+1, xval); break;
	case 3:	place_secret_door(yval, x_left-1); break;
	case 4:	place_secret_door(yval, x_right+1); break;
	}
      for (i = y_height; i <= y_depth; i++)
	for (j = x_left; j <= x_right; j++)
	  if (0x1 & (j+i))
	    cave[i][j].fval = 8;
      /* Monsters just love mazes...		*/
      vault_monster(yval, xval-5, randint(3));
      vault_monster(yval, xval+5, randint(3));
      /* Traps make them entertaining...	*/
      vault_trap(yval, xval-3, 2, 8, randint(3));
      vault_trap(yval, xval+3, 2, 8, randint(3));
      /* Mazes should have some treasure too..	*/
      for (i = 0; i < 3; i++)
	random_object(yval, xval, 1);
      break;
    case 5:	/* Four small rooms...	*/
      for (i = y_height; i <= y_depth; i++)
	cave[i][xval].fval = 8;
      for (i = x_left; i <= x_right; i++)
	cave[yval][i].fval = 8;
      switch(randint(2))
	{
	case 1:
	  i = randint(10);
	  place_secret_door(y_height-1, xval-i);
	  place_secret_door(y_height-1, xval+i);
	  place_secret_door(y_depth+1, xval-i);
	  place_secret_door(y_depth+1, xval+i);
	  break;
	case 2:
	  i = randint(3);
	  place_secret_door(yval+i, x_left-1);
	  place_secret_door(yval-i, x_left-1);
	  place_secret_door(yval+i, x_right+1);
	  place_secret_door(yval-i, x_right+1);
	  break;
	}
      /* Treasure in each one...		*/
      random_object(yval, xval, 2+randint(2));
      /* Gotta have some monsters...		*/
      vault_monster(yval+2, xval-4, randint(2));
      vault_monster(yval+2, xval+4, randint(2));
      vault_monster(yval-2, xval-4, randint(2));
      vault_monster(yval-2, xval+4, randint(2));
      break;
    }
}


/* Builds a room at a row, column coordinate		-RAK-	*/
/* Type 3 unusual rooms are cross shaped				*/
build_type3(yval, xval)
int yval, xval;
{
  int y_height, y_depth;
  int x_left, x_right;
  register int i0, i, j;
  floor_type cur_floor;
  register cave_type *c_ptr;

  if (dun_level <= randint(25))
    cur_floor = lopen_floor;	/* Floor with light	*/
  else
    cur_floor = dopen_floor;	/* Dark floor		*/
  i0 = 2 + randint(2);
  y_height = yval - i0;
  y_depth  = yval + i0;
  x_left   = xval - 1;
  x_right  = xval + 1;
  for (i = y_height; i <= y_depth; i++)
    for (j = x_left; j <= x_right; j++)
      {
	cave[i][j].fval  = cur_floor.ftval;
	cave[i][j].fopen = cur_floor.ftopen;
      }
  for (i = (y_height - 1); i <= (y_depth + 1); i++)
    {
      c_ptr = &cave[i][x_left-1];
      c_ptr->fval  = rock_wall1.ftval;
      c_ptr->fopen = rock_wall1.ftopen;
      c_ptr = &cave[i][x_right+1];
      c_ptr->fval  = rock_wall1.ftval;
      c_ptr->fopen = rock_wall1.ftopen;
    }
  for (i = x_left; i <= x_right; i++)
    {
      c_ptr = &cave[y_height-1][i];
      c_ptr->fval  = rock_wall1.ftval;
      c_ptr->fopen = rock_wall1.ftopen;
      c_ptr = &cave[y_depth+1][i];
      c_ptr->fval  = rock_wall1.ftval;
      c_ptr->fopen = rock_wall1.ftopen;
    }
  i0 = 2 + randint(9);
  y_height = yval - 1;
  y_depth  = yval + 1;
  x_left   = xval - i0;
  x_right  = xval + i0;
  for (i = y_height; i <= y_depth; i++)
    for (j = x_left; j <= x_right; j++)
      {
	cave[i][j].fval  = cur_floor.ftval;
	cave[i][j].fopen = cur_floor.ftopen;
      }
  for (i = (y_height - 1); i <= (y_depth + 1); i++)
    {
      c_ptr = &cave[i][x_left-1];
      if (c_ptr->fval != cur_floor.ftval)
	{
	  c_ptr->fval  = rock_wall1.ftval;
	  c_ptr->fopen = rock_wall1.ftopen;
	}
      c_ptr = &cave[i][x_right+1];
      if (c_ptr->fval != cur_floor.ftval)
	{
	  c_ptr->fval  = rock_wall1.ftval;
	  c_ptr->fopen = rock_wall1.ftopen;
	}
    }
  for (i = x_left; i <= x_right; i++)
    {
      c_ptr = &cave[y_height-1][i];
      if (c_ptr->fval != cur_floor.ftval)
	{
	  c_ptr->fval  = rock_wall1.ftval;
	  c_ptr->fopen = rock_wall1.ftopen;
	}
      c_ptr = &cave[y_depth+1][i];
      if (c_ptr->fval != cur_floor.ftval)
	{
	  c_ptr->fval  = rock_wall1.ftval;
	  c_ptr->fopen = rock_wall1.ftopen;
	}
    }
  /* Special features...			*/
  switch(randint(4))
    {
    case 1:	/* Large middle pillar		*/
      for (i = yval-1; i <= yval+1; i++)
	for (j = xval-1; j <= xval+1; j++)
	  cave[i][j].fval = 8;
      break;
    case 2:	/* Inner treasure vault		*/
      for (i = yval-1; i <= yval+1; i++)
	{
	  cave[i][xval-1].fval   = 8;
	  cave[i][xval+1].fval   = 8;
	}
      cave[yval-1][xval].fval  = 8;
      cave[yval+1][xval].fval  = 8;
      switch(randint(4))	/* Place a door	*/
	{
	case 1:	place_secret_door(yval-1, xval); break;
	case 2:	place_secret_door(yval+1, xval); break;
	case 3:	place_secret_door(yval, xval-1); break;
	case 4:	place_secret_door(yval, xval+1); break;
	}
      /* Place a treasure in the vault		*/
      place_object(yval, xval);
      /* Let's guard the treasure well...	*/
      vault_monster(yval, xval, 2+randint(2));
      /* Traps naturally			*/
      vault_trap(yval, xval, 4, 4, 1+randint(3));
      break;
    case 3:
      if (randint(3) == 1)
	{
	  cave[yval-1][xval-2].fval = 8;
	  cave[yval+1][xval-2].fval = 8;
	  cave[yval-1][xval+2].fval = 8;
	  cave[yval-1][xval+2].fval = 8;
	  cave[yval-2][xval-1].fval = 8;
	  cave[yval-2][xval+1].fval = 8;
	  cave[yval+2][xval-1].fval = 8;
	  cave[yval+2][xval+1].fval = 8;
	  if (randint(3) == 1)
	    {
	      place_secret_door(yval, xval-2);
	      place_secret_door(yval, xval+2);
	      place_secret_door(yval-2, xval);
	      place_secret_door(yval+2, xval);
	    }
	}
      else if (randint(3) == 1)
	{
	  cave[yval][xval].fval = 8;
	  cave[yval-1][xval].fval = 8;
	  cave[yval+1][xval].fval = 8;
	  cave[yval][xval-1].fval = 8;
	  cave[yval][xval+1].fval = 8;
	}
      else if (randint(3) == 1)
	cave[yval][xval].fval = 8;
      break;
    case 4:
      break;
    }
}


/* Constructs a tunnel between two points		*/
build_tunnel(row1, col1, row2, col2)
int row1, col1, row2, col2;
{
  register int tmp_row, tmp_col;
  int row_dir, col_dir;
  register int i, j;
  coords tunstk[1000];
  coords wallstk[1000];
  int tunptr;
  int wallptr;
  int stop_flag, door_flag;
  cave_type *c_ptr, *d_ptr;

  /* Main procedure for Tunnel			*/
  /* Note: 9 is a temporary value		*/
  stop_flag = FALSE;
  door_flag = FALSE;
  tunptr    = 0;
  wallptr   = 0;
  correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);
  do
    {
      if (randint(100) > DUN_TUN_CHG)
	rand_dir(&row_dir, &col_dir, row1, col1, row2, col2, DUN_TUN_RND);
      tmp_row = row1 + row_dir;
      tmp_col = col1 + col_dir;
      while (!in_bounds(tmp_row, tmp_col))
	{
	  rand_dir(&row_dir, &col_dir, row1, col1, row2, col2, DUN_TUN_RND);
	  tmp_row = row1 + row_dir;
	  tmp_col = col1 + col_dir;
	}
      c_ptr = &cave[tmp_row][tmp_col];
      if (c_ptr->fval == rock_wall1.ftval)
	{
	  row1 = tmp_row;
	  col1 = tmp_col;
	  wallstk[wallptr].y = row1;
	  wallstk[wallptr].x = col1;
	  if (wallptr < 1000)
	    wallptr++;
	  for (i = row1-1; i <= row1+1; i++)
	    for (j = col1-1; j <= col1+1; j++)
	      if (in_bounds(i, j))
		{
		  d_ptr = &cave[i][j];
		  if ((d_ptr->fval >= 10) && (d_ptr->fval <= 12))
		    d_ptr->fval = 9;
		}
	}
      else if (c_ptr->fval == corr_floor1.ftval)
	{
	  row1 = tmp_row;
	  col1 = tmp_col;
	  if (!door_flag)
	    {
	      if (doorptr < 100)
		{
		  doorstk[doorptr].y = row1;
		  doorstk[doorptr].x = col1;
		  doorptr++;
		}
	      door_flag = TRUE;
	    }
	  if (randint(100) > DUN_TUN_CON)
	    stop_flag = TRUE;
	}
      else if (c_ptr->fval == 0)
	{
	  row1 = tmp_row;
	  col1 = tmp_col;
	  tunstk[tunptr].y = row1;
	  tunstk[tunptr].x = col1;
	  if (tunptr < 1000)
	    tunptr++;
	  door_flag = FALSE;
	}
      else if (c_ptr->fval != 9)
	{
	  row1 = tmp_row;
	  col1 = tmp_col;
	}
    }
  while (((row1 != row2) || (col1 != col2)) && (!stop_flag));
  for (i = 0; i < tunptr; i++)
    {
      cave[tunstk[i].y][tunstk[i].x].fval  = corr_floor1.ftval;
      cave[tunstk[i].y][tunstk[i].x].fopen = corr_floor1.ftopen;
    }
  for (i = 0; i < wallptr; i++)
    {
      c_ptr = &cave[wallstk[i].y][wallstk[i].x];
      if (c_ptr->fval == 9)
	{
	  if (randint(100) < DUN_TUN_PEN)
	    place_door(wallstk[i].y, wallstk[i].x);
	  else
	    {
	      c_ptr->fval  = corr_floor2.ftval;
	      c_ptr->fopen = corr_floor2.ftopen;
	    }
	}
    }
}


int next_to(y, x)
register int y, x;
{
  int next;

  if (next_to8(y, x, 4, 5, 6) > 2)
    if (((cave[y-1][x].fval >= 10) && (cave[y-1][x].fval <= 12)) &&
	((cave[y+1][x].fval >= 10) && (cave[y+1][x].fval <= 12)))
      next = TRUE;
    else if (((cave[y][x-1].fval >= 10) && (cave[y][x-1].fval <= 12)) &&
	     ((cave[y][x+1].fval >= 10) && (cave[y][x+1].fval <= 12)))
      next = TRUE;
    else
      next = FALSE;
  else
    next = FALSE;
  return(next);
}

/* Places door at y, x position if at least 2 walls found	*/
try_door(y, x)
register int y, x;
{
  if (randint(100) > DUN_TUN_JCT)
    if (cave[y][x].fval == corr_floor1.ftval)
      if (next_to(y, x))
	place_door(y, x);
}


/* Cave logic flow for generation of new dungeon		*/
cave_gen()
{
  struct spot_type
    {
      int endx;
      int endy;
    };
  int room_map[20][20];
  register int i, j, k;
  int y1, x1, y2, x2;
  int pick1, pick2;
  int row_rooms, col_rooms;
  int alloc_level;
  worlint yloc[400];
  worlint xloc[400];

  int set_1_2();
  int set_1_2_4();
  int set_4();

  row_rooms = 2*(cur_height/SCREEN_HEIGHT);
  col_rooms = 2*(cur_width /SCREEN_WIDTH);
  for (i = 0; i < row_rooms; i++)
    for (j = 0; j < col_rooms; j++)
      room_map[i][j] = FALSE;
  for (i = 0; i < randnor(DUN_ROO_MEA, 2); i++)
    room_map[randint(row_rooms)-1][randint(col_rooms)-1] = TRUE;
  k = 0;
  for (i = 0; i < row_rooms; i++)
    for (j = 0; j < col_rooms; j++)
      if (room_map[i][j] == TRUE)
	{
	  yloc[k] = i * (QUART_HEIGHT * 2 + 1) + QUART_HEIGHT;
	  xloc[k] = j * (QUART_WIDTH * 2 + 1) + QUART_WIDTH;
	  if (dun_level > randint(DUN_UNUSUAL))
	    switch(randint(3))
	      {
	      case 1: build_type1(yloc[k], xloc[k]); break;
	      case 2: build_type2(yloc[k], xloc[k]); break;
	      case 3: build_type3(yloc[k], xloc[k]); break;
	      }
	  else
	    build_room(yloc[k], xloc[k]);
	  k++;
	}
  for (i = 0; i < k; i++)
    {
      pick1 = randint(k) - 1;
      pick2 = randint(k) - 1;
      y1 = yloc[pick1];
      x1 = xloc[pick1];
      yloc[pick1] = yloc[pick2];
      xloc[pick1] = xloc[pick2];
      yloc[pick2] = y1;
      xloc[pick2] = x1;
      }
  doorptr = 0;
  for (i = 0; i < k-1; i++)
    {
      y1 = yloc[i];
      x1 = xloc[i];
      y2 = yloc[i+1];
      x2 = xloc[i+1];
      build_tunnel(y2, x2, y1, x1);
      }
  fill_cave(rock_wall1);
  for (i = 0; i < DUN_STR_MAG; i++)
    place_streamer(rock_wall2, DUN_STR_MC);
  for (i = 0; i < DUN_STR_QUA; i++)
    place_streamer(rock_wall3, DUN_STR_QC);
  place_boundary();
  /* Place intersection doors	*/
  for (i = 0; i < doorptr; i++)
    {
      try_door(doorstk[i].y, doorstk[i].x-1);
      try_door(doorstk[i].y, doorstk[i].x+1);
      try_door(doorstk[i].y-1, doorstk[i].x);
      try_door(doorstk[i].y+1, doorstk[i].x);
    }
  alloc_level = (dun_level/3);
  if (alloc_level < 2)
    alloc_level = 2;
  else if (alloc_level > 10)
    alloc_level = 10;
  place_stairs(2, randint(2)+2, 3);
  place_stairs(1, randint(2), 3);
  alloc_monster(set_1_2, (randint(8)+MIN_MALLOC_LEVEL+alloc_level), 0, TRUE);
  alloc_object(set_4, 3, randint(alloc_level));
  alloc_object(set_1_2, 5, randnor(TREAS_ROOM_ALLOC, 3));
  alloc_object(set_1_2_4, 5, randnor(TREAS_ANY_ALLOC, 3));
  alloc_object(set_1_2_4, 4, randnor(TREAS_GOLD_ALLOC, 3));
  alloc_object(set_1_2_4, 1, randint(alloc_level));
  if (dun_level >= WIN_MON_APPEAR)  place_win_monster();
}


/* Builds a store at a row, column coordinate			*/
build_store(store_num, y, x)
int store_num, y, x;
{
  int yval, y_height, y_depth;
  int xval, x_left, x_right;
  register int i, j;
  int cur_pos;
  register cave_type *c_ptr;

  yval	   = y*10 + 5;
  xval     = x*16 + 16;
  y_height = yval - randint(3);
  y_depth  = yval + randint(4);
  x_left   = xval - randint(6);
  x_right  = xval + randint(6);
  for (i = y_height; i <= y_depth; i++)
    for (j = x_left; j <= x_right; j++)
      {
	cave[i][j].fval  = boundary_wall.ftval;
	cave[i][j].fopen = boundary_wall.ftopen;
      }
  switch(randint(4))
    {
    case 1:
      i = randint(y_depth-y_height) + y_height - 1;
      j = x_left;
      break;
    case 2:
      i = randint(y_depth-y_height) + y_height - 1;
      j = x_right;
      break;
    case 3:
      i = y_depth;
      j = randint(x_right-x_left) + x_left - 1;
      break;
    case 4:
      i = y_height;
      j = randint(x_right-x_left) + x_left - 1;
      break;
    }
  c_ptr = &cave[i][j];
  c_ptr->fval  = corr_floor3.ftval;
  c_ptr->fopen = corr_floor3.ftopen;
  popt(&cur_pos);
  c_ptr->tptr = cur_pos;
  t_list[cur_pos] = store_door[store_num];
}


/* Town logic flow for generation of new town		*/
town_gen()
{
  register int i, j, k, l, m;
  int rooms[6];

  int set_1_2();

  set_seed(town_state, town_seed);
  for (i = 0; i < 6; i++)
    rooms[i] = i;
  l = 6;
  for (i = 0; i < 2; i++)
    for (j = 0; j < 3; j++)
      {
	k = randint(l) - 1;
	build_store(rooms[k], i, j);
	for (m = k; m < l-1; m++)
	  rooms[m] = rooms[m+1];
	l--;
      }
  fill_cave(dopen_floor);
  /* make stairs before reset_seed, so that they don't move around */
  place_stairs(2, 1, 0);
  place_boundary();
  reset_seed();
  if (0x1 & (turn / 5000))
    {		/* Night	*/
      for (i = 0; i < cur_height; i++)
	for (j = 0; j < cur_width; j++)
	  if (cave[i][j].fval != dopen_floor.ftval)
	    cave[i][j].pl = TRUE;
      alloc_monster(set_1_2, MIN_MALLOC_TN, 3, TRUE);
    }
  else
    {		/* Day	*/
      for (i = 0; i < cur_height; i++)
	for (j = 0; j < cur_width; j++)
	  cave[i][j].pl = TRUE;
      alloc_monster(set_1_2, MIN_MALLOC_TD, 3, TRUE);
    }
  store_maint();
}


/* Generates a random dungeon level			-RAK-	*/
generate_cave()
{
  panel_row_min	= 0;
  panel_row_max	= 0;
  panel_col_min	= 0;
  panel_col_max	= 0;
  char_row		= -1;
  char_col		= -1;

  tlink();
  mlink();
  blank_cave();

  if (dun_level == 0)
    {
      cur_height = SCREEN_HEIGHT;
      cur_width  = SCREEN_WIDTH;
      max_panel_rows = (cur_height/SCREEN_HEIGHT)*2 - 2;
      max_panel_cols = (cur_width /SCREEN_WIDTH )*2 - 2;
      panel_row = max_panel_rows;
      panel_col = max_panel_cols;
      town_gen();
    }
  else
    {
      cur_height = MAX_HEIGHT;
      cur_width  = MAX_WIDTH;
      max_panel_rows = (cur_height/SCREEN_HEIGHT)*2 - 2;
      max_panel_cols = (cur_width /SCREEN_WIDTH )*2 - 2;
      panel_row = max_panel_rows;
      panel_col = max_panel_cols;
      cave_gen();
    }
}

