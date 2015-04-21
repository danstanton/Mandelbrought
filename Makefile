CC=g++
CFLAGS=-c -g -Wall -march=native #-fprofile-arcs -ftest-coverage
LDFLAGS=-g -lgmpxx -lgmp -lmpfr -march=native #-fprofile-arcs -ftest-coverage
# SOURCES=mb_data.cpp mb_find.cpp mb_img.cpp fractal_image.cpp
# OBJECTS=$(SOURCES:.cpp=.o)
DATAOBJ=mb_data.o fractal_image.o z_and_c_math.o bound_check.o init_and_delete.o read_in_file.o path_finder.o
FINDOBJ=mb_find.o read_in_file.o
IMGOBJ=mb_img.o

all : MBDat MBImg MBFind

MBDat : $(DATAOBJ)
	$(CC) $(DATAOBJ) -o $@ $(LDFLAGS) 

MBFind : $(FINDOBJ)
	$(CC) $< -o $@ $(LDFLAGS)

MBImg : $(IMGOBJ)
	$(CC) $< -o $@ -lpng $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
