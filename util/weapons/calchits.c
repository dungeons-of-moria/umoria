/* util/weapons/calchits.c: calculates relative weapon effectiveness

   Copyright (c) 1989-1992 Wayne Schlitt, James E. Wilson

   This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>

#define	max(a,b)	( (a) > (b) ? (a) : (b) )
#define min(a,b)	( (a) < (b) ? (a) : (b) )
#define fabs(a)		( (a) < 0 ? -(a) : (a) )

char blows_table[7][6] = {
/* STR/W:	   9  18  67 107 117 118   : DEX */
/* <2 */	{  1,  1,  1,  1,  1,  1 },
/* <3 */	{  1,  1,  1,  1,  2,  2 },
/* <4 */	{  1,  1,  1,  2,  2,  3 },
/* <5 */	{  1,  1,  2,  2,  3,  3 },
/* <7 */	{  1,  2,  2,  3,  3,  4 },
/* <9 */	{  1,  2,  2,  3,  4,  4 },
/* >9 */	{  2,  2,  3,  3,  4,  4 }
};


/* Weapon weight VS strength and dexterity		-RAK-	*/
void attack_blows(weight, cstr, cdex, max_blows, real_blows, adj_weight )
    int weight, cstr, cdex;
    int *max_blows, *real_blows;
    int *adj_weight;
{
  int dex_index, str_index;

    if ((cstr*15) < weight)
    {
	*max_blows = 0;
	*real_blows = 0;
	*adj_weight = 0.0;
    }
    else
    {
	if      (cdex <  10)  dex_index = 0;
	else if (cdex <  19)  dex_index = 1;
	else if (cdex <  68)  dex_index = 2;
	else if (cdex < 108)  dex_index = 3;
	else if (cdex < 118)  dex_index = 4;
	else                  dex_index = 5;
	*adj_weight = ((cstr*10)/weight);
	if      (*adj_weight < 2)  str_index = 0;
	else if (*adj_weight < 3)  str_index = 1;
	else if (*adj_weight < 4)  str_index = 2;
	else if (*adj_weight < 5)  str_index = 3;
	else if (*adj_weight < 7)  str_index = 4;
	else if (*adj_weight < 9)  str_index = 5;
	else                       str_index = 6;
	*max_blows = blows_table[6][dex_index];
	*real_blows = blows_table[str_index][dex_index];
    }

}


#define calc_chance(low,high,weight) \
    max( 0., min( (high - low), \
		 (high - low) - (high - (650. + weight)) \
		 ) \
	) / 650.


/* Critical hits, Nasty way to die...			-RAK-	*/
int critical_blow(weight, plus, dam, player_level)
    int weight, player_level;
    double plus, dam;
{
    int critical;

    int		added_dam;
    double	chance_critical, chance_dam, total_chance;

    int		debug = 0;


    critical = dam;
    chance_critical = ( (int)(weight+5*plus+3*player_level) ) / 5000.;

    if( debug ) printf( "chance_critical=%g  chance_dam=(", chance_critical );

    chance_dam = calc_chance( weight, 400., weight );
    if( debug ) printf( "%g", chance_dam );
    total_chance = chance_dam;
    added_dam  = (2*dam +  5) * chance_dam;

    chance_dam = calc_chance( 400., 700., weight );
    if( debug ) printf( "+%g", chance_dam );
    total_chance += chance_dam;
    added_dam += (3*dam + 10) * chance_dam;

    chance_dam = calc_chance( 700., 900., weight );
    if( debug ) printf( "+%g", chance_dam );
    total_chance += chance_dam;
    added_dam += (4*dam + 15) * chance_dam;

    chance_dam = calc_chance( 900., 2000., weight );
    if( debug ) printf( "+%g)  ", chance_dam );
    total_chance += chance_dam;
    added_dam += (5*dam + 20) * chance_dam;

    if( debug ) printf( "added_dam = %d\n", added_dam );

    if( fabs( total_chance - 1. ) > 1e-14 )
    {
	printf( "ERROR:  total_chance=%g (%g)\n",
	       total_chance, total_chance - 1. );
	exit( 1 );
    }

    return( critical + added_dam * chance_critical );

}


int get_stat( stat )
    int		*stat;
{
    int		tmp, c;


    if( 1 != scanf( " %d", &tmp ) )
	return( 0 );

    *stat = tmp;

    if( tmp == 18 )
    {
	c = getchar();
	if( c == '/' )
	{
	    if( 1 != scanf( "%d", &tmp ) )
		return( 0 );
	    *stat += tmp;
	}
	else
	    ungetc(c, stdin);

    }

    return( 1 );
}





#define MWEAPONS 100

int main()
{
    int		cstr, cdex, player_level;

    int		weight[MWEAPONS];
    double	to_hit[MWEAPONS], to_dam[MWEAPONS], weapon_dam[MWEAPONS];

    int		num_weapons, i;

    int		max_blows, real_blows;
    int		adj_weight;
    double	avg_dam, critical;

    int		num_die, die_sides;


    printf( "Enter level of your character: " );
    if( 1 != scanf( " %d", &player_level ) )
    {
	putchar( '\n');
	exit( 1 );
    }


    printf( "Enter number of weapons: " );
    if( 1 != scanf( " %d", &num_weapons ) )
    {
	putchar( '\n');
	exit( 1 );
    }


    for( i = 0; i < num_weapons; i++ )
    {
	printf( "Enter weight, weapon_dam, to_hit, to_dam for weapon #%d: ",
	       i );
	if( 5 != scanf( " %d %dd%d %lf %lf",
		       &weight[i], &num_die, &die_sides,
		       &to_hit[i], &to_dam[i] )
	   )
	{
	    putchar( '\n');
	    exit( 1 );
	}

	weapon_dam[i] = ( num_die * die_sides + num_die )/2.;
    }


    while( 1 )
    {
	printf( "Enter cstr, cdex: " );
	if( !get_stat( &cstr ) || !get_stat( &cdex ) )
	{
	    putchar( '\n');
	    exit( 1 );
	}


	if( cstr == 0 && cdex == 0 )
	    break;

	printf( "  Weapon | Max blows | Blows | weight ratio | hp of dam |");
	printf( " w/ critical |\n" );

	for( i = 0; i < num_weapons; i++ )
	{
	    attack_blows( weight[i], cstr, cdex,
			 &max_blows, &real_blows, &adj_weight );
#if 0
	    /* multiply to dam bonuses by max/real number of hits */
	    if (real_blows != 0)
	      avg_dam = (int) (real_blows*(weapon_dam[i]+
				 (max_blows * (int)to_dam[i] / real_blows)));
	    else
	      avg_dam = 0;
	    critical = critical_blow( weight[i], to_hit[i], avg_dam,
				     player_level );
#else
	    /* this is the old way, to_dam added after critical */
	    avg_dam = (int) (real_blows*weapon_dam[i]);
	    critical = critical_blow( weight[i], to_hit[i], avg_dam,
				     player_level );
	    avg_dam += (int) real_blows*to_dam[i];
	    critical += (int) real_blows*to_dam[i];
#endif

	    printf( "  %4d   | %6d    | %4d  | %8d     | %6g    |  %7g    |\n",
		   i, max_blows, real_blows, adj_weight, avg_dam, critical );
	}
	printf( "\n" );
    }
    return( 0 );
}

