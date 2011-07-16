bin_PROGRAMS = tyranny_game_server
tyranny_game_server_SOURCES = \
	aiplayer.cpp aiplayer.h \
	clientsocket.cpp clientsocket.h \
	configfile.cpp configfile.h \
	game.cpp game.h \
	gameserver.cpp gameserver.h \
	human.cpp human.h \
	packet.cpp packet.h \
	player.cpp player.h \
	protocol.cpp protocol.h \
	protspec.h \
	room.cpp room.h \
	roomengine.cpp roomengine.h \
	serversocket.cpp serversocket.h \
	utilities.cpp utilities.h 

AM_CPPFLAGS = $(all_includes) -I/usr/include/libxml2 `mysql_config --cflags`

tyranny_game_server_LDFLAGS = $(all_libraries) `mysql_config --libs`
tyranny_game_server_LDADD = -lpthread -lxml2

