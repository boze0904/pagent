#
# DB P-Agent
# ＧＷ用 Makefile です。
#

TOOL = ../../Tool
LOG = ../LOG
PC = PROC
DB_BASE = /opt/oracle/product/8.0.6
DB_LDIR = $(DB_BASE)/lib


SRC = IsdnMain.c IsdnFindFreePortNumber.c import_dar.c

PAOBJ    = ../dbpag_if.o
TLOBJ    =  $(TOOL)/tcp_if.o
DBOBJ    = $(LOG)/output_log.o $(LOG)/read_prop_file.o $(PC)/db_func.o

# PRO*C オブジェクトを書いてね
PROC_OBJ = $(PC)/ProcFindFreePortNumber.o \
			$(PC)/proc_import_dar_delete.o $(PC)/proc_import_dar_insert.o

OBJ     = $(PAOBJ) $(TLOBJ) $(DBOBJ) $(PROC_OBJ)
DB_LIB = -lclntsh -lcommon -lcore4 -lnlsrtl3

CCC     = /opt/SUNWspro/bin/CC-4.2
#CCC     = /usr/local/bin/gcc
CFLAGS  = -c -O2 -I.. -I$(PC) -I$(TOOL) -I$(LOG)
#CFLAGS  = -c -g -I.. -I$(PC) -I$(TOOL) -I$(LOG) -DDEBUG
LFLAGS  = -lsocket -lnsl -ljapanese -L$(DB_LDIR) $(DB_LIB)


all : isdnMain

isdnMain: $(SRC:.c=.o)
	$(CCC) $(LFLAGS) $(OBJ) $(PROOBJ) $(SRC:.c=.o) -o $@

%.o: %.c
	$(CCC) $(CFLAGS) $<


clean : 
	@rm -f isdnMain $(SRC:.c=.o)

