CC=$(CROSS_COMPILE)gcc

SRC = $(wildcard ../*.c \
        ../common/*.c \
        ../common/log/*.c \
        ../common/log/arch/linux/*.c \
        ../network/mbedtls/library/*.c \
        ../network/mbedtls/wrapper/*.c \
        ../mqtt/*.c \
        ../mqttclient/*.c \
        ../network/*.c \
        ../platform/linux/*.c \
        )

INC =   -lpthread \
        -I../common \
        -I../common/log \
        -I../network/mbedtls/include \
        -I../network/mbedtls/include/mbedtls \
        -I../network/mbedtls/wrapper \
        -I../mqtt \
        -I../mqttclient \
        -I../network \
        -I../platform/linux \
        -I../test

OBJS = $(patsubst %.c, %.o, $(SRC))

FLAG = -g -fpic -std=gnu99 -I. -Iinclude $(INC) 
TARGET = libmqttclient.so


$(TARGET):$(OBJS)
	$(CC) -shared $^ -o $@

%.o:%.c
	$(CC) -c $(FLAG) $^ -o $@

clean:
	rm -rf $(TARGET) $(OBJS)

install:
	sudo cp -rdf $(TARGET) /usr/lib/.

remove:
	sudo rm -rdf /usr/lib/$(TARGET)

.PHONY:clean
