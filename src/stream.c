#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "../include/stream.h"
#include "../include/net.h"
#include "../include/core.h"
#include "../include/errno.h"
#include "../include/error.h"
#include "../include/qc_buffer.h"

struct stream_s{
	io_core_t io_ctl;
	qc_buffer_t bufs[2];
	connection_cb on_connection;
	data_cb on_data;
	data_cb on_close;
	data_cb on_error;
	int accepted_fd;
};

static void _io_activity_cb(loop_t * loop, io_core_t * ioc, uint8_t status){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(ioc != NULL && "io_core_t pointer is NULL");
	int err, total;
	stream_t * stream;
	stream = container_of(ioc, stream_t, io_ctl);



	if((status == EPOLLIN) && (stream != NULL)){
		err = buffer_recv(stream->io_ctl.fd, &stream->bufs[IN_BUFF]);
		if(err > 0 && stream->on_data != NULL){
			stream->on_data(stream, err);
		}else if(err == 0 && stream->on_close != NULL){
			stream->on_close(stream, 0);
		}else if(stream->on_error != NULL){
			stream->on_error(stream, err);
		}
	}
	/*Now there is space available again, send what was left out.*/
	if(status == EPOLLOUT){
		err = buffer_send(ioc->fd, &stream->bufs[OUT_BUFF]);
		if(err > 0){
			ioctl(&stream->io_ctl, IOCTL_DEL, EPOLLOUT);
			nepoll_ctl(loop->efd, EPOLL_CTL_MOD, ioc->fd, ioc->events);
			qc_buffer_reset(&stream->bufs[OUT_BUFF]);
		}
	}

}

static void server_cb(loop_t * loop, io_core_t * ioc, uint8_t status){
	assert(ioc != NULL && "io_core_t pointer is NULL");
	stream_t * server;
	int peer_fd;

	server = container_of(ioc, stream_t, io_ctl);

	peer_fd = _io_accept(&server->io_ctl);
	if(peer_fd < 0) return;

	server->accepted_fd = peer_fd;
	server->on_connection(server);
}

int stream_init(loop_t * loop, stream_t * stream){
	assert(stream != NULL && "stream pointer is NULL");
	memset(stream, 0, sizeof(stream_t));
	int err;
    uint32_t events;
	/*_io_activity_cb is the default callback for io_cores but it can be modified
	 * if the io_core is used for other types of sockets, for exmaple if it is 
	 * supposed to accept connections it can be changed to point to server_cb*/
	events = EPOLLIN | EPOLLET;
	io_core_init(&stream->io_ctl, IO_OFF, events, _io_activity_cb);

	/*init io buffers*/
	for(int i = 0; i < 2; i++){
		err = buffer_init(&stream->bufs[i]);
		if(err < 0) return err;
	}
		
	stream->accepted_fd = -1;
	stream->on_connection = NULL;
	stream->on_data = NULL;
	stream->on_close = NULL;
	stream->on_error = NULL;

	err = loop_start_io(loop, &stream->io_ctl);
	if(err < 0) return err;

	return OP_SUCCESS;
}



int stream_server(stream_t * server, char * hostname, char * port){
	assert(server!= NULL && "stream_t pointer is NULL");

	int err;

	server->io_ctl.cb = server_cb;

	/*what should the loop do with this io when this happens?*/
	if(server->io_ctl.fd != IO_OFF)
		return -EIO_BUSY;

	err = ntcp_server(hostname, port);
	if(err < 0) return err;

	server->io_ctl.fd = err;
	return OP_SUCCESS;
}

int stream_listen(stream_t * server, connection_cb on_connection){
	assert(server != NULL && "stream_t pointer is NULL");
	int err;

	if(on_connection == NULL)
		return -EINVAL;

	server->on_connection = on_connection;

	err = ntcp_listen(server->io_ctl.fd, 10);
	if(err < 0){
		perror("ntcp_listen");
		return err;
	}

	server->io_ctl.status |= STS_LISTEN;

	return OP_SUCCESS;
}

int stream_accept(const stream_t * server , stream_t * peer){
	assert(server != NULL && "stream_t pointer is NULL");
	assert(peer != NULL && "stream_t pointer is NULL");


	if((server->io_ctl.status & STS_LISTEN) != STS_LISTEN)
		return -EIO_ACCEPT_LISTEN;


	if(server->accepted_fd == -1)
		return -EIO_ACCEPT_LISTEN;

	nsock_nonblock(server->accepted_fd);
	peer->io_ctl.fd = server->accepted_fd;

	return 0;
}

stream_t * stream_new(loop_t * loop){
	assert(loop != NULL && "loop_t pointer is NULL");
	stream_t * new_stream;
	int err;

	new_stream = malloc(sizeof(stream_t));
	if(new_stream == NULL){
		LOG_ERROR(EALLOC);
		return NULL;
	}

	memset(new_stream, 0, sizeof(stream_t));
	err = stream_init(loop, new_stream);
	if(err < 0) LOG_ERROR(err);
		
	return new_stream;
}

void stream_free(stream_t * stream){
	qc_buffer_free(&stream->bufs[IN_BUFF]);
	qc_buffer_free(&stream->bufs[OUT_BUFF]);
	free(stream);
}

int stream_read(stream_t * stream, char * buff, int size){
	assert(stream != NULL && "stream_t pointer is NULL");
	assert(stream->bufs[IN_BUFF].start <= stream->bufs[IN_BUFF].end);
	qc_buffer_t * in_buff;

	if(buffer_empty(&stream->bufs[IN_BUFF]))
		return 0;

	in_buff = &stream->bufs[IN_BUFF];
	size = (size > (in_buff->end - in_buff->start)) ? in_buff->end - in_buff->start : size;
	memcpy(buff, in_buff->data + in_buff->start, size);
	in_buff->start += size;	
	return size;
}

int stream_write(loop_t * loop, stream_t * stream, char * buff, size_t size){
	assert(stream != NULL && "stream_t pointer is NULL");
	int err;
	err = qc_buffer_append(&stream->bufs[OUT_BUFF], buff, size);
	queue_insert(loop->write_q, &stream->io_ctl);
	if(err < 0) return err;
	return err;
}

int stream_send_ready(const stream_t * stream){
	assert(stream != NULL && "stream_t pointer is NULL");
	return !buffer_empty(&stream->bufs[OUT_BUFF]);
}

int stream_close(loop_t * loop, stream_t * stream){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(stream != NULL && "stream_t pointer is NULL");

	if(loop->io_watchers.arr[stream->io_ctl.fd] != &stream->io_ctl)
		return -EIO_BAD_IO;

	vector_remove(&loop->io_watchers, stream->io_ctl.fd);
	close(stream->io_ctl.fd);
	stream_free(stream);
	return OP_SUCCESS;
}

void stream_on_event(stream_t * stream, int ev, data_cb cb){
	assert(stream != NULL && "stream_t pointer is NULL");
	if(ev == EV_READ)
		stream->on_data = cb;
	if(ev == EV_CLOSE)
		stream->on_close = cb;
	if(ev == EV_ERROR)
		stream->on_error = cb;
}

int stream_has_data(const stream_t * stream){
	assert(stream != NULL && "stream_t pointer is NULL");
	return stream->bufs[IN_BUFF].start < stream->bufs[IN_BUFF].end;
}
