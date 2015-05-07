/****************************************************************************/
/* file nanme  IsdnFindFreePortNumber.c                                     */
/*                                                                          */
/*  �v�`DB�A�g                                                              */
/*  DAS�`���l���󂫏�񌟍�                                                 */
/*                                                                          */
/* Function:                                                                */
/*   createSendData():                                                      */
/*   findFreePortNumber():                                                  */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include "output_log.h"
#include "IsdnFindFreePortNumber.h"
#include "ProcFindFreePortNumber.h"


/*** �p�����[�^�� ***/
#define		BRANCH_CD	8	/* �x�X�R�[�h     */
#define		BUILD_CD	10	/* �r���R�[�h     */
#define		UNIT_CD		7	/* ���j�b�g�R�[�h */



/****************************************************************************/
/* Name : createSendData                                                    */
/*                                                                          */
/* Parameter : struct PORT_INFO port     :  IN: �|�[�g�����                */
/*             char*            send_data: OUT: ���M�f�[�^                  */
/*                                                                          */
/* Return : 0  ����I��                                                     */
/*         -1  �ُ�I��                                                     */
/*                                                                          */
/* Description : NetExpert�֑��郁�b�Z�[�W�̍쐬                            */
/*               ���b�Z�[�W�w�b�_�������ŕt������                           */
/*               ���b�Z�[�W�`���͈ȉ��Ƃ���                                 */
/*               INS_PORT_INFO,�󂫃|�[�g��,�^�p��������,�H�����|�[�g��,    */
/*                             �H����������,�p�~�|�[�g                      */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int createSendData( struct PORT_INFO port, char* send_data )
{
	printLog( DEBUGING, "*** createSendData() ***" );

	sprintf( send_data, "%s,%d,%d,%d,%d,%d", SEND_HEAD, 
											port.unused_port, port.all_free_port, 
											port.work_port, port.all_work_port, 
											port.wait_port );

	printLog( DEBUGING, "create message %s", send_data );
	printLog( DEBUGING, "create message length %d", strlen(send_data) );

	return strlen(send_data);

} /* createSendData() */


/****************************************************************************/
/* Name : findFreePortNumber                                                */
/*                                                                          */
/* Parameters: char* recv_data:  IN: ��M�f�[�^                             */
/*             char* send_data: OUT: ���M�f�[�^                             */
/*                                                                          */
/* Return    : int: ���M�f�[�^��                                            */
/*                                                                          */
/* Description : ��M�f�[�^����x�X�R�[�h�A�r���R�[�h�A���j�b�g�R�[�h       */
/*               ��؂�o��                                                 */
/*               �󂫃|�[�g���A�H�����|�[�g���A�p�~�|�[�g�����擾���A       */
/*               NetExpert�֑��M���郁�b�Z�[�W���쐬����                    */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int findFreePortNumber( char* recv_data, char* send_data )
{
	int					ret;
	int					data_len;
	char				*work_p, *data;
	char				branch_cd[BRANCH_CD+1];
	char				build_cd[BUILD_CD+1];
	char				unit_cd[UNIT_CD+1];
	struct PORT_INFO	port_info;


	printLog( DEBUGING, "*** findFreePortNumber() ***" );

	data = strchr( recv_data, ',' );
	if( data == NULL ){
		/* ��M�f�[�^���w�b�_�̂� */
		printLog( ERROR, "��M�f�[�^���w�b�_�̂�" );
		return -1;
	}
	data++;

	/*** �O�̂��ߏ����� ***/
	memset( branch_cd, 0, BRANCH_CD+1 );
	memset( build_cd,  0, BUILD_CD +1 );
	memset( unit_cd,   0, UNIT_CD  +1 );

	/*** �x�X�R�[�h ***/
	work_p = strtok( data, "," );
	if( work_p == NULL ){
		printLog( ERROR, "�x�X�R�[�h������܂���" );
		return -1;
	}
	strncpy( branch_cd, work_p, BRANCH_CD );

	/*** �r���R�[�h ***/
	work_p = strtok( NULL, "," );
	if( work_p == NULL ){
		printLog( ERROR, "�r���R�[�h������܂���" );
		return -1;
	}
	strncpy( build_cd, work_p, BUILD_CD );

	/*** ���j�b�g�R�[�h ***/
	work_p = strtok( NULL, "," );
	if( work_p != NULL ){
		strncpy( unit_cd, work_p, UNIT_CD );
	}

	printLog( DEBUGING, "�x�X�R�[�h    : %s", branch_cd );
	printLog( DEBUGING, "�r���R�[�h    : %s", build_cd );
	printLog( DEBUGING, "���j�b�g�R�[�h: %s", unit_cd );

	/*** �󂫃|�[�g���̌��� ***/
	ret = getFreePort( branch_cd, build_cd, unit_cd, &port_info );
	if( ret ){
		return -1;
	}

	/*** �H�����|�[�g���̌��� ***/
	ret = getWorkPort( branch_cd, build_cd, unit_cd, &port_info );
	if( ret ){
		return -1;
	}

	/*** �p�~�|�[�g���̌��� ***/
	ret = getWaitPort( branch_cd, build_cd, unit_cd, &port_info );
	if( ret ){
		return -1;
	}

	/*** ���M�f�[�^�̍쐬 ***/
	data_len = createSendData( port_info, send_data );

	return data_len;

} /* findFreePortNumber() */

/* End of File */

