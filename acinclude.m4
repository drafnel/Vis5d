##########################################################################
# This file contains various auxiliary autoconf macros, adapted from
# the FFTW 2.1.3 package (www.fftw.org).  FFTW is under the GNU GPL too,
# so there are no licensing troubles.  This stuff is copyright (c)
# 1999 by the Massachusetts Institute of Technology.
##########################################################################

AC_DEFUN([ACX_CHECK_CC_FLAGS],
[
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether ${CC-cc} accepts $1, ac_$2,
[echo 'void f(){}' > conftest.c
if test -z "`${CC-cc} $1 -c conftest.c 2>&1`"; then
	ac_$2=yes
else
	ac_$2=no
fi
rm -f conftest*
])
if test "$ac_$2" = yes; then
	:
	$3
else
	:
	$4
fi
])

AC_DEFUN([ACX_PROG_GCC_VERSION],
[
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether we are using gcc $1.$2 or later, ac_cv_prog_gcc_$1_$2,
[
dnl The semicolon after "yes" below is to pacify NeXT's syntax-checking cpp.
cat > conftest.c <<EOF
#ifdef __GNUC__
#  if (__GNUC__ > $1) || (__GNUC__ == $1 && __GNUC_MINOR__ >= $2)
     yes;
#  endif
#endif
EOF
if AC_TRY_COMMAND(${CC-cc} -E conftest.c) | egrep yes >/dev/null 2>&1; then
  ac_cv_prog_gcc_$1_$2=yes
else
  ac_cv_prog_gcc_$1_$2=no
fi
])
if test "$ac_cv_prog_gcc_$1_$2" = yes; then
	:
	$3
else
	:
	$4
fi
])

AC_DEFUN([ACX_PROG_CC_EGCS],
[ACX_PROG_GCC_VERSION(2,90,acx_prog_egcs=yes,acx_prog_egcs=no)])

AC_DEFUN([ACX_PROG_CC_MAXOPT],
[
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([ACX_PROG_CC_EGCS])
AC_REQUIRE([AC_CANONICAL_HOST])

# Try to determine "good" native compiler flags if none specified on command
# line.  This is modified from the FFTW version to be a little more
# conservative, and to not complain so much to the user when good flags
# aren't found.
if test "$ac_test_CFLAGS" != "set"; then
  CFLAGS=""
  case "${host_cpu}-${host_os}" in

  *linux*)
	echo "*******************************************************"
	echo "*       Congratulations! You are running Linux.       *"
	echo "*******************************************************"
	;;
  sparc-solaris2*) if test "$CC" = cc; then
                    CFLAGS="-native -fast -xO5 -dalign"
                 fi;;

  alpha*-osf*)  if test "$CC" = cc; then
                    CFLAGS="-newc -w0 -O5 -fp_reorder -tune host -arch host -std1"
                fi;;

  hppa*-hpux*)  if test "$CC" = cc; then
                    CFLAGS="-Ae +O3 +Oall"
                fi;;

   *-aix*)  if test "$CC" = cc -o "$CC" = xlc; then
                    CFLAGS="-O3 -w"
                fi;;
  esac

  # use default flags for gcc on all systems
  if test $ac_cv_prog_gcc = yes; then
     CFLAGS="-O3 -fomit-frame-pointer"
  fi

  # test for gcc-specific flags:
  if test $ac_cv_prog_gcc = yes; then
    # -malign-double for x86 systems
    ACX_CHECK_CC_FLAGS(-malign-double,align_double,
	CFLAGS="$CFLAGS -malign-double")
  fi

  CPU_FLAGS=""
  if test "$GCC" = "yes"; then
	  dnl try to guess correct CPU flags, at least for linux
	  case "${host_cpu}" in
	  i586*)  ACX_CHECK_CC_FLAGS(-mcpu=pentium,cpu_pentium,
			[CPU_FLAGS=-mcpu=pentium],
			[ACX_CHECK_CC_FLAGS(-mpentium,pentium,
				[CPU_FLAGS=-mpentium])])
		  ;;
	  i686*)  ACX_CHECK_CC_FLAGS(-mcpu=pentiumpro,cpu_pentiumpro,
			[CPU_FLAGS=-mcpu=pentiumpro],
			[ACX_CHECK_CC_FLAGS(-mpentiumpro,pentiumpro,
				[CPU_FLAGS=-mpentiumpro])])
		  ;;
	  powerpc*)
		cputype=`(grep cpu /proc/cpuinfo | head -1 | cut -d: -f2 | sed 's/ //g') 2> /dev/null`
		is60x=`echo $cputype | egrep "^60[0-9]e?$"`
		if test -n "$is60x"; then
			ACX_CHECK_CC_FLAGS(-mcpu=$cputype,m_cpu_60x,
				CPU_FLAGS=-mcpu=$cputype)
		elif test "$cputype" = 750; then
                        ACX_PROG_GCC_VERSION(2,95,
                                ACX_CHECK_CC_FLAGS(-mcpu=750,m_cpu_750,
					CPU_FLAGS=-mcpu=750))
		fi
		if test -z "$CPU_FLAGS"; then
		        ACX_CHECK_CC_FLAGS(-mcpu=powerpc,m_cpu_powerpc,
				CPU_FLAGS=-mcpu=powerpc)
		fi
		if test -z "$CPU_FLAGS"; then
			ACX_CHECK_CC_FLAGS(-mpowerpc,m_powerpc,
				CPU_FLAGS=-mpowerpc)
		fi
	  esac
  fi

  if test -n "$CPU_FLAGS"; then
        CFLAGS="$CFLAGS $CPU_FLAGS"
  fi

  if test -z "$CFLAGS"; then
	echo "Don't know good CFLAGS...guessing -O3."
	ACX_CHECK_CC_FLAGS(-O3,opt3,CFLAGS="-O3")
	if test -z "$CFLAGS"; then
		echo "-O3 didn't work...guessing -O."
		ACX_CHECK_CC_FLAGS(-O,opt1,CFLAGS="-O")
	fi
  else
  	ACX_CHECK_CC_FLAGS(${CFLAGS}, guessed_cflags, , [
	     echo "Guessed CFLAGS don't seem to work...disabling optimization."
             CFLAGS=""
  	])
  fi

fi
])
