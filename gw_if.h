/****************************************************************************/
/* file name  gw_if.h                                                       */
/*																			*/
/*   NTT IP DB P-Agent GW I/F�֐��p�w�b�_                                   */
/*																			*/
/* Create : 2000.12.15(DoCoMo gw_if.h�̗��p)                                */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef GW_IF_H
#define	GW_IF_H

#include <netinet/in.h>

/****************************************************************************/
/* �ő�l																	*/
/****************************************************************************/
#define	GW_MXIFDTLEN	8192				/* GW - PAgent I/F �f�[�^�ő咷	*/

/****************************************************************************/
/* Gateway ������															*/
/****************************************************************************/
#define	GW_MXFILELEN	1024				/* �ő�t�@�C���p�X����	PATH_MAX*/
#define	GW_MXLINELEN	128					/* �s���ő啶����				*/
#define	GW_MXHSTLEN		10					/* �ő�z�X�g����				*/
#define	GW_MXSECLEN		16					/* �ő�Z�N�V��������			*/
#define	GW_MXNAMELEN	16					/* �ő�f�v���ʖ���				*/
#define	GW_MXGWSUU		255					/* �ő�N���f�^�v��				*/

/* �f�^�v���e�[�u��														*/
typedef struct {
	char			exefile[GW_MXFILELEN+1];/* �N���t�@�C��					*/
	char			name[GW_MXNAMELEN+1];	/* �f�v���ʖ�					*/
	int				pid;					/* �v���Z�X�h�c					*/
	int				sd;						/* �\�P�b�g�L�q�q(GW)			*/
	char			arg5[GW_MXFILELEN+1];	/* ��T�p�����[�^				*/
	char			arg6[GW_MXFILELEN+1];	/* ��U�p�����[�^				*/
	char			arg7[GW_MXFILELEN+1];	/* ��V�p�����[�^				*/
	char			arg8[GW_MXFILELEN+1];	/* ��W�p�����[�^				*/
	char			arg9[GW_MXFILELEN+1];	/* ��X�p�����[�^			V0.3*/
	char			arg10[GW_MXFILELEN+1];	/* ��10�p�����[�^			V0.4*/
} GW_EXEINF;

typedef struct {
	char			env_path[GW_MXFILELEN+1];/* ���t�@�C���p�X			*/
	char			exe_dir[GW_MXFILELEN+1];/* ���s�`���t�@�C���f�B���N�g��	*/
	char			pagenthost[GW_MXHSTLEN+1];/* P-Agent �z�X�g��			*/
	char			pagentport[5+1];		/* P-Agent �|�[�g�ԍ� 5�P�^		*/
	int				pagentlsnt;				/* P-Agent Listen �^�C�}�[		*/
	int				sd;						/* �\�P�b�g�L�q�q(P-Agent)		*/
	char			listen_flag;			/* LISTEN FLAG		1999.09.18	*/
#define				GW_INFLISTENOFF	0x00		/* LISTEN ��				*/
#define				GW_INFLISTENON	0x01		/* LISTEN ��				*/
	struct sockaddr_in	name;				/* �\�P�b�g�A�h���X��			*/
	int				exeinfsuu;				/* �L�� G/W�ʏ��			*/
	GW_EXEINF		exeinf[GW_MXGWSUU];		/* G/W �ʏ��					*/
} GW_INF;


/****************************************************************************/
/*  P-Agent Configuration File												*/
/****************************************************************************/
#define	GW_CNFPAGHST	"PAGENTHOST="	/* TAG���́FP-Agent �z�X�g��        */
#define	GW_CNFPAGPRT	"PAGENTPORT="	/* TAG���́FP-Agent �|�[�gID        */
#define	GW_CNFPAGLTM	"PAGENTLTIM="	/* TAG���́FP-Agent LISTEN �^�C�}�[ */
#define	GW_CNFPAGENV	"GW_ENVPATH="	/* TAG���́FP-Agent ���t�@�C���p�X*/
#define	GW_CNFGWEXED	"GW_EXEDIR="	/* TAG���́FG/W ���s�`���f�B���N�g��*/
#define	GW_CNFGW		"GW="			/* TAG���́FG/W �N���`��			*/


/****************************************************************************/
/*  P-Agent - G/W Prossecc I/F												*/
/****************************************************************************/
/*  P-Agent �f�[�^�\��														*/
typedef struct {
	int				data_len;				/* �f�[�^��						*/
	char			data[GW_MXIFDTLEN];		/* �f�[�^						*/
} GW_PAGIF;

/*  ���於(GW_PAGFOR_NETX �ȊO�͓��������ł��邱��)							*/
#define	GW_PAGFOR_NOLEN	4					/* ����ԍ���					*/
#define	GW_PAGFOR_NETX	"NETEXPERT"			/* �m�����d����������			*/
#define	GW_PAGFOR_OPE	"OPE_"				/* �n�o�d						*/
#define	GW_PAGFOR_SCPE	"SCPE"				/* �r�b�o�d						*/
#define	GW_PAGFOR_REC	"REC_"				/* �q�d�b						*/
#define	GW_PAGFOR_OCPE	"OCPE"				/* �n�b�o�d						*/
#define	GW_PAGFOR_OCSE	"OCSE"				/* �n�b�r�d					V0.5*/
#define	GW_PAGFOR_UDP	"UDP_"				/* �t�c�o						*/



extern int		gw_exec(char*);		/* �f�^�v�v���Z�X�N��             */
extern void		gw_kill();			/* �f�^�v�v���Z�X�I��             */
extern void		gw_fdset(fd_set*);	/* �f�^�v�\�P�b�g�L�q�q�ݒ�       */
extern int		*gw_getvalidfd(fd_set*,int*);	/* G/W�\�P�b�g�L���L�q�q�l�� */
extern GW_INF	*gw_getgwinfaddr();	/* G/W���e�[�u���A�h���X�l�� */



#endif

/* END OF FILE */
