LDLIBS = -lm # <-- only necessary for Ubuntu users (mostly, although macos can benefit sometimes)

p1: p1.o pav_analysis.o fic_wave.o

p1.o: pav_analysis.h fic_wave.h 

pav_analysis.o: pav_analysis.h

fic_wave.o: fic_wave.h

#experimental config
main: main.o pav_analysis.o fic_wave.o
main.o: pav_analysis.h fic_wave.h