/****************************************************************************/
/* file Name  dbp_port.cc                                                   */
/*                                                                          */
/* Functions:																*/
/*	 Exported:																*/
/*		openPort															*/
/*		closePort															*/
/*		readPort															*/
/*		writePort															*/
/*		breakPort															*/
/*		getDeviceName														*/
/*		getPortName															*/
/*		toAscii																*/
/*		fromAscii															*/
/*		parseArgs															*/
/*																			*/
/* Description:																*/
/*	 This module has all of the functions necessary to send commands to		*/
/*	 a tcp/ip port.															*/
/*                                                                          */
/* Create : 2000.12.15(dcm_pot.c����̗��p)                                 */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <limits.h>
#include "dbp_port.h"
#include "gw_if.h"
#include "pagent_trace.h"
#include "dbp_portsub.h"
#include "db_pagent.h"
#include "output_log.h"
#include "read_prop_file.h"

#define	OSI_HOME	"OSI_HOME"


/****************************************************************************/
/* Static Global Variables */
static char device[41];				/* �f�o�C�X��                        */
DBP_INF		dbp_inf;				/* NTT IP DB P-Agent ������        */
int	port_fd;						/* �ǂݍ��ݑΏ�fd:processInput() I/F */

/****************************************************************************/
/* Functions */
int	writePort_2(char*,unsigned int);
void	closePort();
void	breakPort();
char	*getDeviceName();
char	*getPortName();
void	checkIPMH();
void	dummy_term(int);

/*** ���C�u�����֐� ***/
extern void setFifoReaders(fd_set*);
extern void setSocketReaders(fd_set*);


/****************************************************************************/
/* Name:																	*/
/*   openPort                                                               */
/*																			*/
/* Parameters:																*/
/*   char	*host_config: �z�X�g�����R���t�B�O�t�@�C����					*/
/*   char	*sspeed		: baud rate to be used			NO USED				*/
/*   int	tandem		: set XON/XOFF flow control if > 0					*/
/*																			*/
/* Return:																	*/
/*   file descriptor, otherwise -1											*/
/*																			*/
/* Description:																*/
/*   It opens the port in read/write mode and sets the characteristics.		*/
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int openPort( char *host_config, char *sspeed, int tandem )
{
	int		i;								/* ���[�N               */
	int		ret;
	int		lvl;							/* ���O���x��           */
	char	*c_p;							/* ���[�N               */
	char	*lpath;							/* ���O�p�X             */
	char	*home;							/* OSI_HOME �p�X        */
	char	configfile[DCM_MXCNFLEN+1];		/* �R���t�B�O�t�@�C���� */
	char	logfile[512];


	sighold( SIGUSR2 );


	/* ��������z�X�g���ƃR���t�B�O�t�@�C�������l�� */
	sscanf( host_config, "%s %s ", dbp_inf.hostname, configfile );
	/* �������d�n�k�������Ō���I */
	dbp_inf.eol[0] = DCM_EOLCODE;
	dbp_inf.eol[1] = 0x00;
	dbp_inf.eol_len = 1;

	/* �R���t�B�O�t�@�C����	�i�t���p�X�j�l�� */
	c_p = getenv( DBP_ENVPATH );
	if( c_p == NULL ){
		/* �G���[���b�Z�[�W�\�� */
		printf( "%s:%d ���ϐ�(%s)���ݒ�\n", __FILE__, __LINE__, DBP_ENVPATH );
		fflush( stdout );
		/* �ُ�I�� */
		return -1;
	}
	sprintf( dbp_inf.configfile, "%s/%s", c_p, configfile );

	/* ���݊m�F */
	ret = access( dbp_inf.configfile, R_OK );
	if( ret ){
		/* �G���[���b�Z�[�W�\�� */
		printf( "%s:%d �R���t�B�O�t�@�C�� access(%s) error:%d\n",
								__FILE__,__LINE__, dbp_inf.configfile, errno );
		fflush( stdout );
		/* �ُ�I�� */
		return -1;
	}
#ifdef DEBUG
printf( "%s:%d �R���t�B�O�t�@�C�� access(%s)\n",
                                __FILE__,__LINE__, dbp_inf.configfile );
#endif
/********** �ǉ� �������� V01 **********/
	/*** ���O�o�͂̏��� ***/
	/* �ݒ�t�@�C�����I�[�v�� */
	ret = openPropFile( dbp_inf.configfile );
	if( ret ){
		/* �G���[���b�Z�[�W�\�� */
		printf( "%s:%d �R���t�B�O�t�@�C�� openPropFile(%s) error:%d\n",
								__FILE__, __LINE__, dbp_inf.configfile, errno );
		fflush( stdout );
		/* �ُ�I�� */
		return -1;
	}

	/* ���O�֌W�v���p�e�B�l�̎擾 */
	lpath = getProperty( LOGPATH );
	c_p = getProperty( LOGLVL );
#ifdef DEBUG
printf( "Log path %s\n", lpath );
printf( "Log level %s\n", c_p );
#endif

	if( lpath == NULL ){
		home = getenv( OSI_HOME );
		if( home == NULL ){
            printf("�V�X�e���G���[\n");
            fflush( stdout );
            exit(1);
		}
		sprintf( logfile, "%s/Log", home );
		lpath = logfile;
    }

	if( c_p == NULL  ){
		lvl = WARNING;
	}
	else{
		lvl = atoi( c_p );
	}

	/* ���O�t�@�C���̏��� */
	openLog( lpath, lvl );

	/* �ݒ�t�@�C�������̌�n�� */
	closePropFile();

/********** �ǉ� �����܂� V01 **********/


	/*	������̃f�[�^���͂Ȃ��� readPort( )�ɃC�x���g���N�������߁AFIFO ��	*/
	/*	�쐬���ăf�[�^���������݁A���̋L�q�q�� main( ) �ɕԋp����B			*/
	/*	readPort( ) �ōŏ��̌Ăяo���𔻒f���邽�߂ɁATEMP FIFO �t�@�C���L	*/
	/*	�q�q�̖������m�F���Ă���B											*/

	/* TMP FIFO�t�@�C���쐬 */
	c_p = (char*)tempnam( "/tmp", "dbp" );
	strcpy( dbp_inf.tmpfifo, c_p );
	free( c_p );	/* tempnam() �g�p���free()���� */

	/* FIFO�t�@�C���쐬 */
	i = mkfifo( dbp_inf.tmpfifo, 0666 );
	if( i == -1 ){
		/* �G���[���b�Z�[�W�\�� */
		printLog( ERROR, "%s:%d -1 = mkfifo(%s, 0666) errno:%d",__FILE__,__LINE__,
														dbp_inf.tmpfifo, errno);
		return -1;
	}

	/* FIFO��OPEN ���A"TEMP FIFO �t�@�C���L�q�q" ��L���ɂ��� */
	dbp_inf.tmpfifo_fd = open( dbp_inf.tmpfifo, O_NDELAY | O_RDWR, 0666 );
	if( dbp_inf.tmpfifo_fd == -1 ){
		/* �G���[���b�Z�[�W�\�� */
		printLog( ERROR, "%s:%d -1 = open(%s, O_NDELAY | O_RDWR, 0666) errno:%d",
								__FILE__,__LINE__,	dbp_inf.tmpfifo, errno);

		/* "TEMP FIFO �t�@�C���L�q�q" �𖳌��ɂ��� */
		dbp_inf.tmpfifo_fd = 0;

		return -1;
	}

	/* FIFO�t�@�C���ɁA�K���ȃf�[�^����� */
	i = write( dbp_inf.tmpfifo_fd, "dbp", 3 );
	if ( i != 3 ){
		/* �G���[���b�Z�[�W�\�� */
		printLog( ERROR, "%s:%d %d = write(%d, \"dbp\", 3 ) errno:%d",
						__FILE__, __LINE__,	i, dbp_inf.tmpfifo_fd, errno );
		return -1;
	}

	/* �V�O�i������:SIGCHLD	(�q���̎��ɗl�ɋ����͂Ȃ�) */
	signal( SIGCHLD, SIG_IGN );

	signal( SIGSEGV, dummy_term );

	/* �f�o�C�X�������� */
	device[0] = 0x00;

	/* TEMP FIFO �t�@�C���L�q�q" ��ԋp���� */
	return dbp_inf.tmpfifo_fd;

	sigrelse( SIGUSR2 );

	printLog( DEBUGING, "%s:%d openPort(%d) ����I��", 
									__FILE__, __LINE__, dbp_inf.tmpfifo_fd );

} /* openPort */



/****************************************************************************/
/* Name:                                                                    */
/*   closePort                                                              */
/*																			*/
/* Parameters:																*/
/*	 NONE																	*/
/*																			*/
/* Return:																	*/
/*	 NONE																	*/
/*																			*/
/* Description:																*/
/*	Closes the ttyport and sets the port to -1 as an indication.			*/
/*	TMP FIFO ���L���ȏꍇ�㏈��������B										*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
void closePort()
{
	printLog( DEBUGING, "%s:%d closePort() call", __FILE__, __LINE__ ); /* V01 */

	/* TEMP FIFO �t�@�C�����J���Ă���ꍇ */
	if( dbp_inf.tmpfifo_fd != 0 ){
		/* TEMP FIFO �t�@�C�� ����� */
		close( dbp_inf.tmpfifo_fd );
	}
	/* TEMP FIFO �t�@�C���L���̏ꍇ */
	if( dbp_inf.tmpfifo[0] != 0x00 ){
		/* TEMP FIFO �t�@�C���폜 */
		unlink( dbp_inf.tmpfifo );
	}

	/* �f�^�v�I�� */
	gw_kill();

	/* NTT IP DB P-Agent G/W �T�u���� �I������ */
	dbpsub_term();

} /* closePort */



/****************************************************************************/
/* Name:                                                                    */
/*   writePort                                                              */
/*																			*/
/* Parameters:																*/
/*   char	*data_buff: NetExpert����̓��̓f�[�^                           */
/*	 int	data_len  : length of data if set                               */
/*																			*/
/* Return:																	*/
/*   number of characters actually written							    	*/
/*																			*/
/* Description:																*/
/*   NetExpert ����̃��b�Z�[�W���������܂�                                 */
/*   Writes the contents of buff to the ttyport.							*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int writePort( char *data_buff, unsigned int data_len )
{
	static char sbuf[DCM_MXSNDLEN+1] ;				/* SAVE�o�b�t�@			*/
	static int	slen = 0;						 	/* SAVE�o�b�t�@��		*/
	int			i,i2;								/* ���[�N 				*/
	int			cnt;								/* �J�E���^ 			*/
	char		*c_p, *c_p2;						/* ���[�N				*/
	char		c_buff[DCM_MXSNDLEN+DCM_MXEOLLEN+sizeof(DCM_TRACE_DATA_TERM)+1];
													/* ���[�N				*/
	printLog( DEBUGING, "*** writePort() ***" );
	
	printDump( data_buff );

	c_p2 = data_buff;
	while( 1 ){
		/* �c��f�[�^�������߂�	 -> i */
		i = (int)( (unsigned int)c_p2 - (unsigned int)data_buff );
		i = data_len - i;
		printLog( DEBUGING, "%s:%d �c�f�[�^�T�C�Y %d byte", __FILE__, __LINE__, i );	/* V01 */
		/* EOL���� */
		c_p = strstr( c_p2, dbp_inf.eol );

		/* EOL�Ȃ� */
		if( c_p == NULL ){
			printLog( DEBUGING, "%s:%d (writePort) EOL �݂���Ȃ�", __FILE__, __LINE__ );	/* V01 */
			/* �ۑ��f�[�^���m�F */
			if( (slen + i) > sizeof(sbuf) ){
				printLog( DEBUGING, "%s:%d �ۑ��f�[�^��:(%d + NEW:%d) > �ۑ��o�b�t�@��:%d",
							__FILE__, __LINE__, slen, i, sizeof(sbuf) );	/* V01 */
				/* SAVE�f�[�^�N���A */
				memset( sbuf, 0, sizeof(sbuf) );
				slen = 0;
				return -1;
			}

			/* SAVE�o�b�t�@�Ƀf�[�^��ۑ� */
			strncpy( &sbuf[slen], c_p2, i );		/* �ǉ�         */
			slen += i;								/* �ǉ����̉��Z */
			printLog( DEBUGING, "%s:%d (writePort) SAVE�o�b�t�@�Ƀf�[�^��ۑ� slen:%d data_len:%d", __FILE__, __LINE__, slen, data_len );	/* V01 */
			/* ���̃f�[�^�ɍ���̃f�[�^�̑���������E�E�E�E */
			return data_len;
		}

		/* string�֐����g�p���邽�ߕ��������B */
		*c_p = 0;

		/* ����f�[�^���̊m�F */
		i = strlen(c_p2) + strlen(sbuf);
		printLog( DEBUGING, "%s:%d �ő呗�M�f�[�^��    :%d byte", __FILE__, __LINE__, DCM_MXSNDLEN );	/* V01 */
		printLog( DEBUGING, "%s:%d ���񏈗�����f�[�^��:%d byte", __FILE__, __LINE__, i );	/* V01 */
		if( sizeof(c_buff) < (i+DCM_MXEOLLEN) ){ /* �o�b�t�@���傫��	*/
			printLog( DEBUGING, "%s:%d ���[�N�T�C�Y:%d byte ����f�[�^�p��", 
											__FILE__, __LINE__, sizeof(c_buff) );	/* V01 */
			/* SAVE�f�[�^�N���A */
			memset( sbuf, 0, sizeof(sbuf) );
			slen = 0;
		}
		else{	/* ��ƃo�b�t�@�Ɏ��܂� */
			/* ��Ɨp�o�b�t�@�ɕۑ��f�[�^���R�s�[ */
			strcpy( c_buff, sbuf );

			/* ����f�[�^��ǉ��i�����f�[�^�ƌď́j */
			strcat( c_buff, c_p2 );

			/* SAVE�f�[�^�N���A */
			memset( sbuf, 0, sizeof(sbuf) );
			slen = 0;

			/* �����f�[�^���𓾂� */
			i2 = strlen( c_buff );

			/* ���b�Z�[�W�f�[�^�̏��� */
			i =  writePort_2( c_buff, i2 );
			printLog( DEBUGING, "%s:%d %d = writePort_2(, %d)", __FILE__, __LINE__, i, i2 );	/* V01 */

			/* �H */
			usleep( 10 );
		}

		/* ����f�[�^�� EOL �̎��iMIN ����̂Q�߂̃f�[�^�̐擪�j�� */
		/* �|�C���g����B                                            */
		c_p2 = c_p + dbp_inf.eol_len;

		printLog( DEBUGING, "%s:%d(writePort) c_p2:%d - data_buff:%d == data_len:%d",
							__FILE__, __LINE__, c_p2, data_buff, data_len );	/* V01 */

		/* ����f�[�^����|�[�g�ɏo�͂�����������f�[�^���Ɠ����ꍇ�A		*/
		/* �܂�A���܈������f�[�^�̍Ōオ����f�[�^�̍Ō�Ȃ�I������		*/
		if( (int)(c_p2 - data_buff) == data_len )
			break;
	}
	return data_len ;

} /* writePort */



/****************************************************************************/
/* Name:                                                                    */
/*   writePort_2                                                            */
/*																			*/
/* Parameters:																*/
/*   char	*data_buff: NetExpert����̓��̓f�[�^                           */
/*	 int	data_len  : length of data if set                               */
/*																			*/
/* Return:																	*/
/*      -1: �I���R�}���h��M                                                */
/*   other: number of characters actually written                           */
/*																			*/
/* Description:																*/
/*   Writes the contents of buff to the ttyport.							*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int	writePort_2( char *data_buff, unsigned int data_len )
{
	int		i;	/* ���[�N */


	printLog( DEBUGING, "*** writePort_2() ***" );

	/**** NetExpert ����̃��b�Z�[�W�̉�� ****/
	/*** P-Agent�I���R�}���h ***/
	if( !strncmp(data_buff, DBP_TERM, strlen(DBP_TERM)) ){
		/* �I���R�}���h��G/W�ɑ��M���� */
		return -1;
	}
	/*** DBP P-Agent����R�}���h�ȊO�iG/W �R�}���h�j***/
	else {
		/* NTT IP DB P-Agent SUB NetExpert -> G/W */
		dbpsub_netx_gw( data_buff, data_len );
	}

	/* ��ΐ���I�� */
	return data_len;

} /* writePort_2 */



/****************************************************************************/
/* Name:																	*/
/*   readPort                                                               */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*data_buff: G/W����̓��̓f�[�^                                 */
/*   int	len       : maximun len of data to read                         */
/*																			*/
/* Return:																	*/
/*   number of characters actually read									    */
/*																			*/
/* Description:																*/
/*   Writes the contents of buff to the ttyport.							*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int readPort( char *data_buff, unsigned int data_len )
{
	fd_set		readers;			/* �t�@�C���f�B�X�N���v�^  */
	int			rcv_len=0;			/* ��M�f�[�^��            */
	int			rc;					/* select() ���^�[���R�[�h */
	int 		cnt;				/* �J�E���^                */
	int 		i;					/* ���[�N                  */
	char		c_buff[5];			/* ���[�N                  */
	char		*c_p;				/* ���[�N                  */

	/****************************/
	/*	SIGUSR2���z�[���h����	*/
	sighold( SIGUSR2 );

	printLog( DEBUGING, "%s:%d readPort(buff,len:%d)", __FILE__, __LINE__, data_len );	/* V01 */
	memset( c_buff, 0, sizeof(c_buff) );

	/* TMP FIFO �̗L�����m�F���� */
	if( dbp_inf.tmpfifo_fd != 0 ){	/* �����Ăяo�� */
		/* TEMP FIFO�t�@�C������ */
		read( dbp_inf.tmpfifo_fd, c_buff, 3 );
		close( dbp_inf.tmpfifo_fd );
		unlink( dbp_inf.tmpfifo );
		/* TEMP FILE ��񖳌��� */
		dbp_inf.tmpfifo_fd = 0;
		dbp_inf.tmpfifo[0] = 0x00;

		/* �f�^�v�v���Z�X�N�� */
		i = gw_exec( dbp_inf.configfile );
		if( i == -1 ){
			sigrelse(SIGUSR2);
			/* -1 �� return ���Ă����ȂȂ��̂Ŏ����� SIGTERM ���M*/
			kill(getpid( ), SIGTERM);
			pause( );
		}

		/* NTT IP DB P-Agent G/W �T�u���� �������� */
		i = dbpsub_init();
		if( i == -1 ){
			printLog( DEBUGING, "%s:%d -1 = dbpsub_init()", __FILE__, __LINE__ );	/* V01 */
			/* �ُ�I�� */
			sigrelse(SIGUSR2);
			/* -1 �� return ���Ă����ȂȂ��̂Ŏ����� SIGTERM ���M */
			kill(getpid( ), SIGTERM);
			pause( );
		}

		/* ��������{���[�v */
		while( 1 ){
			/* �����ݒ� */
			FD_ZERO( &readers );
			setFifoReaders( &readers );
			setSocketReaders( &readers );
			gw_fdset( &readers );
			printLog( DETAIL, "%s:%d call select()", __FILE__, __LINE__ );	/* V01 */

			sigrelse( SIGUSR2 );
			rc = select( FD_SETSIZE, &readers, (fd_set *)0, (fd_set *)0,
													(struct timeval *)0 );
			/* ���͂ӂ�킯 */
			if( rc == -1 ) {
				checkIPMH();
			} else {
				sighold( SIGUSR2 );
				processInput( rc, &readers, errno );
			}
		} /* �������[�v */
	}


	/* �ȍ~�A�f�v�v���Z�X����f�[�^��ǂݍ��ޏ���                   */
	/* readPort( )�̃p�����[�^�́A�o�b�t�@�ƃo�b�t�@�� �Œ�Ȃ̂ŁA */
	/* port_fd �O���ϐ��� I/F �ɂ���B                              */
	/* �f�[�^���ǂݍ��݁i�S�o�C�g�j                                 */
	for( cnt = 0, i = 0, c_p = (char *)&rcv_len; cnt < 4 ; cnt += i ){
		i = read( port_fd, (c_p+cnt), 4 - cnt );
		printLog( DEBUGING, "%s:%d %d = read( %d, (c_p+%d), %d)",
							__FILE__, __LINE__, i, port_fd, cnt, 4 - cnt );	/* V01 */
		if( i <= 0 ){
			printLog( DEBUGING, "%s:%d %d = read( %d, (c_p+%d), %d) errno:%d",
								__FILE__, __LINE__, i, port_fd, cnt, 4 - cnt, errno );	/* V01 */
			/* �ُ�I�� */
			return -1;
		}
	} /* for */

	printLog( DEBUGING, "%s:%d �f�[�^���l�� %d", __FILE__, __LINE__, rcv_len );	/* V01 */

	/* �f�[�^���`�F�b�N */
	if( rcv_len > (int)data_len ){
		printLog( DEBUGING, "%s:%d �f�[�^�� %d > �o�b�t�@�� %d",
							 __FILE__, __LINE__, rcv_len, data_len );	/* V01 */
		return 0;
	}

	/* �f�[�^�ǂݍ��� */
	for( cnt = 0, i = 0; cnt < rcv_len; cnt += i ){
		i = read( port_fd, (data_buff+cnt), rcv_len - cnt );
		if( i <= 0 ){
			printLog( ERROR, "%s:%d %d = read( %d, (c_p+%d), %d) errno:%d",
					__FILE__, __LINE__, i, port_fd, cnt, rcv_len - cnt, errno );	/* V01 */
			/* �ُ�I�� */
			return -1;
		}
	} /* for */

	printLog( DEBUGING, "recv_data = %s", data_buff );	/* V01 */

	/* NTT IP DB P-Agent SUB G/W -> NetExpert        */
	/* NetExpert�f�[�^�̏ꍇ�́A��M�f�[�^����Ԃ��A */
	/* ���� G/W �f�[�^�̏ꍇ�́A0 ��Ԃ��B           */
	i = dbpsub_gw_netx( data_buff, rcv_len );

	/* NetExpert �ւ̃f�[�^�� */
	return i;

} /* readPort */



/****************************************************************************/
/* Name:                                                                    */
/*   breakPort                                                              */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   Sends a break to the ttyport.                                          */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
void breakPort()
{
	 return;
}



/****************************************************************************/
/* Name:                                                                    */
/*   getDeviceName                                                          */
/*                                                                          */
/* Parameters:                                                              */
/*   char *port: device name                                                */
/*                                                                          */
/* Return:                                                                  */
/*   char *device upon success, otherwiae NULL                              */
/*                                                                          */
/* Description:                                                             */
/*   Returns a pointer to the absolute path of the port name given.	        */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
char *getDeviceName( char *port )
{
	printLog( DEBUGING, "%s:%d getDeviceName(port:%s device:%s)", 
								__FILE__, __LINE__, port, device );	/* V01 */

	if( *device )
		return device;

	if( !port ){		/* requestor only, can't set the device name */
		return( (char *) 0 );
	 }

	strcpy( device, port );

	printLog( DEBUGING, "%s:%d device:%s", __FILE__, __LINE__, device );	/* V01 */

	return device;
}



/****************************************************************************/
/* Name:                                                                    */
/*   getPortName                                                            */
/*                                                                          */
/* Parameters:                                                              */
/*   char *port: IN	�z�X�g�����R���t�B�O�t�@�C����	�i���F�X�y�[�X�j        */
/*                                                                          */
/* Return:                                                                  */
/*   char *hostconfig: "�z�X�g���Q�R���t�B�O�t�@�C����"                     */
/*                                                                          */
/* Description:                                                             */
/*   ������ port ����A"hostname_confgifile" ���쐬����B                   */
/*   P-Agent ������̈ȉ��̍��ڂ�ݒ肷��B                               */
/*      ���z�X�g��                                                          */
/*      �R���t�B�O�t�@�C����                                                */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
char *getPortName( char *port )
{
	static char	host_config[DCM_MXHSTLEN+1+DCM_MXCNFLEN+1] = {0x00};
							/* �z�X�g���Q�R���t�B�O�t�@�C�����i�[�̈�	*/

	char		configfile[DCM_MXCNFLEN+1];	/* �R���t�B�O�t�@�C����		*/
	char		hostname[DCM_MXHSTLEN+1];	/* �z�X�g��					*/


	sighold( SIGUSR2 );

	printLog( DEBUGING, "%s:%d getPortName(port:%s)", __FILE__, __LINE__, port );	/* V01 */

	/* ����Ăяo���𔻒f���� */
	if( host_config[0] == 0x00 ) {	/* �N�������̂� �^ */
		/* ��������z�X�g���ƃR���t�B�O�t�@�C�������l�� */
		sscanf( port, "%s %s ",hostname, configfile );
		/* �X�� "�z�X�g���Q�R���t�B�O�t�@�C����" ���쐬 */
		sprintf( host_config, "%s_%s", hostname, configfile);
		printLog( DEBUGING, "%s:%d %s = getPortName()",__FILE__,__LINE__,host_config );	/* V01 */
	}

	/* "�z�X�g���Q�R���t�B�O�t�@�C����" ��Ԃ� */
	sigrelse( SIGUSR2 );
	return host_config;
} /* getPortName */



/****************************************************************************/
/* Name:                                                                    */
/*   parseArgs                                                              */
/*                                                                          */
/* Parameters:                                                              */
/*   char *agrs: pointer to list of additional program arguments            */
/*                                                                          */
/* Return:                                                                  */
/*   0 on success, otherwise -1                                             */
/*                                                                          */
/* Description:                                                             */
/*   This function is intended any additional arguments need for the        */
/*   interface to a device greater than the number normally sent to the     */
/*   p_agent by the parent.                                                 */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int parseArgs( char *args )
{
	 if ( !args || !*args )
		return -1;

	 return 0;
}



/****************************************************************************/
/* Name:                                                                    */
/*   toAscii �i���g�p�j                                                     */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*source  : pointer to input data                                */
/*   int	sourcelen: length of souce bytes                                */
/*   char	*dest    : pointer to output data                               */
/*   int	destlen  : maximun bytes possible in dest                       */
/*                                                                          */
/* Return:                                                                  */
/*   number of source bytes translated                                      */
/*                                                                          */
/* Description:                                                             */
/*   This module translates source data into ascii equivalents in dest.     */
/*   For the serial port we will use the toascii conversion function.       */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int toAscii( char *source, int srclen, char *dest, int destlen )
{
	 register int i = 0;
	 register char *src, *dst;

	 src = source, dst = dest;

	 while ( i < srclen && i < destlen ) {
		*dst++ = *src++;
		++i;
	 }

	 return i;
}



/****************************************************************************/
/* Name:                                                                    */
/*   fromAscii�i���g�p�j                                                    */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*source  : pointer to input data                                */
/*   int	sourcelen: length of souce bytes                                */
/*   char	*dest    : pointer to output data                               */
/*   int	*destlen : value-result parameter                               */
/*   input - maximun bytes possible in dest                                 */
/*   output - number of characters put into dest                            */
/*                                                                          */
/* Return:                                                                  */
/*   number of source bytes translated                                      */
/*                                                                          */
/* Description:                                                             */
/*   This module translates source data into raw equivalents in dest.       */
/*   For the serial port we will simply put out what we have gotten in.     */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int fromAscii( char *source, int srclen, char *dest, int *destlen )
{
	if ( srclen > *destlen )
		srclen = *destlen;
	else
		*destlen = srclen;

	memcpy( dest, source, *destlen );

	return srclen;
}


/****************************************************************************/
/* Name:																	*/
/*   dummy_term                                                             */
/*																			*/
/* Parameters:																*/
/*   int	sig_num: �V�O�i���ԍ�                                           */
/*																			*/
/* Return:																	*/
/*   NONE                                                                   */
/*																			*/
/* Description:																*/
/*   signal()�֐��̈����ɓo�^���邽�߂̃_�~�[�I���֐�                       */
/*   dcm_port.c �ł́AclosePort()���o�^����Ă������AC++�ł́A              */
/*   �^�`�F�b�N�����������߁A����closePort(void)��o�^�ł��Ȃ����߂ɍ쐬    */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
void dummy_term( int sig_num )
{
	closePort();
}

/* END OF FILE */
