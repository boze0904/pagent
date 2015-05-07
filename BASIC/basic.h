/****************************************************************************/
/* file name  basic.h                                                      */
/*                                                                          */
/* DB Pagent G/W 用の共通ヘッダファイル                                     */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.02.2001: V01: ファイルの見出しを作成                        */
/*                            dbp.confの位置を追加しました                  */
/****************************************************************************/
#ifndef BASIC_H
#define BASIC_H

extern int build_insert(char*);
extern int build_update(char*);
extern int build_delete(char*);
extern int branch_insert(char*);
extern int branch_update(char*);
extern int branch_delete(char*);
extern int isp_contact_insert(char*);
extern int isp_contact_update(char*);
extern int isp_contact_delete(char*);
extern int isp_branch_insert(char*);
extern int isp_branch_update(char*);
extern int isp_branch_delete(char*);
extern int isp_nte_insert(char*);
extern int isp_nte_update(char*);
extern int isp_nte_delete(char*);
extern int medo_contact_insert(char*);
extern int medo_contact_update(char*);
extern int medo_contact_delete(char*);
extern int maker_contact_insert(char*);
extern int maker_contact_update(char*);
extern int maker_contact_delete(char*);
extern int eqp_insert(char*);
extern int eqp_update(char*);
extern int eqp_delete(char*);
extern int component_insert(char*);
extern int component_delete(char*);

#endif


