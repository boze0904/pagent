#############################
#	log_send.cอั			#
#	Makefile				#
#############################

SRC = output_log.c read_prop_file.c log_send.c

CCC = /usr/local/bin/gcc
#CCC = /opt/SUNWspro/bin/CC-4.2
#CFLAGS = -c -g -DDEBUG
CFLAGS = -c -O2

all: $(SRC:.c=.o)

%.o : %.c
	$(CCC) $(CFLAGS) $(SRC)

clean :
	@rm -f $(SRC:.c=.o)

