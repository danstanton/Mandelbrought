all : MBDat MBImg MBFind MBSeries

MBDat : mb_data.cpp
	g++ $< -o $@ -Wall -pipe -O3 -lgmpxx -lgmp 

MBFind : mb_find.cpp
	g++ $< -o $@ -Wall -pipe -O3 -lgmpxx -lgmp 

MBImg : mb_img.cpp
	g++ $< -o $@ -Wall -pipe -O3 -lpng -lz 

MBSeries : mb_series.cpp
	g++ $< -o $@ -Wall -pipe -O3 -lgmpxx -lgmp 
