#ifndef ERROR_H
#define ERROR_H

#define THROW_ERROR(err)  do { \
	fprintf(stderr, "Error in %s:%d: %s.\n", __FILE__, __LINE__, errstr(err)); \
	exit(EXIT_FAILURE); } while(0)

#define LOG_ERROR(err) do { \
	fprintf(stderr, "Error: %s:%d: %s.\n", __FILE__, __LINE__, errstr(err)); \
} while(0)

void error_log(char * msg);
void error_exit(int ret, char * msg);
void error_malloc(void * mem);
void error_alert(int ret, char * msg);
const char * errstr(unsigned int err_code);

#endif 
