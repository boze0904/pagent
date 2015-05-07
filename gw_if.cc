/****************************************************************************/
/* file name	gw_if.cc                                                    */
/*                                                                          */
/* NTT IP DB P-Agent G/W ���� I/F�Ǘ�                                       */
/*                                                                          */
/* Function:                                                                */
/*   gw_accept      : �f�^�v�ڑ��҂�                                        */
/*   gw_create      : �f�^�v�v���Z�X����                                    */
/*   gw_bind        : �\�P�b�g�A�h���X�o�^                                  */
/*   gw_sigalrm     : SIGALRM ��M�֐�                                      */
/*   gw_exec        : �f�^�v�v���Z�X�N��                                    */
/*   gw_fdset       : �f�^�v�\�P�b�g�L�q�q�ݒ�                              */
/*   gw_getvalidfd  : �f�^�v�\�P�b�g�L���L�q�q�l��                          */
/*   gw_getgwinfaddr: �f�^�v���e�[�u���A�h���X�l��                        */
/*   gw_kill        : �f�^�v�I��                                            */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <strings.h>
#include "gw_if.h"
#include "tcp_if.h"
#include "output_log.h"


/****************************************************************************/
/* �ϐ��ꗗ																	*/
/****************************************************************************/
static GW_INF		gwinf;					/* �f�^�v���e�[�u��			*/
											/* �v���Z�X�h�c:0 ����          */
static int			sigalrm_flag = 0;		/* SIGALRM ��M�֐�	I/F			*/
											/* 0:���g�p		1:�^�C���A�E�g	*/

/****************************************************************************/
/* Name:                                                                    */
/*   gw_accept: �f�^�v�ڑ��҂�                                              */
/*                                                                          */
/* Parameters:                                                              */
/*   GW_INF		*gwinf_p   : IN: �f�^�v���e�[�u��                         */
/*   GW_EXEINF	*gwexeinf_p: IN: �f�^�v�ʏ��e�[�u��                     */
/*                                                                          */
/* Return:                                                                  */
/*    0: ����I��                                                           */
/*   -1: �ُ�I���i�v���I�j                                                 */
/*   -2: �ُ�I���i�^�C���A�E�g�j                                           */
/*                                                                          */
/* Description:                                                             */
/*   listen()�Aaccept()�ɂ��AG/W�v���Z�X����̐ڑ��҂����s���B            */
/*   ����I�����AG/W �ʏ��̃\�P�b�g�L�q�q(GW)���L���ƂȂ�B             */
/*   �ُ�I�����A�f�^�v��� �̃\�P�b�g�L�q�q(P-Agent) �� close�������Ȃ�    */
/*   �̂ŌĂяo�����Ō㏈�����s���B                                         */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
static int gw_accept( GW_INF *gwinf_p, GW_EXEINF *gwexeinf_p )
{
	int		len;					/* �\�b�P�g�A�h���X����	*/
	int		i;						/* ���[�N				*/


	/* ��t����	��x listen( ) ����� close() ����܂ł��Ȃ��B */
	if( gwinf_p->listen_flag == GW_INFLISTENOFF ){
		i = listen( gwinf_p->sd, 1 );
		if( i == -1 ){
			printLog( ERROR, "%s %d: -1 = listen(%d,1) errno:%d",
									__FILE__, __LINE__, gwinf_p->sd, errno );	/* V01 */
			/* �ُ�I���i�v���I�j */
			return -1;
		}
		/* LISTEN FLAG  ON  */
		gwinf_p->listen_flag = GW_INFLISTENON;
	}

	printLog( DEBUGING, "%s %d: %d = listen(%d,1)",
									__FILE__, __LINE__, i, gwinf_p->sd );	/* V01 */

	/* LISTEN �^�C�}�[�Z�b�g */
	if( gwinf_p->pagentlsnt > 0 ){
		/* SIGALRM ��M�֐�	I/F		���g�p */
		sigalrm_flag = 0;
		alarm(gwinf_p->pagentlsnt);
	}

	/* �ڑ��҂� */
	len = sizeof(struct sockaddr);
	gwexeinf_p->sd=accept(gwinf_p->sd,(struct sockaddr *)&(gwinf_p->name),&len);
	if( gwexeinf_p->sd == -1 ){
		printLog( ERROR, "%s %d: -1 = accept(%d,,) errno:%d\n",__FILE__, __LINE__,
														gwexeinf_p->sd, errno);
		/* �^�C�}�[��~�i�^�C���A�E�g�ȊO�ُ̈�̏ꍇ�ɔ����Ă���j			*/
		alarm(0);

		/* �\�P�b�g�L�q�q(GW) �𖳌��ɂ���B */
		gwexeinf_p->sd = 0;

		/* �^�C���A�E�g���� */
		if( (sigalrm_flag == 1) && (errno == EINTR) ){
			printLog( ERROR, "%s %d: �^�C���A�E�g(%d sec)",
									__FILE__, __LINE__, gwinf_p->pagentlsnt );	/* V01 */
			/* �ُ�I���i�^�C���A�E�g�j */
			return -2;
		}
		else {
			/* �ُ�I���i�v���I�j */
			return -1;
		}
	}
	/* �^�C�}�[��~ */
	alarm(0);

	printLog( DEBUGING, "%s %d: %d = accept(%d,,)",
						__FILE__, __LINE__, gwexeinf_p->sd, gwinf_p->sd );	/* V01 */

	/* ����I�� */
	return 0;

} /* gw_accept */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_create                                                              */
/*                                                                          */
/*   �f�^�v�v���Z�X����                                                     */
/*                                                                          */
/* Parameters:                                                              */
/*   GW_INF		*gwinf_p   : IN: �f�^�v���e�[�u��                         */
/*   GW_EXEINF	*gwexeinf_p: IN: �f�^�v�ʏ��e�[�u��                     */
/*                                                                          */
/* Return:                                                                  */
/*    0: ����I��                                                           */
/*   -1: �ُ�I��                                                           */
/*                                                                          */
/* Description:                                                             */
/*   �f�^�v�v���Z�X�𐶐����ATCP/IP �R�l�N�V�����ڑ�����B                  */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
static int gw_create( GW_INF *gwinf_p, GW_EXEINF *gwexeinf_p )
{
	char		*arg5_p = NULL;				/* ��܈��� */
	char		*arg6_p = NULL;				/* ��Z���� */
	char		*arg7_p = NULL;				/* �掵���� */
	char		*arg8_p = NULL;				/* �攪���� */
	char		*arg9_p = NULL;				/* ������ */
	char		*arg10_p = NULL;			/* ��\���� */
	int			i;							/* ���[�N   */


	printLog( DEBUGING, "%s:%d gw_create()", __FILE__, __LINE__ );	/* V01 */

	/* �q�v���Z�X���� */
	gwexeinf_p->pid = fork();
	if( gwexeinf_p->pid == -1 ){
		printLog( DEBUGING, "%s:%d -1 = fork( ) errno:%d", __FILE__, __LINE__, errno );	/* V01 */
		/* �ُ�I�� */
		return -1;
	}
	/* �q�v���Z�X */
	if( gwexeinf_p->pid == 0 ){
		/* ��܁`�\�p�����[�^�ݒ� */
		if( gwexeinf_p->arg5[0] != '\0' )
			arg5_p = gwexeinf_p->arg5;
		if( gwexeinf_p->arg6[0] != '\0' )
			arg6_p = gwexeinf_p->arg6;
		if( gwexeinf_p->arg7[0] != '\0' )
			arg7_p = gwexeinf_p->arg7;
		if( gwexeinf_p->arg8[0] != '\0' )
			arg8_p = gwexeinf_p->arg8;
		if( gwexeinf_p->arg9[0] != '\0' )
			arg9_p = gwexeinf_p->arg9;
		if( gwexeinf_p->arg10[0] != '\0' )
			arg9_p = gwexeinf_p->arg10;

/********** �ύX �������� V01 **********/
		printLog( DEBUGING, "%s:%d �q�v���Z�X           pid(%d)",__FILE__,__LINE__,(int)getpid() );
		printLog( DEBUGING, "%s:%d ���s�`���t�@�C��     %s", __FILE__,__LINE__,gwexeinf_p->exefile );
		printLog( DETAIL, "%s:%d ���t�@�C���p�X     %s", __FILE__,__LINE__,gwinf_p->env_path );
		printLog( DETAIL, "%s:%d P-Agent �z�X�g       %s", __FILE__,__LINE__,gwinf_p->pagenthost );
		printLog( DETAIL, "%s:%d P-Agent �|�[�g       %s", __FILE__,__LINE__,gwinf_p->pagentport );
		if( arg5_p != NULL )
			printLog( DETAIL, "%s:%d ��܈���             %s", __FILE__, __LINE__, arg5_p );
		if( arg6_p != NULL )
			printLog( DETAIL, "%s:%d ��Z����             %s", __FILE__, __LINE__, arg6_p );
		if( arg7_p != NULL )
			printLog( DETAIL, "%s:%d �掵����             %s", __FILE__, __LINE__, arg7_p );
		if( arg8_p != NULL )
			printLog( DETAIL, "%s:%d �攪����             %s", __FILE__, __LINE__, arg8_p );
		if( arg9_p != NULL )
			printLog( DETAIL, "%s:%d ������             %s", __FILE__, __LINE__, arg9_p );
		if( arg10_p != NULL )
			printLog( DETAIL, "%s:%d ��\����             %s", __FILE__, __LINE__, arg10_p );
/********** �ύX �����܂� V01 **********/

		/* �f�^�v�v���Z�X�ɃI�[�o�[���C����B V0.4 V0.5 */
		usleep((useconds_t)3000);
		i = execl( gwexeinf_p->exefile, gwexeinf_p->exefile, gwinf_p->env_path,
				gwinf_p->pagenthost, gwinf_p->pagentport,
				arg5_p, arg6_p, arg7_p, arg8_p, arg9_p, arg10_p, NULL );
	}

	/*** �ȍ~�A�e�v���Z�X ***/

	/* ��܈��������ʖ��Ƃ��ēo�^�B */
	strcpy( gwexeinf_p->name, gwexeinf_p->arg5 );

	/* �f�^�v�ڑ��҂� */
	i = gw_accept( gwinf_p, gwexeinf_p );
	if( i != 0 ){
		/* �ُ�I�� */
		return -1;
	}

	/* ����I�� */
	return 0;

} /* gw_create */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_bind: �\�P�b�g�A�h���X�o�^                                          */
/*                                                                          */
/* Parameters:                                                              */
/*   GW_INF		*gwinf_p: I/O: �f�^�v���e�[�u��                           */
/*                                                                          */
/* Return:                                                                  */
/*    0: ����I��                                                           */
/*   -1: �ُ�I��                                                           */
/*                                                                          */
/* Description:                                                             */
/*   �\�P�b�g�𐶐����A�A�h���X�o�^�ibind�j���s���B                         */
/*   ����I�����A�f�^�v���e�[�u���̃\�b�P�g�A�h���X���ƁA�\�P�b�g�L�q�q   */
/*   (P-Agent)���L���ƂȂ�B                                                */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
static int gw_bind( GW_INF *gwinf_p )
{
	struct linger	linger_data;			/* manipulating linger option.	*/
	int				optval;					/* �I�v�V�����ݒ�p				*/
	int				i;						/* ���[�N						*/


	printLog( DEBUGING, "%s %d:gw_bind() pagenthost:%s pagentport:%s", 
				__FILE__, __LINE__, gwinf_p->pagenthost, gwinf_p->pagentport ) ;	/* V01 */

	/* �C���^�l�b�g�A�h���X�A�X�g���[���\�P�b�g�o�^ */
	gwinf_p->sd = socket(AF_INET, SOCK_STREAM, 0);
	if( gwinf_p->sd == -1 ) {
		gwinf_p->sd = 0;
		printLog( ERROR, "%s %d:-1 = socket( ) errno:%d", __FILE__, __LINE__, errno );	/* V01 */
		return -1;
	}

	/* �\�P�b�g�ݒ�i�Ďg�p�j */
	optval = 1;
	i = setsockopt( gwinf_p->sd, SOL_SOCKET, SO_REUSEADDR,
		(char *)&optval, sizeof(optval));
	if ( i < 0 ) {
		printLog( ERROR, "%s %d: %d = setsockopt(%d, SOL_SOCKET, SO_REUSEADDR,, ) errno:%d",
									__FILE__, __LINE__, i, gwinf_p->sd, errno );	/* V01 */
		/* �\�P�b�g���������� */
		close(gwinf_p->sd);
		gwinf_p->sd = 0;
		return -1;
	}

	/* �\�P�b�g�ݒ�i�����K�[�F�n�e�e �����K�[���ԁF�O�j */
	i = sizeof(struct linger) ;
	linger_data.l_onoff = 0;
	linger_data.l_linger = 0 ;
	i = setsockopt( gwinf_p->sd, SOL_SOCKET, SO_LINGER, (char *)&linger_data,
													sizeof(struct linger));
	if( i < 0 ){
		printLog( ERROR, "%s %d: %d = setsockopt(%d, SOL_SOCKET, SO_LINGER,, ) errno:%d",
									__FILE__, __LINE__, i, gwinf_p->sd, errno );	/* V01 */
		/* �\�P�b�g���������� */
		close( gwinf_p->sd );
		gwinf_p->sd = 0;
		return -1;
	}

	/* �\�P�b�g�ɖ��O�����蓖�Ă� */
	memset( &(gwinf_p->name), 0 ,sizeof(struct sockaddr) );
	gwinf_p->name.sin_family = AF_INET;
	gwinf_p->name.sin_addr.s_addr = inet_addr(hostnametoaddr(gwinf_p->pagenthost));
	gwinf_p->name.sin_port = htons(atol(gwinf_p->pagentport)) ;
	i = bind( gwinf_p->sd, (struct sockaddr *)&(gwinf_p->name),
													sizeof(struct sockaddr) );
	if( i == -1 ) {
		printLog( ERROR, "%s %d: -1 = bind(  ) errno:%d", __FILE__, __LINE__, errno );	/* V01 */
		/* �\�P�b�g���������� */
		close(gwinf_p->sd);
		gwinf_p->sd = 0;
		return -1;
	}

	/* ����I�� */
	return 0;

} /* gw_bind */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_sigalrm: SIGALRM ��M�֐�                                           */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   SIGALRM ��M�֐� I/F ���^�C���A�E�g�ɐݒ肷��B                        */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify :                                                                 */
/****************************************************************************/
static void		gw_sigalrm( int sig_num )
{
	sigalrm_flag = 1;

} /* gw_sigalrm */

/****************************************************************************/
/* Name:                                                                    */
/*   gw_exec: �f�^�v�v���Z�X�N��                                            */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*gw_exec_file: IN: �f�^�v�N���R���t�B�O���[�V�����t�@�C��       */
/*                                                                          */
/* Return:                                                                  */
/*   >= 0: �N���f�^�v��                                                     */
/*     -1: �ُ�I��                                                         */
/*                                                                          */
/* Description:                                                             */
/*   �R���t�B�O���[�V�����t�@�C����ǂݍ��݁A�f�^�v���e�[�u���Ɋi�[����B */
/*   P-Agent	���̃|�[�g�i�T�[�o�j���J�݂���B                            */
/*   G/W �v���Z�X�𐶐����A�z�X�g��/�|�[�gID ��LISTEN �^�C�}�[�b��          */
/*   TCP/IP�ڑ��҂�                                                         */
/*   ������B�ڑ��҂��^�C���A�E�g�b���ɐڑ����Ȃ��ƈُ�I������B           */
/*   GW= �̎w��͍Ō�ɍs���B                                               */
/*   LISTEN �^�C�}�[�̏����l�͂O(�i�v�ɑ҂�)�ŏȗ��\�B                    */
/*                                                                          */
/*   �y�R���t�B�O���[�V�����t�@�C���t�H�[�}�b�g�z                           */
/*   +----------------------------------------------------------            */
/*   |PAGENTHOST=P-Agent �z�X�g��                                           */
/*   |PAGENTPORT=P-Agent �|�[�gID                                           */
/*   |PAGENTLTIM=P-Agent LISTEN �^�C�}�[                                    */
/*   |GW_EXEPATH=�f�v���s�`���f�B���N�g��                                   */
/*   |GW=���s�`���t�@�C�� arg5 arg6 arg7 arg8 arg9 arg10                    */
/*                                                                          */
/*   �����v���Z�X(���s�`���t�@�C��)�ւ̈����͂S�Ԗڂ܂ł��Œ�ŁA           */
/*   �ȉ��̓��e���ݒ肳���B                                               */
/*   ���s�`���ŗL�̒l�͂T�Ԗڈȍ~�ɍő�U�w��ł���B                     */
/*   �������U�Ԗڂ̈����� �f�v���ʖ� �Ƃ���B                               */
/*   P-Agent �́A���̎��ʖ��ɂ��f�v�֑��M����f�[�^��U�蕪����B         */
/*                                                                          */
/*   �y�Œ�̈����z                                                         */
/*   exefile_name $DBP_ENV_PATH $PAGENTHOST $PAGENTPORT                     */
/*                                          �� DBP_ENV_PATH�F���ϐ�       */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int	gw_exec( char *gw_exec_file )
{
    FILE	*stream;					/* �R���t�B�O���[�V�����t�@�C���p */
	char	line_buff[GW_MXLINELEN+1];	/* �s�o�b�t�@                     */
	char	c_buff[GW_MXFILELEN+1];		/* ���[�N                         */
	int		cnt;						/* G/W�N���J�E���^                */
	int		i;							/* ���[�N                         */


	printLog( DEBUGING, "%s:%d gwexec(%s)", __FILE__, __LINE__, gw_exec_file );	/* V01 */

	/* SIGALRM ��M�֐��o�^ */
	signal( SIGALRM, gw_sigalrm );

	/* �f�^�v���e�[�u�������� */
	memset( &gwinf, 0, sizeof(gwinf) );

	/* �f�^�v�N���R���t�B�O���[�V�����t�@�C�����J�� */
	stream = fopen( gw_exec_file, "r" );
	if( stream == NULL ){
		printLog( DEBUGING, "%s:%d fopen(%s, r)", __FILE__, __LINE__, gw_exec_file );	/* V01 */
		return -1;
	}

	/* �t�@�C���ǂݍ��� */
	for( cnt = 0;fgets( line_buff, sizeof(line_buff), stream ) != NULL; ){
		/* ���s */
		if( line_buff[0] == '\n' )
			continue;

		/* �R�����g�s */
		if( line_buff[0] == '#' )
			continue;

		/* �s�������s�̏ꍇ�k���I�[�ɂ���B */
		i = strlen(line_buff);
		if( line_buff[i-1] == '\n' )
			line_buff[i-1] = 0x00;

		printLog( DEBUGING, "%s:%d %s(%s)", 
							__FILE__, __LINE__, gw_exec_file, line_buff );	/* V01 */

		/* P-Agent �z�X�g���l�� */
		if ( !strncmp(line_buff,GW_CNFPAGHST,sizeof(GW_CNFPAGHST)-1) ){
			strcpy( gwinf.pagenthost,&line_buff[sizeof(GW_CNFPAGHST)-1] );
			/* �\�P�b�g�L�q�q�������ŁA�z�X�g���^�|�[�gID���l���ς݂̏ꍇ */
			if( gwinf.sd == 0 && gwinf.pagentport[0] != 0x00 ){
				/* �\�P�b�g�A�h���X�o�^ */
				i = gw_bind( &gwinf );
				if( i == -1 ){
					fclose( stream );
					return -1;
				}
			}
		}
		/* P-Agent �|�[�gID�l�� */
		else if( !strncmp(line_buff,GW_CNFPAGPRT,sizeof(GW_CNFPAGPRT)-1) ){
			strcpy(gwinf.pagentport, &line_buff[sizeof(GW_CNFPAGPRT)-1]);
			/* �\�P�b�g�L�q�q�������ŁA�z�X�g���^�|�[�gID���l���ς݂̏ꍇ */
			if( gwinf.sd == 0 && gwinf.pagenthost[0] != 0x00 ){
				/* �\�P�b�g�A�h���X�o�^ */
				i = gw_bind( &gwinf );
				if( i == -1 ){
					fclose( stream );
					return -1;
				}
			}
		}
		/* P-Agent LISTEN �^�C�}�[�l�� */
		else if( !strncmp(line_buff,GW_CNFPAGLTM,sizeof(GW_CNFPAGLTM)-1) ){
			gwinf.pagentlsnt = atoi( &line_buff[sizeof(GW_CNFPAGLTM)-1]);
		}
		/* NTT IP ���t�@�C���p�X�l�� */
		else if( !strncmp(line_buff,GW_CNFPAGENV,sizeof(GW_CNFPAGENV)-1) ){
			strcpy( gwinf.env_path, &line_buff[sizeof(GW_CNFPAGENV)-1] );
		}
		/* G/W ���s�`���t�@�C���f�B���N�g���l�� */
		else if( !strncmp(line_buff,GW_CNFGWEXED,sizeof(GW_CNFGWEXED)-1) ){
			strcpy(gwinf.exe_dir, &line_buff[sizeof(GW_CNFGWEXED)-1]);
			/* ���t�@�C���͎g���ĂȂ��̂ŁA�Ƃ肠����         */
			/* G/W ���s�`���t�@�C���f�B���N�g���Ɠ����ɂ��Ă��� */
			strcpy( gwinf.env_path, &line_buff[sizeof(GW_CNFGWEXED)-1] );
		}
		/* G/W �N���`�� �v���Z�X�N�� */
		else if( !strncmp(line_buff,GW_CNFGW,strlen(GW_CNFGW)) ){
			/* ���ݒ�`�F�b�N */
			if( gwinf.pagenthost == NULL || gwinf.pagentport == NULL
				|| gwinf.pagentlsnt == 0 || gwinf.exe_dir == NULL ){
/********** �ύX �������� V01 **********/
				printLog( DEBUGING, "%s:%d ���ݒ肪�s�[���ł�", __FILE__, __LINE__ );
				printLog( DEBUGING, "PagentHost�F%s", gwinf.pagenthost );
				printLog( DEBUGING, "PagentPort�F%s", gwinf.pagentport );
				printLog( DEBUGING, "ListenTimer�F%d", gwinf.pagentlsnt );
				printLog( DEBUGING, "GW DIR�F%s", gwinf.exe_dir );
/********** �ύX �����܂� V01 **********/
				return -1;
			}
			printLog( DEBUGING, "%s:%d �v���Z�X�N�� %s", __FILE__, __LINE__, line_buff );	/* V01 */
			/* �f�^�v���e�[�u���ݒ� V0.4 V0.5 */
			sscanf( &line_buff[sizeof(GW_CNFGW)-1],"%s %s %s %s %s %s %s", c_buff,
				gwinf.exeinf[cnt].arg5, gwinf.exeinf[cnt].arg6,
				gwinf.exeinf[cnt].arg7, gwinf.exeinf[cnt].arg8,
				gwinf.exeinf[cnt].arg9, gwinf.exeinf[cnt].arg10 );
			sprintf( gwinf.exeinf[cnt].exefile, "%s/%s", gwinf.exe_dir, c_buff );

			/* �f�^�v���� */
			i = gw_create( &gwinf, &(gwinf.exeinf[cnt]) );
			if( i == -1 ){
				fclose( stream );
				return -1;
			}
			/* �L�� G/W�ʏ�񐔁AG/W�N���J�E���^�C���N�������g */
			gwinf.exeinfsuu++;
			printLog( DEBUGING, "%s:%d G/W�N���J�E���^ %d", __FILE__, __LINE__, gwinf.exeinfsuu );	/* V01 */
			cnt++;
		}
		/* �����ȍs */
		else{
			printLog( WARNING, "%s:%d file:%s �����ȍs:%s", 
										__FILE__, __LINE__, gw_exec_file, line_buff);	/* V01 */
		}
	}/* for */

	/* G/W�N���R���t�B�O���[�V�����t�@�C������� */
	fclose( stream );

	/* ����I�� */
	return cnt;
} /* gw_exec() */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_fdset: �f�^�v�\�P�b�g�L�q�q�ݒ�                                     */
/*																			*/
/* Parameters:                                                              */
/*   fd_set		*readers_p: OUT: �L�q�q�Z�b�g                               */
/*                                                                          */
/* Return:																	*/
/*   NONE                                                                   */
/*																			*/
/* Description:                                                             */
/*   �f�^�v�Ƃ̃\�P�b�g�L�q�q���L�q�q�Z�b�g�ɐݒ肷��B                     */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
void gw_fdset( fd_set *readers_p )
{
	int		cnt;					/* �J�E���^ */

	printLog( DEBUGING, "%s:%d gw_fdset(%d)", __FILE__,__LINE__, *readers_p );	/* V01 */

	/* �L�� G/W�ʏ��̃\�P�b�g�L�q�q(GW)���m�F */
	for ( cnt = 0 ; cnt < gwinf.exeinfsuu; cnt++ ){
		/* �\�P�b�g�L�q�q(GW)�m�F */
		if( gwinf.exeinf[cnt].sd != 0 ){
			/* �L���̏ꍇ�A�\�P�b�g�L�q�q��ǉ� */
			FD_SET( gwinf.exeinf[cnt].sd, readers_p );
		}
	}
} /* gw_fdset() */


/****************************************************************************/
/* Name:																	*/
/*	gw_getvalidfd		�f�^�v�\�P�b�g�L���L�q�q�l��						*/
/*																			*/
/* Parameters:																*/
/*	fd_set		*readers_p		�FIN	�L�q�q�Z�b�g						*/
/*	int			*fdsuu_p		�FOUT	�L���L�q�q���i�[�̈�A�h���X		*/
/*																			*/
/* Return:																	*/
/*		ret						�F�L���L�q�q�i�[�̈�A�h���X				*/
/*																			*/
/* Description:																*/
/*	�ǂݍ��ݏ����̐������\�P�b�g�L�q�q��Ԃ��B								*/
/*	�Ԓl�̎����A�h���X���� "�L���L�q�q��" �̋L�q�q���L���B					*/
/*	�L�q�q�Z�b�g�́AFD_CLR ���s���B											*/
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int	*gw_getvalidfd( fd_set *readers_p, int *fdsuu_p )
{
	static int		fds[GW_MXGWSUU];		/* �L���L�q�q�i�[�̈�			*/
	int				cnt;					/* �J�E���^						*/


	printLog( DEBUGING, "%s:%d gw_getvalidfd(%d,%d)",
						__FILE__,__LINE__, *readers_p, *fdsuu_p );	/* V01 */

	/* �L�� G/W�ʏ��̃\�P�b�g�L�q�q(GW)���m�F							*/
	for ( cnt = 0, *fdsuu_p = 0; cnt < gwinf.exeinfsuu; cnt++ ) {
		/* �\�P�b�g�L�q�q(GW)�m�F											*/
		if( gwinf.exeinf[cnt].sd != 0 ) {
			/* �����n�j���m�F����											*/
			if( FD_ISSET( gwinf.exeinf[cnt].sd, readers_p ) ){
				FD_CLR( gwinf.exeinf[cnt].sd, readers_p );
				/* �����n�j�ȃ\�P�b�g�L�q�q��ǉ�							*/
				fds[(*fdsuu_p)++] = gwinf.exeinf[cnt].sd;
			}
		}
	} /* for */

	/* �L���L�q�q�i�[�̈�A�h���X��Ԃ� */
	return fds;

} /* gw_getvalidfd() */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_getgwinfaddr                                                        */
/*                                                                          */
/* Parameters:																*/
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   GW_INF *ret: �f�^�v���e�[�u���A�h���X                                */
/*                                                                          */
/* Description:																*/
/*   �f�^�v���e�[�u���A�h���X�𓾂�                                       */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
GW_INF	*gw_getgwinfaddr()
{
	printLog( DEBUGING, "%s:%d gw_getgwinfaddr()", __FILE__,__LINE__ );	/* V01 */

	return &gwinf;

} /* gw_getgwinfaddr() */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_kill: �f�^�v�I��                                                    */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   �f�^�v�Ƃ̃R�l�N�V���������B                                       */
/*   �f�^�v�v���Z�X�ɏI���V�O�i���𑗐M����B                               */
/*                                                                          */
/*   ���f�^�v���e�[�u���̎Q�Ɓ^�X�V���s���B                               */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
void gw_kill()
{
	int				cnt;						/* �J�E���^					*/

	printLog( DEBUGING, "%s:%d gw_kill( )", __FILE__, __LINE__ );	/* V01 */

	/* �f�^�v�v���Z�X�̃R�l�N�V��������� */
	for( cnt = 0; cnt < GW_MXGWSUU; cnt++ ){
		/* �\�P�b�g�L�q�q(GW)���L�������f���� */
		if( gwinf.exeinf[cnt].sd != 0 ){
			/* �R�l�N�V�������������� */
			close( gwinf.exeinf[cnt].sd );
			gwinf.exeinf[cnt].sd = 0;
		}
	}

	/* �f�^�v�v���Z�X�ɏI���V�O�i���𑗐M���� */
	for( cnt = 0; cnt < GW_MXGWSUU; cnt++ ){
		/* �v���Z�X�h�c���L�������f���� */
		if( gwinf.exeinf[cnt].pid != 0 ){
			/* �v���Z�X�̐������m�F */
			if( kill(gwinf.exeinf[cnt].pid, 0) == 0 ){
				/* �I���V�O�i�����M */
				kill( gwinf.exeinf[cnt].pid, SIGTERM );
				printLog( DEBUGING, "%s:%d kill(%d, SIGTERM)", 
										__FILE__, __LINE__, gwinf.exeinf[cnt].pid );	/* V01 */
				gwinf.exeinf[cnt].pid = 0;
			}
		}
	}

	/* �L�� G/W�ʏ�񐔃��Z�b�g */
	gwinf.exeinfsuu = 0;

	/* P-Agent ���̃R�l�N�V��������� */
	close( gwinf.sd );
	gwinf.sd = 0;
	gwinf.listen_flag = GW_INFLISTENOFF;	 /* LISTEN FLAG=OFF */

} /* gw_kill() */

/* END OF FILE  */
