SRCS = main.cpp

OBJS = $(SRCS:.cpp=.o)

CC = gcc
INCLUDES = -I./

BUILD_TYPE := RELEASE

ifeq ($(BUILD_TYPE), DEBUG)
OPTIMFLAGS = -g
else
OPTIMFLAGS = -O2
endif

LIBS = -lbe
CFLAGS =  $(OPTIMFLAGS) -Wall
CPPFLAGS =  $(OPTIMFLAGS) -Wall
PROGRAM = Folstein

default: setgcc2 $(OBJS)
	$(CC) -shared -Wl,-soname,$(PROGRAM) -o $(PROGRAM) $(OBJS) $(LDFLAGS) $(LIBS) 

.cpp.o:
	$(CC) $(INCLUDES) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# current haiku release has tracker compiled with gcc2, so the addon must also
# use gcc2
setgcc2:
	setgcc x86 gcc2

install : uninstall
	rc -o addon.rsrc addon.rdef
	xres -o $(PROGRAM) addon.rsrc
	mkdir -p /boot/common/add-ons/Tracker
	cp -p $(PROGRAM) /boot/common/add-ons/Tracker/

uninstall:
	rm -f /boot/common/add-ons/Tracker/$(PROGRAM)

clean:
	rm -f *.o
	rm -f addon.rsrc
	rm -f $(PROGRAM)

