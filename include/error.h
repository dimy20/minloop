#ifndef ERROR_H
#define ERROR_H

#define THROW_ERROR(err)  do { \
	fprintf(stderr, "Error in %s:%d: %s.\n", __FILE__, __LINE__, errstr(err)); \
	exit(EXIT_FAILURE); } while(0)

#define LOG_ERROR(err) do { \
	fprintf(stderr, "Error: %s:%d: %s.\n", __FILE__, __LINE__, errstr(err)); \
} while(0)


/*Let 1 be reserved for errno errors*/

/*Error codes*/
#define OP_SUCCESS        0x01 /*All good*/
#define EIO_ACCEPT        0x02 /*Failed to accept*/
#define EIO_START         0x04 /*failed to insert new io in the loop*/
#define EIO_LOOP_WATCH    0x08 /*Loop failed to start watching io*/
#define EALLOC            0x10 /*Memort allocation error*/
#define EIO_ACCEPT_LISTEN 0x20 /*Bad fd*/
#define EIO_INVAL_EV      0x40 /*invalid event passed to ioctl*/
#define EIO_BUSY          0x80 /*Operation attempt on busy io, fd is in use*/
#define EIO_BAD_IO        9 
#define E_TIMER_RUNNING   10
/*Status codes*/
#define IO_OFF -1 /*This means not descriptor has been assigned yet.*/

#define EIO_START_STR "Failed to insert new io in the loop"
#define EIO_BUSY_STR "Operation on active io"
#define EIO_LOOP_WATCH_STR "Filed to start watching io"
#define EIO_ACCEPT_STR "Failed to accept peer connection"
#define EIO_ACCEPT_LISTEN_STR "Attempt to accept on non-listening stream"
#define EIO_INVAL_EV_STR "Invalid event passed to ioctl"
#define EIO_BAD_IO_STR "Bad io_core access in the loop"
#define E_TIMER_RUNNING_STR "start operation on active timer"

void error_log(char * msg);
void error_exit(int ret, char * msg);
void error_malloc(void * mem);
void error_alert(int ret, char * msg);
const char * errstr(int err_code);
int catch_error(int err);


#endif 
