#********************************************************************
#        Copyright (c) 2006, Lee Patterson & Ant Works Software
#        http://ssobjects.sourceforge.net
#
#        created  :    July 4, 2001
#        filename :    Makefile
#        author   :    Lee Patterson (workerant@users.sourceforge.net)
#        
#        purpose  :    Make file for Linux operating system.
#*********************************************************************

#when adding a new source file, you have to rebuild the dependancy
#easiest way is to 'make clean all' to rebuild everything,
#or 'rm makefile.dep'

TERM    := dumb

SERVER  := pingserver
CLIENT	:= pingclient
SERVER_DBG  := pingserver_dbg
CLIENT_DBG	:= pingclient_dbg

#
# source files used for project(s)
#
SERVER_SRCS := server.cpp 
CLIENT_SRCS := client.cpp

#
#-----------------------------------------------------
#

ALL_SRCS := $(SERVER_SRCS) $(CLIENT_SRCS)

#
# Convert the source files into object files
#
SERVER_OBJS 	:= $(SERVER_SRCS:.cpp=.o)
SERVER_OBJS_DBG := $(SERVER_SRCS:.cpp=_dbg.o)
CLIENT_OBJS 	:= $(CLIENT_SRCS:.cpp=.o)
CLIENT_OBJS_DBG := $(CLIENT_SRCS:.cpp=_dbg.o)

#
# Include and library paths used in source
#
INCLUDES := -I../.. -I.
LIBS_DBG := -L../.. -lssobjects_dbg -lpthread 
LIBS     := -L../.. -lssobjects     -lpthread 

#
# Build options
#
#WARNINGS = -W -Wall -Woverloaded-virtual -Weffc++
CCFLAGS_DBG := $(INCLUDES) $(WARNINGS) -g -fPIC  -D_REENTRANT -DDEBUG  -DUSE_NCURSES
CCFLAGS     := $(INCLUDES) $(WARNINGS) -fPIC -O2 -D_REENTRANT -DNDEBUG -DUSE_NCURSES

#
# Compiler used
#
CC := g++

#
# Targets
#
none         : debug
all          : debug release
release      : tags $(SERVER)     $(CLIENT) 
debug        : tags $(SERVER_DBG) $(CLIENT_DBG)
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

$(SERVER): $(SERVER_OBJS)
	@echo "*** Building release $(SERVER)"
	@$(CC) -o $(SERVER) $(SERVER_OBJS) $(LIBS) -lpthread

$(SERVER_DBG): $(SERVER_OBJS_DBG)
	@echo "*** Building debug $(SERVER_DBG)"
	@$(CC) -o $(SERVER_DBG) $(SERVER_OBJS_DBG) $(LIBS_DBG) -lpthread

$(CLIENT): $(CLIENT_OBJS)
	@echo "*** Building release $(CLIENT)"
	@$(CC) -o $(CLIENT) $(CLIENT_OBJS) $(LIBS) -lpthread

$(CLIENT_DBG): $(CLIENT_OBJS_DBG)
	@echo "*** Building debug $(CLIENT_DBG)"
	@$(CC) -o $(CLIENT_DBG) $(CLIENT_OBJS_DBG) $(LIBS_DBG) -lpthread

tidy :
	@echo "Removing object files"
	@rm -f *.o

clean : 
	@echo "Removing all intermediate files"
	@rm -f  $(SERVER) $(SERVER_OBJS) $(SERVER_DBG) $(SERVER_OBJS_DBG) \
		$(CLIENT) $(CLIENT_OBJS) $(CLIENT_DBG) $(CLIENT_OBJS_DBG) \
		core makefile.dep TAGS *.log *~

../../ssobjects.a:
	cd ../..; $(MAKE) release

../../ssobjects_dbg.a:
	cd ../..; $(MAKE) debug

tags : 
	@echo Started build at:
	@date
	@echo "Building tags file"
	@etags *.cpp

depend: 
	@if [ ! -f makefile.dep ]; then \
	echo "Building dependancies"; \
	rm -f makefile.dep; \
	for i in $(ALL_SRCS) ; \
	do \
	$(CC) -DUSE_NCURSES $(INCLUDES) -MM -c $$i >> makefile.dep ; \
	$(CC) -DUSE_NCURSES $(INCLUDES) -MM -c $$i | sed 's/\.o/_dbg\.o/' >> makefile.dep ; \
	done; \
	echo "$(SERVER): ../../ssobjects.a" >> makefile.dep; \
	echo "$(SERVER_DBG): ../../ssobjects_dbg.a" >> makefile.dep; \
	echo "$(CLIENT): ../../ssobjects.a" >> makefile.dep; \
	echo "$(CLIENT_DBG): ../../ssobjects_dbg.a" >> makefile.dep; \
	fi

-include makefile.dep
