OBJECTS = main.o common.o stream_converter.o packet_analyzer.o section_analyzer.o EPG_manager.o

extractor: $(OBJECTS)
	gcc -o extractor.out $(OBJECTS)

main.o: main.c
	gcc -c -g main.c

common.o: common.h common.c
	gcc -c -g common.c

stream_converter.o: stream_converter.h stream_converter.c
	gcc -c -g stream_converter.c

packet_analyzer.o: packet_analyzer.h packet_analyzer.c
	gcc -c -g packet_analyzer.c

section_analyzer.o: section_analyzer.h section_analyzer.c
	gcc -c -g section_analyzer.c

EPG_manager.o: EPG_manager.h EPG_manager.c
	gcc -c -g EPG_manager.c

clean: 
	rm -rf *.o extractor.out
