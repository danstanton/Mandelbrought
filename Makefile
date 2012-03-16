all : MBDat MBImg MBFind MBSeries

MBDat : mb_data.cpp
	g++ $< -o $@ -Wall -pipe -lgmpxx -lgmp -ggdb

MBFind : mb_find.cpp
	g++ $< -o $@ -Wall -pipe -lgmpxx -lgmp 

MBImg : mb_img.cpp
	g++ $< -o $@ -Wall -pipe -lpng -ggdb

MBSeries : mb_series.cpp
	g++ $< -o $@ -Wall -pipe -lgmpxx -lgmp 

