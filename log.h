#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#include "string_utils.h"

#define LOG_DEBUG_LEVEL		1
#define LOG_INFO_LEVEL		2
#define LOG_WARN_LEVEL		3
#define LOG_ERROR_LEVEL		4
#define LOG_FATAL_LEVEL		5

#define LOG_DEFAULT_LEVEL	2

/*	define the default verbose	*/
extern int verbose;// = LOG_DEFAULT_LEVEL;

/*	define the default log_level	*/
extern int log_level;// = 1;

/*	define the default log_level	*/
extern char *log_filename;// = NULL;

#define LOG_VERBOSE(level) {		\
	set_log_verbose(level);			\
}

#define LOG_LEVEL(level) {		\
	set_log_level(level);			\
}

#define LOG_FILE(filename) {		\
	set_log_filename(filename);			\
}

#define LOG_DEBUG(msg) {			\
	print_log_message(LOG_DEBUG_LEVEL, "DEBUG",		\
	__FILE__, __LINE__, __func__, msg);				\
}

#define LOG_INFO(msg) {			\
	print_log_message(LOG_INFO_LEVEL, "INFO",		\
	__FILE__, __LINE__, __func__, msg);				\
}

#define LOG_WARN(msg) {			\
	print_log_message(LOG_WARN_LEVEL, "WARNING",		\
	__FILE__, __LINE__, __func__, msg);				\
}

#define LOG_ERROR(msg) {			\
	print_log_message(LOG_ERROR_LEVEL, "ERROR",		\
	__FILE__, __LINE__, __func__, msg);				\
}

#define LOG_FATAL(msg) {			\
	print_log_message(LOG_FATAL_LEVEL, "FATAL",		\
	__FILE__, __LINE__, __func__, msg);				\
	exit(-1);										\
}

#define LOG_DEBUG_F(msg, ...) {            \
    print_log_message_with_format(LOG_DEBUG_LEVEL, "DEBUG",     \
    __FILE__, __LINE__, __func__, msg, __VA_ARGS__);             \
}

#define LOG_INFO_F(msg, ...) {         \
    print_log_message_with_format(LOG_INFO_LEVEL, "INFO",       \
    __FILE__, __LINE__, __func__, msg, __VA_ARGS__);             \
}

#define LOG_WARN_F(msg, ...) {         \
    print_log_message_with_format(LOG_WARN_LEVEL, "WARNING",        \
    __FILE__, __LINE__, __func__, msg, __VA_ARGS__);             \
}

#define LOG_ERROR_F(msg, ...) {            \
    print_log_message_with_format(LOG_ERROR_LEVEL, "ERROR",     \
    __FILE__, __LINE__, __func__, msg, __VA_ARGS__);             \
}

#define LOG_FATAL_F(msg, ...) {            \
    print_log_message_with_format(LOG_FATAL_LEVEL, "FATAL",     \
    __FILE__, __LINE__, __func__, msg, __VA_ARGS__);             \
    exit(-1);                                       \
}


#define LOG_IF(level, cond, msg) {							\
	if(cond) {												\
		switch(level) {										\
			case LOG_DEBUG_LEVEL:	LOG_DEBUG(msg); break;	\
			case LOG_INFO_LEVEL:	LOG_INFO(msg); break;	\
			case LOG_WARN_LEVEL:	LOG_WARN(msg); break;	\
			case LOG_ERROR_LEVEL:	LOG_ERROR(msg); break;	\
			case LOG_FATAL_LEVEL:	LOG_FATAL(msg); break;	\
			default: break;									\
		}													\
	}														\
}

/**
 * Initialize the logging system with default values for level, verbosity (show in stdout) and filename.
 * - Default level: INFO
 * - Default verbosity: no
 * - Default filename: output.log
 */
void init_log();

/**
 * Initialize the logging system, allowing to set the values for level, verbosity (show in stdout) and filename.
 * 
 * @param level default severity level to log
 * @param verbose whether to show log messages through stderr
 * @param log_filename 
 */
void init_log_custom(int level, int verbose, char *log_filename);

void set_log_verbose(int verbose);

void set_log_level(int level);

void set_log_filename(char *filename);

void print_log_message(int level, char *log_level_word, char *filename, int num_line, const char *func, char *msg);

void print_log_message_with_format(int level, char *log_level_word, char *filename, int num_line, const char *func, char *msg_format, ...);

#endif