srcdir = .

SHELL = /bin/sh
CC = gcc
LEX = flex
YACC = bison -y
CFLAGS = -g -O2 -g -O0
YFLAGS = -d
DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DYYTEXT_POINTER=1 -DSTDC_HEADERS=1 -DHAVE_SYS_WAIT_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1 -DHAVE_LIMITS_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_SYS_RESOURCE_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_TERMIO_H=1 -DHAVE_X11_EXTENSIONS_SHAPE_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1 -DRETSIGTYPE=void -DHAVE_WAIT3=1 -DHAVE_SELECT=1 -DHAVE_STRDUP=1 -DHAVE_WAITPID=1 -DHAVE_XSHAPE=1 -DBSD_STYLE_GETTIMEOFDAY=1 -DHAVE_XCREATEFONTSET=1 -DHAVE_XMBTEXTESCAPEMENT=1 -DHAVE_XUTF8DRAWIMAGESTRING=1 -DUSE_FONTSETS=1 -DLAME_ENDIAN=1
ALL_CFLAGS = -I. -I. -I./libami $(DEFS) $(CFLAGS) 
LIBS =   -lSM -lICE -Llibami -lami  -lXext -lXmu \
	-lX11 -lXrender  
prefix = /usr/local
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
mandir = $(prefix)/man
INSTALL = /usr/bin/install -c
LN_S = ln -s
RM = -rm -f
LIBAMI = libami/libami.a
DT_DIR = /etc/dt
STRIPFLAG = -s

PROGS = amiwm requestchoice executecmd workbench setup ppmtoinfo viewer reqasl

MODULES = $(srcdir)/Background Keyboard Launcher

OBJS  = main.o screen.o client.o frame.o icc.o \
		icon.o menu.o diskobject.o gram.o lex.o rc.o \
		module.o

SRCS = main.c screen.c client.c frame.c icc.c \
		icon.c menu.c diskobject.c gram.c lex.c rc.c \
		module.c \
		requestchoice.c executecmd.c setup.c workbench.c reqasl.c kbdmodule.c kbdlexer.c \
		launchermodule.c viewer.c

DISTFILES = README README.modules INSTALL LICENSE amiwm.1 \
	configure configure.in Makefile.in install-sh smakefile scoptions \
	*.[chly] system.amiwmrc def_*.info *.map \
	Background Xresources.amiwm.in Amilogo.?m Xsession*.in Xinitrc.in \
	amiwm-init libami/Makefile.in libami/*.[chly]

PACKAGENAME = amiwm

AMIWM_HOME = $(libdir)/amiwm

all : lib_all
	@$(MAKE) local_all

local_all : $(PROGS) $(MODULES)

lib_all :
	@( cd libami; $(MAKE) all )

.c.o:
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) $<

menu.o : menu.c
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

rc.o : rc.c
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

diskobject.o : diskobject.c
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

filesystem.o : joke_fs.c
	$(CC) -o $@ -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

workbench.o : workbench.c
	$(CC) -o $@ -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

viewer.o : viewer.c
	$(CC) -o $@ -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

reqasl.o : reqasl.c
	$(CC) -o $@ -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

ppmtoinfo.o : ppmtoinfo.c
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

gram.h gram.c: gram.y
	$(YACC) $(YFLAGS) $<
	mv y.tab.c gram.c
	mv y.tab.h gram.h

lex.c : lex.l
	$(LEX) -t $< > lex.c

kbdmodule.h kbdmodule.c: kbdmodule.y
	$(YACC) $(YFLAGS) $<
	mv y.tab.c kbdmodule.c
	mv y.tab.h kbdmodule.h

kbdlexer.c : kbdlexer.l
	$(LEX) -t $< > kbdlexer.c

install : $(PROGS) $(MODULES) Xsession Xsession2 Xinitrc amiwm-init
	-mkdir -p $(AMIWM_HOME)/icons
	$(INSTALL) $(STRIPFLAG) requestchoice $(AMIWM_HOME)/requestchoice
	$(INSTALL) $(STRIPFLAG) executecmd $(AMIWM_HOME)/executecmd
	$(INSTALL) $(STRIPFLAG) setup $(AMIWM_HOME)/setup
	$(INSTALL) $(STRIPFLAG) workbench $(AMIWM_HOME)/workbench
	$(INSTALL) $(STRIPFLAG) viewer $(AMIWM_HOME)/viewer
	$(INSTALL) $(STRIPFLAG) reqasl $(AMIWM_HOME)/reqasl
	for module in $(MODULES); do \
	  if [ "$$module" = "$(srcdir)/Background" ]; then \
	    $(INSTALL) $$module $(AMIWM_HOME)/Background; \
	  else \
	    $(INSTALL) $(STRIPFLAG) $$module $(AMIWM_HOME)/$$module; \
	  fi; \
	done
	$(INSTALL) -m 644 $(srcdir)./icons/def_disk.info $(AMIWM_HOME)/icons/def_disk.info
	$(INSTALL) -m 644 $(srcdir)./icons/def_drawer.info $(AMIWM_HOME)/icons/def_drawer.info
	$(INSTALL) -m 644 $(srcdir)./icons/def_project.info $(AMIWM_HOME)/icons/def_project.info
	$(INSTALL) -m 644 $(srcdir)./icons/def_tool.info $(AMIWM_HOME)/icons/def_tool.info
	$(INSTALL) -m 644 $(srcdir)./icons/browser.info $(AMIWM_HOME)/icons/browser.info
	$(INSTALL) -m 644 $(srcdir)./icons/compress.info $(AMIWM_HOME)/icons/compress.info
	$(INSTALL) -m 644 $(srcdir)./icons/filer.info $(AMIWM_HOME)/icons/filer.info
	$(INSTALL) -m 644 $(srcdir)./icons/ftp.info $(AMIWM_HOME)/icons/ftp.info
	$(INSTALL) -m 644 $(srcdir)./icons/harddisk.info $(AMIWM_HOME)/icons/harddisk.info
	$(INSTALL) -m 644 $(srcdir)./icons/irc.info $(AMIWM_HOME)/icons/irc.info
	$(INSTALL) -m 644 $(srcdir)./icons/mail.info $(AMIWM_HOME)/icons/mail.info
	$(INSTALL) -m 644 $(srcdir)./icons/network.info $(AMIWM_HOME)/icons/network.info
	$(INSTALL) -m 644 $(srcdir)./icons/newsreader.info $(AMIWM_HOME)/icons/newsreader.info
	$(INSTALL) -m 644 $(srcdir)./icons/paint.info $(AMIWM_HOME)/icons/paint.info
	$(INSTALL) -m 644 $(srcdir)./icons/shell.info $(AMIWM_HOME)/icons/shell.info
	$(INSTALL) -m 644 $(srcdir)./icons/soundeditor.info $(AMIWM_HOME)/icons/soundeditor.info
	$(INSTALL) -m 644 $(srcdir)./icons/texteditor.info $(AMIWM_HOME)/icons/texteditor.info
	$(INSTALL) -m 644 $(srcdir)./icons/viewer.info $(AMIWM_HOME)/icons/viewer.info
	$(INSTALL) -m 644 $(srcdir)./icons/texteditor2.info $(AMIWM_HOME)/icons/texteditor2.info
	$(INSTALL) -m 644 $(srcdir)./icons/idedevel.info $(AMIWM_HOME)/icons/idedevel.info
	$(INSTALL) -m 644 $(srcdir)./icons/files.info $(AMIWM_HOME)/icons/files.info
	$(INSTALL) -m 644 $(srcdir)./icons/xephyr.info $(AMIWM_HOME)/icons/xephyr.info
	$(INSTALL) -m 644 $(srcdir)./icons/screenrec.info $(AMIWM_HOME)/icons/screenrec.info
	$(INSTALL) -m 644 $(srcdir)./icons/reqasl.info $(AMIWM_HOME)/icons/reqasl.info
	$(INSTALL) -m 644 $(srcdir)./icons/mupdf.info $(AMIWM_HOME)/icons/mupdf.info
	$(INSTALL) -m 644 $(srcdir)./icons/xcalc.info $(AMIWM_HOME)/icons/xcalc.info
	$(INSTALL) -m 644 $(srcdir)/system.amiwmrc $(AMIWM_HOME)/system.amiwmrc
	$(INSTALL) -m 644 $(srcdir)/system.map $(AMIWM_HOME)/system.map
	$(INSTALL) -m 644 $(srcdir)/magicwb.map $(AMIWM_HOME)/magicwb.map
	$(INSTALL) -m 644 $(srcdir)/schwartz.map $(AMIWM_HOME)/schwartz.map
	$(INSTALL) -m 755 Xsession $(AMIWM_HOME)/Xsession
	$(INSTALL) -m 755 Xsession2 $(AMIWM_HOME)/Xsession2
	$(INSTALL) -m 755 Xinitrc $(AMIWM_HOME)/Xinitrc
	$(INSTALL) -m 755 $(srcdir)/amiwm-init $(AMIWM_HOME)/amiwm-init
	-mkdir -p $(bindir)
	$(INSTALL) $(STRIPFLAG) amiwm $(bindir)/amiwm
	$(INSTALL) $(STRIPFLAG) ppmtoinfo $(bindir)/ppmtoinfo
	$(RM) $(bindir)/requestchoice
	$(LN_S) $(AMIWM_HOME)/requestchoice $(bindir)/requestchoice
	-mkdir -p $(mandir)/man1
	$(INSTALL) -m 644 $(srcdir)/amiwm.1 $(mandir)/man1/amiwm.1

fs-install : Filesystem
	$(INSTALL) $(STRIPFLAG) Filesystem $(AMIWM_HOME)/Filesystem
	$(INSTALL) -m 644 def_disk.info $(AMIWM_HOME)/def_disk.info
	$(INSTALL) -m 644 def_drawer.info $(AMIWM_HOME)/def_disk.info

cde-install : Xresources.amiwm Amilogo.bm Amilogo.pm
	-mkdir -p $(DT_DIR)/config/C/Xresources.d
	$(INSTALL) -m 644 Xresources.amiwm $(DT_DIR)/config/C/Xresources.d/Xresources.amiwm
	-mkdir -p $(DT_DIR)/appconfig/icons/C
	$(INSTALL) -m 644 Amilogo.bm $(DT_DIR)/appconfig/icons/C/Amilogo.bm
	$(INSTALL) -m 644 Amilogo.pm $(DT_DIR)/appconfig/icons/C/Amilogo.pm


amiwm : $(OBJS) $(LIBAMI)
	$(CC) -o amiwm $(OBJS) $(LIBS)

requestchoice : requestchoice.o $(LIBAMI)
	$(CC) -o requestchoice requestchoice.o $(LIBS)

executecmd : executecmd.o $(LIBAMI)
	$(CC) -o executecmd executecmd.o $(LIBS)

workbench : workbench.o $(LIBAMI)
	$(CC) -o workbench workbench.o $(LIBS)

viewer : viewer.o $(LIBAMI)
	$(CC) -o viewer viewer.o $(LIBS)

setup : setup.o $(LIBAMI)
	$(CC) -o setup setup.o $(LIBS)

reqasl : reqasl.o $(LIBAMI)
	$(CC) -o reqasl reqasl.o $(LIBS)

Filesystem : filesystem.o $(LIBAMI)
	$(CC) -o Filesystem filesystem.o $(LIBS)

Keyboard : kbdmodule.o kbdlexer.o $(LIBAMI)
	$(CC) -o Keyboard kbdmodule.o kbdlexer.o $(LIBS)

Launcher : launchermodule.o $(LIBAMI)
	$(CC) -o Launcher launchermodule.o $(LIBS)

ppmtoinfo : ppmtoinfo.o
	$(CC) -o ppmtoinfo ppmtoinfo.o -lm

localetest : localetest.o $(LIBAMI)
	$(CC) -o localetest localetest.o $(LIBS)

menutest : menutest.o $(LIBAMI)
	$(CC) -o menutest menutest.o $(LIBS)

clean :
	$(RM) core $(PROGS) $(LIBAMI) Keyboard Launcher *.o libami/*.o
	$(RM) lex.yy.c lex.c y.tab.c y.tab.h gram.h gram.c
	$(RM) kbdlexer.c kbdmodule.h kbdmodule.c
	$(RM) config.log

distclean : clean
	$(RM) config.status config.cache *~
	$(RM) Makefile libami/Makefile Xresources.amiwm Xsession Xsession2 Xinitrc

spotless : distclean

TAGS:
	etags *.[ch] libami/*.[ch]

patch :
	mv version.h old_version.h
	sed < old_version.h 's/l/"/' | awk '-F"' '{ printf "%s\"%sl%s\"\n",$$1,$$2,1+$$3 }' > version.h
	rm old_version.h

dist : version.h clean
	( version=`sed < version.h -e 's/^[^"]*"//' -e 's/"[^"]*$$//'`; \
	  tarname="$(PACKAGENAME)$$version.tar"; \
	  mkdir $(PACKAGENAME)$$version; \
	  tar cf - $(DISTFILES) | (cd $(PACKAGENAME)$$version/; tar xf - ); \
	  rm -f $$tarname $$tarname.gz; \
	  tar cf $$tarname $(PACKAGENAME)$$version; \
	  rm -rf $(PACKAGENAME)$$version; \
	  gzip $$tarname; \
	)

$(srcdir)/configure: $(srcdir)/configure.in
	autoconf $(srcdir)/configure.in > $(srcdir)/configure

config.status: $(srcdir)/configure
	$(srcdir)/configure

Makefile: $(srcdir)/Makefile.in config.status
	./config.status

libami/Makefile: $(srcdir)/libami/Makefile.in config.status
	./config.status

Xresources.amiwm: $(srcdir)/Xresources.amiwm.in
	sed -e 's:[@]bindir[@]:$(bindir):g' -e 's:[@]AMIWM_HOME[@]:$(AMIWM_HOME):g' < $(srcdir)/Xresources.amiwm.in > Xresources.amiwm

Xsession: $(srcdir)/Xsession.in config.status
	sed -e 's:[@]bindir[@]:$(bindir):g' -e 's:[@]AMIWM_HOME[@]:$(AMIWM_HOME):g' < $(srcdir)/Xsession.in > Xsession

Xsession2: $(srcdir)/Xsession2.in config.status
	sed -e 's:[@]bindir[@]:$(bindir):g' -e 's:[@]AMIWM_HOME[@]:$(AMIWM_HOME):g' < $(srcdir)/Xsession2.in > Xsession2

Xinitrc: $(srcdir)/Xinitrc.in config.status
	sed -e 's:[@]bindir[@]:$(bindir):g' -e 's:[@]AMIWM_HOME[@]:$(AMIWM_HOME):g' < $(srcdir)/Xinitrc.in > Xinitrc


depend:: $(SRCS)
	-@rm Makefile.bak
	mv Makefile Makefile.bak
	( sed '/^#--DO NOT REMOVE THIS LINE--$$/q' < Makefile.bak ; \
	  $(CC) -MM $(ALL_CFLAGS) $(SRCS) ) > Makefile

depend::
	@( cd libami; $(MAKE) depend )

#--DO NOT REMOVE THIS LINE--
