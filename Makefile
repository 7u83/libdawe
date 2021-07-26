
OBJS=\
	dawe.o \
	dawe_wav.o \
	dawe_err.o \
	dawe_wav_print.o \
	alsa.o \
	dawe_device.o \
	dawe_sess.o \
	dawe_tpool.o \

LIBNAME=libdawe.a

PREFIX=~

LIBS=\
	-lasound \
	-lpthread

.c.o:
	$(CC) -c $(CFLAGS) $<

.cc.o:
	$(CC) -c $(CFLAGS) $<


$(LIBNAME): $(OBJS)
	$(AR) rcs $(LIBNAME) $(OBJS)

prg: $(LIBNAME) prg.o
	$(CC) $(LDFLAGS) -o prg prg.o $(LIBNAME) $(LIBS)

example1: $(LIBNAME) example1.o
	$(CC) -o example1 example1.o $(LIBNAME) $(LIBS)

example2: $(LIBNAME) example2.o
	$(CC) -o example2 example2.o $(LIBNAME) $(LIBS)


#CFLAGS=-fsanitize=address -fno-omit-frame-pointer -g -O0 -Wall

clean:
	rm -f $(OBJS)
	rm -f $(LIBNAME)
	rm -f example1.o example1
	rm -f example2.o example2
	rm -f mavtest mavtest.o
	rm -f prg prg.o
	rm -f *.core
	rm -f test.o test

install: $(LIBNAME)
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include/libmavl
	install -p $(LIBNAME) $(PREFIX)/lib/$(LIBNAME)
	install -p mavl.h $(PREFIX)/include/libmavl/mavl.h

test: $(LIBNAME) test.o
	$(CC) $(LDFLAGS) -o test  test.o $(LIBNAME) -lmutests

