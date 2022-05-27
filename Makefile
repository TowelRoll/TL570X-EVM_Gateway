#
ARCH ?= arm
TARGET = modbus

#
BUILD_DIR = build_$(ARCH)
#
SRC_DIR=sources
#
INC_DIR= includes libmqttclient/include libgpiod/include
#
LIB_DIR= -L lib   -L libmqttclient/lib -L libgpiod/lib

#
SRCS=$(wildcard $(SRC_DIR)/*.c)
#
OBJS=$(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))
#
DEPS=$(wildcard $(INC_DIR)/*.h)
#
LIBS=-lmodbus -lpthread -lsqlite3 -lm -lmqttclient -lgpiod

#
CFLAGS=$(patsubst %, -I%, $(INC_DIR))

#
ifeq ($(ARCH), x86)
CC=gcc
else
CC=/opt/gcc-arm-linux-gnueabihf-8.3.0/bin/arm-linux-gnueabihf-gcc
endif

#
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)  $(LIB_DIR) $(LIBS)  

#
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

#
.PHONY: cgi-mb cgi-df cgi-login deb clean cleanall 
cgi-mb:
	$(CC) cgi/modbus.c -o cgi/modbus.cgi -I includes -L lib -lsqlite3
cgi-df:
	$(CC) cgi/downfile.c -o cgi/downfile.cgi -I includes -L lib -lcgic
cgi-login:
	$(CC) cgi/login.c -o cgi/login.cgi -I includes -L lib -lsqlite3
deb:
	cp ./db/* ./gateway_deb/home/gateway/db/
	cp ./$(BUILD_DIR)/$(TARGET) ./gateway_deb/home/gateway
	cp ./cgi/*.cgi ./cgi/*.sh ./gateway_deb/www/cgi-bin
	cp ./cgi/*.html ./gateway_deb/www
	cp ./cgi/HTML-Login-Page/* ./gateway_deb/www -r
	cp ./lib/* ./libgpiod/lib/* ./libmqttclient/lib/* ./gateway_deb/lib -r
	sudo dpkg-deb -b ./gateway_deb ./gateway_1.0.0_armhf.deb
	dpkg --info  ./gateway_1.0.0_armhf.deb
clean:
	rm -rf $(BUILD_DIR)
cleanall:
	rm -rf build_x86 build_arm
