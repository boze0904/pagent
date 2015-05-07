/****************************************************************************/
/* file nanme  dbpag_if.c                                                   */
/*                                                                          */
/* TCP/IP I/F�֐��Q���g�p���āAG/W �� DB P-Agent �ƃf�[�^����M���s���B     */
/*                                                                          */
/* Function:                                                                */
/*   dbp_pagif_init()                                                       */
/*   dbp_pagif_send()                                                       */
/*   dbp_pagif_recv()                                                       */
/*   dbp_pagif_term()                                                       */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h> 
#include <errno.h> 
#include <unistd.h>
#include "tcp_if.h"
#include "gw_if.h"
#include "output_log.h"



/****************************************************************************/
/*	�����ϐ�																*/
static int			dsp_pid;			/* �\���p�v���Z�X�h�c				*/
static GW_PAGIF		*dcmpagif_p = NULL;	/*  P-Agent I/F �f�[�^				*/
static int			data_max;



/****************************************************************************/
/* Name:                                                                    */
/*   dbp_pagif_init                                                         */
/*                                                                          */
/* Parameters:                                                              */
/*   int	max_len: IN: �ő�f�[�^��                                       */
/*                                                                          */
/* Return:                                                                  */
/*    0: ����I��                                                           */
/*   -1�F�ُ�I��                                                           */
/*                                                                          */
/* Description:                                                             */
/*   P-Agent I/F ��������                                                   */
/*   P-Agent I/F �f�[�^�̈���m�ۏ���������B                               */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int dbp_pagif_init( int max_len )
{
	printLog( DEBUGING, "*** dbp_pagif_init() ***" );	/* V01 */

	/* �\���p�v���Z�X�h�c�l�� */
	dsp_pid = getpid();

	printLog( DEBUGING, "%d=getpid()", dsp_pid );		/* V01 */

	/* P-Agent I/F �f�[�^�̈�m�� */
	dcmpagif_p = (GW_PAGIF*)malloc( sizeof(dcmpagif_p->data_len)+max_len );
	if( dcmpagif_p == NULL ){
		printLog( ERROR, "%s:%d(%d): NULL = malloc( %d ) errno:%d", __FILE__,
			__LINE__, dsp_pid, sizeof(dcmpagif_p->data_len)+max_len, errno);
		return -1;
	}

	data_max = max_len;

	/* ����I�� */
	return 0;

} /* dbp_pagif_init */



/****************************************************************************/
/* Name:                                                                    */
/*   dbp_pagif_send                                                         */
/*																			*/
/* Parameters:																*/
/*   TCPIF_INF	*tcpinf_p: IN: TCP/IP ������                              */
/*   char		*to_buff : IN: �����ʖ�                                     */
/*   char		*data_p  : IN: �f�[�^                                       */
/*																			*/
/* Return:																	*/
/*    0: ����I��                                                           */
/*   -1: �ُ�I��                                                           */
/*																			*/
/* Description:																*/
/*   NTT IP DB P-Agent �Ƀf�[�^�𑗐M����B                                 */
/*   TCP/IP ������́A���ɐڑ��ς݂ł��邱�ƁB                            */
/*   P-Agent I/F ��(�f�[�^���{�f�[�^)�Ȃ̂ŁA�܂��擪�S�o�C�g�Ƀf�[�^����	*/
/*   �R�s�[���A�����ăf�[�^�����̃f�[�^���R�s�[���Ĉ�C�ɑ��M����B			*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int dbp_pagif_send( TCPIF_INF *tcpinf_p, char *to_buff, char *data_p ) 
{
	int		i;				/* ���[�N         */
	int		len;			/* ���M�f�[�^��   */


	/* P-Agent I/F �f�[�^ �������m�F */
	if( dcmpagif_p == NULL ){
		printLog( ERROR, "%s:%d(%d)  P-Agent I/F �f�[�^ ��������", 
											__FILE__, __LINE__, dsp_pid );	/* V01 */
		/* �ُ�I�� */
		return -1;
	}

	memset( dcmpagif_p, 0, data_max+sizeof(dcmpagif_p->data_len) );

	/* P-Agent I/F �f�[�^�ݒ� */
	/* �����ʖ��E�f�[�^�ݒ� */
	if( strlen(to_buff) != 0 ){
		sprintf( dcmpagif_p->data, "%s,%s", to_buff, data_p );
	}
	else{
		sprintf( dcmpagif_p->data, "%s", data_p );
	}	
	dcmpagif_p->data[strlen(dcmpagif_p->data)] = '\012';
	dcmpagif_p->data[strlen(dcmpagif_p->data)] = '\0';
	dcmpagif_p->data_len = strlen( dcmpagif_p->data );	/* �f�[�^�� */

	/* ���M�f�[�^���l���i�f�[�^���i�[�̈敪�{�{�̂̃f�[�^���j */
	len = ( sizeof(dcmpagif_p->data_len) + dcmpagif_p->data_len );

/********** �ύX �������� V01 **********/
	printLog( DEBUGING, "G/W -> P-Agent Data : %s", dcmpagif_p->data );
	printLog( DEBUGING, "Data Length : %d", dcmpagif_p->data_len );
	printLog( DEBUGING, "Send Data Length : %d", len );
/********** �ύX �������� V01 **********/

	/* �f�[�^���M */
/***
	i = tcp_send( tcpinf_p, dcmpagif_p->data, len );
***/
	i = tcp_send( tcpinf_p, (char*)dcmpagif_p, len );
	if( i == -1 ){
		/* �ُ�I�� */
		return -1;
	}

	/* ����I�� */
	return 0;

} /* dbp_pagif_send */



/****************************************************************************/
/* Name:                                                                    */
/*   dbp_pagif_recv                                                         */
/*                                                                          */
/* Parameters:																*/
/*   TCPIF_INF	*tcpinf_p:  IN: TCP/IP ������                             */
/*   char		*data_p  : OUT: �f�[�^                                      */
/*																			*/
/* Return:																	*/
/*	 etc		�F��M�f�[�^��												*/
/*	 -1			�F�ُ�I��													*/
/*																			*/
/* Description:																*/
/*   NTT IP DB P-Agent ����f�[�^����M����B                               */
/*   TCP/IP ������́A���ɐڑ��ς݂ł��邱�ƁB                            */
/*   P-Agent I/F (�f�[�^���{�f�[�^)�ɍ��킹�āA�܂��擪�S�o�C�g����M����   */
/*   �ȍ~�̃f�[�^����M����B                                               */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int dbp_pagif_recv( TCPIF_INF *tcpinf_p, char *data_p )
{
	int		len;		/* �f�[�^��       */
	int		len_len;	/* �f�[�^���̒��� */
	int		cnt;		/* �J�E���^       */
	int		i;			/* ���[�N         */
	char	*c_p;		/* ���[�N         */

	printLog( DEBUGING, "*** dbp_pagif_recv() ***" );	/* V01 */

	/* P-Agent I/F �f�[�^ �������m�F */
	if( dcmpagif_p == NULL ){
		printLog( DEBUGING, "%s:%d(%d) P-Agent I/F��������", 
									__FILE__, __LINE__, dsp_pid );
		/* �ُ�I�� */
		return -1;
	}

	/* �f�[�^���ǂݍ���	 -> len */
	c_p = (char *)&len;
	len_len = sizeof(dcmpagif_p->data_len);
	for( cnt = 0, i = 0; cnt < len_len ; cnt += i ){
		i = tcp_recv( tcpinf_p, (c_p+cnt), len_len - cnt );
		if( i == -1 ){
			/* �ُ�I�� */
			return -1;
		}
	} /* for */
	printLog( DEBUGING, "%s:%d(%d) ->P-Agent �f�[�^��:0x%08x(%d)", 
									__FILE__, __LINE__, dsp_pid, len,len );	/* V01 */

	/* �f�[�^�����̃f�[�^��ǂݍ��� */
	for( cnt = 0, i = 0; cnt < len ; cnt += i ){
		i = tcp_recv( tcpinf_p, (data_p+cnt), len - cnt );
		if( i == -1 ){
			/* �ُ�I�� */
			return -1;
		}
	} /* for */

	/* ����I�� */
	return len;

} /* dbp_pagif_recv */



/****************************************************************************/
/* Name:                                                                    */
/*   dbp_pagif_term                                                         */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   P-Agent�h/F �I������                                                   */
/*   P-Agent I/F �f�[�^�̈���������                                       */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
void dbp_pagif_term()
{
	printLog( DEBUGING, "%s:%d(%d)  dbp_pagif_term �Ăяo��", 
									__FILE__, __LINE__, dsp_pid );	/* V01 */

	/*  P-Agent I/F �f�[�^ �m�F */
	if( dcmpagif_p == NULL ) 				/* ������� */
		return;

	/* P-Agent I/F �f�[�^�̈��� */
	free( dcmpagif_p );

	/* P-Agent I/F �f�[�^������ */
	dcmpagif_p = NULL;

} /* dbp_pagif_term */

/* End of File */
