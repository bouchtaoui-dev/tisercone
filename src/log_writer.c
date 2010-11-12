/***************************************************************************
 *   Author: Andaluz                                                       *
 *   Email: andaluz.coder AT gmail . com                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdarg.h> //this is needed for the va_list :)

#include <servercore/log_writer.h>

#define MAX_LOG_LEN 1024

static char* default_log_name = "network.server.log";
static FILE* fp_debug = NULL;
static char glb_debug_buff[MAX_LOG_LEN] = {0};
static char glb_debug_msg[MAX_LOG_LEN] = {0};
static int glb_debug_running = 0;
static char time_string[64] = {0};

void init_debug(char* filename)
{
#ifdef DEBUG
    glb_debug_running = 1;
#else
	if(filename)
		logging_file = filename;
	else logging_file = default_log_name;

	fp_debug = fopen(logging_file, "w+b");
    if(fp_debug)
        glb_debug_running = 1;
#endif
}

char* get_timestamp(void)
{
	struct tm* currenttime = NULL;
	time_t now = time(NULL);

	memset(time_string, 0x00, sizeof(time_string));
	currenttime = localtime(&now);

#ifdef DEBUG
	sprintf(time_string, "%02d:%02d:%02d", currenttime->tm_hour, currenttime->tm_min, currenttime->tm_sec);
#else
	sprintf(time_string, "%s", asctime(currenttime));
	time_string[strlen(time_string)-1] = '\0';
#endif
	return time_string;
}

//FIXME: debug_msg is limited to 256 bytes resize it to 1024 btyes!
void write_debug_msg(char* file, int line, const char* format, ...)
{
	if(glb_debug_running) {
		va_list args;
		memset(glb_debug_buff, 0x00, MAX_LOG_LEN);
		memset(glb_debug_msg, 0x00, MAX_LOG_LEN);

#ifdef DEBUG
        va_start ( args, format );
        vsprintf(glb_debug_buff, format, args );
        va_end ( args );
        sprintf(glb_debug_msg, "[%s]: %s\n", get_timestamp(), glb_debug_buff );
        fprintf ( stdout, glb_debug_msg );
        fflush( fp_debug );
#else
		if(fp_debug) {
			va_start ( args, format );
			vsprintf(glb_debug_buff, format, args );
			va_end ( args );
			sprintf(glb_debug_msg, "[%s][%s-%d]: %s\n", get_timestamp(), file, line, glb_debug_buff );
			fprintf ( fp_debug, glb_debug_msg );
			fflush( fp_debug );
		}
#endif
	}
}

void close_debug(void)
{
	if(fp_debug)
		fclose(fp_debug);
}


