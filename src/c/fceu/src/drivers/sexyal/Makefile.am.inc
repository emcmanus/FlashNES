sexyal_SOURCES	=      drivers/sexyal/sexyal.c drivers/sexyal/md5.c drivers/sexyal/smallc.c drivers/sexyal/convert.c

if WIN32
TMP_SEXYAL	=	drivers/sexyal/drivers/dsound.c
endif

if UNIX
TMP_SEXYAL	=	drivers/sexyal/drivers/oss.c
endif

sexyal_SOURCES	+=	$(TMP_SEXYAL)
