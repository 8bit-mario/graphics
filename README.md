# Interactive Double Pendulum
Simulation of Double Pendulum. Features click and drag and some buttons.

# Requirement
Need Cairo Library

Need SDL2 Library

# Compiling
download this repository:

git clone https://github.com/8bit-mario/graphics.git

cd graphics

compile with:

gcc double_pend_app.c solver.c -lSDL2 -lcairo -lm -o pendulum

and
run with ./pendulum

# Mandelbrot Set
It's a set of complex number for which function f(z) = z^2 + c does not diverge when iterated from z = 0.
Currently the color palette, with high iteration count produce unpleasing image. Histogram equalization and other
smoothing technique will be implemented later. This program create an image of 800 x 600 pixel plot of mandelbrot, uses hsv_to_rgb converter and vary saturation according to iteration value.

# Compiling
gcc mandel.c graphics.c -lm -o mandel

running ./mandel will produce mandel.ppm file which can be view by image viewer application.

# Double Pendulum using cairo rendering engine
The two double pendulum start at nearly identical position, one is slightly displace from other.
RK4 integrator is use for solving double pendulum equation. Integration step size is 60fps or 0.167 second.

# Dependency
You will need cairo library

# Compiling
compile:
gcc solver.c double_pend.cairo.c -lm -lcairo -o test

and run ./test

This will create 5000 frame of animation

You can use ffmpeg command to create video
# Double Pendulum
Solving double pendulum.

# Compiling

compile:
gcc graphics.c solver.c double_pendulum.c -lm -o test

and run ./test

This creates sequence of files named test000.ppm ... test999.ppm

Creating video

You can use ffmpeg command to create video of sequence of images.
# Introduction
A program to create sierpinski triangle.
It use random number to create sierpinski triangle.

# Compiling
Download this repository.

compile:
gcc sierpinski.c -o sierpinski

and run ./sierpinski
 
This creates "sierpinski_triangle.ppm" file by default size of image is 1000x1000
