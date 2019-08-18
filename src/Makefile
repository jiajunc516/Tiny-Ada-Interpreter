OBJECTS = tiny-ada.tab.o lex.yy.o

int: $(OBJECTS)
	g++ $(OBJECTS) $(LDFLAGS) -o int
	
tiny-ada.tab.o: tiny-ada.y
	bison -d tiny-ada.y
	mv tiny-ada.tab.c tiny-ada.tab.cc
	g++ -c tiny-ada.tab.cc

lex.yy.o: tiny-ada.l tiny-ada.tab.o
	flex -i tiny-ada.l 
	mv lex.yy.c lex.yy.cc
	g++ -c lex.yy.cc

test: int
	int tiny-ada.input

clean:
	/bin/rm -f tiny-ada.tab.cc tiny-ada.tab.h lex.yy.cc  *.o int

tar: clean
	tar cf all.tar *
	chmod a+r all.tar
