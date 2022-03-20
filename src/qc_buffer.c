#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>

#include "../include/qc_buffer.h"
#include "../include/error.h"

int buffer_init(qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
	memset(buff, 0, sizeof(qc_buffer_t));

	buff->start = buff->end = buff->last_pos = 0;
	buff->capacity = BUFF_CAPACITY;
	buff->data = malloc(sizeof(char) * buff->capacity);
	if(buff->data == NULL) return -EALLOC;
	memset(buff->data, 0, sizeof(char) * buff->capacity);

	return OP_SUCCESS;
}

int qc_buffer_append(qc_buffer_t * buff, char * from, size_t size){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");

	int err;
    if(size == 0)
		return -EINVAL;

	err = buffer_maybe_resize(buff, size);
	if(err < 0) return err;

	memcpy(buff->data + buff->end, from, size);
	buff->end += size;

	return buff->end;
}

int buffer_maybe_resize(qc_buffer_t * buff, int size){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
	assert((buff->end < buff->capacity) && "buffer size should never exceed capacity");

	size_t new_cap;
	int err;

	err = 0;
	if(size >= (buff->capacity - buff->end)){
		new_cap = ((buff->capacity * 2) - buff->end ) > size ? buff->capacity * 2 : size * 2;
		buff->data= realloc(buff->data, sizeof(char) * new_cap);
		if(buff->data == NULL) return -EALLOC;
		memset(buff->data + buff->end, 0, new_cap - buff->capacity);
		buff->capacity = new_cap;
		err = new_cap;
	}

	return err;
}

int buffer_recv(int fd, qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
	int nbytes, err, total;

	total = nbytes = 0;
	while(1){
		err = buffer_maybe_resize(buff, CHUNK_SIZE);
		if(err < 0) return err;
		assert(CHUNK_SIZE < (buff->capacity - buff->end));
		nbytes = recv(fd, buff->data + buff->end, CHUNK_SIZE, 0);
		if(nbytes > 0){
			total += nbytes;
			buff->end += nbytes;
		}else if(nbytes == 0){
			return nbytes;
		}else{
			if(errno != EAGAIN && errno != EWOULDBLOCK){
				return nbytes;
			}
			break; /*no data yet do nothing, return total so far*/
		}
	}
	return total;
}

int buffer_send(int fd, qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
	int total, nbytes, size;
	char * tmp_buff;
	if((buff->end <= 0) | (fd < 0))
		return -EINVAL;

	total = nbytes = 0;
	/*Are we dealing with a left out chunk or a complete buffer?*/
	tmp_buff = (buff->last_pos > 0) ? buff->data + buff->last_pos : buff->data;

	while(1){
		size = buff->end - total;
		if(size == 0){ /*size became zero, nothing else to send*/
			buff->last_pos = 0; /*the entire buffer was sent*/
			break;
		}

		nbytes = send(fd, tmp_buff + total, size, MSG_NOSIGNAL);
		if(nbytes > 0){
			total += nbytes;
		}else if(nbytes == -1){ /*kernel write buff ran out of room*/
			buff->last_pos = (errno == EAGAIN || errno == EWOULDBLOCK ) ? total : 0;
			return nbytes;
		}else{
			return nbytes;
		}
	}

	return total;
}

void qc_buffer_debug(const qc_buffer_t * buff, u_int8_t flag){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
    if( flag & DEBUG_RAW_BYTES ){
        for(int i = buff->start; i < buff->capacity; i++){
            printf("%d ",buff->data[i]);
        }
        printf("\n");
    }
    if( flag & DEBUG_STRING){
        printf("%s\n",buff->data);
    }
    if( flag & DEBUG_BUFFINFO){
        printf("(start = %ld), (end = %ld), (size = %ld) \n", buff->start, buff->end, buff->capacity);
    }
    
}
void qc_buffer_reset(qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
    memset(buff->data,0,buff->end);
    buff->start = 0;
    buff->end = 0;
}

int buffer_empty(const qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
	return buff->end == 0;
}

void qc_buffer_free(qc_buffer_t * buff){
    free(buff->data);
}
