SRCS = Hash.cpp Http.cpp VtHttp.cpp HttpFile.cpp \
       ResultView.cpp VtUploader.cpp \
       VtGui.cpp VtUi.cpp VtTui.cpp main.cpp

OBJS = $(SRCS:.cpp=.o)

CC = gcc
INCLUDES = -I./ -I/boot/common/include/libxml2/
INCLUDES += -I./ -I/boot/common/include/linprog/

BUILD_TYPE := RELEASE

ifeq ($(BUILD_TYPE), DEBUG)
OPTIMFLAGS = -g
else
OPTIMFLAGS = -O2
endif

LDFLAGS = -L/boot/system/lib/gcc4 -L/boot/common/lib
LIBS = -lbe -lcurl -lcrypto -lnetwork -lssl -lz -lstdc++ -lxml2 -lalm
CFLAGS =  $(OPTIMFLAGS) -Wall -Werror
CPPFLAGS =  $(OPTIMFLAGS) -Wall -Werror
PROGRAM = Folstein

default: setgcc4 $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS) $(LIBS)

all: default addon

randomfile:
	head --byte=24k /dev/random > testfile

archive:
	cd .. && zip -r -q -9 folstein.src.zip ./

addon:
	cd tracker && make

.cpp.o:
	$(CC) $(INCLUDES) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

setgcc4:
	setgcc x86 gcc4

install : uninstall
	rc -o app.rsrc app.rdef
	xres -o $(PROGRAM) app.rsrc
	cp -p $(PROGRAM) /boot/common/bin/
	cd tracker && make install

uninstall:
	rm -f /boot/common/bin/$(PROGRAM)
	cd tracker && make uninstall

clean:
	rm -f *.o
	rm -f testfile
	rm -f $(PROGRAM)
	rm -f app.rsrc
	cd tracker && make clean

