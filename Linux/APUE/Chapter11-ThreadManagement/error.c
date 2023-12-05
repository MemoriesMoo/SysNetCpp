#include "apue.h"
#include <errno.h>	/* 用于定义错误号 */
#include <stdarg.h> /* ISO C 可变参数 */

static void err_doit(int, int, const char *, va_list);

/*
 * 非致命与系统调用相关的错误。
 * 打印消息并返回。
 */
void err_ret(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
}

/*
 * 致命与系统调用相关的错误。
 * 打印消息并终止程序。
 */
void err_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * 非致命与非系统调用相关的错误。
 * 错误码作为显式参数传递。
 * 打印消息并返回。
 */
void err_cont(int error, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
}

/*
 * 致命与非系统调用相关的错误。
 * 错误码作为显式参数传递。
 * 打印消息并终止程序。
 */
void err_exit(int error, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * 致命与系统调用相关的错误。
 * 打印消息，转储内存，然后终止程序。
 */
void err_dump(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	abort(); /* 转储内存并终止程序 */
	exit(1); /* 不应该执行到这里 */
}

/*
 * 非致命与非系统调用相关的错误。
 * 打印消息并返回。
 */
void err_msg(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
}

/*
 * 致命与非系统调用相关的错误。
 * 打印消息并终止程序。
 */
void err_quit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * 打印消息并返回给调用者。
 * 调用者指定 "errnoflag"。
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf[MAXLINE];

	vsnprintf(buf, MAXLINE - 1, fmt, ap);
	if (errnoflag)
		snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": %s",
				 strerror(error));
	strcat(buf, "\n");
	fflush(stdout); /* 如果stdout和stderr相同，刷新stdout */
	fputs(buf, stderr);
	fflush(NULL); /* 刷新所有stdio输出流 */
}
