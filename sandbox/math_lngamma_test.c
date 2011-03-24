/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// math_lngamma_test.c
//
// compute approximation to log(Gamma(z))
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"

float z[41] = {
      0.00100000,   0.00125890,   0.00158490,   0.00199530, 
      0.00251190,   0.00316230,   0.00398110,   0.00501190, 
      0.00630960,   0.00794330,   0.01000000,   0.01258900, 
      0.01584900,   0.01995300,   0.02511900,   0.03162300, 
      0.03981100,   0.05011900,   0.06309600,   0.07943300, 
      0.10000000,   0.12589000,   0.15849000,   0.19953000, 
      0.25119000,   0.31623000,   0.39811000,   0.50119000, 
      0.63096000,   0.79433000,   1.00000000,   1.25890000, 
      1.58490000,   1.99530000,   2.51190000,   3.16230000, 
      3.98110000,   5.01190000,   6.30960000,   7.94330000, 
     10.00000000};

float lngamma_test[41] = {
      6.90720000,   6.67680000,   6.44630000,   6.21580000, 
      5.98530000,   5.75460000,   5.52390000,   5.29310000, 
      5.06210000,   4.83090000,   4.59950000,   4.36780000, 
      4.13570000,   3.90320000,   3.67010000,   3.43640000, 
      3.20190000,   2.96640000,   2.72990000,   2.49200000, 
      2.25270000,   2.01200000,   1.76980000,   1.52660000, 
      1.28300000,   1.04050000,   0.80154000,   0.57004000, 
      0.35224000,   0.15757000,   0.00000000,  -0.10025000, 
     -0.11440000,  -0.00199580,   0.29308000,   0.84799000, 
      1.76800000,   3.19600000,   5.32420000,   8.41110000, 
     12.80200000};

float sandbox_lngammaf(float _z,
                       float * _v)
{
    float v0 = _v[0];
    float v1 = _v[1];

    // z = 10.^[-3:0.1:1];
    // t0 = log(gamma(z));
    // t1 = (1+z-0.5).*log(1+z) - (1+z) + 0.5*log(2*pi) - log(z) + 0.0405*(1-tanh(0.5*log(z)));
    //
    float g_hat = (0.5f+_z)*logf(1.0f+_z) - (1.0f+_z) + 0.5f*logf(2*M_PI) - logf(_z) +
           v0*(1.0f - tanhf(v1*logf(_z)));

    return g_hat;
}

// gradient search curve-fit error
float gserror(void * _userdata,
              float * _v,
              unsigned int _n)
{
    // compute error
    float rmse = 0.0f;
    unsigned int i;
    for (i=0; i<41; i++) {
        float g = sandbox_lngammaf(z[i], _v);
        float e = g - lngamma_test[i];
        rmse += e*e;
    }
    return sqrtf(rmse / 41.0f);
}

int main() {
    unsigned int num_iterations = 1000; // number of iterations to run
    float v[2] = {0.0405f, 0.5f};       // 

    gradient_search gs = gradient_search_create_advanced(
            NULL,
            v,
            2,
            1e-6f,  // delta : gradient approximation step size
            0.002f, // gamma : vector step size
            0.1f,   // alpha : momentum parameter
            0.999f, // mu    : decremental gamma paramter (best if not exactly 1.0)
            gserror,
            LIQUID_OPTIM_MINIMIZE);

    // execute search one iteration at a time
    unsigned int i;
    float rmse;
    for (i=0; i<num_iterations; i++) {
        rmse = gserror(NULL,v,2);

        gradient_search_step(gs);

        if (((i+1)%100)==0)
            gradient_search_print(gs);
    }

    gradient_search_destroy(gs);

    // print results
    for (i=0; i<41; i++)
        printf(" z = %12.8f, g = %12.8f (%12.8f)\n", z[i], lngamma_test[i], sandbox_lngammaf(z[i], v));
    printf("rmse = %12.4e;\n", rmse);

    printf("v0 = %12.8f\n", v[0]);
    printf("v1 = %12.8f\n", v[1]);

    printf("done.\n");
    return 0;
}

