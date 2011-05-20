bin_PROGRAMS = tyranny_lobby_server
tyranny_lobby_server_SOURCES = \
	lobbyserver.cpp \
	serversocket.cpp serversocket.h

AM_CPPFLAGS = $(all_includes)

tyranny_lobby_server_LDFLAGS = $(all_libraries)
tyranny_lobby_server_LDADD = -lpthread

noinst_HEADERS = \
	lobbyserver.h \
	serversocket.h

