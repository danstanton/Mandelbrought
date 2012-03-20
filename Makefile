CC=g++
CFLAGS=-c -Wall -O2 #-fprofile-arcs -ftest-coverage
LDFLAGS=-lgmpxx -lgmp -O2 #-fprofile-arcs -ftest-coverage
# SOURCES=mb_data.cpp mb_find.cpp mb_img.cpp fractal_image.cpp
# OBJECTS=$(SOURCES:.cpp=.o)
DATAOBJ=mb_data.o fractal_image.o z_and_c_math.o bound_check.o init_and_delete.o path_finder.o
FINDOBJ=mb_find.o
IMGOBJ=mb_img.o

all : MBDat MBImg MBFind

MBDat : $(DATAOBJ)
	$(CC) $(LDFLAGS) $(DATAOBJ) -o $@

MBFind : $(FINDOBJ)
	$(CC) $(LDFLAGS) $< -o $@

MBImg : $(IMGOBJ)
	$(CC) $(LDFLAGS) -lpng $< -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
