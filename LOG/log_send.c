/****************************************************************/
/*	�t�@�C����:													*/
/*		log_send.c												*/
/*	�֐���:														*/
/*		log_send												*/
/*	�p�����[�^:													*/
/*		char *apl		: ���s�A�v���P�[�V������				*/
/*		char *file		: �f�B���N�g��/���O�t�@�C��				*/
/*		char *message	: ���b�Z�[�W							*/
/*	�߂�l:														*/
/*		0		: ����											*/
/*		0�ȊO	: ���s											*/
/*	�T�v:														*/
/*		���O�t�@�C���̏ꏊ�A�y�у��b�Z�[�W��������				*/
/*		�Z�b�g���A�R�[������									*/
/*		���O�t�H�[�}�b�g�͈ȉ��̂��̂Ƃ���						*/
/*		���t ���� �z�X�g�� ���s�A�v���P�[�V���� ���b�Z�[�W		*/
/*	�쐬��:														*/
/*		2001/02/14												*/
/*	�쐬��:														*/
/*		����													*/
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int log_send( char *file, int priority, char *apl, char *message ) {

	FILE		*fp;								/*** �t�@�C���|�C���^	*/
	char		host[64];							/*** �z�X�g�ϐ�			*/
	time_t		ct = time( NULL );					/*** TIME				*/
	struct 		tm *st = localtime( &ct );			/*** TIME				*/
	char		time_string[64];					/*** TIME				*/
	
	int			i = -1;								/*** ���[�N				*/
	
	/********************************************/
	/*	�t�@�C���̃I�[�v��						*/
	if( ( fp = fopen( file, "a" ) ) == NULL ) {
		return( -1 );
	}

	/********************************************/
	/*	�z�X�g���擾							*/
	memset( host, 0, sizeof( host ) );					/*** host�ϐ��N���A�[ */
	if( i = gethostname( host, 63 ) != 0 ) {
		return( -1 );	
	}

	/********************************************/
	/*	���t/���Ԃ��擾							*/
	memset( time_string, 0, sizeof( time_string ) );		/*** time_string�ϐ����N���A�[ */
	sprintf( time_string, "%004d-%02d-%02d %02d:%02d:%02d", 
							1900+st->tm_year, 1+st-> tm_mon, st->tm_mday,
							st->tm_hour, st->tm_min, st->tm_sec );

	/********************************************/
	/*	���O�Z�b�g								*/
	fprintf( fp, "%s %d %s %s : %s\n", time_string, priority, host, apl, message);

	fclose( fp );

	return 0;

}

/*** End Of File */

