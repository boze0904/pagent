/****************************************************************************/
/* file name  gw_if.h                                                       */
/*																			*/
/*   NTT IP DB P-Agent GW I/F関数用ヘッダ                                   */
/*																			*/
/* Create : 2000.12.15(DoCoMo gw_if.hの流用)                                */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef GW_IF_H
#define	GW_IF_H

#include <netinet/in.h>

/****************************************************************************/
/* 最大値																	*/
/****************************************************************************/
#define	GW_MXIFDTLEN	8192				/* GW - PAgent I/F データ最大長	*/

/****************************************************************************/
/* Gateway 制御情報															*/
/****************************************************************************/
#define	GW_MXFILELEN	1024				/* 最大ファイルパス名長	PATH_MAX*/
#define	GW_MXLINELEN	128					/* 行内最大文字列長				*/
#define	GW_MXHSTLEN		10					/* 最大ホスト名長				*/
#define	GW_MXSECLEN		16					/* 最大セクション名長			*/
#define	GW_MXNAMELEN	16					/* 最大ＧＷ識別名長				*/
#define	GW_MXGWSUU		255					/* 最大起動Ｇ／Ｗ数				*/

/* Ｇ／Ｗ情報テーブル														*/
typedef struct {
	char			exefile[GW_MXFILELEN+1];/* 起動ファイル					*/
	char			name[GW_MXNAMELEN+1];	/* ＧＷ識別名					*/
	int				pid;					/* プロセスＩＤ					*/
	int				sd;						/* ソケット記述子(GW)			*/
	char			arg5[GW_MXFILELEN+1];	/* 第５パラメータ				*/
	char			arg6[GW_MXFILELEN+1];	/* 第６パラメータ				*/
	char			arg7[GW_MXFILELEN+1];	/* 第７パラメータ				*/
	char			arg8[GW_MXFILELEN+1];	/* 第８パラメータ				*/
	char			arg9[GW_MXFILELEN+1];	/* 第９パラメータ			V0.3*/
	char			arg10[GW_MXFILELEN+1];	/* 第10パラメータ			V0.4*/
} GW_EXEINF;

typedef struct {
	char			env_path[GW_MXFILELEN+1];/* 環境ファイルパス			*/
	char			exe_dir[GW_MXFILELEN+1];/* 実行形式ファイルディレクトリ	*/
	char			pagenthost[GW_MXHSTLEN+1];/* P-Agent ホスト名			*/
	char			pagentport[5+1];		/* P-Agent ポート番号 5ケタ		*/
	int				pagentlsnt;				/* P-Agent Listen タイマー		*/
	int				sd;						/* ソケット記述子(P-Agent)		*/
	char			listen_flag;			/* LISTEN FLAG		1999.09.18	*/
#define				GW_INFLISTENOFF	0x00		/* LISTEN 未				*/
#define				GW_INFLISTENON	0x01		/* LISTEN 済				*/
	struct sockaddr_in	name;				/* ソケットアドレス名			*/
	int				exeinfsuu;				/* 有効 G/W個別情報数			*/
	GW_EXEINF		exeinf[GW_MXGWSUU];		/* G/W 個別情報					*/
} GW_INF;


/****************************************************************************/
/*  P-Agent Configuration File												*/
/****************************************************************************/
#define	GW_CNFPAGHST	"PAGENTHOST="	/* TAG名称：P-Agent ホスト名        */
#define	GW_CNFPAGPRT	"PAGENTPORT="	/* TAG名称：P-Agent ポートID        */
#define	GW_CNFPAGLTM	"PAGENTLTIM="	/* TAG名称：P-Agent LISTEN タイマー */
#define	GW_CNFPAGENV	"GW_ENVPATH="	/* TAG名称：P-Agent 環境ファイルパス*/
#define	GW_CNFGWEXED	"GW_EXEDIR="	/* TAG名称：G/W 実行形式ディレクトリ*/
#define	GW_CNFGW		"GW="			/* TAG名称：G/W 起動形式			*/


/****************************************************************************/
/*  P-Agent - G/W Prossecc I/F												*/
/****************************************************************************/
/*  P-Agent データ構造														*/
typedef struct {
	int				data_len;				/* データ長						*/
	char			data[GW_MXIFDTLEN];		/* データ						*/
} GW_PAGIF;

/*  宛先名(GW_PAGFOR_NETX 以外は同じ長さであること)							*/
#define	GW_PAGFOR_NOLEN	4					/* 宛先番号長					*/
#define	GW_PAGFOR_NETX	"NETEXPERT"			/* ＮｅｔＥｘｐｅｒｔ			*/
#define	GW_PAGFOR_OPE	"OPE_"				/* ＯＰＥ						*/
#define	GW_PAGFOR_SCPE	"SCPE"				/* ＳＣＰＥ						*/
#define	GW_PAGFOR_REC	"REC_"				/* ＲＥＣ						*/
#define	GW_PAGFOR_OCPE	"OCPE"				/* ＯＣＰＥ						*/
#define	GW_PAGFOR_OCSE	"OCSE"				/* ＯＣＳＥ					V0.5*/
#define	GW_PAGFOR_UDP	"UDP_"				/* ＵＤＰ						*/



extern int		gw_exec(char*);		/* Ｇ／Ｗプロセス起動             */
extern void		gw_kill();			/* Ｇ／Ｗプロセス終了             */
extern void		gw_fdset(fd_set*);	/* Ｇ／Ｗソケット記述子設定       */
extern int		*gw_getvalidfd(fd_set*,int*);	/* G/Wソケット有効記述子獲得 */
extern GW_INF	*gw_getgwinfaddr();	/* G/W情報テーブルアドレス獲得 */



#endif

/* END OF FILE */
