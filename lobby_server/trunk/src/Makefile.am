bin_PROGRAMS = tyranny_lobby_server
tyranny_lobby_server_SOURCES = \
	configfile.cpp configfile.h \
	dbmysql.cpp dbmysql.h \
	lobbyserver.cpp lobbyserver.h \
	packet.cpp packet.h \
	protocol.cpp protocol.h \
	serversocket.cpp serversocket.h

AM_CPPFLAGS = $(all_includes) -I/usr/include/libxml2 `mysql_config --cflags`

tyranny_lobby_server_LDFLAGS = $(all_libraries) `mysql_config --libs`
tyranny_lobby_server_LDADD = -lpthread -lxml2

