AM_CFLAGS		= -Iinclude -Isrc/include

lib_LTLIBRARIES		= libxpadf.la
libxpadf_la_SOURCES	= src/posix/common/objects.c		\
				src/posix/common/status.c	\
				src/posix/thread/thread.c	\
				src/posix/thread/looper.c	\
				src/posix/thread/tpool.c	\
				src/posix/io/io.c		\
				src/posix/io/socket.c		\
				src/posix/sync/locks.c

if HAVE_EPOLL
libxpadf_la_SOURCES	+= src/posix/io/epollip.c
endif

bin_PROGRAMS	= tchat_server tchat_client

tchat_server_SOURCES	= examples/chat/tserver.c
tchat_server_LDADD	= .libs/libxpadf.a

tchat_client_SOURCES	= examples/chat/tclient.c
tchat_client_LDADD	= .libs/libxpadf.a
