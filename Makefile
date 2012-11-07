#********************************************************************
#        Copyright (c) 2006, Lee Patterson & Ant Works Software
#        http://ssobjects.sourceforge.net
#
#        created  :    3/11/2000
#        filename :    Makefile
#        author   :    Lee Patterson (workerant@users.sourceforge.net)
#        
#        purpose  :    Make file for Linux operating system.
#*********************************************************************

TERM=dumb


OS_NAME = -DOS_LINUX

OUTPUT     = libssobjects.a
OUTPUT_DBG = libssobjects_dbg.a

SRCS =  \
	Parseit.cpp \
	asyncclientconnector.cpp \
	bufferedsocket.cpp \
	clientconnector.cpp \
	cstr.cpp \
	file.cpp \
	flags.cpp \
	generalexception.cpp \
	logs.cpp \
	mclautolock.cpp \
	mclcritsec.cpp \
	mclevent.cpp \
	mclthread.cpp \
	packetbuffer.cpp \
	packetmessageque.cpp \
	serverhandler.cpp \
	serversocket.cpp \
	simplemanager.cpp \
	simpleserver.cpp \
	socketinstance.cpp \
	stopwatch.cpp \
	threadutils.cpp \
	tsleep.cpp \
    telnetserver.cpp \
    telnetserversocket.cpp \
#	simpletime.cpp \

LIB_OBJS     := $(SRCS:.cpp=.o)
LIB_OBJS_DBG := $(SRCS:.cpp=_dbg.o)

INCLUDES = -I. -I/usr/local/ssl/include
LINK_INCLUDES = -L. 
EXCEPTIONS =
INSTALL_DIRECTORY = /usr/local/develop

WARNINGS = -W -Wall -Woverloaded-virtual -Weffc++
CC = g++
CCFLAGS_DBG := $(INCLUDES) $(WARNINGS) -g -fPIC  -D_REENTRANT -DDEBUG  -DUSE_NCURSES 
CCFLAGS     := $(INCLUDES) $(WARNINGS) -fPIC -O2 -D_REENTRANT -DNDEBUG -DUSE_NCURSES 
LINK_FLAGS = $(LINK_INCLUDES)

none    : debug 
all     : debug release examples
Debug   : debug
Release : release
install : all
	@echo "Installing files to $(INSTALL_DIRECTORY)"
	cp *.h $(INSTALL_DIRECTORY)
	cp *.a $(INSTALL_DIRECTORY)

debug        : tags $(OUTPUT_DBG)
release      : tags $(OUTPUT)
makefile.dep : depend

#
# Build commands
#
%.o : %.cpp
	@echo "Compiling release $<"
	@$(CC) -c $(CCFLAGS) $< -o $@

%_dbg.o : %.cpp
	@echo "Compiling debug $<"
	@$(CC) -c $(CCFLAGS_DBG) $< -o $@

$(OUTPUT): $(LIB_OBJS)
	@echo "Building release library ..."
	@ar rs $(OUTPUT) $(LIB_OBJS)

$(OUTPUT_DBG): $(LIB_OBJS_DBG)
	@echo "Building debug library ..."
	@ar rs $(OUTPUT_DBG) $(LIB_OBJS_DBG)

examples: debug release
	cd examples/example2; $(MAKE) 
	cd examples/example3; $(MAKE) 

clean:
	rm -f *.o *.a core *~ TAGS

cleanall: 
	rm -f *.o *.a core *~ TAGS makefile.dep
	cd examples/example2; $(MAKE) clean
	cd examples/example3; $(MAKE) clean

tidy :
	@echo "Removing object files"
	@rm -f *.o *~ core core.* 

tags : 
	@echo Starting build at:
	@date
	@echo "Building tags file"
	@etags *.cpp *.h

depend: 
	@if [ ! -f makefile.dep ]; then \
	echo "Building dependancies"; \
	rm -f makefile.dep; \
	for i in $(SRCS) ; \
	do \
	$(CC) $(INCLUDES) -MM -c $$i | sed 's/.*\.o/&/' >> makefile.dep ; \
	$(CC) $(INCLUDES) -MM -c $$i | sed 's/.*\.o/&/' | sed 's/\.o/_dbg\.o/' >> makefile.dep ; \
	done; \
	fi

-include makefile.dep
