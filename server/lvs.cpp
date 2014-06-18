/*
 * This sample code shows how to use Libevent to read from a named pipe.
 * XXX This code could make better use of the Libevent interfaces.
 *
 * XXX This does not work on Windows; ignore everything inside the _WIN32 block.
 *
 * On UNIX, compile with:
 * cc -I/usr/local/include -o event-read-fifo event-read-fifo.c \
 *     -L/usr/local/lib -levent
 */

#include <event2/event-config.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event2/event.h>

struct event *evfifo;

static void
fifo_read(evutil_socket_t fd, short event, void* arg)
{
	char buf[255];
	int len;
	struct event *ev = evfifo;

	fprintf(stderr, "fifo_read called with fd: %d, event: %d, arg: %p\n",
	    (int)fd, event, ev);
	len = read(fd, buf, sizeof(buf) - 1);

	if (len <= 0) {
		if (len == -1)
			perror("read");
		else if (len == 0)
			fprintf(stderr, "Connection closed %d\n", errno);
		//event_del(ev);
		//event_base_loopbreak(event_get_base(ev));
		return;
	}

	buf[len] = '\0';

	fprintf(stdout, "Read: %s\n", buf);
    //////
    //char writeBuffer[32];
    //memset(writeBuffer, 0, sizeof(char)*32);
    //sprintf(writeBuffer, "tutu -that's ok");
    //write(fd,writeBuffer, 32);
}

static void
signal_cb(evutil_socket_t fd, short event, void *arg)
{
    printf("caught SIGINT\n");
	struct event_base *base = static_cast<event_base*>(arg);
	event_base_loopbreak(base);
}

int
main(int argc, char **argv)
{
	struct event_base* base;
	struct event *signal_int;
	struct stat st;
	const char *fifo = "/tmp/led/led_pipe";
	const char *fifoDir = "/tmp/led";
    int socket;

    int status = mkdir(fifoDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status != 0  && errno != EEXIST) {
        printf("return mkdir %d\n", status);
        exit(1);
    }

   	if (lstat(fifo, &st) == 0) {
		if ((st.st_mode & S_IFMT) == S_IFREG) {
			errno = EEXIST;
			perror("lstat");
			exit(1);
		}
	}

	unlink(fifo);
	if (mkfifo(fifo, 0600) == -1) {
		perror("mkfifo");
		exit(1);
	}

	socket = open(fifo, O_RDWR | O_NONBLOCK, 0);

	if (socket == -1) {
		perror("open");
		exit(1);
	}

	fprintf(stderr, "Write data to %s\n", fifo);
	
/* Initalize the event library */
	base = event_base_new();

	/* Initalize one event */
	/* catch SIGINT so that event.fifo can be cleaned up */
	signal_int = evsignal_new(base, SIGINT, signal_cb, base);
	event_add(signal_int, NULL);

	evfifo = event_new(base, socket, EV_READ|EV_PERSIST, fifo_read,
                           NULL);

	/* Add it to the active events, without a timeout */
	event_add(evfifo, NULL);

	event_base_dispatch(base);

	event_base_free(base);

	close(socket);
	unlink(fifo);

//	libevent_global_shutdown();
	return (0);
}


