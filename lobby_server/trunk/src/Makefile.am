bin_PROGRAMS = tyranny_lobby_server
tyranny_lobby_server_SOURCES = \
	configfile.cpp configfile.h \
	lobbyserver.cpp lobbyserver.h \
	serversocket.cpp serversocket.h

AM_CPPFLAGS = $(all_includes) -I/usr/include/libxml2

tyranny_lobby_server_LDFLAGS = $(all_libraries)
tyranny_lobby_server_LDADD = -lpthread -lxml2

noinst_HEADERS = \
	configfile.h \
	lobbyserver.h \
	serversocket.h

