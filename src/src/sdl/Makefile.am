bin_PROGRAMS = VisualBoyAdvance

VisualBoyAdvance_SOURCES = \
	Controller.h \
	Controller.cpp \
	Event.h \
	Event.cpp \
	RomSelector.h \
	RomSelector.cpp \
	Options.h \
	Options.cpp \
	GLUtil.h \
	GLUtil.cpp \
	OptionMenu.h \
	OptionMenu.cpp \
	HelpText.h \
	VBA.h \
	resize++.h \
	resize++.cpp \
	SDL.cpp			\
	esFunc.h		\
	../AutoBuild.h		\
	../Cheats.cpp		\
	../Cheats.h		\
	../EEprom.cpp		\
	../EEprom.h		\
	../Flash.cpp		\
	../Flash.h		\
	../GBA.cpp		\
	../GBA.h		\
	../GBAinline.h		\
	../Gfx.cpp		\
	../Gfx.h		\
	../Globals.cpp		\
	../Globals.h		\
	../Mode0.cpp		\
	../Mode1.cpp		\
	../Mode2.cpp		\
	../Mode3.cpp		\
	../Mode4.cpp		\
	../Mode5.cpp		\
	../NLS.h		\
	../Port.h		\
	../RTC.cpp		\
	../RTC.h		\
	../Sound.cpp		\
	../Sound.h		\
	../Sram.cpp		\
	../Sram.h		\
	../System.h		\
	../Text.cpp		\
	../Text.h		\
	../Util.cpp		\
	../Util.h		\
	../admame.cpp		\
	../arm-new.h		\
	../bios.cpp		\
	../bios.h		\
	../elf.cpp		\
	../elf.h		\
	../expr-lex.cpp		\
	../expr.cpp		\
	../expr.cpp.h		\
	../exprNode.cpp		\
	../exprNode.h		\
	../getopt.c		\
	../getopt.h		\
	../getopt1.c		\
	../interframe.cpp	\
	../interp.h		\
	../memgzio.c		\
	../memgzio.h		\
	../pixel.cpp		\
	../thumb.h		\
	../unzip.cpp		\
	../unzip.h

VisualBoyAdvance_LDADD = @VBA_LIBS@ @SDL_LIBS@ -lGLESv2 -lsrv_um -lIMGegl

#strip the binary
VisualBoyAdvance_LDFLAGS = -Wl,-S

VisualBoyAdvance_DEPENDENCIES = @VBA_LIBS@


AM_CPPFLAGS = \
	-I$(top_srcdir)/src		\
	-DSDL				\
	-DSYSCONFDIR=\"$(sysconfdir)\"

AM_CXXFLAGS = -fno-exceptions @SDL_CFLAGS@
