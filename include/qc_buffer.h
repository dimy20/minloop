#ifndef QC_BUFFER_H
#define QC_BUFFER_H
#include <stdlib.h>

#define DEBUG_RAW_BYTES 0x01
#define DEBUG_STRING 0x02
#define DEBUG_BUFFINFO 0x04

#define BUFF_CAPACITY 32768  /*32kb*/
#define CHUNK_SIZE 1024

/*Data container to keep data received and data to send*/
typedef struct qc_buffer {
	char * data;
	size_t start;
	size_t end;
	size_t capacity;
	size_t last_pos; /*Keep track of last state if send returned EAGAIN*/
} qc_buffer_t;


void qc_buffer_free(qc_buffer_t * buff);
int  qc_buffer_append(qc_buffer_t * buff, char * from , size_t size);
void qc_buffer_debug(const qc_buffer_t * buff, u_int8_t flag);

void qc_buffer_reset(qc_buffer_t * buff);

int buffer_init(qc_buffer_t * buff);
int buffer_send(int fd, qc_buffer_t * buff);
int buffer_empty(const qc_buffer_t * buff);
int buffer_maybe_resize(qc_buffer_t * buff, int size);
int buffer_recv(int fd, qc_buffer_t* buff);
#endif
