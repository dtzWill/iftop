dnl
dnl configure.ac:
dnl Autoconf input for iftop.
dnl
dnl I hate autoconf with a passion. It's an utter pain to write these bloody
dnl things and even once you have you find yourself testing for more and more
dnl special cases. But that's OK. Paul is going to maintain it :)
dnl     -- Chris Lightfoot
dnl
dnl $Id$
dnl
dnl To regenerate everything from source, do:
dnl    autoheader
dnl    aclocal
dnl    automake
dnl    autoconf
dnl Now you should have good sources to make into a tarball and distribute.
dnl    ./configure     (perhaps with some arguments)
dnl    make
dnl Tested with automake 1.9.6-1.14 and autoconf 2.59-2.69.
dnl
dnl Boilerplate configuration
dnl

AC_INIT([iftop], [1.0pre4], [pdw@ex-parrot.com], [iftop], [http://www.ex-parrot.com/pdw/iftop/])

AC_CONFIG_AUX_DIR(config)

AC_CANONICAL_SYSTEM

AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

dnl Make sure we have a C compiler....
AC_PROG_CC
AC_HEADER_STDC

dnl
dnl Options to configure.
dnl

AC_ARG_WITH(resolver,
    [  --with-resolver=TYPE    Technique iftop should use for name resolution.  
                          Valid options are:
			  netdb          use gethostbyaddr_r in multiple
			                 threads.
			  netdb_1thread  use gethostbyaddr_r and
					 assume it is not reentrant.
			  ares           use the MIT ARES asynchronous
					 resolver library.
			  forking        use the REALLY SUCKY forking resolver.
			  guess          run experiments to guess a
					 reasonable value.  Only works if you
					 aren't cross-compiling.  This
					 is the default.  guess will
					 either select netdb or netdb_1thread.
			  none           don't do name resolution.],
    [resolver=$withval],
    [resolver=guess])

AC_ARG_WITH(libpcap,
    [  --with-libpcap=WHERE    Where the libpcap packet-capture library is found.
                          The pcap library should be installed  in WHERE/lib,
                          and the header file in either WHERE/include or
                          WHERE/include/pcap.
                          [default=look in standard locations]],
    [libpcap_prefix=$withval],
    [libpcap_prefix=""])

dnl
dnl Fairly generic checks.
dnl

dnl Checks for system headers.
AC_CHECK_HEADERS([sgtty.h sys/ioctl.h sys/time.h sys/sockio.h termio.h termios.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

dnl
dnl Are we on a system that uses the STREAMS low-level DLPI interface?
dnl

AC_CHECK_HEADER([sys/dlpi.h],[AC_DEFINE([HAVE_DLPI],1,[Are we running on a STREAMS system with DLPI?])])

dnl Checks for library functions.
AC_CHECK_FUNCS(regcomp select strdup strerror strspn)

AC_SEARCH_LIBS(socket, socket)
AC_SEARCH_LIBS(log, m)
AC_CHECK_FUNC(gethostbyname, ,
	[AC_CHECK_LIB(nsl, gethostbyname)] )

AC_SEARCH_LIBS(inet_aton, [socket nsl])
AC_SEARCH_LIBS(inet_pton, [socket nsl])
AC_CHECK_FUNCS(inet_aton inet_pton)

dnl
dnl Find integers of known physical size. This is a pain in the arse because
dnl we can't use AC_CHECK_SIZEOF to find the original variables, since that
dnl function doesn't permit us to include a header file. Sigh.
dnl

for type in u_int8_t u_int16_t u_int32_t ; do
    AC_MSG_CHECKING([size of $type])
    AC_RUN_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <stdio.h>
int main() {
    $type dummy;
    FILE *f=fopen("conftestval", "w");
    if (!f) exit(1);
    fprintf(f, "%d\n", sizeof($1));
    exit(0);
}
    ])], [
        x=`cat conftestval`
        eval "size_$type=$x"
        AC_MSG_RESULT([$x])
    ], [
        eval "size_$type=0"
        AC_MSG_RESULT([unknown type])
    ], [
        eval "size_$type=0"
        AC_MSG_RESULT([can't determine when cross-compiling])
    ])
done

dnl Groan. Have to do things this way so that autoheader can do its thing....
AC_DEFINE_UNQUOTED(SIZEOF_U_INT8_T,  [$size_u_int8_t],  [size of u_int8_t])
AC_DEFINE_UNQUOTED(SIZEOF_U_INT16_T, [$size_u_int16_t], [size of u_int16_t])
AC_DEFINE_UNQUOTED(SIZEOF_U_INT32_T, [$size_u_int32_t], [size of u_int32_t])

dnl If we already have these types, don't piss about any more....

if test $size_u_int8_t != 1 || test $size_u_int16_t != 2 || test $size_u_int32_t != 4 ; then
dnl XXXif test $size_u_int8_t != 1 -o $size_u_int16_t != 2 -o $size_u_int32_t != 4 ; then
    do_int_types=1
    AC_CHECK_HEADERS(
        stdint.h             dnl C99
        sys/inttypes.h,      dnl Solaris
        [do_int_types=0; break])

    if test $do_int_types = 1 ; then
        dnl No C99 int types, so figure them out from basic types.
        AC_CHECK_SIZEOF(unsigned short int)
        AC_CHECK_SIZEOF(unsigned int)
        AC_CHECK_SIZEOF(unsigned long int)
    else
        dnl Just use the C99 ones.
        AC_DEFINE(HAVE_C99_INTS, 1, [C99 fixed-width int types available])
    fi
fi

dnl
dnl Name resolution.
dnl
dnl This is complicated because we need some sort of reentrant mechanism for
dnl name resolution. Naturally, UNIX vendors have come up with a variety of
dnl incompatible schemes for this, many of which don't work at all.
dnl


AC_SEARCH_LIBS([getnameinfo], [nsl resolv])
AC_SEARCH_LIBS([getifaddrs], [nsl resolv])
AC_CHECK_FUNCS([ \
  getifaddrs \
  getnameinfo \
])

if test "x$ac_cv_func_getifaddrs" = "xyes"; then
  AC_DEFINE(USE_GETIFADDRS, 1, [use getifaddrs])
  AC_MSG_NOTICE([getifaddrs found, using (setting USE_GETIFADDRS=1)])
fi

dnl First, the default resolver, which uses getnameinfo or gethostbyaddr_r. If
dnl not available, we fall back to gethostbyaddr. We could fall back to ARES,
dnl but that's probably not available on typical machines.

dnl If we've been asked to guess, remember that fact in specified_resolver.
dnl From this point on, resolver is our preferred resolver given the
dnl experiments we've done so far, or "guess" if we have no idea.
specified_resolver=$resolver
if test x$specified_resolver = xguess ; then
    dnl Best possibility is getnameinfo.
    if test "x$ac_cv_func_getnameinfo" = "xyes"; then
      AC_DEFINE(USE_GETNAMEINFO, 1, [use getnameinfo for name resolution])
      resolver=getnameinfo # not guess
      AC_MSG_NOTICE([getnameinfo found, using for name resolution (setting USE_GETNAMEINFO=1)])
    else
      dnl Best hope is netdb, which presently means gethostbyaddr_r.
      resolver=netdb
    fi
fi

if test x$resolver = xnetdb ; then
    dnl Can use gethostbyaddr_r?
    AC_SEARCH_LIBS(gethostbyaddr_r, [nsl], , [resolver=guess])
    if test x$resolver = xguess && test x$specified_resolver != xguess ; then
       dnl They wanted gethostbyaddr_r, but they can't have it, so stop.
       AC_MSG_ERROR([no library defines gethostbyaddr_r])
    fi
fi

dnl We still might do gethostbyaddr_r.  Figure out whether we have
dnl glibc-style or Solaris-style gethostbyaddr_r (or neither...).
dnl Separate determining how to call gethostbyaddr_r from testing
dnl whether it works so we can support cross-compilation.            
if test x$resolver = xnetdb ; then
    AC_MSG_CHECKING([how to call gethostbyaddr_r])
    dnl Try 7 arguments returning a struct hostent*.
    AC_LINK_IFELSE([AC_LANG_SOURCE([`cat config/hostentp_ghba_r.c`])],
                   [AC_MSG_RESULT([7 args])
		    ghba_args=8
	            AC_DEFINE(GETHOSTBYADDR_R_RETURNS_HOSTENT_P, 1,
                    [7-argument gethostbyaddr_r returns struct hostent*])], [
    dnl Try 8 arguments returning an int.
    AC_LINK_IFELSE([AC_LANG_SOURCE([`cat config/int_ghba_r.c`])],
                   [AC_MSG_RESULT([8 args, int return])
		    ghba_args=8
	            AC_DEFINE(GETHOSTBYADDR_R_RETURNS_INT, 1,
                    [8-argument gethostbyaddr_r returns int])], [
    dnl Neither.
    AC_MSG_RESULT([don't know how])
    resolver=guess])])
    if test x$resolver = xguess && test x$specified_resolver != xguess ; then
       dnl They wanted gethostbyaddr_r, but they can't have it, so stop.
       AC_MSG_ERROR([gethostbyaddr_r has no known calling convention])
    fi
fi

dnl If we still want to do gethostbyaddr_r, and we aren't
dnl cross-compiling, test it.
if test x$resolver = xnetdb ; then
    if test x$ghba_args = x8 ; then
       testfile=int_ghba_r
    else
       testfile=hostentp_ghba_r
    fi
    AC_MSG_CHECKING(gethostbyaddr_r usability)
    AC_RUN_IFELSE([AC_LANG_SOURCE([`cat config/$testfile.c`])],
                  [AC_MSG_RESULT([yes])],
		  [AC_MSG_RESULT([no])
		   resolver=guess],
		  [AC_MSG_RESULT([can't test because we are cross-compiling])])
    if test x$resolver = xguess ; then
        if test x$specified_resolver = xguess ; then
           AC_MSG_RESULT([gethostbyaddr_r doesn't work, so we'll try something else])
        else
           dnl They wanted gethostbyaddr_r, but it doesn't work, so stop.
           AC_MSG_ERROR([gethostbyaddr_r doesn't work])
        fi
    fi
fi

dnl We found a gethostbyaddr_r we know how to use and which seems to
dnl work.
if test x$resolver = xnetdb ; then
    AC_DEFINE(USE_GETHOSTBYADDR_R, 1, [use gethostbyaddr_r for name resolution])
fi

dnl They may have asked for ares.
if test x$resolver = xares ; then
    dnl See if ares is to hand....
    AC_SEARCH_LIBS(ares_init, [ares], [
        AC_DEFINE(USE_ARES, 1, [use ARES for name resolution])
        ], [
        dnl They asked for ares, but we can't give it to them, so stop.
        AC_MSG_ERROR([can't find ARES.  Re-run configure and ask for a different resolver.])])
fi

dnl Last thing to try if we haven't decided yet is netdb_1thread.
if test x$resolver = xguess ; then
   resolver=netdb_1thread
fi

dnl Ugh. Both the single-threaded and the forking resolvers use gethostbyaddr.
if test x$resolver = xnetdb_1thread || test x$resolver = xforking ; then
    AC_SEARCH_LIBS(gethostbyaddr, [nsl], , [
        AC_MSG_ERROR([gethostbyaddr is not available.  You will have to 
  recompile with no name resolution at all.])])

    if test x$resolver = xnetdb_1thread ; then
        AC_MSG_WARN([using single-threaded resolver with gethostbyaddr
  Consider obtaining ARES or a machine with a working gethostbyaddr_r.])
        AC_DEFINE(USE_GETHOSTBYADDR, 1, [use gethostbyaddr for name resolution])
    else
        AC_DEFINE(USE_FORKING_RESOLVER, 1, [use a REALLY SUCKY forking resolver for name resolution])
    fi
fi

dnl Otherwise, no resolver at all. Boo hoo.

dnl
dnl Find libpcap.
dnl

if test x$libpcap_prefix = x ; then
    libpcap_prefix="/usr /usr/local /opt /software"
fi

AC_MSG_CHECKING([where to find pcap.h])
foundpcaph=0
oldCPPFLAGS=$CPPFLAGS
for test_prefix in "" $libpcap_prefix ; do
    for x in "" /pcap ; do
        if test x$test_prefix != x ; then
            CPPFLAGS="$oldCPPFLAGS -I$test_prefix/include$x"
        fi
        AC_TRY_CPP([
#include <pcap.h>
        ], [
        AC_MSG_RESULT([$test_prefix/include$x])
        foundpcaph=1
        break
        ])
    done
    if test $foundpcaph = 1 ; then
        break
    fi
done

if test $foundpcaph = 0 ; then
    AC_MSG_RESULT([no idea])
    AC_MSG_ERROR([can't find pcap.h
  You're not going to get very far without libpcap.])
else
    dnl assume that -lpcap is under $test_prefix/lib
    if test x$test_prefix != x ; then
        LDFLAGS="$LDFLAGS -L$test_prefix/lib"
    fi
    AC_CHECK_LIB(pcap, pcap_open_live, , [
            AC_MSG_ERROR([can't find libpcap
  You're not going to get very far without libpcap.])
        ])
fi

foundpcap=0
AC_CHECK_HEADERS([pcap.h pcap/pcap.h], [
    foundpcap=1
    break
    ])

if test $foundpcap = 0 ; then
    AC_MSG_ERROR([can't find pcap.h
  You're not going to get very far without libpcap.])
fi

dnl
dnl Curses. Really, we need ncurses or something similarly advanced, since
dnl we use the (apparently obscure) mvchgat function. Unfortunately, there's
dnl a solid chance that mvchgat is a macro, so we can't just use
dnl AC_SEARCH_LIBS....
dnl

AC_MSG_CHECKING([for a curses library containing mvchgat])
oldLIBS=$LIBS
for curseslib in ncursesw curses ncurses ; do
    LIBS="$oldLIBS -l$curseslib"
    AC_TRY_LINK([
#include <$curseslib.h>
        ], [
        mvchgat(0, 0, 1, A_REVERSE, 0, NULL)
        ], [
        foundcurseslib=$curseslib
        break
        ])
done

if test x$foundcurseslib = x ; then
    AC_MSG_RESULT([none found])
    AC_MSG_ERROR([Curses! Foiled again!
  (Can't find a curses library supporting mvchgat.)
  Consider installing ncurses.])
else
    AC_MSG_RESULT([-l$foundcurseslib])
fi


dnl
dnl POSIX threads. Different systems like different combinations of flags,
dnl libraries, etc. We use a test program to figure this stuff out.
dnl

AC_MSG_CHECKING([POSIX threads compilation])
thrfail=1
oldCFLAGS=$CFLAGS
oldLIBS=$LIBS
for flag in "" -mt -pthread -thread ; do
    CFLAGS="$oldCFLAGS $flag"
    for lib in "" -lpthread "-lpthread -lposix4" ; do
        LIBS="$oldLIBS $lib"
        AC_LINK_IFELSE([AC_LANG_SOURCE([`cat config/pthread.c`])], [
            foundthrlib=$lib
            foundthrflag=$flag
            thrfail=0
            break
            ])
    done
    if test $thrfail = 0 ; then
        break
    fi
done

if test $thrfail = 1 ; then
    AC_MSG_RESULT([no idea])
    AC_MSG_ERROR([can't figure out how to compile with POSIX threads
  If your system actually supports POSIX threads, this means we've messed up.])
fi

AC_MSG_RESULT([CFLAGS=$foundthrflag and LIBS=$foundthrlib])
AC_MSG_CHECKING([POSIX threads usability])
AC_RUN_IFELSE([AC_LANG_SOURCE([`cat config/pthread.c`])],
	      [AC_MSG_RESULT([yes])],
              [AC_MSG_ERROR(
	       [it fails.  We probably guessed the wrong CFLAGS.])],
	      [AC_MSG_RESULT([can't test because we are cross-compiling])])

dnl
dnl Are we on a system (like Solaris) that requires promiscuous mode in order to
dnl see any outgoing packets?
dnl

AC_MSG_CHECKING([if we need to enable promiscuous mode by default])

enable_default_promiscuous="no"

case "$host_os" in
solaris*) enable_default_promiscuous="yes" ;;
esac

AC_ARG_ENABLE(default-promiscuous,
	[  --enable-default-promiscuous If enabled, iftop will operate in promiscuous mode 
                          to capture outgoing packets])

AC_MSG_RESULT([$enable_default_promiscuous])

if test x"$enable_default_promiscuous" = x"yes"; then
	AC_DEFINE([NEED_PROMISCUOUS_FOR_OUTGOING],1,[Enable default promiscuous mode to capture outgoing packets])
fi

dnl
dnl Wahey! This might even work.
dnl

AC_SUBST(ac_aux_dir)

AC_OUTPUT(Makefile config/Makefile)

