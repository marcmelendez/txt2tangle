/* sine.c sin(x) values in the interval [0, 2 pi] */

# include <stdio.h>
# include <math.h>

/* Makefile to compile this code
%! codefile: Makefile
all:
	gcc -Wall -o sine sine.c -lm
	./sine > sin.dat
	gnuplot sine.gnuplot
%! codeend
*/

int main(int argc, char * argv[])
{
  /* Output data for a sine function in the interval [0, 2 pi] */
  float x;
  for(x = 0; x < 2*M_PI; x += 0.02) printf("%f %f\n", x, sin(x));

  /* Gnuplot code to represent the function graphically
     (run with gnuplot sine.gnuplot)

     %! codefile: sine.gnuplot
     set title "Sine function"
     set xlabel "x"
     set ylabel "sin(x)"
     plot [0:2*pi] "sin.dat" w l notitle
     pause -1
     %! codeend
  */
  return 0;
}
