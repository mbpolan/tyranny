bin_PROGRAMS = tyranny_game_server
tyranny_game_server_SOURCES = \
	configfile.cpp configfile.h \
	gameserver.cpp gameserver.h \
	packet.cpp packet.h \
	protspec.h \
	serversocket.cpp serversocket.h

AM_CPPFLAGS = $(all_includes) -I/usr/include/libxml2 `mysql_config --cflags`

tyranny_game_server_LDFLAGS = $(all_libraries) `mysql_config --libs`
tyranny_game_server_LDADD = -lpthread -lxml2

