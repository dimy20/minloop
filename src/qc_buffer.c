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

void qc_buffer_free(qc_buffer_t * buff){
    free(buff->buff);
    buff->buff = NULL;
    buff = NULL;
}

int qc_buffer_resize(qc_buffer_t * buff, size_t size){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");

	buff->buff = realloc(buff->buff, size);
	/*Failed to allocte*/
	if(buff->buff == NULL && size != 0)
		return -EALLOC;

    buff->size += size;
    return buff->size;
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

int qc_buffer_recv(int fd, qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
    int nbytes, ret, is_blocking, data_available;
    size_t total = 0;
    char aux[DEFAULT_BUFF_SIZE];
    size_t size = DEFAULT_BUFF_SIZE;
    memset(aux,0,sizeof(aux));
    ret = fcntl(fd,F_GETFL,0);
    is_blocking = !(ret & SOCK_NONBLOCK);

    data_available = 1;
    nbytes = 1;
    /*The fd socket is set to be non-blockig.
     * We need to check for the return value of recv to make the propper adjustments,
     * if -1 is returned this means no data is available yet, we can break out of the
     * loop yet because we will not get notified again when data is available to read
     * (non-blocking), we only break out of the loop when no more data is available,
     * that is retur value 0.
     * When we implement a multiplexing function here, we wont need to do this,
     * given than will suscribed this fd to be monitored for reading and will get
     * nofified by the kernel when data is available, them will now for sure data is 
     * available and we can read it all, still in a non-blocking manner.
     * Until that is put in place, will use this solution.
     * A way to avoid using epoll (no as efficient )would be to specify on the very first bytes(header),
     * how much data we should expect to receive!(request size), so all we need to do
     * is keep track of how much data we have received yet until it matches the specified
     * request size!!.(This probably will stil have to be done when we add epoll to this.)
     * */

    while(data_available){
        nbytes = recv(fd,aux + total, size - total,0);
        if(nbytes > 0){
            total+=nbytes;
            qc_buffer_append(buff,aux,nbytes);
            /*Warning!!*/
            if(is_blocking)
                break;
        }else if(nbytes == -1){
            /*break when EAGAIN*/
            if(errno != EAGAIN && errno != EWOULDBLOCK){
                perror("recv()");
            }
            data_available = 0;
            //break;
        }else{
            /*connection is closing!!!*/
            data_available = 0;
            break;
        }
    }
    /*nbytes will be 0 when the other end drops the connection or when EAGAIN*/
    return total;
}

int buffer_send(int fd, qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
	int total, nbytes, size;
	if((buff->end <= 0) | (fd < 0))
		return -EINVAL;

	/*Are we dealing with a left out chunk or a complete buffer?*/
	char * tmp_buff;
	if(buff->last_pos > 0)
		tmp_buff = buff->buff + buff->last_pos;
	else
		tmp_buff = buff->buff;

	total = nbytes = 0;
	while(1){
		size = buff->end - total;
		nbytes = send(fd, tmp_buff + total, size, MSG_NOSIGNAL);
		if(nbytes > 0){
			total += nbytes;
		}else if(nbytes == -1){
			if(nbytes != EAGAIN && nbytes != EWOULDBLOCK) 
				perror("send");
			buff->last_pos = total;
			break; /*kernel write buff ran out of room*/
		}else{
			buff->last_pos = 0; /*the entire buffer was sent*/
			break; /*size became zero, nothing else to send*/
		}
	}
	return nbytes == 0 ? total : nbytes;
}

void qc_buffer_debug(const qc_buffer_t * buff, u_int8_t flag){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
    if( flag & DEBUG_RAW_BYTES ){
        for(int i = buff->start; i < buff->size; i++){
            printf("%d ",buff->buff[i]);
        }
        printf("\n");
    }
    if( flag & DEBUG_STRING){
        printf("%s\n",buff->buff);
    }
    if( flag & DEBUG_BUFFINFO){
        printf("(start = %ld), (end = %ld), (size = %ld) \n", buff->start, buff->end, buff->size);
    }
    
}
void qc_buffer_reset(qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
    memset(buff->buff,0,buff->end);
    buff->start = 0;
    buff->end = 0;
}

int buffer_empty(const qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
	return buff->end == 0;
}
