#
# *.pc ファイル用メイクファイル
# *.pcファイルからオブジェクトファイルを生成します
#


.SUFFIXES: .o .c .pc

# ソースファイル名を書き連ねる
PCFILE = ProcFindFreePortNumber.pc proc_import_dar_insert.pc proc_import_dar_delete.pc


# 実際に作りたいオブジェクトファイル群
OBJS = $(PCFILE:.pc=.c)
OBJS1 = $(OBJS:.c=.o)

PROCBASE = /opt/oracle/product/8.0.6
PROC = $(PROCBASE)/bin/proc
#PROCFLAG = code=ansi_c ireclen=132 oreclen=132 select_error=no include=../
PROCFLAG = code=cpp ireclen=132 oreclen=132 select_error=no include=../ sys_include=/usr/include

#CCC = /usr/local/bin/gcc
CCC = /opt/SUNWspro/bin/CC-4.2
#CCC = cc
# -Werror -Wall をつけるとエラーがいっぱい出るのでとりあえず削除
#CFLAG = -c -g -DDEBUG
CFLAG = -c -O2 -I..

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
	@rm -f $(PCFILE:.pc=.c) $(PCFILE:.pc=.o)

