include defs.mak
MFDEPS=defs.mak

CXXFLAGS$(CXXFLAGS):=-std=c++98 -g -O0 -fsanitize=address -fno-omit-frame-pointer -Wall -Wextra
CFLAGS$(CFLAGS):=-std=c89 -g -O0 -fsanitize=address -fno-omit-frame-pointer -Wall -Wextra
LDFLAGS$(LDFLAGS):=-fsanitize=address
BUILD$(BUILD):= debug
BINDIR$(BINDIR):= ./$(BUILD)
LIBDIR$(LIBDIR):= ./$(BUILD)/lib
OBJDIR$(OBJDIR):= ./$(BUILD)/obj


EXEFILE:= $(BINDIR)/play

CSRC=\
	dawe.c \
	dawe_wav.c \
	dawe_err.c \
	dawe_wav_print.c \
	dawe_device.c \
	dawe_sess.c \
	dawe_tpool.c \
	dawe_buffer.c \
	dawe_device_reformat.c \
	alsa.c
	
CXXSRC=\
	DaweException.cc \
	drv/DaweAlsaDevice.cc \
	DaweSess.cc \
	DawePort.cc \
	DaweBus.cc \
	DaweDevice.cc \
#	drv/DaweAlsa.cc
	
#LIBARCHDIR      := $(LIBDIR)/$(KERNEL)/$(ARCH)

SNAME           := $(LIBDIR)/libdawe.a
DNAME           := $(LIBDIR)/libdawe.so

PREFIX=~

LIBS=\
	-lasound \
	-lpthread

#.c.o:
#	$(CC) -c $(CFLAGS) $<

#.cc.o:
#	$(CC) -c $(CXXFLAGS) $<




#example1: $(LIBNAME) example1.o
#	$(CC) -o example1 example1.o $(LIBNAME) $(LIBS)

#example2: $(LIBNAME) example2.o
#	$(CC) -o example2 example2.o $(LIBNAME) $(LIBS)
	
#test1: test.o
#	$(CC) -o test1 test.o 

#all:
#	make $(ANAME)
	
	
#CFLAGS=-fsanitize=address -fno-omit-frame-pointer -g -O0 -Wall


$(EXEFILE): play.cc $(SNAME)
	$(CXX) $(CXXFLAGS) play.cc -o $(EXEFILE) $(SNAME) $(LDFLAGS) -lasound
	

rel:
	$(MAKE) BUILD=release CFLAGS='-O3 -Wall' CXXFLAGS='-O3 -Wall' 

rel-clean:
	$(MAKE) BUILD=release clean


	
include lib.mak

clean::
	rm -f $(EXEFILE)

clean-all:
	$(MAKE) clean
	rm -rf html
	rm -rf latex
	rm -rf man


