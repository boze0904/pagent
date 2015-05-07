/****************************************************************************/
/* file nanme  basic.c  	                                                */
/*                                                                          */
/* NetExpert�O��DB�pG/W														*/
/*                                                                          */
/* Create : 2001.06.27                                                      */
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
#include "pagent_trace.h"
#include "proc_str.h"
#include "basic.h"
#include "common.h"
#include "output_log.h"
#include "read_prop_file.h"

#define  MAX_DATA_LEN   GWP_MXPAGDTLEN + 1


/****************************************************************************/
/*	�����֐�																*/
/*	��{�����n																*/
static int		gwp_getarg(int,char**,GWP_INF*);	/* �N���p�����[�^�l��   */
static void		gwp_term();				/* G/W �I������                     */
static void		gwp_sigterm(int);			/* G/W �v���Z�X�I������(SIGNAL��M) */

/****************************************************************************/
/*	(SCPE I/F)�����ʕϐ�													*/
int						dsp_pid;		/* �\���p�v���Z�X�h�c				*/


/****************************************************************************/
/*	�����ϐ�																*/
static GWP_INF	gwp_inf;				/* DB P-Agent G/W ������          */


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
/* Description : DB�Ɛڑ������āAselect���ȊO��SQL��DB�ɔ��s���܂��B        */
/*                                                                          */
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
int main( int argc, char **argv ) {
	fd_set	readers;					/* �t�@�C���f�B�X�N���v�^  */
	char	pag_data[GWP_MXPAGDTLEN];	/* P-Agent �f�[�^�i�[��    */
	int		data_len;					/* �f�[�^�i�[��L����      */
	int		pagent_sd = 0;				/* P-Agent �L�q�q          */
	int		sel_ret;						/* select() �Ԃ�l         */
	int		ret;							/* ���[�N                  */
	char	head_buff[GWP_MXPAGDTLEN];
	char	*c_p;
	char	*dbp_conf;
	char	configfile[64];
	char	*lpath;						/* ���O�p�X					*/
	int		lvl;						/* ���O���x��				*/
	char	*home;						/* OSI_HOME					*/
	char	logfile[512];				/* ���O�t�@�C��				*/
	int     err_resp;
	int     con_cnt = 0;

	/* �ǉ� �������� V01 */
	/* ���ϐ��擾 */
	if((dbp_conf = getenv("DBP_ENV_PATH")) == NULL){
		printf("�V�X�e���G���[\n");
		fflush(stdout);
		exit(-1);
	}
	memset(configfile, 0, sizeof(configfile));
	sprintf(configfile, "%s/dbp.conf", dbp_conf);

	/*** ���O�o�͂̏��� ***/
	/* �ݒ�t�@�C�����I�[�v�� */
	ret = openPropFile( configfile );
	if( ret ){
		/* �G���[���b�Z�[�W�\�� */
		printf( "%s:%d �R���t�B�O�t�@�C�� openPropFile(%s) error:%d\n",
							__FILE__,__LINE__, configfile, errno );
		fflush( stdout );
		/* �ُ�I�� */
		exit(-1);
    }
	/* ���O�֌W�v���p�e�B�l�̎擾 */
	lpath = getProperty( LOGPATH );
	c_p = getProperty( LOGLVL );
#ifdef DEBUG
printf( "Log path %s\n", lpath );
printf( "Log level %s\n", c_p );
#endif

	if( lpath == NULL ){
		home = getenv( "OSI_HOME" );
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

	printLog(DEBUGING,"%s:%d basic", __FILE__, __LINE__);
#ifdef DEBUG
printf( "%s:%d basic DEBUG\n", __FILE__, __LINE__);
#endif


	/* �c�d�a�t�f�\���p�v���Z�X�h�c */
	dsp_pid = getpid();

	/* DB P-Agent G/W ������ ������ */
	memset( &gwp_inf, 0, sizeof(gwp_inf) );

	/* �N���p�����[�^�l�� */
	ret = gwp_getarg( argc, argv, &gwp_inf );
	if( ret == -1 ){
		printLog( ERROR, "�����G���[");
		exit( 1 );
	}

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
											__FILE__, __LINE__, dsp_pid);

	/* DB P-Agent �h�^�e�ڑ� */
	sleep(2);
	while(con_cnt < 30){
		sleep(1);
		ret = tcp_conn( &(gwp_inf.tcpinf_pagent), gwp_inf.pagent_host,
						gwp_inf.pagent_port );
		if(ret != -1){
			break;
		}

		con_cnt++;
	}

	if( ret == -1 ){
		printLog(ERROR, "TCP�ڑ����ł��܂���ł���");
		/* G/W �v���Z�X�I������ */
		gwp_term();
		/* �I�� */
		exit( 1 );
	}

	printLog( DEBUGING, "%s:%d(%d) DB P-Agent �h�^�e�ڑ�����", 
										__FILE__, __LINE__, dsp_pid);

	/* P-Agent I/F �������� */
	ret = dbp_pagif_init( GWP_MXPAGDTLEN );	/* P-Agent I/F �̍ő咷���w�� */
	if( ret == -1 ){
		printLog(ERROR, "�������m�ۂ��ł��܂���ł���");
		/* G/W �v���Z�X�I������ */
		gwp_term();
		/* �I�� */
		exit( 1 );
	}

	/* P-Agent �L�q�q�l�� */
	pagent_sd = tcp_getsd( &(gwp_inf.tcpinf_pagent) );

	/* DB�ڑ� */
	ret = DbConnect(gwp_inf.usr_name, gwp_inf.passwd, gwp_inf.sid);
	if( ret != 0 ){
		printLog(ERROR, "�f�[�^�x�[�X�ɐڑ����ł��܂���ł���");
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
		sel_ret = select( FD_SETSIZE, &readers, (fd_set *)0, (fd_set *)0, NULL );
		/* P-Agent ����̓��͂��m�F */
		if( FD_ISSET( pagent_sd , &readers ) ){
			/* P-Agent�f�[�^��M */
			data_len = dbp_pagif_recv( &(gwp_inf.tcpinf_pagent), pag_data );

			printLog(DEBUGING, "%s:%d ��M�f�[�^�F%s\n", __FILE__, __LINE__, pag_data);

			/*m_BUILD�e�[�u���փf�[�^��}������*/
			if(data_len > 0 && data_len <= MAX_DATA_LEN) {

				/* �w�b�_�[���킯�� */
				memset(head_buff, 0, sizeof(head_buff));
				if((c_p = strchr(pag_data, ',')) != NULL){
					strncpy(head_buff, pag_data, c_p - pag_data);

					printLog(DEBUGING, "%s:%d �w�b�_�[ : %s\n", __FILE__, __LINE__, head_buff);

					/* �w�b�_�[�ɂ��֐��ʂɂ�т��� */
					if( ! strcmp(head_buff, "BUILD_Insert")){
						ret = build_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "build_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "build_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "build_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "BUILD_Update")){
						ret = build_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "build_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "build_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "build_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "BUILD_Delete")){
						ret = build_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "build_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "build_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "BRANCH_Insert")){
						ret = branch_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "branch_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "branch_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "branch_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "BRANCH_Update")){
						ret = branch_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "branch_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "branch_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "branch_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "BRANCH_Delete")){
						ret = branch_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "branch_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "branch_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_CONTACT_Insert")){
						ret = isp_contact_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_contact_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_contact_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_contact_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_CONTACT_Update")){
						ret = isp_contact_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_contact_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_contact_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_contact_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_CONTACT_Delete")){
						ret = isp_contact_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "isp_contact_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_contact_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_BRANCH_Insert")){
						ret = isp_branch_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_branch_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_branch_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_branch_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_BRANCH_Update")){
						ret = isp_branch_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_branch_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_branch_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_branch_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_BRANCH_Delete")){
						ret = isp_branch_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "isp_branch_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_branch_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_NTE_Insert")){
						ret = isp_nte_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_nte_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_nte_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_nte_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_NTE_Update")){
						ret = isp_nte_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_nte_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_nte_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_nte_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_NTE_Delete")){
						ret = isp_nte_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "isp_nte_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_nte_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "MEDO_CONTACT_Insert")){
						ret = medo_contact_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "medo_contact_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "medo_contact_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "medo_contact_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "MEDO_CONTACT_Update")){
						ret = medo_contact_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "medo_contact_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "medo_contact_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "medo_contact_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "MEDO_CONTACT_Delete")){
						ret = medo_contact_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "medo_contact_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "medo_contact_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "MAKER_CONTACT_Insert")){
						ret = maker_contact_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "maker_contact_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "maker_contact_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "maker_contact_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "MAKER_CONTACT_Update")){
						ret = maker_contact_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "maker_contact_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "maker_contact_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "maker_contact_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "MAKER_CONTACT_Delete")){
						ret = maker_contact_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "maker_contact_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "maker_contact_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "EQP_Insert")){
						ret = eqp_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "eqp_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "eqp_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "eqp_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "EQP_Update")){
						ret = eqp_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "eqp_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "eqp_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "eqp_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "EQP_Delete")){
						ret = eqp_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "eqp_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "eqp_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "COMPONENT_Insert")){
						ret = component_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "component_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "component_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "component_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "COMPONENT_Delete")){
						ret = component_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "component_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "component_delete:DELETE...OK");
						}
					} else {
						printLog(WARNING, "P-Agent�����M�����f�[�^�G���[");
					}
				} else {
					printLog(WARNING, "P-Agent�����M�����f�[�^�G���[");
				}
			}
		}
	} /* while */

	/* G/W �v���Z�X�I������ */
	gwp_term();

	exit(1);

} /* main */



/****************************************************************************/
/* Name : gwp_getarg                                                        */
/*                                                                          */
/* Parameters : int		argc       �FIN  main argc                          */
/*              char	**argv     �FIN  main argv                          */
/*              GWP_INF	*gwp_inf_p �FOUT G/W �v���Z�X������e�[�u��       */
/*                                                                          */
/* Return : 0 �F����I��                                                    */
/*          -1�F�ُ�I��                                                    */
/*                                                                          */
/* Description : �N���p�����[�^�l��                                         */
/*               main() �̈������� G/W �v���Z�X������e�[�u���̈ȉ��̍��� */
/*               �ɒl��ݒ肷��B�s���Ȉ����̏ꍇ�ُ͈�I������B           */
/*              �EDocomo���t�@�C���p�X��                                  */
/*              �EP-Agent �z�X�g��                                          */
/*              �EP-Agent �|�[�g�ԍ�                                        */
/*              �E�� ���ʖ�                                                 */
/*              �EDB ���[�U�[��                                             */
/*              �EDB SID                                                    */
/*              �E�p�X���[�h                                                */
/*              �Epipe�p�X���i�ȗ��j                                      */
/*                                                                          */
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
static int gwp_getarg(int argc, char **argv, GWP_INF *gwpinf_p )
{
	int		dbg_cnt;

	printLog( DEBUGING, "%s:%d(%d) MAIN �̈����̐�", __FILE__, __LINE__, dsp_pid);
	for(dbg_cnt = 0; dbg_cnt < argc; dbg_cnt++)
		printLog(DEBUGING, "\t[%d]:%s",dbg_cnt,*(argv+dbg_cnt));

	/* �p�����[�^���m�F */
	if( argc < 7 ){
		printLog( ERROR, "%s:%d(%d) MAIN �̈����̐�(%d)���K�萔()�łȂ�",
										__FILE__, __LINE__, dsp_pid, argc);
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

	printLog( DEBUGING, "%s:%d ���t�@�C����     :%s",__FILE__,__LINE__,
													gwpinf_p->dcmenvpath );
	printLog( DEBUGING, "%s:%d P-Agent            :%s %d",__FILE__,__LINE__,
							gwpinf_p->pagent_host, gwpinf_p->pagent_port );
	printLog( DEBUGING, "%s:%d �� ���ʖ�          :%s",__FILE__,__LINE__,
													gwpinf_p->owneqpname );
	printLog( DEBUGING, "%s:%d ���[�U��           :%s",__FILE__,__LINE__,
													gwpinf_p->usr_name );
	printLog( DEBUGING, "%s:%d �p�X���[�h         :%s",__FILE__,__LINE__,
														gwpinf_p->passwd );
	/* SID */
	if( argc > 7 ){
		strcpy( gwpinf_p->sid, *(argv+7) );
		printLog( DEBUGING, "%s:%d SID         :%s",__FILE__,__LINE__,
														gwpinf_p->sid );
	}

	/* ����I�� */
	return 0;

} /* gwp_getarg */



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
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
static void gwp_term()
{

	/* DB�Ƃ̐ؒf */
	DbDisconnect();
	
	printLog(WARNING, "basic �f�[�^�x�[�X�ؒf");

	/* Pagent TCP/IP�\�P�b�g����� */
	tcp_close( &(gwp_inf.tcpinf_pagent) );

	/* P-Agent I/F �I������ */
	dbp_pagif_term();

} /* gwp_term */



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
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
static void gwp_sigterm( int sig_num )
{
	printLog(DEBUGING, "%s:%d(%d) gwp_sigterm(%d)",__FILE__,__LINE__,dsp_pid, sig_num);

	if( sig_num != SIGTERM )
		printLog( WARNING, "%s %d(%d):�s���V�O�i��(%d)��M", __FILE__, __LINE__,
															dsp_pid, sig_num);
	else
		printLog( DEBUGING, "%s %d(%d):�I���V�O�i��(%d)��M", __FILE__, __LINE__,
															dsp_pid, sig_num);

	/* G/W �v���Z�X�I������ */
	gwp_term();

	/* �v���Z�X�̏I���l��Ԃ� */
	if( sig_num == SIGTERM )
		/* �v���Z�X����I�� */
		exit(0);
	else
		/* �v���Z�X�ُ�I�� */
		exit(1);
} /* gwp_sigterm */
/* End of File */

