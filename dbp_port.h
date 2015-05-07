/****************************************************************************/
/* file name  dbp_port.h                                                    */
/*                                                                          */
/*	NTT IP DB P-Agent 用ヘッダ                                              */
/*																			*/
/* Create : 2000.12.15(dcm_port.hから流用)                                  */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef DBP_PORT_H
#define DBP_PORT_H

#define	DBP_PAGENT_VER	"NTT IP P-Agent V1.0 2000.12.15" /* P-Agent Version	*/

/****************************************************************************/
/* ＳＧファイル情報															*/
/****************************************************************************/
#define	DBP_ENVPATH		"DBP_ENV_PATH"	/* 環境変数：ファイルパス			*/
										/* コンフィグファイル				*/
										/* 端末設定ファイル					*/

/****************************************************************************/
/* 制限値																	*/
/****************************************************************************/
#define	DCM_MXFILELEN		1024 		/* 最大ファイルパス名長 PATH_MAX	*/

#define	DCM_MXCNFLEN		16			/* 最大コンフィグファイル名長		*/
#define	DCM_MXHSTLEN		10			/* 最大ホスト名長					*/

#define	DCM_MXSNDLEN		4096		/* 最大送信メッセージ長				*/
#define	DCM_MXMSGLEN		20600		/* 最大メッセージ長(for NETXP MIN)	*/
#define	DCM_MXTRCLEN		DCM_MXMSGLEN/* 最大トレースデータ長				*/
#define	DCM_MXEOLLEN		1			/* 最大ＥＯＬ長						*/
#define	DCM_EOLCODE			012			/* ＥＯＬコード（現在固定）			*/


/****************************************************************************/
/*	P-Agent 制御コマンド													*/
/****************************************************************************/
#define	DCM_CMD_TRACE_ON	"TRACE-ON"		/* Trace開始要求コマンド		*/
#define	DCM_CMD_TRACE_OFF	"TRACE-OFF"		/* Trace停止要求コマンド		*/
#define DBP_TERM			"TERM"			/* P-Agent終了コマンド          */


/****************************************************************************/
/*	P-Agent メッセージ														*/
/****************************************************************************/
#define	DCM_TRACE_SFC_MIN	"<--"			/* MINメッセージサフィックス	*/
#define	DCM_TRACE_SFC_MOUT	"-->"			/* MOUTメッセージサフィックス	*/

#define	DCM_MSG_HEADER		"NetExpert  "	/* メッセージヘッダ				*/
#define	DCM_MSG_COMMANDERR	"Command Error"	/* 制御コマンド不正				*/


/****************************************************************************/
/*	トレース																*/
/****************************************************************************/
/* 開始／終了時のフォーマット											*/
#define	DCM_TRACE_ON_FORM	"TRACE_ON ***********************************************************************\n"
#define	DCM_TRACE_OFF_FORM 	"TRACE_OFF***********************************************************************\n"

#define	DCM_TRACE_DATA_TERM	"\377"			/* データ境界（データ終端）		*/
#define	DCM_TRACE_FILE_TERM	"\0"			/* データ境界（ファイル出力）	*/


/****************************************************************************/
/* P-Agent 制御情報															*/
/****************************************************************************/
typedef struct {
	char	eol[DCM_MXEOLLEN+1];		/* 出力メッセージ用ＥＯＬ			*/
	int		eol_len;					/* 出力メッセージ用ＥＯＬ長			*/
	char	msgdata[DCM_MXMSGLEN+sizeof(DCM_TRACE_DATA_TERM)];
										/* メッセージバッファ（作業領域）	*/
	int		msgdisc_len;				/* メッセージ識別長					*/
	char	hostname[DCM_MXHSTLEN+1];	/* 自ホスト名						*/
	char	configfile[DCM_MXFILELEN+1];/* コンフィグファイルフルパス名		*/
	char	tmpfifo[DCM_MXFILELEN+1];	/* TEMP FIFO ファイルパス名			*/
	int		tmpfifo_fd;					/* TEMP FIFO ファイル記述子	0:無効	*/
} DBP_INF;

#endif

/* END OF FILE */
