set term postscript eps enhanced size 8cm,4cm font "Roman"
set output "mu_vs_E0.eps"
set ylabel "{/Symbol m} [a.u.]"
set xlabel "E_0 / (k_B T)"

mu(x) = 2.5*(x**2)*exp(-x)

set multiplot
plot [0:10][0:4] "mu_vs_E0.dat" u 1:5:6 w errorbars notitle lc -1, \
                 mu(x) w l dt 2 lc -1 notitle
set size 0.55, 0.55
set origin 0.4, 0.4
set logscale y
unset xlabel
unset ylabel
plot [0:10][:4] "mu_vs_E0.dat" u 1:5:6 w errorbars notitle lc -1, \
                 mu(x) w l dt 2 lc -1 notitle
unset multiplot
