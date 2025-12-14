/*
 * Copyright (C) 2021  7u83 <7u83@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 */


#include <stdio.h>
#include <iostream>

#include "dawe.h"
#include "dawe_wav.h"


#include "drv/DaweAlsaDevice.h"
#include "Session.h"
#include "WavPlayer.h"
#include "MidiPlayer.h"

#include "RingBuffer.h"

#include "jack/JackDevice.h"

#include "Device.h"
#include "Exception.h"

#include "midi/MidiFile.h"

#include "JackSess.h"

#include "song/Track.h"

#include "song/Song.h"

#include "midi/MidiEventArrayList.h"

#include "midi/MidiInstrument.h"
#include "midi/MidiEventQueue.h"

#include "midi/MidiIndex.h"
#include "song/MidiClipLink.h"

#include "tools/RegEx.h"

#include "algo/List.h"
#include "midi/MidiEvent.h"
#include "song/MidiClip.h"

#include "song/MidiClipList.h"
#include "algo/Val.h"

#include "algo/SkipList.h"
#include "midi/MidiListIndex.h"

struct Tube {
	float val;
	//char text[64];
	const char *txt;
	int iv;
	/*	bool operator <(const Tube & o) const{
		return val < o.val;
	}

	bool operator <(const double o) const{
		return val < o;
	}
*/

	friend std::ostream& operator<<(std::ostream& os, const Tube& obj) {
		os << "*" << obj.val << "-" << obj.iv << "";
		return os;
	}
};

struct Xv {
	int val;
	int ctr;
	//char text[64];
	const char *txt;
	bool operator <(const Xv & o) const{
		return val < o.val;
	}

	bool operator <(const int o) const{
		return val < o;
	}


	friend std::ostream& operator<<(std::ostream& os, const Xv& obj) {
		os << "*" << obj.val << "-" << obj.ctr << "";
		return os;
	}
};


void test()
{
	//	Dawe::SkipList<int> sl;

	Dawe::SkipList<Xv> sl;

	//	int nx=1000000;

	int nx=1000000;

	for (int h=0; h<50; h++){
		std::cout << "RUN " << h << std::endl;
		std::cout << "INSERTING " << nx << " RANDOM INTEGERS" << std::endl;
		Xv v;

		for(int i=0; i<nx; i++){
			v.val=rand(); //%100000;
			v.ctr=i;
			sl.insert(v);
		}
		std::cout << "HEIGHT: " << sl.height() << std::endl;

		std::cout << "VERIFYING" << std::endl;
		if (!sl.verify()){
			std::cout << "VERIFY FAIL" << std::endl;
			sl.print();
			return;
		}
		std::cout << "VERIFY OK" << std::endl;

		std::cout << "DELETING " << nx/2 << " RANDOM VALUES" << std::endl;
		for (int i=0; i<nx/2; i++){
			Dawe::SkipList<Xv>::iterator it;
			int index = rand()% (sl.size());
			it = sl.getIteratorAtIndex(index);
			sl.erase(it);
		}
		std::cout << "HEIGHT: " << sl.height() << std::endl;

		std::cout << "VERIFYING" << std::endl;
		if (!sl.verify()){
			std::cout << "VERIFY FAIL" << std::endl;
			return;
		}
		std::cout << "VERIFY OK" << std::endl;
	}
}

struct Compi {
	bool operator()(const Tube & a, const Tube & b){
		return a.val<b.val;
	}
	bool operator()(const Tube & a, int b) const {
		return a.val<b;
	}
};

int main(int argc, char * argv[])
{
	test();
	std::cout << "TEST DONE" << std::endl;
	return 0;

	Dawe::SkipList<Xv,int> sl(9);

	for (int i=0; i<10; i++){
		Xv v;
		v.ctr=i;
		v.val=rand()%100;
		sl.print();
		std::cout << "INSERT " << v << std::endl;
		sl.insert(v);
	}

	sl.print();

	Xv v;
	v.val=83;
	v.ctr=11;
	sl.insert(v);

	sl.print();

	v.val=83;
	v.ctr=19;
	sl.insert(v,5);
	sl.print();


	v.val=15;
	v.ctr=5;
	sl.insert(v,0);
	sl.print();


	v.val=15;
	v.ctr=7;
	sl.insert(v,1);
	sl.print();

	v.val=16;
	v.ctr=0;
	sl.insert(v,3);
	sl.print();

	if (!sl.verify()){
		std::cout << "verify fail" << std::endl;
	}
	return 0;



	//	test();
	//	return 0;

	/*	Dawe::MidiEvent tev = Dawe::MidiEvent(Dawe::MidiEvent::TRACKNAME);
	tev.setText("Tube");
	tev.dump();
	tev.setText("Hallo Welt!");
	tev.dump();
	std::string st = tev.getText();
	std::cout << "TEXT: " << st << std::endl;
	exit(0);
*/



	Dawe::XMidiList newl;
	//Dawe::MidiList::Index * newi;

	Dawe::MidiEvent event0(Dawe::MidiEvent::NOTE_ON);
	event0.setTime(17);
	newl.insert(event0);

	for (int i=0; i<2; i++){
		Dawe::MidiEvent xevent1(Dawe::MidiEvent::NOTE_ON);
		xevent1.setTime(i*4);
		newl.insert(xevent1);
		//		newl.push_back(xevent1);
	}
	newl.dump();
	return 0;

	//newl.sort();
	//newl.rebuild();

	for (int i=0; i<25; i++){
		Dawe::MidiEvent xevent1(Dawe::MidiEvent::TIMESIG);
		xevent1.setTime(i*3);
		xevent1.setTimeSig(3,4);
		newl.insert(xevent1);
		//		newl.push_back(xevent1);

	}
	//	newl.sort();
	//	newl.rebuild();
	newl.dump();
	newl.verify();


	Dawe::MidiListIndex ind(newl);
	ind.rebuild();

	std::cout << "----------" << std::endl;
	for(int i=0; i<ind.size(); i++){
		Dawe::MidiEvent * e;
		e = ind.getValPtrAtIndex(i);
		e->dump();
		//ind.print();

	}
	//ind.ilist.print();
	return 0;


	struct Test{
		int operator()(Dawe::MidiEvent & a) const{
			return a.getTime();
		}
	};

	printf("%d\n",(Dawe::nmiditicks_T)event0);

	newl.dump();

	printf("=== index 0 ===\n");
	//	Dawe::XMidiList::Index *i = newl.getIndex(0);
	//	i->dump();

	printf("=== index 1 ===\n");
	//	i = newl.getIndex(1);
	//	i->dump();

	return 0;

	Dawe::MidiClip md;
	Dawe::MidiEvent event(Dawe::MidiEvent::NOTE_ON,0);
	event.setNote(17);
	md.insert(event);

	Dawe::MidiEvent event1(Dawe::MidiEvent::NOTE_OFF,40);
	md.insert(event1);


	md.dump();

	//Dawe::List<Dawe::nmiditicks_T, Dawe::MidiClip> mlist;
	Dawe::MidiClipList cliplist;

	Dawe::MidiClipLink c= Dawe::MidiClipLink(&md);
	c.setTime(900);
	c.setDuration(900);
	c.setName("Clip 900");
	cliplist.push_back(c);

	c.setTime(12);
	c.setDuration(900);
	c.setName("Clip 12");
	cliplist.push_back(c);

	c.setTime(20);
	c.setDuration(900);
	c.setName("Clip 20");
	cliplist.push_back(c);

	//	cliplist.dump();

	cliplist.sort();
	cliplist.dump();


	Dawe::Accessor< Dawe::nmiditicks_T,Dawe::MidiEvent> * acc;
	acc = cliplist.createAccessor();

	for (int i=0; i<10; i++){
		Dawe::MidiEvent eav = acc->getVal();
		printf("EAV[%d]:",eav.getTime()); eav.dump();

		acc->moveForward();
		if (acc->isAtEnd())
			break;

	}



	exit(0);



	Dawe::MidiList ml;
	Dawe::MidiList::iterator mli;

	Dawe::MidiEvent exx = Dawe::MidiEvent(Dawe::MidiEvent::TEMPO);
	exx.setTempo(0x40000);
	ml.push_back(exx);
	ml.dump();

	//	exit(0);



	//	Dawe::XIterator<Dawe::MidiEvent> mli;
	//	mli = ++mli;
	/*

	Dawe::Track t = Dawe::Track();
	Dawe::Clip	a("clip-a",10),
			b("clip-b",10),
			c("clip-c",100),
			d("clip-d",5500),
			e("clip-e"),
			f("clip-f");

	t.insert(10,&a);
	t.dump();

	t.insert(20,&b);
	t.dump();


	t.insert(10,&c);
	t.dump();



	t.remove(&c);
	t.dump();

	t.remove(&a);
	t.dump();



	t.remove(&b);
	t.dump();

	t.insert(0,&c);
	t.insert(5,&a);
	t.dump();
	exit(0);



*/






	Dawe::MidiEventArrayList al;
	Dawe::MidiIndex index = Dawe::MidiIndex(al);
	index.setFilter(Dawe::MidiEvent::TEMPO);


	Dawe::MidiEvent e = Dawe::MidiEvent(Dawe::MidiEvent::TEMPO);
	e.setTempo(0x40000);
	Dawe::MidiList::iterator li;
	//	li = al.insert(e);
	//	index.insert(li);

	Dawe::MidiEvent en = Dawe::MidiEvent(Dawe::MidiEvent::NOTE_ON);
	li = al.append(en);
	index.insert(li);

	e.setTime(700);
	e.setTempo(0x555);
	li = al.append(e);
	index.insert(li);


	e.setTime(0);
	e.setTempo(0x98076);
	li = al.insert(e);
	index.insert(li);

	const Dawe::MidiEvent *er;
	er = index.getLatest(90);

	if (er){
		printf("%d: (%d)",er->getTime(), er->getTempo());
		er->dump();
	}
	printf("Done\n");

	//	Dawe::MidiIndex::Accessor it(index);

	//	it.seek(900);

	/*	for (it.seek(900); !it.isAtEnd(); it.moveForward()){
		Dawe::MidiEvent *e = it.getValue();
		printf("DumpAcessor [%d]: ",e->getTime());
		e->dump();
	}
*/

	exit(0);






	Dawe::InstrumentList il;

	il.load("PSR-S710.ins");

	//	Dawe::InstrumentData id;

	const char * ins_name = "Yamaha PSR-S710";

	if (il.find(ins_name) == il.end()){
		printf("Instrument not found\n");
		return 1;
	}
	Dawe::Instrument& ins = il[ins_name];

	const Dawe::InstrumentData& notes = ins.notes(16256,0);
	for (size_t i=0; i<notes.count(); i++){
		printf("NOTE: %s\n",notes[i].c_str());
	}


	exit(0);

	Dawe::MidiFile mft;
	mft.read("kraftwerk-modell.mid");
	//	mft.read("HungUp.mid");
	mft.dump_info();



	//	mft.read("play.cc");

	/*	for (int i=0; i<mft.tracks.size(); i++){
		printf("TL: %d\n",mft.tracks[i]->size());
		for (int i1=0; i1<mft.tracks[i]->size(); i1++){
			printf("amp:");
			mft.tracks[i]->getAtIndex(i1)->dump();
			//exit(0);
		}


	}
*/

	exit(0);



	Dawe::Session session = Dawe::Session("Tube");

	Dawe::MidiPlayer player(&session);


	Dawe::MidiEventQueue q;


	/*
	Dawe::MidiEvent te(Dawe::MidiEvent::TEMPO,0);
	te.setTempo(500000);
	ex.insert(te);


	Dawe::MidiEvent te1(Dawe::MidiEvent::NOTE_ON,960);
	//te1.setTempo(500000);
	ex.insert(te1);

	Dawe::MidiEvent te2(Dawe::MidiEvent::TEMPO,960*3);
	te2.setTempo(500000*3);
	ex.insert(te2);



	Dawe::MidiEvent te3(Dawe::MidiEvent::NOTE_ON,960*4);
	ex.insert(te3);


	//	ex.dump();
	//	ex.dump_index();
*/



	/*	ex.deleteAtIndex(2);

	ex.dump();
	ex.dump_index();
*/
	//	ex.rebuildFramesMapIfInvalid();

	//	Dawe::nmiditicks_T t =





	Dawe::MidiEvent te11(Dawe::MidiEvent::TEMPO,12);
	te11.setTempo(50000);
	te11.setNote(121);
	//	ex.insert(te11);



	//	ex.insert(te2);



	/*
	ex.dump();
	ex.dump_index();

	printf("----------- deleting -------------\n");
	ex.deleteAtIndex(0);

	ex.dump();

	ex.eraseFirst();

	printf("After erase first\n");
	ex.dump();
	printf("After erase first2\n");
	ex.eraseFirst();
	ex.dump();

	printf("After erase first3\n");
	ex.eraseFirst();
	ex.dump();
	exit(0);
*/

	for (int i=0; i<128; i++){

		Dawe::MidiEvent qa(Dawe::MidiEvent::NOTE_ON,0+i*session.getTicksPerBeat());
		qa.setNote(60 + (i%2)*12 + (i%16)*2);
		qa.setVelocity(110);
		q.insert(qa);
	}

	player.setQeueue(&q);


	q.dump();
	player.start();
	sleep(100);

	exit(0);






	Dawe::MidiEvent qb(Dawe::MidiEvent::NOTE_ON,10);
	qb.setNote(34);
	q.insert(qb);

	Dawe::MidiEvent qc(Dawe::MidiEvent::NOTE_ON,170);
	qc.setNote(111);
	q.insert(qc);

	Dawe::MidiEvent qd(Dawe::MidiEvent::NOTE_ON,5);
	qd.setNote(90);
	q.insert(qd);
	q.dump();

	printf("Erase\n");
	q.eraseFirst();
	q.dump();

	printf("Erase\n");
	q.eraseFirst();
	q.dump();

	printf("Erase\n");
	q.eraseFirst();
	q.dump();

	printf("Erase\n");
	q.eraseFirst();
	q.dump();

	exit(0);


	/*	Dawe::MidiInstrument mi("Tobias",3);
	std::string n =mi.getNoteName(90);
	std::cout << "HELO" << n << std::endl;
	exit(0);
*/


	printf("Hello\n");

	/*
	Dawe::MidiEventArrayList l;
	Dawe::MidiEvent a(Dawe::MidiEvent::NOTE_ON,10);
	l.insert(a);
	Dawe::MidiEvent b(Dawe::MidiEvent::NOTE_ON,7);
	l.insert(b);
	Dawe::MidiEvent c(Dawe::MidiEvent::NOTE_ON,19);
	l.insert(c);
	Dawe::MidiEvent d(Dawe::MidiEvent::NOTE_ON,11);
	l.insert(d);
*/

	//l.dump();
	exit(0);



	/*Dawe::Song::TimeSig ts= Dawe::Song::TimeSig(2,8);
	std::cout << (unsigned int)ts.getNumerator() << ","
		  << (unsigned int)ts.getDenomintaor();
	*/
	/*Dawe::Song s(0);
	Dawe::Song::Tempo tp = s.tm.getAt(100);
	std::cout << "Tempo:" << tp.get() << std::endl;
*/
	Dawe::Song s(0);
	//s.sig_map.insertAt(134,Dawe::TimeSigEvent(3,8));

	//Dawe::TimeSigEvent ts; // = Dawe::Song::TimeSig();
	//ts = s.sig_map.getAt(32);
	//std::cout << "Sig:" << (unsigned)ts.getNumerator()
	//	  << "/" << (unsigned)ts.getDenomintaor() << std::endl;

	//	ts = s.sig_map.getAt(1000);
	//	std::cout << "Sig2:" << (unsigned)ts.getNumerator()
	//		  << "/" << (unsigned)ts.getDenomintaor() << std::endl;

	return 0;





	(void)argc; (void)argv;
	std::cout << "LIBDAWE" << std::endl;

	std::string fn = "tobias";
	Dawe::MidiFile mf;

	//	mf.convertToRosegarden("long.mid");
	std::cout << "format:" << mf.m_format << std::endl;

	//mf.read("thunderstruck.mid");
	//mf.read("midi34.mid");

	std::cout << "Number of Tracks:" << std::dec << mf.m_numberOfTracks << std::endl;
	std::cout << "Format: " << mf.m_format << std::endl;
	std::cout << "Timing Div: " << mf.m_time_division << std::endl;

	/*	for (unsigned int i=0; i<mf.m_trackNames.size() ; i++){
		std::cout << mf.m_trackNames[i] << std::endl;
	}
*/

	float tictime = 500000/960;
	std::cout << "TICTICTCTIME: "<< tictime << " mySec" << std::endl;

	float frametime = 1.0/48000.0 * 1000000;
	std::cout << "FRAMTIME:" << frametime << " mySec" << std::endl;

	//	dawe::JackSess x("hello");
	//	x.run_server();

	//	dawe::MidiPlayer mp = dawe::MidiPlayer("Tubes MidiPlayer");
	//	mp.play();

	//	Dawe::WavPlayer wp = Dawe::WavPlayer("waver","porx");
	//	wp.play();

	/*
	Dawe::Track t = Dawe::Track();
	Dawe::Clip	a("clip-a",10),
			b("clip-b",10),
			c("clip-c",100),
			d("clip-d",5500),
			e("clip-e"),
			f("clip-f");

	t.insert(10,&a);
	t.print();

	t.insert(20,&b);
	t.print();


	t.insert(10,&c);
	t.print();
*/

	/*
	t.insert(30,&d);
	t.print();
*/



	//	t.insert(79,&c);
	//	t.insert(19,&d);






	//	delete sess;
}
