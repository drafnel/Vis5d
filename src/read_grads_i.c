/* read_grads.c */

/*
 * Functions for reading GRADS files
 */

/*
 * Vis5D system for visualizing five dimensional gridded data sets.
 * Copyright (C) 1990 - 2000 Bill Hibbard, Johan Kellum, Brian Paul,
 * Dave Santek, and Andre Battaiola.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * As a special exception to the terms of the GNU General Public
 * License, you are permitted to link Vis5D with (and distribute the
 * resulting source and executables) the LUI library (copyright by
 * Stellar Computer Inc. and licensed for distribution with Vis5D),
 * the McIDAS library, and/or the NetCDF library, where those
 * libraries are governed by the terms of their own licenses.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "../config.h"


#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binio.h"
#include "file_i.h"
#include "grid_i.h"
#include "misc_i.h"
#include "proj_i.h"
#include "projlist_i.h"
#include "tokenize_i.h"
#include "../src/v5d.h"



#define MAX(A,B)  ( (A) > (B) ? (A) : (B) )


/* Max chars on input line in CTRL file: */
#define MAXLINE 100



/*
 * Given a time/date string in GrADS format, return it as a date (YYDDD)
 * and time (HHMMSS).
 */
static int parse_time( char *str, int *date, int *time )
{
   int hh, mm, year, day, leap;
   int k;

   /* Extract hours and minutes */
   if (str[2]==':') {
      /* extract hh:mmZ */
      if (!isdigit(str[0]) || !isdigit(str[1])) {
         return 0;
      }
      hh = (str[0] - '0') * 10 + str[1] - '0';
      if (!isdigit(str[3]) || !isdigit(str[4])) {
         return 0;
      }
      mm = (str[3] - '0') * 10 + str[4] - '0';
      if (str[5]!='Z' && str[5]!='z') {
         return 1;
      }
      k = 6;
   }
   else if (str[2]=='Z' || str[2]=='z') {
      /* extract hhZ */
      if (!isdigit(str[0]) || !isdigit(str[1])) {
         return 0;
      }
      hh = (str[0] - '0') * 10 + str[1] - '0';
      mm = 0;
      k = 3;
   }
   else if (str[1]==':') {
      /* extract h:mmZ */
      if (!isdigit(str[0])) {
         return 0;
      }
      hh = str[0] - '0';
      if (!isdigit(str[2]) || !isdigit(str[3])) {
         return 0;
      }
      mm = (str[2] - '0') * 10 + str[3] - '0';
      if (str[4]!='Z' && str[4]!='z') {
         return 0;
      }
      k = 5;
   }
   else if (str[1]=='Z') {
      /* extract hZ */
      if (!isdigit(str[0])) {
         return 0;
      }
      hh = str[0] - '0';
      mm = 0;
      k = 2;
   }
   else {
      hh = mm = 0;
      k = 0;
   }

   *time = 100 * (100 * hh + mm);

   /* Extract day */
   if (isdigit(str[k])) {
      /* day */
      if (isdigit(str[k+1])) {
         day = (str[k] - '0') * 10 + str[k+1] - '0';
         k += 2;
      }
      else {
         day = str[k] - '0';
         k += 1;
      }
   }
   else {
      day = 0;
   }

   /* Extract year from str[k+3] .. str[k+(4 or 6)] */
   if (isdigit(str[k+3]) && isdigit(str[k+4])) {
      if (isdigit(str[k+5]) && isdigit(str[k+6])) {
         year = (str[k+5] - '0') * 10 + str[k+6] - '0';
      }
      else {
         year = (str[k+3] - '0') * 10 + str[k+4] - '0';
      }
   }
   else {
      return 0;
   }

   /* check if leap year */
   leap = ( (year % 4) == 0);

   /* Extract month to increment days */
   if (strncmp(str+k,"jan",3)==0 || strncmp(str+k,"JAN",3)==0) {
      day += 0;
   }
   else if (strncmp(str+k,"feb",3)==0 || strncmp(str+k,"FEB",3)==0) {
      day += 31;
   }
   else if (strncmp(str+k,"mar",3)==0 || strncmp(str+k,"MAR",3)==0) {
      day += 59 + leap;
   }
   else if (strncmp(str+k,"apr",3)==0 || strncmp(str+k,"APR",3)==0) {
      day += 90 + leap;
   }
   else if (strncmp(str+k,"may",3)==0 || strncmp(str+k,"MAY",3)==0) {
      day += 120 + leap;
   }
   else if (strncmp(str+k,"jun",3)==0 || strncmp(str+k,"JUN",3)==0) {
      day += 151 + leap;
   }
   else if (strncmp(str+k,"jul",3)==0 || strncmp(str+k,"JUL",3)==0) {
      day += 181 + leap;
   }
   else if (strncmp(str+k,"aug",3)==0 || strncmp(str+k,"AUG",3)==0) {
      day += 211 + leap;
   }
   else if (strncmp(str+k,"sep",3)==0 || strncmp(str+k,"SEP",3)==0) {
      day += 242 + leap;
   }
   else if (strncmp(str+k,"oct",3)==0 || strncmp(str+k,"OCT",3)==0) {
      day += 272 + leap;
   }
   else if (strncmp(str+k,"nov",3)==0 || strncmp(str+k,"NOV",3)==0) {
      day += 303 + leap;
   }
   else if (strncmp(str+k,"dec",3)==0 || strncmp(str+k,"DEC",3)==0) {
      day += 333 + leap;
   }
   else {
      return 0;
   }

   *date = 1000 * year + day;

   return 1;
}



/*
 * Given a time increment string in GrADS format, return the increment
 * in days and seconds.
 */
static int parse_time_inc( char *inc, int *days, int *seconds )
{
   int i, k;

   i = inc[0] - '0';
   if (isdigit(inc[1])) {
      i = i * 10 + inc[1] - '0';
      k = 2;
   }
   else {
      k = 1;
   }

   if ((inc[k]=='M' && inc[k+1]=='N') || (inc[k]=='m' && inc[k+1]=='n')) {
      *days = 0;
      *seconds = i * 60;
   }
   else if ((inc[k]=='H' && inc[k+1]=='R') || (inc[k]=='h' && inc[k+1]=='r')) {
      *days = 0;
      *seconds = i * 60 * 60;
   }
   else if ((inc[k]=='D' && inc[k+1]=='Y') || (inc[k]=='d' && inc[k+1]=='y')) {
      *days = i;
      *seconds = 0;
   }
   else if ((inc[k]=='M' && inc[k+1]=='O') || (inc[k]=='m' && inc[k+1]=='o')) {
      *days = 30 * i;
      *seconds = 60 * 60 * 10;
   }
   else if ((inc[k]=='Y' && inc[k+1]=='R') || (inc[k]=='y' && inc[k+1]=='r')) {
      *days = 365 * i;
      *seconds = 0;
   }
   else {
      *days = 0;
      *seconds = 0;
      return 0;
   }

   return 1;
}



static void flip_layer( float *data, int rows, int cols, float missing_value )
{
   int i, j;
   float temp[MAXROWS*MAXCOLUMNS];

#define DATA(R,C)  data[R*cols+C]
#define TEMP(R,C)  temp[C*rows+rows-R-1]

   for (i=0;i<rows;i++) {
      for (j=0;j<cols;j++) {

         /* NOTE: floating-point errors on next line probably indicate
            improperly byteswapped data file! How to catch in program??? */
         if (DATA(i,j)==missing_value) {
            TEMP(i,j) = MISSING;
         }
         else {
            TEMP(i,j) = DATA(i,j);
         }
      }
   }

   memcpy( data, temp, rows*cols*sizeof(float) );

#undef DATA
#undef TEMP
}

/**************************************************************************/

/*

  [ The following specification is based on a description by Conrad Poelman,
    and was implemented by S. G. Johnson without reference to the original
    GrADS code. ]

We need a function:
void expand_GrADS_file_template( const char* format, char output[1024],
  int year, int month, int day, int hour, int minute,
  int forecastHour, int initialYear, int initialMonth, int initialDay,
  int initialHour, int initialMinute )
that takes a format string (e.g. "my-file%y2-%mc-%d2-%h2:%n2.grd") and
makes the appropriate format string substitutions to produce an output
string (e.g. "my-file-92-Mar-06-02:15.grd") using the following
conversions:

%y2   -  2 digit year (last 2 digits)
%y4   -  4 digit year
%m1   -  1 or 2 digit month
%m2   -  2 digit month (leading zero if needed)
%mc   -  3 character month abbreviation (lower case)
%mh   -  'a' if in first 15 days of month, and 'b' otherwise
%mH   -  as %mh, but upper case
%d1   -  1 or 2 digit day
%d2   -  2 digit day
%h1   -  1 or 2 digit hour
%h2   -  2 digit hour
%h3   -  3 digit hour (e.g., 120 or 012)
%f2   -  2 or 3 digit forecast hour
%f3   -  3 digit forecast hour
%n2   -  2 digit minute (leading zero if needed)
%ixx  -  as %xx, but using initial time

This list of conversions was taken from the on-line published GrADS
documentation, so I presume it's considered "published," unlike the
source code itself.

If you check it in, you'll find that in Vis5D we don't have a separate
"initial time" and current time or a separate "forecast hour", so when
calling the function for now I guess we'll just have to pass the same
year/month/day/hour/minute for both current and initial time, and pass
the hour as the "forecast hour" - the GrADS documentation doesn't
clearly define what the differences are between all the times.

*/

#define TEMPLATE_OUTPUT_LEN 1024

void expand_GrADS_file_template(const char* format, 
				char output[TEMPLATE_OUTPUT_LEN],
				int year, int month, int day,
				int hour, int minute,
				int forecastHour, int initialYear, 
				int initialMonth, int initialDay,
				int initialHour, int initialMinute)
{
     char buffer[32];
     int i_in = 0, i_out = 0;

     while (format[i_in] && i_out < TEMPLATE_OUTPUT_LEN-1) {
	  if (format[i_in] != '%')
	       output[i_out++] = format[i_in++];
	  else {
	       int iY, iM, iD, ih, im, n;
	       
	       i_in++;
	       if (format[i_in] == 'i') {
		    iY = initialYear; iM = initialMonth; iD = initialDay;
		    ih = initialHour; im = initialMinute;
		    ++i_in;
	       }
	       else {
		    iY = year; iM = month; iD = day; ih = hour; im = minute;
	       }
	       if (!format[i_in] || !format[i_in+1])
		    goto error;

	       switch (format[i_in]) {
		   case 'y': n = iY; break;
		   case 'm': n = iM; break;
		   case 'd': n = iD; break;
		   case 'h': n = ih; break;
		   case 'f': n = forecastHour; break;
		   case 'n': n = im; break;
		   default: goto error;
	       }

	       switch (format[i_in+1]) {
		   case '1':
			sprintf(buffer, "%d", n % 100);
			break;
		   case '2':
			sprintf(buffer, "%02d", n %
				(format[i_in] == 'f' ? 1000 : 100));
			break;
		   case '3':
			sprintf(buffer, "%03d", n % 1000);
			break;
		   case '4':
			sprintf(buffer, "%04d", n % 10000);
			break;
		   case 'h': case 'H':
			if (format[i_in] == 'm') {
			     buffer[1] = 0;
			     if (iD <= 15)
				  buffer[0] = (format[i_in+1]=='h' ? 'a':'A');
			     else
				  buffer[0] = (format[i_in+1]=='h' ? 'b':'B');
			}
			else
			     goto error;
		   case 'c':
			if (format[i_in] == 'm') {
			     const char monthAbbrevs[][4] = {
				  "jan", "feb", "mar", "apr", "may", "jun",
				  "jul", "aug", "sep", "oct", "nov", "dec"
			     };
			     strcpy(buffer, monthAbbrevs[(iM-1) % 12]);
			}
			else
			     goto error;
			break;
		   default:
			goto error;
	       }
	       i_in += 2;
	       n = strlen(buffer);
	       if (i_out + n < TEMPLATE_OUTPUT_LEN) {
		    output[i_out] = 0;
		    strcat(output, buffer);
		    i_out += n;
	       }
	       else
		    goto error;
	  }
     }
     output[i_out] = 0;

     return;

 error:
     fprintf(stderr,"vis5d+: invalid GrADS file template: %s\n", format);
     exit(EXIT_FAILURE);
}

/**************************************************************************/



void julian2mmdd( int inYYYY, int inDDD,
	          int* outMM, int* outDD )
{
  static int days_per_month[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  /* A year is a leap year if it is divisible by 4 but not by 100,
     except that years divisible by 400 ARE leap years. */
  if ( ((inYYYY % 4)==0) && (((inYYYY % 100)!=0)||(inYYYY % 400)==0)) {
    days_per_month[1] = 29;     /* leap year */
  } else {
    days_per_month[1] = 28;
  }

  /* Iteratively increase *outMM while decreasing *outDD until it's
     less than or equal to days_per_month[*outMM - 1]. */
  *outMM = 1;
  *outDD = inDDD;
  while( *outMM <= 12 && *outDD > days_per_month[*outMM - 1] ) {
    /* Subtract off number of days in this month and move on to the next. */
    *outDD -= days_per_month[*outMM - 1];
    (*outMM)++;
  }
}

/* Convert a 2-digit year to a 4-digit year.
   This should be placed in a general utility file so it can be used
   in more places, so the hack will be in only one place. */
int yy2yyyy( int inYearYY )
{
  /* Eventually check the current century to make a better decision. */
  if ( inYearYY > 50 ) {
    return inYearYY+1900;
  } else {
    return inYearYY+2000;
  }
}

/*
 * Scan the named file, createing a grid_info struct for each grid.  Store
 * the grid_info structs in the grid data base.
 * Input:  name - name of grads file.
 *         db - the grid data base
 * Return:  number of grids found.
 */
int get_grads_info( char *name, struct grid_db *db )
{
   FILE *f;
   char **token;
   int ntokens;
   int time, var;
   int grids = 0;
   struct grid_info *info;
   int pos = 0;
   int fileheader_size = 0;
   float args[1000];
   struct projection *proj;
   struct vcs *vcs[MAXVARS];

   /* GrADS file info: */
   char dset_name[1000];
   float missing_value;
   int byteswapped = 0;
   int nr, nc, nl[MAXLEVELS], maxnl;
   int vertical;
   float westbound, northbound, bottombound;
   float rowinc, colinc, levinc;
   float height[MAXLEVELS];
   char varname[MAXVARS][100];
   int timestamp[MAXTIMES], datestamp[MAXTIMES];
   int numtimes, numvars;
   int projection_type = PROJ_LINEAR;
   int use_file_template = 0;
   char specific_file_name[TEMPLATE_OUTPUT_LEN];

   /* Initialize args */
   args[0] = args[1] = args[2] = args[3] = args[4] = 0.0f;

   f = fopen( name, "r" );
   if (!f) {
      printf("Error: couldn't open %s\n", name );
      return 0;
   }
   

   while (1) {
      char line[MAXLINE];
      /* read a line of the control file */
      if (!fgets( line, MAXLINE, f ))
         break;

      /* break line into tokens */
      token = tokenize( line, &ntokens );

      if (ntokens==0) continue;

      if (strcasecmp(token[0],"DSET")==0) {
         if (ntokens<2) {
            printf("Error: missing filename argument on DSET line\n");
         }
         else {
            if (token[1][0]=='^') {

               /* skip the ^ (current directory) character */
               /* This is not the current directory, rather it is the same 
                  directory in which the control file is located */
              int i;
              for(i=strlen(name)-1;i>=0;i--){
                if(name[i] == '/'){
                  strncpy(dset_name,name,i);
                  i++;
                  break;
                }
              } 
              if(i<0) i=0; /* No path in ctl file name */
              strcpy( dset_name+i, token[1]+1 );
            }
            else {
               strcpy( dset_name, token[1] );
            }
         }
      }
      else if (strcasecmp(token[0],"TITLE")==0) {
         /* ignore the title line */
      }
      else if (strcasecmp(token[0],"UNDEF")==0) {
         missing_value = atof(token[1]);
      }
      else if (strcasecmp(token[0],"BYTESWAPPED")==0) {
         /* Is this valid??? Shouldn't it be "OPTIONS BYTESWAPPED" ? */
         byteswapped = 1;
      }
      else if (strcasecmp(token[0],"FORMAT")==0) {
         if (strcasecmp(token[1],"SEQUENTIAL")==0) {
               /* this is the only format currently supported; */
               /* also note: FORMAT keyword has been replaced by OPTIONS */
         }
         else {
            printf("Warning: FORMAT not fully supported\n");
            printf("         only SEQUENTIAL format is allowed.\n");
         }
      }
      else if (strcasecmp(token[0],"OPTIONS")==0) {
         if (strcasecmp(token[1],"SEQUENTIAL")==0) {
            /* Don't need to do anything, supported by default??? */
         }
         else if (strcasecmp(token[1],"BYTESWAPPED")==0) {
            byteswapped=1;
         }
         else if (strcasecmp(token[1],"TEMPLATE")==0) {
            use_file_template=1;
         }
#ifdef WORDS_BIGENDIAN
	      else if (strcasecmp(token[1],"LITTLE_ENDIAN")==0 ) {
           byteswapped=1;
         }
#else
	      else if (strcasecmp(token[1],"BIG_ENDIAN")==0  ) {
           byteswapped=1;
         }
#endif
         else {
            printf("Warning: OPTIONS %s not supported\n", token[1]);
         }
      }
      else if (strcasecmp(token[0],"FILEHEADER")==0) {
         if (ntokens<2) {
            printf("Error: missing position argument on FILEHEADER line\n");
         }
         fileheader_size = atoi( token[1] );
         pos = fileheader_size;
      }
      else if (strcasecmp(token[0],"XDEF")==0) {
         if (ntokens<4) {
            printf("Error: missing arguments to XDEF line\n");
         }
         else {
            nc = atoi( token[1] );
            if (strcasecmp(token[2],"LINEAR")==0) {
               westbound = -atof( token[3] );
               colinc = atof( token[4] );
            }
            else if (strcasecmp(token[2],"LEVELS")==0) {
               printf("Warning: XDEF LEVELS not fully supported\n");
               westbound = -atof( token[3] );
               colinc = fabs( atof(token[3]) - atof(token[4]) );
            }
            else {
               printf("Warning: XDEF %s not fully supported\n", token[2]);
            }
         }
      }
      else if (strcasecmp(token[0],"YDEF")==0) {
         if (ntokens<4) {
            printf("Error: missing arguments to YDEF line\n");
         }
         else {
            nr = atoi( token[1] );
            if (strcasecmp(token[2],"LINEAR")==0) {
               float southbound = atof( token[3] );
               rowinc = atof( token[4] );
               northbound = southbound + rowinc * (nr-1);
            }
            else if (strncmp(token[2],"GAUSR",5)==0
                     || strncmp(token[2],"gausr",5)==0) {
               printf("Warning: YDEF GAUSRnn not supported\n");
            }
            else {
               printf("Warning: YDEF %s not fully supported\n", token[2]);
            }
         }
      }
      else if (strcasecmp(token[0],"PDEF")==0) {
         if (ntokens<4) {
            printf("Error: missing arguments to PDEF line\n");
         }
         else {
            nc = atoi( token[1] );
            nr = atoi( token[2] );
            if (strcasecmp(token[3],"PSE")==0) {
               if (ntokens<11) {
                  printf("Error: missing arguments to PDEF line\n");
                  printf("'PDEF ... pse ...' must have 10 arguments\n");
               }
               else {
                  double pseLat       = atof( token[4] );    /* degrees */
                  double pseLon       = atof( token[5] );    /* degrees */
                  double pseCenterCol = atof( token[6] );    /* grid units */
                  double pseCenterRow = atof( token[7] );    /* grid units */
                  double pseDeltaX    = atof( token[8] );    /* in km */
                  double pseDeltaY    = atof( token[9] );    /* ignored */
                  int    pseNorthOrSouth = atoi( token[10] );/* +1 or -1 */

                  /* grads documentation describes this format as 
                     "high accuracy polar stereo (eccentric)" while vis5d
                     calls it "azimuthal stereographic". The conversions
                     performed here are somewhat cryptic, and were obtained
                     with some combination of reading the documentation and
                     trial and error. */
                  /* center latitude. NOT TESTED FOR SOUTH POLAR PROJECTION! */
                  args[0] = pseNorthOrSouth * 90.0;

                  /* center longitude. Why minus 270? Seems to work though. */
                  args[1] = pseLon - 270.0;
                  /* put longitude in the range -180 to +180 */
                  args[1] = fmod( args[1], 360.0 );
                  if ( args[1] > 180.0 ) args[1] -= 360.0;
                  if ( args[1] <-180.0 ) args[1] += 360.0;

                  /* center grid row. Why the minus sign and nr-1? */
                  args[2] = nr - ( pseCenterRow + 1 );

                  /* center grid column */
                  args[3] = pseCenterCol;

                  /* column inc. in km, GrADS gives column increment at this
                     lattitude, we need column increment at the pole. */
                  args[4] = pseDeltaX * ( 1.0 + sin( 90/57.29578 ) ) /
                                        ( 1.0 + sin( pseLat/57.29578 ) );

                  /* what to do with token[9] - row inc. in km??? */
                  /* what to do with token[10] - 1 is N pole, -1 is S pole???*/
                  projection_type = PROJ_STEREO;
               }
            }
            else {
               printf("Warning: \"PDEF ... %s ...\" not supported\n", token[3] );
            }
         }
      }
      else if (strcasecmp(token[0],"ZDEF")==0) {
         if (ntokens<3) {
            printf("Error: missing arguments to ZDEF line\n");
         }
         else {
            float pressure[MAXLEVELS];
            int i;
            maxnl = atoi( token[1] );
            if (strcasecmp(token[2],"LINEAR")==0) {
               vertical = VERT_EQUAL_KM;
               bottombound = atof( token[3] );
               levinc = atof( token[4] );
               for (i=0;i<maxnl;i++) {
                  pressure[i] = bottombound + i * levinc;
               }
            }
            else if (strcasecmp(token[2],"LEVELS")==0) {
               vertical = VERT_UNEQUAL_MB ; /*VERT_UNEQUAL_KM;*/
               for (i=0;i<maxnl && i+3<ntokens;i++) {
                  pressure[i] = atof( token[3+i] );
                  height[i] = pressure_to_height(pressure[i]);
               }
            }

         }

      }
      else if (strcasecmp(token[0],"TDEF")==0) {
         if (ntokens!=5) {
            printf("Error: missing arguments to TDEF line\n");
         }
         else {
            int date0, time0, days, seconds;
            int i, it, id, ii;

            numtimes = atoi( token[1] );
            /* token[2]=="LINEAR" */

            if (!parse_time( token[3], &date0, &time0 )) {
               printf("Error reading grads header, bad time: %s\n", token[3] );
            }
            if (!parse_time_inc( token[4], &days, &seconds )) {
               printf("Error reading grads header, bad time increment: %s\n",
                      token[4] );
            }

            id = v5dYYDDDtoDays(date0);
            it = v5dHHMMSStoSeconds(time0);
            for (i=0;i<numtimes&&i<MAXTIMES;i++) {
               timestamp[i] = v5dSecondsToHHMMSS(it);
               datestamp[i] = v5dDaysToYYDDD(id);
               it = it + seconds;
               ii = it / 86400;
               it = it - 86400 * ii;
               id = id + days + ii;
            }
         }
      }
      else if (strcasecmp(token[0],"VARS")==0) {
         /* TODO: variables */
         if (ntokens!=2) {
            printf("Error: wrong number of arguments to VARS line\n");
         }
         else {
            char **vartok;
            int i, ntok;

            numvars = atoi( token[1] );

            for (i=0;i<numvars;i++) {
               fgets( line, MAXLINE, f );
               vartok = tokenize( line, &ntok );
               strcpy( varname[i], vartok[0] );   /* var name */
               nl[i] = atoi( vartok[1] );         /* number of levels */
               if (nl[i]==0)  nl[i] = 1;
               /* vartok[2] = units */
               /* vartok[3] = text description */
               free_tokens( vartok );
            }
         }
      }
      else if (strcasecmp(token[0],"ENDVARS")==0){
         /* ignore */
      }
      else {
         printf("Warning: unknown token: %s\n",token[0] );
      }

   } /*while*/


   /*
    * Generate grid_info structs
    */

   if ( projection_type == PROJ_LINEAR ) {

      args[0] = northbound;
      args[1] = westbound;
      args[2] = rowinc;
      args[3] = colinc;
      proj = new_projection( db, PROJ_LINEAR, nr, nc, args );
   }
   else if ( projection_type == PROJ_STEREO ) {
      /* args have already been set properly. */
      proj = new_projection( db, PROJ_STEREO, nr, nc, args );
   }

   /* Potentially different vcs for each grid because number of levels */
   /* can vary per variable. */
   for (var=0;var<numvars;var++) {
      if (vertical==VERT_EQUAL_KM) {
         args[0] = bottombound;
         args[1] = levinc;
      }
      else {
         int i;
         for (i=0;i<maxnl;i++) {
            args[i] = height[i];
         }
      }
      vcs[var] = new_vcs( db, vertical, nl[var], 0, args );
   }

   if ( numtimes > MAXTIMES ) {
      printf("Warning: %d is too many time steps, %d is limit.\n",
             numtimes, MAXTIMES );
   }
   for (time=0;time<numtimes&&time<MAXTIMES;time++) {
      if ( use_file_template ) {
         int yr, mo, dy, hr, mn, jday;
         char prev_file_name[1000];

         /* Save the old file name so we can see if it has changed. */
         strcpy( prev_file_name, specific_file_name );

         /* Replace FileName, which may be a format string, with result
            of expanding the file format string. */
         yr = yy2yyyy( datestamp[time] / 1000 );
         jday = datestamp[time] - 1000*(datestamp[time] / 1000);
         julian2mmdd( yr, jday, &mo, &dy );
         hr = timestamp[time] / 10000;
         mn = timestamp[time] / 100 - hr*100;
	 expand_GrADS_file_template(dset_name, specific_file_name,
				    yr, mo, dy, hr, mn,
				    hr, yr, mo, dy, hr, mn);

         if ( strcmp( prev_file_name, specific_file_name ) != 0 ) {
            /* Changing files - reset pos to the beginning of the file */
            pos = fileheader_size;
         }
         printf("In get_grads_info, date:%05d, time:%06d, file:%s\n",
                 datestamp[time], timestamp[time], specific_file_name );
      } else {
         strcpy( specific_file_name, dset_name );
      }

      for (var=0;var<numvars;var++) {

         info = alloc_grid_info();


         info->FileName = strdup( specific_file_name );
         info->Format = FILE_GRADS;
         info->TimeStep = time;
         info->VarNum = var;
         info->Position = pos;         /* pos. of data in binary grid file */
         pos += nr * nc * nl[var] * 4;

         info->Nr = nr;
         info->Nc = nc;
         info->Nl = nl[var];

         info->DateStamp = datestamp[time];
         info->TimeStamp = timestamp[time];
         info->VarName = strdup( varname[var] );

         info->Proj = proj;
         info->Vcs = vcs[var];

         info->MissingValue = missing_value;
         info->byteswapped = byteswapped;

         append_grid( info, db );
         grids++;

      }

   }

   return grids;
}



/*
 * Get the grid data described by g.
 * Input:  g - pointer to a grid_info structure.  It tells us which grid
 *             in which file is needed.
 * Return:  pointer to grid data (can be free()'d when finished)
 *          OR return NULL if there's an error.
 */
float *get_grads_data( struct grid_info *g )
{
   int f, n, nread;
   float *data;

   f = open( g->FileName, O_RDONLY );
   if (f<0) {
      printf("Error: couldn't open %s\n", g->FileName );
      return NULL;
   }

   if (lseek( f, g->Position, SEEK_SET )!=g->Position) {
      printf("Error: couldn't get  GrADS data for time %d, var %s\n",
             g->TimeStep, g->VarName );
      close(f);
      return NULL;
   }

   n = g->Nr * g->Nc * g->Nl;
   data = (float *) malloc( n * sizeof(float) );

   nread = read_float4_array( f, data, n );

   if (nread<n) {
      printf("Error: couldn't read GrADS data for time %d, var %s\n",
             g->TimeStep, g->VarName );
      free( data );
      close(f);
      return NULL;
   }
   else {
      int i;
      int should_swap_bytes_here;

      /* This can be confusing. read_float4_array() automatically performs
         byte-swapping on non-BIGENDIAN machines. So if we're on a
         non-BIGENDIAN machine and we asked for byte-swapping, do nothing;
         but if we didn't ask for byte swapping, then swap them back!
         On BIGENDIAN machines, behave normally. */
#ifdef WORDS_BIGENDIAN
      should_swap_bytes_here = g->byteswapped;
#else
      should_swap_bytes_here = !g->byteswapped;
#endif

      if (should_swap_bytes_here) {
        flip4((const unsigned int *) data, (unsigned int*) data, nread);
      }
      /* flip data */
      for (i=0;i<g->Nl;i++) {
         flip_layer( data + i * g->Nr * g->Nc, g->Nr, g->Nc, g->MissingValue );
      }
   }
   close(f);

   return data;
}

