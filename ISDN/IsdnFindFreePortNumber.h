/****************************************************************************/
/* file name  IsdnFindFreePortNumber.h                                      */
/*                                                                          */
/* ISDN G/W IsdnFindFreePortNumber.c 用ヘッダファイル                       */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef ISDN_FIND_FREE_PORT_NUMBER_H
#define ISDN_FIND_FREE_PORT_NUMBER_H

/*** 受信データのヘッダ ***/
#define	FIND_FREE_PORT_NUMBER	"FindFreePortNumber"

/*** 送信データのヘッダ ***/
#define	    SEND_HEAD	"INS_PORT_INFO"

extern findFreePortNumber( char*, char* );

#endif

/* END OF FILE */
