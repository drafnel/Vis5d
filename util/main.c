/* main.c */
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#ifdef HAVE_SYS_LOCK_H
#  include <sys/lock.h>
#endif
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>

char cc[1000];
int Argc;
char *Argv[100];   /* up to 100 arguments (converted to uppercase) */
char *Argvl[100];   /* up to 100 arguments (in unchanged case) */
char **Argvector;



extern int lwopen( char *, int );
extern int F77_FUNC(main0,MAIN0)(void);



/***************************************************************************/
main(argc,argv)
int argc;
char *argv[];
{
   int i;

#ifdef HAVE_SETRLIMIT
   {
	/* disable core dumps */
	struct rlimit rl;
	rl.rlim_cur = rl.rlim_max = 0;
	setrlimit( RLIMIT_CORE, &rl );
   }
#endif

   Argc = argc;
   Argvector = argv;

   /* Convert arguments to uppercase and store in Argv */
   for (i=0; i<argc; i++) {
      char *cp;
      int len;
      len = strlen( argv[i] );
      Argv[i] = (char *) malloc( len + 1 );
      Argvl[i] = (char *) malloc( len + 1 );
      strcpy( Argv[i], argv[i] );
      strcpy( Argvl[i], argv[i] );
      for (cp=Argv[i]; *cp; cp++) {
         if (*cp >= 'a' && *cp <= 'z')
            *cp = *cp + 'A' - 'a';
      }
   }

   i=F77_FUNC(main0,MAIN0)();
   printf("%s done\n", Argv[0] );
   return(0);
}



/***************************************************************************/
#define SUBSTR(sub, str) (memcmp(sub, str, strlen(sub)) ? 0 : 1)

F77_FUNC(nkwp,NKWP)(ckw,lenkw)
int lenkw;
char *ckw;
{
  char ck[20],*cmd;
  int i,j,n;
  strncpy(ck,ckw,lenkw);
  ck[lenkw]=0;

  if (strcmp(ck, " ") == 0) {
    j = 1;
    n = 0;
    cmd = Argv[j];
    while (j < Argc) {
      if (cmd[0] == '-' && (cmd[1] < '0' || cmd[1] > '9'))
        break;
      cmd = Argv[++j];
      n++;
    }
  }
  else {
    n = 0;
    for (i = 1; i < Argc; i++) {
      cmd = Argv[i];
      if (cmd[0] == '-' && (cmd[1] < '0' || cmd[1] > '9') &&
                           SUBSTR(cmd + 1, ck)) {
        j = i + 1;
        n = 0;
        cmd = Argv[j];
        while (j < Argc) {
          if (cmd[0] == '-' && (cmd[1] < '0' || cmd[1] > '9'))
            break;
          cmd = Argv[++j];
          n++;
        }
        if (n == 0) n = 1;
        break;
      }
    }
  }
  return(n);
}



/*
 * Return a string command line argument.
 * Input: arg - argument number
 * Output: str - null terminated string
 *         len - length of str
 */
F77_FUNC(strarg,STRARG)( result, resultmax, arg )
char *result;
int resultmax, *arg;
{
   strncpy( result, Argvl[*arg], resultmax );
}


/* cres = result string, len = length limit for cres
   ckw = keyword string, of length lenkw
   ip = parameter # after keyword
   cin = default value of length lenc
   NOTE strings are FORTRAN strings, so no
   null termination and blank padded to length
*/
F77_FUNC(ckwp,CKWP)(cres,len,ckw,ip,cin,lenkw,lenc)
int *ip,len,lenkw,lenc;
char *cres,*ckw,*cin;
{
  char *d,c[20],ck[20],*cmd;
  int i,j,k,n;
/* printf("cpp: %d %d %s %d\n",len,ip[0],cin,lenc); */
  strncpy(c,cin,lenc);
  c[lenc]=0;
  strncpy(ck,ckw,lenkw);
  ck[lenkw]=0;

  d=c;
  if (strcmp(ck, " ") == 0) {
    j = 1;
    n = 1;
    cmd = Argv[j];
    while (j < Argc) {
      if (cmd[0] == '-' && (cmd[1] < '0' || cmd[1] > '9'))
        break;
      if (n == ip[0]) {
        d = Argv[j];
        break;
      }
      cmd = Argv[++j];
      n++;
    }
  }
  else {
    for (i = 1; i < Argc; i++) {
      cmd = Argv[i];
      if (cmd[0] == '-' && (cmd[1] < '0' || cmd[1] > '9') &&
                           SUBSTR(cmd + 1, ck)) {
        j = i + 1;
        n = 1;
        cmd = Argv[j];
        while (j < Argc) {
          if (cmd[0] == '-' && (cmd[1] < '0' || cmd[1] > '9'))
            break;
          if (n == ip[0]) {
            d = Argv[j];
            break;
          }
          cmd = Argv[++j];
          n++;
        }
        break;
      }
    }
  }
  k=strlen(d);
  if (len<k)
    k=len;
  for (j=0; j<k; j++)
    cres[j]=d[j];
  for (j=k; j<len; j++)
    cres[j]=' ';
  return(0);
}



/****************************************************************************/


#ifndef WORDS_BIGENDIAN
/*
 * Flip the order of the 4 bytes in an array of 4-byte words.
 */
static void flip_words( src, dest, nwords )
unsigned int *src, *dest;
int nwords;
{
   int i;

   for (i=0;i<nwords;i++) {
      register unsigned int n = src[i];

      dest[i] = (n & 0xff000000) >> 24
              | (n & 0x00ff0000) >> 8
              | (n & 0x0000ff00) << 8
              | (n & 0x000000ff) << 24;
   }
}
#endif


#ifndef WORDS_BIGENDIAN
   /* buffer used for byte-swapping: */
#  define TEMP_SIZE 10000
   static unsigned int Temp[TEMP_SIZE];
#endif



#define NFILES 8
int ld=0,nlw=0;
char f[NFILES][256];
int fd[NFILES];


/*
 * Read an array of 4-byte words from a file.
 * Input:  cf - filename
 *         iw - position to read in file (in words)
 *         nw - number of words to read
 *         ibuf - address of buffer
 *         length - length of cf string
 * Return:  0 = ok
 *          -1 = error opening file
 *          -2 = error seeking
 *          -3 = error reading
 */
F77_FUNC(lwi,LWI)(cf,iw,nw,ibuf,length)
int *iw,*nw,*ibuf,length;
char *cf;
{
   int nbytes,ifile,iword,nwords,len,kword;
   long offset;
   char cfile[256];
   len = length;
   strncpy(cfile,cf,len);
   cfile[len]=0;
   len = len-1;
   while (cfile[len] == ' ' && len > 0) {
      cfile[len] = 0;
      len = len-1;
   }
   iword=iw[0];
   nwords=nw[0];
   if ( (ifile=lwopen(cfile, 1)) <0) {
      for ( kword = 0; kword < nwords; kword++)
         ibuf[kword] = 0x80808080;
      return(-1);
   }
   offset=4*iword;
  
   if (lseek(ifile,offset,0)<0) {
      for ( kword = 0; kword < nwords; kword++)
         ibuf[kword] = 0x80808080;
      return(-2);
   }

   nbytes=4*nwords;
   if (read(ifile,ibuf,nbytes)<=0) {
      for ( kword = 0; kword < nwords; kword++)
         ibuf[kword] = 0x80808080;
        return(-3);
   }
#ifndef WORDS_BIGENDIAN
   flip_words( ibuf, ibuf, nwords );
#endif
   return(0);
}


/*
 * Write an array of 4-byte words to a file.
 * Input: cf - pointer to filename
 *        iw - file position in words
 *        nw - number of words to write.
 *        ibuf - address of first work to write.
 *        length - length of cf string.
 * Return:  0 = ok
 *          -1 = unable to open file
 *          -2 = seek failed
 *          -3 = write failed
 */
F77_FUNC(lwo,LWO)(cf,iw,nw,ibuf,length)
int *iw,*nw,length;
char *cf,*ibuf;
{
   int nbytes,ifile,iword,nwords,len,kword;
   int nblocks, iblk;
   long offset,filelen, gapword;
   char cfile[256];
   long int nullbuf[512];
   static int nbuf = 512;

   /* put filename into cfile and remove trailing spaces */
   len = length;
   strncpy(cfile,cf,len);
   cfile[len]=0;
   len = len-1;
   while (cfile[len] == ' ' && len > 0) {
      cfile[len] = 0;
      len = len-1;
   }

   iword=iw[0];
   nwords=nw[0];
   if ( (ifile=lwopen(cfile, 0)) <0)
      return(-1);
   offset=4*iword;
   filelen = lseek(ifile, (long) 0, 2);
   /* check to see if we write past eof */
   if (offset > filelen) {
      for (kword = 0; kword < nbuf; kword++) 
         nullbuf[kword] = 0x80808080;
      gapword = (offset - filelen)/4;
      nblocks = gapword/nbuf;
      if( nblocks != 0) {
          nbytes = nbuf * 4;
          for ( iblk = 0; iblk < nblocks; iblk++)
              write(ifile, nullbuf, nbytes);
      }
      nbytes = 4 * (gapword % nbuf);
      if ( nbytes != 0 )
          write(ifile, nullbuf, nbytes);
   }

   /* seek to file positio and write buffer */
   if (lseek(ifile,offset,0)<0)
      return(-2);
   nbytes=4*nwords;
#ifndef WORDS_BIGENDIAN
   if (nwords>TEMP_SIZE) {
      printf("Internal error in LWO, increase TEMP_SIZE\n");
      exit(0);
   }
   flip_words( ibuf, Temp, nwords );
   if (write(ifile,Temp,nbytes)<0)
      return(-3);
#else
   if (write(ifile,ibuf,nbytes)<0)
      return(-3);
#endif
   return(0);
}



/*
 * Read an array of bytes from a file.
 * Input:  cf - filename
 *         ib - offset of first byte to read in file
 *         nb - number of bytes to read
 *         ibuf - address to buffer to store bytes
 *         length - length of cf string
 * Return:  0 = ok
 *          -1 = unable to open file
 *          -2 = seek failed
 *          -3 = write failed
 */
F77_FUNC(lbi,LBI)(cf,ib,nb,ibuf,length)
int *ib,*nb,length;
char *cf,*ibuf;
{
   int nbytes,ifile,len;
   long offset;
   char cfile[256];
   len = length;
   strncpy(cfile,cf,len);
   cfile[len]=0;
   len = len-1;
   while (cfile[len] == ' ' && len > 0) {
      cfile[len] = 0;
      len = len-1;
   }
   if ( (ifile=lwopen(cfile, 1)) <0) {
      ibuf[0] = 0X80;
      return(-1);
   }
   offset=ib[0];
  
   if (lseek(ifile,offset,0)<0)
      return(-2);
   nbytes=nb[0];
   if (read(ifile,ibuf,nbytes)<=0)
        return(-3);
   return(0);
}



/*
 * Write an array of bytes to a file.
 * Input:  cf - filename
 *         ib - file offset at which to write first byte
 *         nb - number of bytes to write.
 *         ibuf - address of byte buffer
 *         length - length of cf string
 * Return:  0 = ok
 *          -1 = unable to open file
 *          -2 = seek failed
 *          -3 = write failed
 */
F77_FUNC(lbo,LBO)(cf,ib,nb,ibuf,length)
int *ib,*nb,length;
char *cf,*ibuf;
{
   int nbytes,ifile,len;
   long offset;
   char cfile[256];
   len = length;
   strncpy(cfile,cf,len);
   cfile[len]=0;
   len = len-1;
   while (cfile[len] == ' ' && len > 0) {
      cfile[len] = 0;
      len = len-1;
   }
   if ( (ifile=lwopen(cfile, 0)) <0)
      return(-1);
   offset=ib[0];
   if (lseek(ifile,offset,0)<0)
      return(-2);
   nbytes=nb[0]; 
   if (write(ifile,ibuf,nbytes)<0)
      return(-3);
   return(0);
}


/*
 * Open a file for I/O.
 * Input:  cfile - name of file to open
 *         rdflag = 1 = open for read, else open for write.
 * Return:  file descriptor or -1 if error.
 */
lwopen(cfile, rdflag)
char *cfile;
int rdflag;
{
   int id,j,len;
   len=strlen(cfile);
/* printf("\nfile: %s length: %d\n",cfile,len); */
   for (id=0; id<nlw; id++) {
      for (j=0; j<len; j++) {
         if (cfile[j]!=f[id][j])
            goto notf;
      }
      return(fd[id]);
notf: ;
   }
   if(ld < nlw) close(fd[ld]);
   for (j=0; j<=len; j++)
      f[ld][j]=cfile[j];
   umask(0);
   if (rdflag) fd[ld]=open(cfile,O_RDWR,0666);
   else fd[ld]=open(cfile,O_RDWR|O_CREAT,0666);
   if (fd[ld]<0) {
      if (!rdflag) printf("\ncannot open file: %s\n",cfile);
      return(-1);
   }
   id=ld;
   ++ld;
   if (ld>nlw)
      nlw=ld;
   if (ld>NFILES-1)
      ld=0;
   return(fd[id]);
}

/****************************************************************************/


/*
 * Write a string and integer to stdout.
 * Input:  c - the string
 *         i - the integer
 *         len - length of c
 */
F77_FUNC(cdest,CDEST)(c,i,len)
int *i,len;
char *c;
{
   int j;
   c[len]='\0';
   j=i[0];
   printf("%s",c);
   if (j!=0)
      printf("%8d\n",j);
   else
      printf("\n");
   return(0);
}


/*
 * Move a block of bytes.
 * Input:  n - number of bytes
 *         c - source address
 *         d - dest address
 *         k - dest offset
 */
F77_FUNC(movb,MOVB)(n,c,d,k)
int *n,*k;
char *c,*d;
{
   int i;
   for (i=0; i<n[0]; i++)
      d[k[0]+i]=c[i];
   return(0);
}

/*****************************************************************************/

/*
 * nothing
 */
F77_FUNC(luc,LUC)(index)
   int *index;
{
   return(0);
}



/*
 * nothing
 */
F77_FUNC(puc,PUC)(i, index)
   int *i, *index;
{
   return(0);
}




#ifndef HAVE_IDATE
/* IDATE not supported on IBM or HP? */
void F77_FUNC(idate,IDATE)(int *imon, int *iday, int *iyear )
{
   *imon = 1;
   *iday = 1;
   *iyear = 92;
   return 0;
}
#endif



/*
 * If we're running on a little endian machine we need to occasionally
 * re-flip the 4-bytes in a word if it's a character string.
 */
int F77_FUNC(swapchar,SWAPCHAR)( i )
unsigned int *i;
{
#ifndef WORDS_BIGENDIAN
   unsigned int tmp = *i;

   *i = (tmp & 0xff000000) >> 24
      | (tmp & 0x00ff0000) >> 8
      | (tmp & 0x0000ff00) << 8
      | (tmp & 0x000000ff) << 24;
#endif
   return 0;
}
