set term postscript eps enhanced size 8cm,4cm font "Roman"
set output "mu_vs_D_and_R.eps"
set ylabel "{/Symbol m} [a.u.]"
set size square

f(x) = a*x
fit f(x) "mu_vs_D.dat" u 3:5 via a
g(x) = b/(x**2)
fit g(x) "mu_vs_R.dat" u 2:5 via b

set multiplot layout 1,2
set xlabel "{/:Italic D} [a.u.]"
plot [0:2][0:6] "mu_vs_D.dat" u 3:5:6 w errorbars notitle lc -1, \
                f(x) w l dt 2 lc -1 notitle
set xlabel "{/:Italic R} [a.u.]"
plot [0:3][0:5] "mu_vs_R.dat" u 2:5:6 w errorbars notitle lc -1, \
                g(x) w l dt 2 lc -1 notitle
unset multiplot
