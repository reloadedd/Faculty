CC = gcc
LIBS_SERVER = -lssl -lcrypto -lutil -lbsd
LIBS_CLIENT = -lssl -lcrypto
CFLAGS = -g -Wall -Wextra -pedantic -std=c99
SERVER_SOURCES = server.c src/auxiliary.c src/OpenSSL_helper.c src/cryptography.c
CLIENT_SOURCES = client.c src/auxiliary.c src/OpenSSL_helper.c
SERVER_BIN = server
CLIENT_BIN = client

all:
	${CC} -o ${SERVER_BIN} ${CFLAGS} ${LIBS_SERVER} ${SERVER_SOURCES}
	${CC} -o ${CLIENT_BIN} ${CFLAGS} ${LIBS_CLIENT} ${CLIENT_SOURCES}

client: client.c
	${CC} -o ${CLIENT_BIN} ${CFLAGS} ${LIBS_CLIENT} ${CLIENT_SOURCES}

server: server.c
	${CC} -o ${SERVER_BIN} ${CFLAGS} ${LIBS_SERVER} ${SERVER_SOURCES}

clean:
	rm -f ${SERVER_BIN} ${CLIENT_BIN}
