#ifndef ERROR_H
#define ERROR_H

void error_log(char * msg);
void error_exit(int ret, char * msg);
void error_malloc(void * mem);
void error_alert(int ret, char * msg);


#endif 
