/****************************************************************************/
/* file name	:dbp_portsub.h												*/
/*																			*/
/*	NTT IP P-Agent G/W サブ処理用ヘッダ										*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */ 
/****************************************************************************/
#ifndef DBP_PORTSUB_H
#define DBP_PORTSUB_H

extern int		dbpsub_init();				/* 初期処理               */
extern void		dbpsub_netx_gw(char*,int);	/* NetExpert -> G/W       */
extern int		dbpsub_gw_netx(char*,int);	/* G/W       -> NetExpert */
extern void		dbpsub_term();				/* 終了処理               */


#endif

/* END OF FILE */
