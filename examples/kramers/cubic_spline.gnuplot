set term postscript eps enhanced size 7cm,4cm font "Roman"
set output "cubic_spline.eps"
set xlabel "{/:Italic r / R}"
set ylabel "{/:Italic V}({/:Italic r/R}) {/:Italic / E}_0"
plot [0:1.5][-1.2:0.2] (x<1)?-2*x**3 + 3*x**2 - 1:0 w l lw 3 lc -1 notitle, \
                       0 w l lc -1 dt 2 notitle
