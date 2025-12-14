include defs.mak
MFDEPS=defs.mak

#CXXFLAGS$(CXXFLAGS):=-std=c++98 -g -O0 -fsanitize=address -fno-omit-frame-pointer -Wall -Wextra -pedantic
#CFLAGS$(CFLAGS):=-std=c89 -g -O0 -fsanitize=address -fno-omit-frame-pointer -Wall -Wextra -pedantic
#LDFLAGS$(LDFLAGS):=-fsanitize=address

CXXFLAGS$(CXXFLAGS):=-std=c++98 -g -O0 -Wall -Wextra -pedantic
CFLAGS$(CFLAGS):=-std=c89 -g -O0 -Wall -Wextra -pedantic
LDFLAGS$(LDFLAGS):=
BUILD$(BUILD):= Debug
BINDIR$(BINDIR):= ./bin/$(BUILD)
LIBDIR$(LIBDIR):= ./objects/$(BUILD)/lib
OBJDIR$(OBJDIR):= ./objects/$(BUILD)/obj
CXX=clang

EXEFILE:= $(BINDIR)/play

CSRC=\
#	dawe.c \
#	dawe_wav.c \
#	dawe_err.c \
#	dawe_wav_print.c \
#	dawe_device.c \
#	dawe_sess.c \
#	dawe_tpool.c \
#	dawe_buffer.c \
#	dawe_device_reformat.c \
#	alsa.c

CXXSRC=\
	Exception.cc \
	drv/DaweAlsaDevice.cc \
	Session.cc \
	Port.cc \
	DaweBus.cc \
	Device.cc \
	JackSess.cc \
	jack/JackPort.cc \
	WavPlayer.cc \
	RingBuffer.cc \
	jack/JackDevice.cc \
	midi/MidiFile.cc \
	midi/MidiEvent.cc \
	MidiPlayer.cc \
	song/Track.cc \
	song/Song.cc \
	song/Clip.cc \
	song/MidiClipLink.cc \
	midi/MidiEventArrayList.cc \
	midi/MidiInstrument.cc \
	midi/MidiList.cc \
	midi/MidiEventQueue.cc \
	midi/MidiArrayList.cc \
	midi/MidiIndex.cc \
	tools/RegEx.cc \
	midi/MidiListMerger.cc \
	VList.cc \
	VListAccessor.cc \
	VObj.cc \
	algo/Iterator.cc \
	algo/List.cc \
	algo/Accessor.cc \
	algo/ArrayList.cc \
	algo/Val.cc \
	song/MidiClip.cc \
	song/MidiClipList.cc \
	midi/MidiListIndex.cc \
	algo/SkipList.cc
	
#	midi/MidiEventListIdx.cc \
#	Iterator.cc \
# 	midi/MidiEventListEx.cc 

# midi/MidiTempoIdx.cc
	


SNAME           := $(LIBDIR)/libdawe.a
DNAME           := $(LIBDIR)/libdawe.so

PREFIX=~

LIBS=\
	-lasound \
	-lpthread \
	-ljack \
	-ljackserver \
	-lsndfile \
	-lstdc++ \
	$(SNAME)

.PHONY: play
play:
	$(MAKE) $(SNAME)
	$(MAKE) $(EXEFILE)

$(EXEFILE): play.cc $(SNAME) 
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) play.cc -o $(EXEFILE) $(SNAME) $(LDFLAGS) $(LIBS)


play-release:
	$(MAKE) BUILD=Release CFLAGS='-O3 -Wall' CXXFLAGS='-O3 -Wall' play

play-debug:
	$(MAKE) BUILD=Debug play



rel-clean:
	$(MAKE) BUILD=release clean


#Debug:
#	$(MAKE) BUILD=Debug

Debug:
	$(MAKE) BUILD=Debug CFLAGS='-O0 -Wall' CXXFLAGS='-std=c++98 -g -O0 -Wall' $(SNAME)

Release:
	$(MAKE) BUILD=Release CFLAGS='-O3 -Wall' CXXFLAGS='-O3 -Wall'


cleanRelease:
	$(MAKE) BUILD=Release clean

cleanDebug:
	$(MAKE) BUILD=Debug clean

include lib.mak

clean::
	rm -f $(EXEFILE)

clean-all:
	$(MAKE) clean
	rm -rf html
	rm -rf latex
	rm -rf man


