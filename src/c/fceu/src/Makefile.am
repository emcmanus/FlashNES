AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS	=	fceu
fceu_SOURCES 	= 	fceustr.c cart.c cheat.c crc32.c debug.c endian.c fceu.c fds.c file.c filter.c general.c ines.c input.c md5.c memory.c netplay.c nsf.c palette.c ppu.c sound.c state.c unif.c video.c vsuni.c wave.c x6502.c movie.c
# soundexp.c

fceu_SOURCES	+=	unzip.c

include boards/Makefile.am.inc
include input/Makefile.am.inc
include mappers/Makefile.am.inc
include mbshare/Makefile.am.inc
include drivers/common/Makefile.am.inc

if USE_SEXYAL
include drivers/sexyal/Makefile.am.inc
endif
fceu_SOURCES	+=	$(sexyal_SOURCES)

if NATIVEWIN32
include drivers/win/Makefile.am.inc
else
include drivers/pc/Makefile.am.inc
endif

fceu_SOURCES    +=      $(fceud_SOURCES)

DEFAULT_INCLUDES        =        
