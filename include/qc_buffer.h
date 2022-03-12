#ifndef QC_BUFFER_H
#define QC_BUFFER_H
#include <stdlib.h>

#define DEBUG_RAW_BYTES 0x01
#define DEBUG_STRING 0x02
#define DEBUG_BUFFINFO 0x04

#define DEFAULT_BUFF_SIZE 264

typedef struct qc_buffer {
	char * buff;
	size_t start;
	size_t end;
	size_t size;
	size_t last_pos; /*Keep track of last state if send returned EAGAIN*/
} qc_buffer_t;


int qc_buffer_init(qc_buffer_t * buff, size_t size);
void qc_buffer_free(qc_buffer_t * buff);
int  qc_buffer_resize(qc_buffer_t * buff, size_t size); /*should this be exposed?*/
int  qc_buffer_append(qc_buffer_t * buff, char * from , size_t size);
void qc_buffer_debug(const qc_buffer_t * buff, u_int8_t flag);

void qc_buffer_reset(qc_buffer_t * buff);
int qc_buffer_recv(int fd, qc_buffer_t * recv_buff);

int buffer_send(int fd, qc_buffer_t * buff);
int buffer_empty(const qc_buffer_t * buff);
#endif
