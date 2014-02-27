AM_CFLAGS		= -Iinclude -Isrc/include

lib_LTLIBRARIES		= libxpadf.la
libxpadf_la_SOURCES	= src/posix/common/objects.c		\
				src/posix/common/status.c	\
				src/posix/thread/thread.c	\
				src/posix/thread/looper.c	\
				src/posix/thread/tpool.c

if HAVE_EPOLL
libxpadf_la_SOURCES	+= src/posix/io/epollip.c
endif
