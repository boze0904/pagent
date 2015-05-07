#ifndef PROC_IMPORT_DAR_INSERT
#define PROC_IMPORT_DAR_INSERT
EXEC SQL BEGIN DECLARE SECTION;
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
} PROC_IMPORT_DAR;
EXEC SQL END DECLARE SECTION;
#endif

/* End Of File */

