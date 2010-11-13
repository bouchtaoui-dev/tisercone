/**
 * Author: El Andaluz
 * Date: 29/07/2010
 * Description:
 * This is a core network service template,
 * which could be a base for a network service app.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <assert.h>
#include <signal.h>
#include <getopt.h>

#include <servercore/io_interface.h>
#include <servercore/io_process.h>
#include <servercore/log_writer.h>
#include <servercore/select_timer.h>


static char* setting_file = NULL;
static char* debug_file = NULL;
static int32_t portnr = 4321;

static void print_usage()
{
	printf("Usage: ServerCore\n");
}

static void print_help()
{
	printf("  Some useful help...\n");
	printf("  -h --help this text\n");
	printf("  -c --config-file filename \"The config file to use\".\n");
	printf("  -d --debug-file filename \"The debug file to use\".\n");
	printf("  -p --port portnumber");
	printf("  -i --iface interface-card \"eth0, eth1...\".");
}


static void handle_options(int argc, char** argv)
{
	while (1) {
		int option_index = 0, c;
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"version", 1, 0, 'v'},
			{"port", 1, 0, 'p'},
			{"config-file", 1, 0, 'c'},
			{"debug-file", 1, 0, 'd'},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "c:hd:p:",
						long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
			case 'h':
				print_usage();
				print_help();
				exit(0);
                break;
            case 'd':
                debug_file = strdup(optarg);
				break;
			case 'c':
				setting_file = strdup(optarg);
				break;
            case 'p':
                portnr = atoi(optarg);
                break;
			default:
				/* ignore */
				break;
		}
	}
}

void free_memory(void)
{
    if(debug_file)
        free(debug_file);

    if(setting_file)
        free(setting_file);
}

void receive_sock(int fd, char* data, int len)
{
	DEBUG_MSG("Received data from tcp-connection");
}


int main(int argc, char** argv)
{
    int32_t running = 0;

    handle_options(argc, argv);
    init_debug(debug_file);

    DEBUG_MSG("Starting TCP server for remote connection...");
    if(init_tcp_server(portnr, (char*)NULL, receive_sock) < 0) {
        DEBUG_MSG("error during tcp server initialization.");
        return EXIT_FAILURE;
    }



    DEBUG_MSG("Start listening for connections on port %d.", portnr);
    running = 1;
    while(running)
	{
		io_select();
	}

	DEBUG_MSG("Terminating server...");
	close_debug();

    return 0;
}
