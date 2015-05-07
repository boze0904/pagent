/****************************************************************************/
/* file name  dbp_portsub.cc                                                */
/*																			*/
/* Function:                                                                */
/*   dbpsub_init                                                            */
/*   dbpsub_netx_gw                                                         */
/*   dbpsub_gw_netx                                                         */
/*   dbpsub_term                                                            */
/*                                                                          */
/*   NTT IP DB P-Agent G/W I/F �T�u����                                     */
/*   NetExpert������̃f�[�^�� G/W �ɐU�蕪�����M����B						*/
/*   G/W ������̃f�[�^�� NetExpert �� G/W �ɐU�蕪���A                     */
/*   G/W �̏ꍇ�͑��M����B                                                 */
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "dbp_port.h"
#include "gw_if.h"
#include "output_log.h"



/****************************************************************************/
/* �����ϐ�																	*/
static GW_INF		*gwinf_p = NULL;		/* �f�^�v���e�[�u���|�C���^	*/
#define MASTER		"BASIC"


/****************************************************************************/
/* Name:                                                                    */
/*   dbpsub_init                                                            */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*    0: ����I��                                                           */
/*   -1: �ُ�I��                                                           */
/*                                                                          */
/* Description:                                                             */
/*   NTT IP DB P-Agent SUB ��������                                         */
/*   �f�^�v���e�[�u�����Q�Ƃ��邽�߁A                                     */
/*   �f�^�v���e�[�u���A�h���X�l���֐����Ă�                               */
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int dbpsub_init()
{
	printLog( DEBUGING, "%s:%d dbpsub_init() start", __FILE__, __LINE__ );	/* V01 */

	/* �f�^�v���e�[�u���A�h���X�l�� */
	gwinf_p = gw_getgwinfaddr();

	/* ����I�� */
	return 0;

} /* dcmsub_init */



/****************************************************************************/
/* Name:																	*/
/*   dbpsub_netx_gw                                                         */
/*																			*/
/* Parameters:																*/
/*   char	*data_p	: IN: NetExpert �f�[�^                                  */
/*   int	data_len: IN: NetExpert �f�[�^��                                */
/*																			*/
/* Return:																	*/
/*   NONE                                                                   */
/*                                                                          */
/* Description:																*/
/*   NetExpert -> G/W �f�[�^����                                            */
/*   NetExpert�f�[�^����͂��Ĉ�����擾���A���Y G/W �ɑ��M����B			*/
/*   �f�[�^�͉������H���Ȃ��ő��M����B										*/
/*   ����� G/W �͐����ς݂ł��邱�ƁB										*/
/*   ����� G/W ���Y�����Ȃ��ꍇ�́A�f�[�^��j������B						*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
void dbpsub_netx_gw( char *data_p, int data_len )
{
	GW_EXEINF	*gwexeinf_p = NULL;		/* G/W �ʏ��|�C���^         */
	GW_PAGIF	pagif;					/* P-Agent - G/W Prossecc I/F   */
	char		*cp;					/* ���[�N                       */
	int			write_len;				/* ���ۂ� write() ���� �f�[�^�� */
	int			cnt;					/* �J�E���^                     */
	int			i;						/* ���[�N                       */


	printLog( DEBUGING, "%s:%d dbpsub_netx_gw() start", __FILE__, __LINE__ );	/* V01 */
	printLog( DEBUGING, "data_p = %s, data_len = %d", data_p, data_len );	/* V01 */

	memset( &pagif, 0, sizeof(pagif) );

	/* �L�� G/W�ʏ��̂f�v���ʖ����父������� */
	for( cnt = 0 ; cnt < gwinf_p->exeinfsuu; cnt++ ){
		if( !strncmp(data_p,gwinf_p->exeinf[cnt].name,
									strlen(gwinf_p->exeinf[cnt].name)) ){
			/* G/W �ʏ��|�C���^�ݒ� */
			gwexeinf_p = &(gwinf_p->exeinf[cnt]);
			/* NetExpert ����̃��b�Z�[�W���父����������f�[�^�����o�� */
			cp = strchr( data_p, '_' );
			cp++;
			break;
		} 
	} /* for */

	if( gwexeinf_p == NULL ){
		for( cnt = 0 ; cnt < gwinf_p->exeinfsuu; cnt++ ){
			if( !strcmp(gwinf_p->exeinf[cnt].name, MASTER)){
				/* G/W �ʏ��|�C���^�ݒ� */
				gwexeinf_p = &(gwinf_p->exeinf[cnt]);
				cp = data_p;
				break;
			}
    	} /* for */
		if( gwexeinf_p == NULL ){
			printLog( DEBUGING, "%s:%d �Y������f�v������܂���", __FILE__, __LINE__ ); /* V01 */
			printLog( DEBUGING, "G/W���ʖ� : %.10s", data_p );  /* V01 */
			return;
		}
	}


	/* ���M�f�[�^���Z�b�g */
	printLog( DETAIL, "�R�s�[�O %s", cp );
	memcpy( pagif.data, cp, strlen(cp) );
/*
 	pagif.data[strlen(pagif.data)-1] = '\0';
*/
 	pagif.data[strlen(pagif.data)] = '\0';
	printLog( DETAIL, "�R�s�[�� %s", pagif.data );

	/* ���f�[�^�����Z�b�g */
	pagif.data_len = strlen( pagif.data );

	/* ���M�f�[�^�����Z�b�g */
	write_len = sizeof(pagif.data_len) + pagif.data_len;

	/* ���Y�f/�v�Ƀf�[�^�𑗐M���� */
	i = write( gwinf_p->exeinf[cnt].sd, &pagif, write_len );
	if( i != write_len ){
		/* �G���[���b�Z�[�W�\�� */
		printLog( ERROR, "%s:%d %d = write(%d,,%d ) errno:%d", __FILE__, __LINE__, i,
				gwinf_p->exeinf[cnt].sd, write_len, errno );	/* V01 */
		return;
	}

/********** �ύX �������� V01 **********/
	printLog( DEBUGING, "%s:%d ���Y�f/�v�Ƀf�[�^���M (%d byte)", __FILE__, __LINE__, i );
	printLog( DEBUGING, "G/W : %s", gwinf_p->exeinf[cnt].name );
	printLog( DEBUGING, "���f�[�^�� : %d, ���M�f�[�^�� : %d", pagif.data_len, write_len );
	printLog( DEBUGING, "���M�f�[�^ : %s", pagif.data );
/********** �ύX �����܂� V01 **********/

	return;

} /* dbpsub_netx_gw */



/****************************************************************************/
/* Name:																	*/
/*   dbpsub_gw_netx                                                         */
/*																			*/
/* Parameters:																*/
/*   char	*data_p		�FIN	G/W �f�[�^                                  */
/*   int	data_len	�FIN	G/W �f�[�^��                                */
/*																			*/
/* Return:																	*/
/*     0: G/W �f�[�^���M                                                    */
/*    -1: �ُ�I��(�v���I)                                                  */
/*   etc: NetExpert �f�[�^��                                                */
/*																			*/
/* Description:																*/
/*   G/W -> NetExpert �f�[�^����                                            */
/*   NetExpert �f�[�^�̏ꍇ�́AG/W �f�[�^���父���������                   */
/*   ���̃f�[�^����ԋp����B                                               */
/*   ����� G/W ���Y�����Ȃ��ꍇ�́A�f�[�^��j������B                      */
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
int dbpsub_gw_netx( char *data_p, int data_len )
{
/********** �ύX �������� V01 **********/
	printLog( DEBUGING, "%s:%d dbpsub_gw_netx() start", __FILE__, __LINE__ );
	printLog( DEBUGING, "data_p = %s", data_p );
	printLog( DEBUGING, "data_len = %d", data_len );
/********** �ύX �������� V01 **********/

	return data_len;

} /* dbpsub_gw_netx */



/****************************************************************************/
/* Name:																	*/
/*   dbpsub_term                                                            */
/*																			*/
/* Parameters:																*/
/*   NONE                                                                   */
/*																			*/
/* Return:																	*/
/*   NONE                                                                   */
/*																			*/
/* Description:																*/
/*   NTT IP DB P-Agent SUB  �I������                                        */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ���O�o�͂̕ύX                                */
/****************************************************************************/
void dbpsub_term()
{
	printLog( DEBUGING, "%s:%d dbpsub_term() start", __FILE__, __LINE__ );	/* V01 */
	return;
} /* dbpsub_term */

/* End of File */
