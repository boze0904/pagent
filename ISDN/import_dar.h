#ifndef IMPORT_DAR_INSERT
#define IMPORT_DAR_INSERT


#define H_IMPORT_INSERT "IMPORT_DAR_INSERT"
#define H_IMPORT_DELETE "IMPORT_DAR_DELETE"

typedef struct{
	char dar_no[11];
	char branch_c[9];
	char build_c[11];
	char unit_c[8];
	char circuit_no[4];
	char port_max[5];
	char dar_hope_day[9];
	char status_memo[101];
	char das_use[2];
	char dar_status[2];
} IMPORT_DAR;

extern int proc_import_dar_insert(IMPORT_DAR*);
extern int proc_import_dar_delete(char*);
extern int import_dar_insert(char*, char*);
extern int import_dar_delete(char*, char*);


#endif

/* End Of File */

