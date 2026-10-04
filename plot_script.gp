set title 'POSITIVE Correlation (Linear Regression)' font ',14'
set xlabel 'X Axis (Input Data)' font ',11'
set ylabel 'Y Axis (Target Value)' font ',11'
set grid
set key top left
f(x) = 6.13095 * x + (46.0357)
plot 'data.temp' using 1:2 title 'Actual Data Points' with points pt 7 ps 1.8 lc rgb 'blue', \
     f(x) title 'Regression Line' with lines lw 2.5 lc rgb 'red'
pause -1 'Close graph window to return to console'
