.res.o:		
		windres -o $@ $<

fceud_SOURCES	=	drivers/win/cheat.c drivers/win/debug.c drivers/win/input.c drivers/win/joystick.c drivers/win/keyboard.c drivers/win/log.c drivers/win/main.c drivers/win/netplay.c drivers/win/res.res
