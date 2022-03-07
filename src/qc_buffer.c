#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>

#include "../include/qc_buffer.h"
#include "../include/core.h"

int qc_buffer_init(qc_buffer_t * buff, size_t size){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
    memset(buff,0,sizeof(qc_buffer_t));

    buff->buff = malloc(sizeof(char) * size);
	/*failed to allocate*/
	if(buff->buff == NULL && size != 0)
		return -EALLOC;

	/*initialize this*/
    memset(buff->buff,0,sizeof(char) * size); 

    buff->start = 0;
    buff->end = 0;
    buff->size = size;

	return OP_SUCCESS;
}

void qc_buffer_free(qc_buffer_t * buff){
    free(buff->buff);
    buff->buff = NULL;
    buff = NULL;
}

int  qc_buffer_resize(qc_buffer_t * buff, size_t size){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
    buff->size+=size;
    if((buff->buff = realloc(buff->buff,buff->size)) == NULL){
        perror("realloc()");
    }

    #ifdef DEBUG
        printf("resizing buffer by %ld\n",size);
    #endif
    return 0;
}

int qc_buffer_append(qc_buffer_t * buff, char * from, size_t size){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
    int res = -1;
    if(size > 0){
        if((buff->end << 1) > buff->size){
            // resize by two times current end of buff
            qc_buffer_resize(buff,buff->end << 1);
        }
        if((buff->end + size) > buff->size){
            // resize by extra space needed
            qc_buffer_resize(buff,size);
        }
        memcpy(buff->buff + buff->end ,from,size);
        //strcpy(buff->buff + buff->end ,from);
        buff->end+=size;

        /*Since we could have resized, initialize all bytes to 0 after the end
         * of the buff.*/
        memset(buff->buff + buff->end,0,buff->size - buff->end); 
        res = buff->size;
    }
    return res;
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
            /*printf("is blocking: %d\n", is_blocking);*/
            /*connection is closing!!!*/
            data_available = 0;
            break;
        }
    }
    /*nbytes will be 0 when the other end drops the connection or when EAGAIN*/
    return total;
}

/*take a look a this function*/
int qc_buffer_send(int fd, qc_buffer_t * buff){
	assert(buff != NULL && "qc_buffer_t pointer is NULL");
    int res = -1;
    if(buff->end > 0 && fd > 0){
        int total = 0;
        int nbytes = 0;
        while((nbytes = send(fd,buff->buff + total, buff->end- total, MSG_NOSIGNAL)) > 0){
            total +=nbytes;
        };

        res = nbytes == -1 ? nbytes : total;
        if(nbytes == -1){
            if(errno != EAGAIN && errno!=EWOULDBLOCK){
                perror("send()");
            }
        }
        /*resets buffer's allocated memory, this could be a free but, if the client stays connected
         *unnecessary reallocs will be called again despite havin already allocated enough memory for this
         client, specially if it has stored big messages priviously*/
        qc_buffer_reset(buff);
    }
    return res;
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

