/****************************************************************************/
/* file nanme  db_pagent.cc                                                 */
/*                                                                          */
/* NTT IP DB P-Agent�p �T�u�֐�                                             */
/*                                                                          */
/* Function:                                                                */
/*   processInput :                                                         */
/*   msgtomout    : ���b�Z�[�W�o��                                          */
/*   illegalmsgout: �s������R�}���h���b�Z�[�W�o��                          */
/*                                                                          */
/* Create : 2000.12.15(dcm_pagent.c����̗��p)                              */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include "dbp_port.h"
#include "db_pagent.h"
#include "gw_if.h"
#include "output_log.h"


/****************************************************************************/
/* External Value 															*/
extern char 	*processName;
extern DBP_INF	dbp_inf;				/* Docomo P-Agent ������			*/
extern int		port_fd;				/* �ǂݍ��ݑΏ�fd�FreadPort( ) I/F	*/

/* Externally Defined Functions 											*/
extern void	writeDataToFifos(fd_set*,int*,char*,int);
extern void	writeDataToSocket(char*,int);
extern char	*readDataFromFifo(fd_set*,int*,int*);
extern char	*readDataFromSocket(fd_set*,int*,int*);
extern int readPort(char*,unsigned int);
extern int writePort(char*,unsigned int);


/****************************************************************************/
/* Name:                                                                    */
/*   processInput                                                           */
/*                                                                          */
/* Parameters:                                                              */
/*   int	selected    : value returned from the last select               */
/*   fd_set	*readers    : fd set of all possible descriptors to read from   */
/*   int	select_errno: value of errno after the last select              */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   This is the main dispatching routine for input into a p_agent.         */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
void processInput( int select_ret, fd_set *readers_p, int select_errno )
{
	char		raw[DCM_MXMSGLEN+sizeof(DCM_TRACE_DATA_TERM)];
											/* �|�|�g����̎�M�f�[�^   */
    int 		raw_len;					/* �|�[�g����̎�M�f�[�^�� */
	int			*fds_p;						/* �L���L�q�q�|�C���^       */
	int			fds_suu;					/* �L���L�q�q��             */
	int			cnt;						/* �J�E���^                 */
    char		*sptr;						/* FIFO ���� �[���̃f�[�^   */
	int			s_len;						/* FIFO ���� �[���̃f�[�^�� */
    int			i;							/* ���|�N                   */
	int		d_cnt;


	printLog( DEBUGING, "%s:%d processInput(select_ret:%d, ,select_errno:%d)",
								__FILE__,__LINE__, select_ret, select_errno );	/* V01 */
	printLog( DEBUGING, "%s:%d processName:%s port_fd:%d", __FILE__,__LINE__,
													processName, port_fd );	/* V01 */

	/* ��M�f�[�^�̈揉���� */
	memset( raw, 0, sizeof(raw) );
	/* select( )�֐��l�`�F�b�N */
	if( select_ret < 0 ){
		/* select failed */
		printf("%s %d: %d = select( ) errno %d\n", __FILE__, __LINE__,
												select_ret, select_errno );
		return  ;
	}
	else if ( select_ret == 0 ) {
		/* �^�C���A�E�g�i���g�p�Ȃ̂ŗ��Ȃ��͂����j */
		printf("%s %d: %d = select( ) timed out errno %d\n", __FILE__, __LINE__,
												select_ret, select_errno );
		return  ;
	}

	/* �f�^�v�\�P�b�g�L���L�q�q�l�� */
	fds_p = gw_getvalidfd(readers_p, &fds_suu );
	if( fds_suu ){	/* �L���L�q�q����̏ꍇ */
		/* �|�[�g�̃f�B�X�N���v�^���`�F�b�N */
		for( cnt = 0; cnt < fds_suu; cnt++ ){
			/* �|�[�g�ǂݍ��� */
			port_fd = *(fds_p+cnt);		/* readPort I/F */
			raw_len = readPort( raw, sizeof(raw) );
			if( raw_len > 0 ) {


				/* �g���[�X�t�@�C���֏������� */
				memcpy(&raw[raw_len],DCM_TRACE_DATA_TERM,
												sizeof(DCM_TRACE_DATA_TERM));
				/* �e�h�e�n�ɏ����o�� */
				writeDataToFifos( NULL, NULL, raw, raw_len );
				/* �[����NETEXPERT�p�f�[�^���������� */
				writeDataToSocket( raw, raw_len );
			}
			else if( raw_len == 0 ){
				printLog( DEBUGING, "%s:%d 0 = readPort( ) ���̎q���s�f�[�^�̎�M����?",
														__FILE__, __LINE__ );	/* V01 */
			}
			else {
				printLog( DEBUGING, "%s:%d -1 = readPort( ) �I��", 
													__FILE__, __LINE__ );	/* V01 */
				/* ��M�ُ�́A���܂̂Ƃ���I���Ƃ���B */
				kill( getpid( ), SIGTERM );
				pause();
			}
		} /* for end */
		/* select������A�L���|�[�g�����폜 */
		select_ret -= fds_suu;
	}

	/* �e�h�e�n�̓��͂��`�F�b�N */
	if( select_ret ){
		/* �e�h�e�n������͂��� */
    	sptr = readDataFromFifo( readers_p, &select_ret, &s_len );
		/* ���͂��`�F�b�N */
    	if( sptr ){
			printLog( DEBUGING, "%s:%d readDataFromFifo() ����̓���", 
													__FILE__, __LINE__ );	/* V01 */
			/* �|�[�g�ɏo�͂��� */
			i = writePort( sptr, s_len );	/* sendToPort() �͂�߂��Ⴄ */
			printLog( DEBUGING, "%s:%d %d = writePort( , len:%d)", 
										__FILE__, __LINE__, i, s_len );	/* V01 */
			/* �o�͌��ʃ`�F�b�N */
			if( i != s_len ){
				printLog( DEBUGING, "%s:%d writePort( ) �ُ�I�� �ۗ�", 
													__FILE__, __LINE__ );	/* V01 */
			}
		}
	}

	/* �[������̓��͂��`�F�b�N */
	if( select_ret ){
		/* �[��������͂��� */
		sptr = readDataFromSocket( readers_p, &select_ret, &s_len );
		/* ���͂��`�F�b�N */
		if( sptr ){
			/* �|�[�g�ɏo�͂��� */
			printLog( DEBUGING, "%s:%d readDataFromSocket( ) ����̓���", 
													__FILE__, __LINE__ );	/* V01 */
			i = writePort( sptr, s_len );	/* sendToPort() �͂�߂��Ⴄ */
			/* �o�͌��ʃ`�F�b�N */
			if( i != s_len ){
				printLog( DEBUGING, "%s:%d writePort( ) �ُ�I�� �ۗ�", 
													__FILE__, __LINE__);	/* V01 */
			}
		}
	}

	/* ���͎c��̃`�F�b�N */
	if( select_ret ){
		printLog( DEBUGING, "%s:%d select_ret:%d", 
								__FILE__, __LINE__, select_ret );	/* V01 */
	}
} /* processInput */



/************************************************************************/
/* P-Agent ���b�Z�[�W�o��												*/
/************************************************************************/

/****************************************************************************/
/* Name:                                                                    */
/*   msgtomout                                                              */
/*                                                                          */
/* Prameters:                                                               */
/*   char	*msg_val: ���b�Z�[�W�ϕ�                                      */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   P-Agent ���b�Z�[�W���l�n�t�s�ɏo�͂���B                               */
/*   ���b�Z�[�W���ʂ� msgtomout_ini()�Ŏw�肵�����e���A���t�E�����y��       */
/*   �d�n�k�͓��֐��ŕt������B                                             */
/*                                                                          */
/*   MOUT �o�͎��t�H�[�}�b�g                                                */
/*   ���b�Z�[�W���ʁ{���t�E�����{���b�Z�[�W�ϕ�                           */
/*                                                                          */
/* Create : 2000.12.15(dcm_pagent.c����̗��p)                              */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
void msgtomout( char *msg_val )
{
	time_t		now_time;			/* ���ݎ����i�ʎZ�b�j				*/
	struct tm	*tm_p;				/* ���ݎ����i�ڍׁj					*/
	int			tm_year;			/* �N�i�P�X�O�O����j				*/
	int			len;				/* ������							*/
	int			i;					/* ���|�N							*/

	printLog( DEBUGING, "%s %d:dbp_inf.msgdisc_len %d", 
								__FILE__, __LINE__, dbp_inf.msgdisc_len );	/* V01 */
	printLog( DEBUGING, "%s %d:dbp_inf.msgdata     %s", 
								__FILE__, __LINE__, dbp_inf.msgdata );	/* V01 */

	/* ���t�E�����ݒ�													*/
	time(&now_time);			    /* ���ݎ����l��						*/
	tm_p = localtime(&now_time);	/* ���ݎ����̏ڍ׊l��				*/
	tm_year = tm_p->tm_year;
	if( tm_year > 99 ){
		/* 2000�N���̏ꍇ												*/
		tm_year -= 100;
	}
	sprintf(&dbp_inf.msgdata[dbp_inf.msgdisc_len],
		"%02d/%02d/%02d %02d:%02d:%02d ",tm_year,tm_p->tm_mon+1,
				tm_p->tm_mday,tm_p->tm_hour,tm_p->tm_min,tm_p->tm_sec);

	printLog( DEBUGING, "%s %d: data      %s", 
								__FILE__, __LINE__, dbp_inf.msgdata);	/* V01 */

	/* ���b�Z�[�W�ϕ��ݒ�												*/
	strcat( dbp_inf.msgdata, msg_val );
	len = strlen( dbp_inf.msgdata );

	printLog( DEBUGING, "%s %d: len:%d  data %s", 
							__FILE__, __LINE__, len, dbp_inf.msgdata );	/* V01 */

	/* P-Agent�o�̓��b�Z�[�W�p�d�n�k�ݒ�								*/
	memcpy(&dbp_inf.msgdata[len],dbp_inf.eol, dbp_inf.eol_len);
	len += dbp_inf.eol_len;

	/* �g���[�X�o�� */
	memcpy(&(dbp_inf.msgdata[len]),DCM_TRACE_DATA_TERM,
											sizeof(DCM_TRACE_DATA_TERM));

    /* ���b�Z�[�W�o�� */
    writeDataToSocket(dbp_inf.msgdata, len);
    writeDataToFifos(NULL, NULL, dbp_inf.msgdata, len);

	printLog( DEBUGING, "%s %d: writeDataToFifos( )", __FILE__, __LINE__ );	/* V01 */

} /* msgtomout */



/****************************************************************************/
/* Name:                                                                    */
/*   illegalmsgout	�s������R�}���h���b�Z�[�W�o��                          */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*cmd_p : IN: �s���Ȑ���R�}���h                                 */
/*   char	cmd_len: IN: ����R�}���h��                                     */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/* Description:                                                             */
/*   �s���Ȑ���R�}���h���ۂ��Ǝ󂯂āA����R�}���h�s�����b�Z�[�W��ҏW     */
/*   ���Amsgtomout()�ɂ�� ���b�Z�[�W���o�͂���B                           */
/*                                                                          */
/*   �s���R�}���h�̍ő�R�s�[���́A�iMML - (MHL + CIL + MEL )�j             */
/*                                                                          */
/*   MML�F�ő僁�b�Z�[�W��                                                  */
/*   MHL�F���b�Z�[�W�w�b�_��                                                */
/*   CIL�F����R�}���h�s�����b�Z�[�W�Œ蕔��                                */
/*   MEL�F���b�Z�[�W�p EOL��                                                */
/*                                                                          */
/* Create : 2000.12.15(dcm_pagent.c����̗��p)                              */
/* Modify :                                                                 */
/****************************************************************************/
void illegalmsgout( char *cmd_p, int cmd_len )
{
	int 	cnt, cnt2;				/* �J�E���^							*/
	int		max_len;				/* �ő�R�s�[��						*/
    int		i;						/* ���|�N							*/
	char	msgbuf[DCM_MXMSGLEN+1];	/* ���b�Z�[�W�ҏW�̈�				*/


	/*	�ő僁�b�Z�[�W���ݒ�											*/
	max_len = DCM_MXMSGLEN;

	/*	���b�Z�[�W�w�b�_�Ǝ����b�i�X�j�Ɛ���R�}���h�s�����b�Z�[�W�Œ�	*/
	/*	���Ƃd�n�k���̍����i�ő�s������R�}���h�ݒ萔�j�����߂�B		*/
	max_len -= (strlen(DCM_MSG_HEADER) + 9 + strlen(DCM_MSG_COMMANDERR) +
				dbp_inf.eol_len);

	/* ����R�}���h�s�����b�Z�[�W�Œ蕔�ݒ�								*/
	strcpy(msgbuf,DCM_MSG_COMMANDERR);

	/*	��ŋ��߂��ő咷���ϕ��ɕt������R�}���h���Z���ꍇ�́A����	*/
	/*	�����������t������B											*/
	i = cmd_len - dbp_inf.eol_len;	/* EOL�������ƂȂ�o�C�g���B		*/
	if( max_len > i )
		max_len = i;

	cnt2 = strlen(DCM_MSG_COMMANDERR);
	for( cnt = 0; cnt < max_len;cnt++,cnt2++ ){
		msgbuf[cnt2] = *(cmd_p+cnt);
	}

	/*	�I�[�� NULL �����āimsgtomout()�̃C���^�t�F�[�X�j�o�́B		*/
	msgbuf[cnt2] = '\0';

	/* ����R�}���h�s�����b�Z�[�W�o��	*/
	msgtomout(msgbuf);
} /* illegalmsgout */

/* END OF FILE */
