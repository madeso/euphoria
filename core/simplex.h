/*
 * A speed-improved simplex noise algorithm for 2D, 3D and 4D in Java.
 *
 * Based on example code by Stefan Gustavson (stegu@itn.liu.se).
 * Optimisations by Peter Eastman (peastman@drizzle.stanford.edu).
 * Better rank ordering method for 4D by Stefan Gustavson in 2012.
 *
 * This could be speeded up even further, but it's useful as it is.
 *
 * Version 2012-03-09
 *
 * This code was placed in the public domain by its original author,
 * Stefan Gustavson. You may use it as you see fit, but
 * attribution is appreciated.
 *
 */

#ifndef CORE_SIMPLE_H
#define CORE_SIMPLE_H


// 2D simplex noise
double noise(double xin, double yin);

// 3D simplex noise
double noise(double xin, double yin, double zin);

// 4D simplex noise, better simplex rank ordering method 2012-03-09
double noise(double x, double y, double z, double w);


#endif // CORE_SIMPLE_H

