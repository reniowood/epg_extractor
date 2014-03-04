OBJECTS = main.o stream_converter.o packet_analyzer.o section_analyzer.o EPG_manager.o
KERNEL_INCLUDE_PATH = /usr/src/kernels/3.6.11-1.fc16.i686.PAE/include/

extractor: $(OBJECTS)
	gcc -o extractor main.o stream_converter.o packet_analyzer.o section_analyzer.o EPG_manager.o

main.o: common.h main.c
	gcc -c main.c -I$(KERNEL_INCLUDE_PATH)

stream_converter.o: stream_converter.h stream_converter.c
	gcc -c stream_converter.c -I$(KERNEL_INCLUDE_PATH)

packet_analyzer.o: packet_analyzer.h packet_analyzer.c
	gcc -c packet_analyzer.c -I$(KERNEL_INCLUDE_PATH)

section_analyzer.o: section_analyzer.h section_analyzer.c
	gcc -c section_analyzer.c -I$(KERNEL_INCLUDE_PATH)

EPG_manager.o: EPG_manager.h EPG_manager.c
	gcc -c EPG_manager.c -I$(KERNEL_INCLUDE_PATH)
