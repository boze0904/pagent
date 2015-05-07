/****************************************************************************/
/* file nanme  IsdnMain.c                                                   */
/*                                                                          */
/* �v�`DB ISDN�p GateWay�v���Z�X                                            */
/*                                                                          */
/* Function:                                                                */
/*   createErrorData()                                                      */
/*   processDB()                                                            */
/*   gwp_getarg()                                                           */
/*   gwp_term()                                                             */
/*   gwp_sigterm()                                                          */
/*   set_signal()                                                           */
/*   ready_log()                                                            */
/*   main()                                                                 */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h> 
#include <errno.h> 
#include <string.h> 
#include "tcp_if.h"
#include "gw_if.h"
#include "dbpag_if.h"
#include "db_func.h"
#include "common.h"
#include "output_log.h"
#include "read_prop_file.h"
#include "IsdnMain.h"
#include "IsdnFindFreePortNumber.h"
#include "import_dar.h"

#define	MAX_DATA_LEN	GWP_MXPAGDTLEN + 1


/****************************************************************************/
/*	(SCPE I/F)�����ʕϐ�													*/
int						dsp_pid;		/* �\���p�v���Z�X�h�c				*/


/****************************************************************************/
/*	�����ϐ�																*/
static GWP_INF	gwp_inf;				/* DB P-Agent G/W ������          */


/****************************************************************************/
/* Name : createErrorData                                                   */
/*                                                                          */
/* Parameter  : char* send_data: OUT: ���M�p�f�[�^                          */
/*                                                                          */
/* Return     :  0 �ȏ�: ���M�f�[�^��                                       */
/*                                                                          */
/* Discripsion: �ُ폈������NetExpert�ֈُ폈���I���ʒm�̃f�[�^�̍쐬       */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int createErrorData( char* send_data )
{
	printLog( DEBUGING, "*** createErrorData() ***" );

    sprintf( send_data, "%s,%d,%d,%d,%d,%d", SEND_HEAD, -1, -1, -1, -1, -1 );

    return strlen(send_data);

} /* createErrorData() */


/****************************************************************************/
/* Name : processDB                                                         */
/*                                                                          */
/* Parameter  : char* recv_data:  IN: ��M�f�[�^                            */
/*              char* send_data: OUT: ���M�p�f�[�^                          */
/*                                                                          */
/* Return     :  0 �ȏ�: ���M�f�[�^��                                       */
/*              -1     : �ُ�I��                                           */
/*                                                                          */
/* Discripsion: ��M�f�[�^�̃w�b�_�ʂɃf�[�^�����p�̊֐����Ăяo��          */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int processDB( char* recv_data, char* send_data )
{
	int		data_len;
	char*	data;

	printLog( DEBUGING, "*** processDB() ***" );

	if( !strncmp( recv_data, FIND_FREE_PORT_NUMBER, strlen(FIND_FREE_PORT_NUMBER) ) ){
		data_len = findFreePortNumber( recv_data, send_data );
		if( data_len == -1 ){
			printLog( WARNING, "�������ُ�I�����܂���" );
			data_len = createErrorData( send_data );
		}
	} else if( !strncmp( recv_data, H_IMPORT_INSERT, strlen(H_IMPORT_INSERT) ) ){
		data = strchr( recv_data, ',' );
		data++;
		data_len = import_dar_insert( data, send_data );
	} else if( !strncmp( recv_data, H_IMPORT_DELETE, strlen(H_IMPORT_DELETE) ) ){
		data = strchr( recv_data, ',' );
		data++;
		data_len = import_dar_delete( data, send_data );
	} else {
		printLog( WARNING, "��M�f�[�^�̃w�b�_(%s)�ɊY�����鏈���͖��o�^", strtok(recv_data,",") );
		return 0;
	}

    return data_len;

} /* processDB() */


/****************************************************************************/
/* Name : gwp_getarg                                                        */
/*                                                                          */
/* Parameters : int		argc       �FIN  main argc                          */
/*              char	**argv     �FIN  main argv                          */
/*              GWP_INF	*gwp_inf_p �FOUT G/W �v���Z�X������e�[�u��       */
/*                                                                          */
/* Return :  0: ����I��                                                    */
/*          -1: �ُ�I��                                                    */
/*                                                                          */
/* Description: �N���p�����[�^�l��                                          */
/*              main() �̈������� G/W �v���Z�X������e�[�u���̈ȉ��̍���  */
/*              �ɒl��ݒ肷��B�s���Ȉ����̏ꍇ�ُ͈�I������B            */
/*              �EDocomo���t�@�C���p�X��                                  */
/*              �EP-Agent �z�X�g��                                          */
/*              �EP-Agent �|�[�g�ԍ�                                        */
/*              �E�� ���ʖ�                                                 */
/*              �EDB ���[�U�[��                                             */
/*              �EDB SID                                                    */
/*              �E�p�X���[�h                                                */
/*              �Epipe�p�X���i�ȗ��j                                      */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int gwp_getarg(int argc, char **argv, GWP_INF *gwpinf_p )
{
	int		dbg_cnt;

	printLog( DEBUGING, "%s:%d(%d) MAIN �̈����̐�", __FILE__, __LINE__, dsp_pid );
	for(dbg_cnt = 0; dbg_cnt < argc; dbg_cnt++)
		printLog( DEBUGING, "\t[%d]:%s", dbg_cnt, *(argv+dbg_cnt) );

	/* �p�����[�^���m�F */
	if( argc < 7 ){
		printLog( ERROR, "%s:%d(%d) MAIN �̈����̐�(%d)���K�萔�łȂ�",
										__FILE__, __LINE__, dsp_pid, argc );
		/* �ُ�I�� */
		return -1;
	}

	/* DB G/W���t�@�C���p�X���l�� */
	strcpy( gwpinf_p->dcmenvpath, *(argv+1) );

	/* P-Agent �z�X�g���l�� */
	strcpy( gwpinf_p->pagent_host, *(argv+2) );

	/* P-Agent �|�[�g�ԍ��l�� */
	gwpinf_p->pagent_port = atol( *(argv+3) );

	/* �� ���ʖ��l�� */
	strcpy( gwpinf_p->owneqpname, *(argv+4) );

	/* ���[�U�[�� */
	strcpy( gwpinf_p->usr_name, *(argv+5) );

	/* �p�X���[�h */
	strcpy( gwpinf_p->passwd, *(argv+6) );

	printLog( DEBUGING, "%s:%d ���t�@�C����     :%s", 
							__FILE__, __LINE__, gwpinf_p->dcmenvpath );
	printLog( DEBUGING, "%s:%d P-Agent            :%s %d", 
						__FILE__, __LINE__, gwpinf_p->pagent_host, gwpinf_p->pagent_port );
	printLog( DEBUGING, "%s:%d �� ���ʖ�          :%s", 
							__FILE__, __LINE__, gwpinf_p->owneqpname );
	printLog( DEBUGING, "%s:%d ���[�U��           :%s", 
							__FILE__, __LINE__, gwpinf_p->usr_name );
	printLog( DEBUGING, "%s:%d �p�X���[�h         :%s", 
							__FILE__, __LINE__, gwpinf_p->passwd );
	/* SID */
	if( argc > 7 ){
		strcpy( gwpinf_p->sid, *(argv+7) );
		printLog( DEBUGING, "%s:%d SID                :%s", 
							__FILE__, __LINE__, gwpinf_p->sid );
	}

	/* ����I�� */
	return 0;

} /* gwp_getarg() */


/****************************************************************************/
/* Name : gwp_term                                                          */
/*                                                                          */
/* Parameters : NONE                                                        */
/*                                                                          */
/* Return : NONE                                                            */
/*                                                                          */
/* Description : G/W �I������                                               */
/*               DB�Ƃ̐ڑ���ؒf����                                       */
/*               "Pagent TCP/IP�\�P�b�g" �����B                         */
/*               Docomo P-Agent I/F �I�������B                              */
/*               ���֐��Ńv���Z�X�̏I���iexit( ) �̌ďo���j�͂��Ȃ��B       */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
static void gwp_term()
{
	printLog( DEBUGING, "*** gwp_term() ***" );
	
    /* DB�Ƃ̐ؒf */
    DbDisconnect();

	printLog(WARNING, "isdnMain �f�[�^�x�[�X�ؒf");

	/* Pagent TCP/IP�\�P�b�g����� */
	tcp_close( &(gwp_inf.tcpinf_pagent) );

	/* P-Agent I/F �I������ */
	dbp_pagif_term();

} /* gwp_term() */


/****************************************************************************/
/* Name : gwp_sigterm                                                       */
/*                                                                          */
/* Parameters : NONE                                                        */
/*                                                                          */
/* Return : 0�F����I��                                                     */
/*          1�F�ُ�I��                                                     */
/*                                                                          */
/* Description : G/W �v���Z�X�V�O�i���I������		SIGTERM �o�^            */
/*               G/W �̓V�O�i���ɂ��I�������s�Ȃ�Ȃ��B                   */
/*               G/W �I���������s�Ȃ��B                                     */
/*               SIGTERM �̎�M�𐳏�̏I���Ƃ��A�s���ȃV�O�i����M��       */
/*               �ُ�I���Ƃ���B                                           */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
void gwp_sigterm( int sig_num )
{
	printLog( DEBUGING, "%s:%d(%d) gwp_sigterm(%d)", 
							__FILE__, __LINE__, dsp_pid, sig_num );

	if( sig_num != SIGTERM )
		printLog( ERROR, "%s %d(%d):�s���V�O�i��(%d)��M",
										__FILE__, __LINE__, dsp_pid, sig_num );
	else
		printLog( DEBUGING, "%s %d(%d):�I���V�O�i��(%d)��M", 
										__FILE__, __LINE__, dsp_pid, sig_num );

	/* G/W �v���Z�X�I������ */
	gwp_term();

	/* �v���Z�X�̏I���l��Ԃ� */
	if( sig_num == SIGTERM )
		/* �v���Z�X����I�� */
		exit(0);
	else
		/* �v���Z�X�ُ�I�� */
		exit(1);
} /* gwp_sigterm() */

 
/****************************************************************************/
/* Name : set_signal                                                        */
/*                                                                          */
/* Parameters : NONE                                                        */
/*                                                                          */
/* Return     : NONE                                                        */
/*                                                                          */
/* Description: �v���Z�X�̏I���V�O�i����o�^����                            */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
void set_signal()
{
	printLog( DEBUGING, "*** set_signal() ***" );

	/* �I���V�O�i���o�^�iG/W �v���Z�X�I�������j */
	signal( SIGTERM, gwp_sigterm );
	/* �ʏ폈���Ŏ�M���Ȃ��V�O�i���̓o�^ */
	signal( SIGHUP,  gwp_sigterm );
	signal( SIGINT,  gwp_sigterm );
	signal( SIGQUIT, gwp_sigterm );
	signal( SIGILL,  gwp_sigterm );
	signal( SIGALRM, gwp_sigterm );
	signal( SIGEMT,  gwp_sigterm );
	signal( SIGFPE,  gwp_sigterm );
	signal( SIGBUS,  gwp_sigterm );
	signal( SIGSEGV, gwp_sigterm );
	signal( SIGSYS,  gwp_sigterm );
	signal( SIGUSR1, gwp_sigterm );
	signal( SIGPWR,  gwp_sigterm );
	signal( SIGPIPE, gwp_sigterm );
	printLog( DEBUGING, "%s:%d(%d) �ʏ폈���Ŏ�M���Ȃ��V�O�i���̓o�^", 
											__FILE__, __LINE__, dsp_pid );
} /* set_signal() */


/****************************************************************************/
/* Name : ready_log                                                         */
/*                                                                          */
/* Parameters : NONE                                                        */
/*                                                                          */
/* Return     :  0: ����                                                    */
/*              -1: �ُ�                                                    */
/*                                                                          */
/* Description: ���O�̏���������                                            */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int ready_log()
{
	int		ret;
	int		lvl;
	char	configful_path[512];
	char    *home;
    char    logfile[512];
	char	*c_p;
	char	*lpath;


	printLog( DEBUGING, "*** read_log() ***" );

	/* �R���t�B�O�t�@�C���� �i�t���p�X�j�l�� */
	c_p = getenv( DBP_ENVPATH );
    if( c_p == NULL ){
		/* �G���[���b�Z�[�W�\�� */
		printf( "%s:%d ���ϐ�(%s)���ݒ�\n", __FILE__, __LINE__, DBP_ENVPATH );
		fflush( stdout );
		/* �ُ�I�� */
		return -1;
    }
	sprintf( configful_path, "%s/%s", c_p, DBP_CONF );

    /* ���݊m�F */
	ret = access( configful_path, R_OK );
    if( ret ){
		/* �G���[���b�Z�[�W�\�� */
		printf( "%s:%d �R���t�B�O�t�@�C�� access(%s) error:%d\n",
		        	__FILE__,__LINE__, configful_path, errno );
		fflush( stdout );

		/* �ُ�I�� */
		return -1;
    }
#ifdef DEBUG
printf( "%s:%d �R���t�B�O�t�@�C�� access(%s)\n",
		        	__FILE__, __LINE__, configful_path );
#endif

	/*** ���O�o�͂̏��� ***/
	/* �ݒ�t�@�C�����I�[�v�� */
	ret = openPropFile( configful_path );
    if( ret ){
		/* �G���[���b�Z�[�W�\�� */
		printf( "%s:%d �R���t�B�O�t�@�C�� openPropFile(%s) error:%d\n",
		        	__FILE__,__LINE__, configful_path, errno );
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
    if( c_p == NULL  ){
		lvl = WARNING;
    }
    else{
		lvl = atoi( c_p );
    }
	
	if( lpath == NULL ){
		home = getenv( OSI_HOME );
		if( home == NULL ){
			printf("�V�X�e���G���[\n");
			fflush( stdout );
			return -1;
		}
		sprintf( logfile, "%s/Log", home );
		lpath = logfile;
	}	

	/* ���O�t�@�C���̏��� */
	openLog( lpath, lvl );

	/* �ݒ�t�@�C�������̌�n�� */
    closePropFile();

	return 0;

} /* ready_log() */


/****************************************************************************/
/* Name : main                                                              */
/*                                                                          */
/* Parameters : exe_name     �F���s�`���t�@�C���p�X��                       */
/*              dcm_env_path �FDocomo���t�@�C���p�X��                     */
/*              pagent_host  �FP-Agent�z�X�g��                              */
/*              pagent_port  �FP-Agent�|�[�g�ԍ�                            */
/*              own_name     �F�� ���ʖ�                                    */
/*              usr_name     �FDB���[�U�[��                                 */
/*              password     �FPass Word                                    */
/*              sid          �FDB SID                                       */
/*              [pipe_path]  �F�p�C�v�t�@�C���p�X��(�ȗ���)                 */
/*                                                                          */
/* Return : NONE                                                            */
/*                                                                          */
/* Description : DB�Ɛڑ������āASQL���v�`DB�ɔ��s���܂��B                  */
/*               NetExpert�֕ԐM������΁A����𑗐M���܂�                  */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int main( int argc, char **argv )
{
	fd_set	readers;					/* �t�@�C���f�B�X�N���v�^   */
	char	pag_data[GWP_MXPAGDTLEN];	/* P-Agent �f�[�^�i�[��     */
	char	send_data[GWP_MXPAGDTLEN];	/* P-Agent�ւ� �f�[�^�i�[�� */
	int		data_len;					/* �f�[�^�i�[��L����       */
	int		pagent_sd = 0;				/* P-Agent �L�q�q           */
	int		sel_ret;					/* select() �Ԃ�l          */
	int		ret;						/* ���[�N                   */
	int     con_cnt = 0;


	/*** ���O�̏��� ***/
	ret = ready_log();
	if( ret ){
		return -1;
 	}

	printLog( DEBUGING, "***ISDN MAIN START ***" );

	/* �c�d�a�t�f�\���p�v���Z�X�h�c */
	dsp_pid = getpid();

	/* DB P-Agent G/W ������ ������ */
	memset( &gwp_inf, 0, sizeof(gwp_inf) );

	/* �N���p�����[�^�l�� */
	ret = gwp_getarg( argc, argv, &gwp_inf );
	if( ret == -1 ){
		printLog( ERROR, "�����G���[" );
		/* �I�� */
		exit( 1 );
	}

	/*** �V�O�i���֐��̓o�^ ***/
	set_signal();

	/* DB P-Agent �h�^�e�ڑ� */
	sleep(2);
	while( con_cnt < 30 ){
		sleep(1);
		ret = tcp_conn( &(gwp_inf.tcpinf_pagent), gwp_inf.pagent_host,
						gwp_inf.pagent_port );
		if(ret != -1){
			break;
		}
		con_cnt++;
	}

	if( ret == -1 ){
		printLog( ERROR, "TCP�ڑ����ł��܂���ł���" );
		/* G/W �v���Z�X�I������ */
		gwp_term();
		/* �I�� */
		exit( 1 );
	}

	printLog( DEBUGING, "%s:%d(%d) DB P-Agent �h�^�e�ڑ�����", 
										__FILE__, __LINE__, dsp_pid );

	/* P-Agent I/F �������� */
	ret = dbp_pagif_init( GWP_MXPAGDTLEN );	/* P-Agent I/F �̍ő咷���w�� */
	if( ret == -1 ){
		printLog( ERROR, "�������m�ۂ��ł��܂���ł���" );
		/* G/W �v���Z�X�I������ */
		gwp_term();
		/* �I�� */
		exit( 1 );
	}

	/* P-Agent �L�q�q�l�� */
	pagent_sd = tcp_getsd( &(gwp_inf.tcpinf_pagent) );

	/*** DB �ւ̐ڑ� ***/ 
    ret = DbConnect( gwp_inf.usr_name, gwp_inf.passwd, gwp_inf.sid );
    if( ret != 0 ){
        printLog( ERROR, "�f�[�^�x�[�X�ɐڑ����ł��܂���ł���" );

        /* G/W �v���Z�X�I������ */
        gwp_term();

        /* �I�� */
        exit( 1 );
    }
	printLog(WARNING,"%s �f�[�^�x�[�X�ڑ�����", argv[0]);

	/* �ʏ폈�� */
	while(1){
		FD_ZERO( &readers );
		FD_SET( pagent_sd, &readers );
		memset(pag_data, 0, sizeof(pag_data));
		sel_ret = select( FD_SETSIZE, &readers, (fd_set *)0, (fd_set *)0,
														NULL );
		/* P-Agent ����̓��͂��m�F */
		if( FD_ISSET( pagent_sd , &readers ) ){
			/* P-Agent�f�[�^��M */
			data_len = dbp_pagif_recv( &(gwp_inf.tcpinf_pagent), pag_data );

			printLog( DEBUGING, "��M�f�[�^: %s", pag_data );

			if( data_len > 0 && data_len <= GWP_MXPAGDTLEN )
			{
				/* �f�[�^�x�[�X�����֐��ւ̐U�蕪�� */ 
				data_len = processDB( pag_data, send_data );

				if( data_len > 0 ){
					/* ���M�f�[�^����0���傫���ꍇ�A�f�[�^�𑗐M���� */
					ret = dbp_pagif_send( &(gwp_inf.tcpinf_pagent), "", send_data );	
				}
			}
			else if( data_len == -1 ) {
				printLog( ERROR, "�f�[�^��M�G���[" );
				break;
			}
			else {
				printLog( WARNING, "P-Agent�����M�����f�[�^�G���[: %s", pag_data );
			}
		}
	} /* while */

	/* G/W �v���Z�X�I������ */
	gwp_term();

	exit(1);

} /* main() */

/* End of File */

