#
# DB P-Agent
# £Ç£×ÍÑ Makefile ¤Ç¤¹¡£
#

TOOL = ../../Tool
PC = PROC
DB_BASE = /opt/oracle/product/8.0.6
DB_LDIR = $(DB_BASE)/lib

PROOBJ = $(PC)/Proc_BUILD_Insert.o $(PC)/Proc_BUILD_Update.o $(PC)/Proc_BUILD_Delete.o \
			$(PC)/Proc_BRANCH_Insert.o $(PC)/Proc_BRANCH_Update.o $(PC)/Proc_BRANCH_Delete.o \
			$(PC)/Proc_EQP_Insert.o $(PC)/Proc_EQP_Update.o $(PC)/Proc_EQP_Delete.o \
			$(PC)/Proc_ISP_BRANCH_Insert.o $(PC)/Proc_ISP_BRANCH_Update.o $(PC)/Proc_ISP_BRANCH_Delete.o \
			$(PC)/Proc_ISP_CONTACT_Insert.o $(PC)/Proc_ISP_CONTACT_Update.o $(PC)/Proc_ISP_CONTACT_Delete.o \
			$(PC)/Proc_ISP_NTE_Insert.o $(PC)/Proc_ISP_NTE_Update.o $(PC)/Proc_ISP_NTE_Delete.o \
			$(PC)/Proc_MAKER_CONTACT_Insert.o $(PC)/Proc_MAKER_CONTACT_Update.o $(PC)/Proc_MAKER_CONTACT_Delete.o \
			$(PC)/Proc_MEDO_CONTACT_Insert.o $(PC)/Proc_MEDO_CONTACT_Update.o $(PC)/Proc_MEDO_CONTACT_Delete.o \
			$(PC)/Proc_COMPONENT_Insert.o $(PC)/Proc_COMPONENT_Delete.o

SRC = basic.c \
		build_insert.c build_update.c build_delete.c \
		branch_insert.c branch_update.c branch_delete.c \
		eqp_insert.c eqp_update.c eqp_delete.c \
		isp_branch_insert.c isp_branch_update.c isp_branch_delete.c \
		isp_contact_insert.c isp_contact_update.c isp_contact_delete.c \
		isp_nte_insert.c isp_nte_update.c isp_nte_delete.c \
		maker_contact_insert.c maker_contact_update.c maker_contact_delete.c \
		medo_contact_insert.c medo_contact_update.c medo_contact_delete.c \
		component_insert.c component_delete.c

PAOBJ   = ../dbpag_if.o
TLOBJ =  $(TOOL)/tcp_if.o
#DBOBJ   = ../LOG/log_send.o ERRLOG_DIR/err_send.o $(PC)/db_func.o
DBOBJ   = ../LOG/output_log.o ../LOG/read_prop_file.o $(PC)/db_func.o 
OBJ     = $(PAOBJ) $(TLOBJ) $(DBOBJ)
DB_LIB = -lclntsh -lcommon -lcore4 -lnlsrtl3 -lclient

CCC     = /opt/SUNWspro/bin/CC-4.2
#CCC     = /usr/local/bin/gcc
CFLAGS  = -c -O2 -I.. -I$(PC) -I$(TOOL) -I../LOG
#CFLAGS  = -c -O2 -I.. -I$(PC) -I$(TOOL) -I../LOG -DDEBUG
LFLAGS  = -lsocket -lnsl -ljapanese -L$(DB_LDIR) $(DB_LIB) -I../LOG
#LFLAGS  = -lsocket -lnsl -ljapanese -L$(DB_LDIR) $(DB_LIB) -I../LOG -DDEBUG


all : basic

basic : $(SRC:.c=.o)
	$(CCC) $(LFLAGS) $(OBJ) $(PROOBJ) $(SRC:.c=.o) -o $@

%.o : %.c
	$(CCC) $(CFLAGS) $<


clean : 
	@rm -f basic $(SRC:.c=.o)

