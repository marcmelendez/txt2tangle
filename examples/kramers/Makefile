all: tangle code data figures weave

tangle:
	txt2tangle kramers.tex

code:
	gcc -Wall -march=native -Ofast -lm -o cubic_spline cubic_spline.c

data:
	bash cubic_spline.sh

figures:
	gnuplot cubic_spline.gnuplot
	gnuplot mu_vs_D_and_R.gnuplot

weave:
	latex kramers.tex
	dvips kramers.dvi
	ps2pdf kramers.ps
