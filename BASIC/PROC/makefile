#
# *.pc ファイル用メイクファイル
# *.pcファイルからオブジェクトファイルを生成します
#


.SUFFIXES: .o .c .pc

# ソースファイル名を書き連ねる
PCFILE = db_func.pc \
		Proc_BUILD_Insert.pc Proc_BUILD_Update.pc Proc_BUILD_Delete.pc \
		Proc_BRANCH_Insert.pc Proc_BRANCH_Update.pc Proc_BRANCH_Delete.pc \
		Proc_ISP_CONTACT_Insert.pc Proc_ISP_CONTACT_Update.pc Proc_ISP_CONTACT_Delete.pc \
		Proc_ISP_BRANCH_Insert.pc Proc_ISP_BRANCH_Update.pc Proc_ISP_BRANCH_Delete.pc \
		Proc_ISP_NTE_Insert.pc Proc_ISP_NTE_Update.pc Proc_ISP_NTE_Delete.pc \
		Proc_MEDO_CONTACT_Insert.pc Proc_MEDO_CONTACT_Update.pc Proc_MEDO_CONTACT_Delete.pc \
		Proc_MAKER_CONTACT_Insert.pc Proc_MAKER_CONTACT_Update.pc Proc_MAKER_CONTACT_Delete.pc \
		Proc_EQP_Insert.pc Proc_EQP_Update.pc Proc_EQP_Delete.pc \
		Proc_COMPONENT_Insert.pc Proc_COMPONENT_Delete.pc


#		Proc_ISP_Insert.pc Proc_ISP_Update.pc Proc_ISP_Delete.pc \
#		Proc_ISPSetting_Insert.pc Proc_ISPSetting_Update.pc Proc_ISPSetting_Delete.pc
#		Proc_COMPONENT_Update.pc
		

# 実際に作りたいオブジェクトファイル群
OBJS = $(PCFILE:.pc=.c)
OBJS1 = $(OBJS:.c=.o)

PROCBASE = /opt/oracle/product/8.0.6
PROC = $(PROCBASE)/bin/proc
#PROCFLAG = code=ansi_c ireclen=132 oreclen=132 select_error=no
PROCFLAG = code=cpp ireclen=132 oreclen=132 select_error=no sys_include=/usr/include

#CCC = /usr/local/bin/gcc
CCC = /opt/SUNWspro/bin/CC-4.2
#CCC = cc
# -Werror -Wall をつけるとエラーがいっぱい出るのでとりあえず削除
###CFLAG = -c -g -DDEBUG -Werror -Wall
###CFLAG = -O2 -Werror -Wall
#CFLAG = -c -g -DDEBUG
CFLAG = -c -O2 

all: $(OBJS1)

$(OBJS1):$(OBJS) 

# *.pc -> *.c への依存関係
.pc.c: 
	$(PROC) $(PROCFLAG) iname=$<

# *.c -> *.o への依存関係
# なぜか、.c.o: という書き方が通用しない
%.o: %.c
	$(CCC) $(CFLAG) $<

clean:
	@rm -f *.[co]

