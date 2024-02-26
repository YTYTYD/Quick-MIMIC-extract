#include <stdarg.h>
#include "defines.h"
#include "sInfoExtract.h"
#include "taskInit.h"
#include "featureExtract.h"
#include "noteExtract.h"

extern const int CSV_STR_MAX_LEN;

void m_strcpy(char **output_buffer, const char *src, int src_len);
void m_sprintf(char **buffer, const char *fmt, ...);
void table_head_output(struct task e_task, char **output_buffer);
void static_info_output(struct task e_task, struct StaticInformation sInfor, char **output_buffer, int iculos);
void extract(int HADM_ID, int SUBJECT_ID, struct task e_task, char **ext_str_result);
