#!/bin/bash
for D in `seq 0.2 0.2 2`; do ./cubic_spline 1 1 $D 1; done | grep -v "#" > mu_vs_D.dat
for R in `seq 1 0.2 3`; do ./cubic_spline 1 $R 1 1; done | grep -v "#" > mu_vs_R.dat
for E0 in `seq 0.25 0.25 7.5`; do ./cubic_spline $E0 1 1 1; done | grep -v "#" > mu_vs_E0.dat
