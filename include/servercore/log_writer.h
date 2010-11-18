/***************************************************************************
 *   Author: El Andaluz
 *                                                                         *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 ***************************************************************************/
#ifndef _UNBUG_H
#define _UNBUG_H

#define DEBUG_MSG(fmt, args...) write_debug_msg(__FILE__, __LINE__, fmt, ## args)

void init_debug(char* filename);
void write_debug_msg(char* file, int line, const char* format, ...);
void close_debug(void);

#endif	//_UNBUG_H
