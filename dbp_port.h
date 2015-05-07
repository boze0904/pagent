/****************************************************************************/
/* file name  dbp_port.h                                                    */
/*                                                                          */
/*	NTT IP DB P-Agent �p�w�b�_                                              */
/*																			*/
/* Create : 2000.12.15(dcm_port.h���痬�p)                                  */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef DBP_PORT_H
#define DBP_PORT_H

#define	DBP_PAGENT_VER	"NTT IP P-Agent V1.0 2000.12.15" /* P-Agent Version	*/

/****************************************************************************/
/* �r�f�t�@�C�����															*/
/****************************************************************************/
#define	DBP_ENVPATH		"DBP_ENV_PATH"	/* ���ϐ��F�t�@�C���p�X			*/
										/* �R���t�B�O�t�@�C��				*/
										/* �[���ݒ�t�@�C��					*/

/****************************************************************************/
/* �����l																	*/
/****************************************************************************/
#define	DCM_MXFILELEN		1024 		/* �ő�t�@�C���p�X���� PATH_MAX	*/

#define	DCM_MXCNFLEN		16			/* �ő�R���t�B�O�t�@�C������		*/
#define	DCM_MXHSTLEN		10			/* �ő�z�X�g����					*/

#define	DCM_MXSNDLEN		4096		/* �ő呗�M���b�Z�[�W��				*/
#define	DCM_MXMSGLEN		20600		/* �ő僁�b�Z�[�W��(for NETXP MIN)	*/
#define	DCM_MXTRCLEN		DCM_MXMSGLEN/* �ő�g���[�X�f�[�^��				*/
#define	DCM_MXEOLLEN		1			/* �ő�d�n�k��						*/
#define	DCM_EOLCODE			012			/* �d�n�k�R�[�h�i���݌Œ�j			*/


/****************************************************************************/
/*	P-Agent ����R�}���h													*/
/****************************************************************************/
#define	DCM_CMD_TRACE_ON	"TRACE-ON"		/* Trace�J�n�v���R�}���h		*/
#define	DCM_CMD_TRACE_OFF	"TRACE-OFF"		/* Trace��~�v���R�}���h		*/
#define DBP_TERM			"TERM"			/* P-Agent�I���R�}���h          */


/****************************************************************************/
/*	P-Agent ���b�Z�[�W														*/
/****************************************************************************/
#define	DCM_TRACE_SFC_MIN	"<--"			/* MIN���b�Z�[�W�T�t�B�b�N�X	*/
#define	DCM_TRACE_SFC_MOUT	"-->"			/* MOUT���b�Z�[�W�T�t�B�b�N�X	*/

#define	DCM_MSG_HEADER		"NetExpert  "	/* ���b�Z�[�W�w�b�_				*/
#define	DCM_MSG_COMMANDERR	"Command Error"	/* ����R�}���h�s��				*/


/****************************************************************************/
/*	�g���[�X																*/
/****************************************************************************/
/* �J�n�^�I�����̃t�H�[�}�b�g											*/
#define	DCM_TRACE_ON_FORM	"TRACE_ON ***********************************************************************\n"
#define	DCM_TRACE_OFF_FORM 	"TRACE_OFF***********************************************************************\n"

#define	DCM_TRACE_DATA_TERM	"\377"			/* �f�[�^���E�i�f�[�^�I�[�j		*/
#define	DCM_TRACE_FILE_TERM	"\0"			/* �f�[�^���E�i�t�@�C���o�́j	*/


/****************************************************************************/
/* P-Agent ������															*/
/****************************************************************************/
typedef struct {
	char	eol[DCM_MXEOLLEN+1];		/* �o�̓��b�Z�[�W�p�d�n�k			*/
	int		eol_len;					/* �o�̓��b�Z�[�W�p�d�n�k��			*/
	char	msgdata[DCM_MXMSGLEN+sizeof(DCM_TRACE_DATA_TERM)];
										/* ���b�Z�[�W�o�b�t�@�i��Ɨ̈�j	*/
	int		msgdisc_len;				/* ���b�Z�[�W���ʒ�					*/
	char	hostname[DCM_MXHSTLEN+1];	/* ���z�X�g��						*/
	char	configfile[DCM_MXFILELEN+1];/* �R���t�B�O�t�@�C���t���p�X��		*/
	char	tmpfifo[DCM_MXFILELEN+1];	/* TEMP FIFO �t�@�C���p�X��			*/
	int		tmpfifo_fd;					/* TEMP FIFO �t�@�C���L�q�q	0:����	*/
} DBP_INF;

#endif

/* END OF FILE */
