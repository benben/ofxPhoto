# add custom variables to this file

# OF_ROOT allows to move projects outside apps/* just set this variable to the
# absoulte path to the OF root folder

OF_ROOT = ../../..


# USER_CFLAGS allows to pass custom flags to the compiler
# for example search paths like:
# USER_CFLAGS = -I src/objects

USER_CFLAGS =


# USER_LDFLAGS allows to pass custom flags to the linker
# for example libraries like:
# USER_LD_FLAGS = libs/libawesomelib.a

USER_LDFLAGS = $(OF_ROOT)/addons/ofxPhoto/libs/libgphoto2/lib/linux/libgphoto2.a $(OF_ROOT)/addons/ofxPhoto/libs/libgphoto2/lib/linux/libgphoto2_port.a


# use this to add system libraries for example:
# USER_LIBS = -lpango

USER_LIBS = -lexif


# change this to add different compiler optimizations to your project

USER_COMPILER_OPTIMIZATION = -march=native -mtune=native -Os


EXCLUDE_FROM_SOURCE="bin,.xcodeproj,obj,.git"
