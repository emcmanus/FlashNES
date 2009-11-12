AC_INIT([src/fceu.c])

FCEU_VERSION='0.98.12'
FCEU_VERSION_NUMERIC=9812

AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([fceu], $FCEU_VERSION)

AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL

AC_DEFINE(_GNU_SOURCE)
AC_CHECK_FUNC(asprintf, AC_DEFINE([HAVE_ASPRINTF]), [])

dnl Check for zlib
AC_CHECK_LIB([z], [zlibVersion],[], AC_MSG_ERROR([*** zlib not found!]))
LIBS="$LIBS -lz"

AC_ARG_WITH(nativewin32,
                [AC_HELP_STRING([--with-nativewin32],
                [use nativewin32])],  
                use_nativewin32=$withval,
                use_nativewin32="no")


AM_CONDITIONAL(WIN32, false)
AM_CONDITIONAL(UNIX, false)
AM_CONDITIONAL(USE_SEXYAL, false)
AM_CONDITIONAL(NATIVEWIN32,false)
AM_CONDITIONAL(OPENGL, false)
AM_CONDITIONAL(HAVE_GTK, false)
AC_DEFINE([NETWORK],[1])


# OS tests
# Added formatting here...

if expr x"$target" : 'x.*beos' > /dev/null; then
	CFLAGS="-no-fpic $CFLAGS"
	CPPFLAGS="-no-fpic $CPPFLAGS"
	AC_DEFINE([PSS_STYLE],[1])
elif expr x"$target" : 'x.*mingw' > /dev/null; then
	AC_DEFINE([PSS_STYLE],[2])
	AC_DEFINE([WIN32])
	AM_CONDITIONAL(WIN32, true)
	
	if test x$use_nativewin32 = xyes; then
	 LIBS="$LIBS -mwindows -lddraw -ldinput -ldsound -lgdi32 -ldxguid -lwinmm -lshell32 -lwsock32 -lcomdlg32 -lole32"
	 AM_CONDITIONAL(NATIVEWIN32,true)
	else
	 AM_CONDITIONAL(USE_SEXYAL, true)
	 AC_DEFINE([USE_SEXYAL])
	 LIBS="$LIBS -ldsound -lwinmm"
	fi
else
	AM_CONDITIONAL(UNIX, true)
	AC_DEFINE([UNIX])
	AC_DEFINE([PSS_STYLE],[1])
	AC_DEFINE([NETWORK],[1])

	AC_CHECK_HEADER([sys/soundcard.h],
	        AM_CONDITIONAL(USE_SEXYAL, true)
		AC_DEFINE([USE_SEXYAL]),[])
fi

dnl if expr x"$target" : 'x.*darwin' > /dev/null; then
dnl AC_DEFINE([MACOSX])
dnl fi

if test x$use_nativewin32 = xno; then
 dnl Check for SDL
 SDL_VERSION=1.2.0
 AM_PATH_SDL($SDL_VERSION, [:],
             AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))

 LIBS="$LIBS $SDL_LIBS"
 CFLAGS="-Wall -fomit-frame-pointer $CFLAGS $SDL_CFLAGS"
 CPPFLAGS="-Wall -fomit-frame-pointer $CPPFLAGS $SDL_CFLAGS"

 dnl Check for SDL_net for net play
 HAS_SDL_NET=no
 dnl AC_CHECK_LIB([SDL_net], [SDLNet_Init],[HAS_SDL_NET=yes])
 dnl if test x$HAS_SDL_NET = xyes; then
 dnl	AC_DEFINE([NETWORK])
 dnl	LIBS="$LIBS -lSDL_net"
 dnl fi

 AC_ARG_WITH(opengl,
 	[AC_HELP_STRING([--with-opengl],
 	[use OpenGL])],
 	 use_opengl=$withval,
 	 use_opengl="no")
 
 if [[ "$use_opengl" = "yes" ]] ; then
 	dnl Check for OpenGL
 	
 	AC_CHECK_HEADER([GL/gl.h],[AC_DEFINE([OPENGL]) AM_CONDITIONAL(OPENGL, true)],
 	[
 		AC_CHECK_HEADER([OpenGL/gl.h],[AC_DEFINE([OPENGL]) AM_CONDITIONAL(OPENGL, true)],[])
 		AC_DEFINE([APPLEOPENGL])
 	])
 fi
 AC_DEFINE([SDL])
fi


if expr match "$target_cpu" 'i.86' > /dev/null; then
	AC_DEFINE([C80x86])
fi

AC_C_BIGENDIAN([], [AC_DEFINE([LSB_FIRST])])
AC_DEFINE_UNQUOTED([FCEU_VERSION],"$FCEU_VERSION")
AC_DEFINE_UNQUOTED([FCEU_VERSION_NUMERIC],$FCEU_VERSION_NUMERIC)

AC_DEFINE(FRAMESKIP)

dnl Output Makefiles
AC_OUTPUT([Makefile src/Makefile])
