#
# DB P-Agent
# £Ð¡Ý£Á£ç£å£î£ôÍÑ Makefile ¤Ç¤¹¡£
#

 
TOOL = ../Tool
PAGLIB = ../lib
TK = ../../TK
LOG = ./LOG

.SUFFIXES: .o .c .cc

SRC = dbp_port.cc dbp_portsub.cc db_pagent.cc gw_if.cc dbpag_if.cc 
#SRC = dbp_port.cc dbp_portsub.cc db_pagent.cc gw_if.cc dbpag_if.cc pagent_trace.cc 
#OBJ = $(SRC:.cc=.o) $(TOOL)/tool.o $(TOOL)/tcp_if.o pagent_trace.o
#OBJ = $(SRC:.cc=.o) $(TOOL)/tool.o $(TOOL)/tcp_if.o $(LOG)/output_log.o $(LOG)/read_prop_file.o
OBJ = $(SRC:.cc=.o) $(TOOL)/tcp_if.o $(LOG)/output_log.o $(LOG)/read_prop_file.o

PA_LIB = $(PAGLIB)/pagent_lib.a $(PAGLIB)/pacomn_sol.a $(PAGLIB)/tk60_sol.a $(PAGLIB)/tk60_jpn.a
OSI_LIB = -L$(TK) -L/export/home/netxroot/Lib -ltk2lite_sol $(TK)/tk2_sol.a $(TK)/tk1_sol.a

CCC     = /opt/SUNWspro/bin/CC-4.2
LFLAGS  = -lsocket -lnsl
CFLAGS  = -O2 -I.. -I$(TK) -I$(TOOL) -I$(LOG)
#CFLAGS  = -g -DDEBUG -I.. -I$(TK) -I$(TOOL) -I$(LOG)


all : db_pagent

db_pagent : $(SRC:.cc=.o)
	$(CCC) $(PA_LIB) $(OSI_LIB) $(LFLAGS) $(OBJ) -o $@

%.o : %.cc
	$(CCC) $(CFLAGS) -c $<


clean:
	@rm -f db_pagent $(SRC:.cc=.o)

