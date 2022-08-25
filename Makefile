CC=gcc
all:
	$(CC) -g -Wall main.c `pkg-config libwebsockets --libs --cflags`
