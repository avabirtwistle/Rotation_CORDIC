#include <math.h>
#include <stdio.h>

#include "cordic_R_fixed_point.h"
#include "z_table.h"

/**
 * Verifies the CORDIC rotation result against reference cosine and sine values
 *
 * @param theta_i_init Initial rotation angle in Q15-scaled radians
 * @param cos_i_ref    Reference cosine value in Q15
 * @param sin_i_ref    Reference sine value in Q15
 * @param x_i          Final CORDIC x-coordinate
 * @param y_i          Final CORDIC y-coordinate
 * @param z_i          Final residual angle in Q15-scaled radians
 */
static void verify(int theta_i_init, int cos_i_ref, int sin_i_ref,int x_i,int y_i,int z_i)
{
    //conver the original q15 scaled angle back to radians
    double theta_d_init = (double)theta_i_init / CORDIC_ONE_Q15;
    
    //convert the q15 reference cosine and sine values back to floating point
    double cos_d_ref = (double)cos_i_ref / CORDIC_ONE_Q15;
    double sin_d_ref = (double)sin_i_ref / CORDIC_ONE_Q15;
    
    //convert the raw cordic output back to floatingpoint
    double x_d = (double)x_i / CORDIC_ONE_Q15;
    double y_d = (double)y_i / CORDIC_ONE_Q15;
    double z_d = (double)z_i / CORDIC_ONE_Q15;
    
    //divide out the processing gain from the output
    double cos_est = x_d / CORDIC_GAIN_FLOAT;
    double sin_est = y_d / CORDIC_GAIN_FLOAT;
    
    // find the error between the output and the reference values (the ones usign math.h)
    double cos_err = cos_est - cos_d_ref;
    double sin_err = sin_est - sin_d_ref;

    printf("theta_i_init = %5d\ttheta_d_init = %.6f (rad)\n\n",
           theta_i_init, theta_d_init);

    printf("Reference (math.h, Q15):\n");
    printf("cos_i_ref = %5d\tcos_d_ref = %.6f\n", cos_i_ref, cos_d_ref);
    printf("sin_i_ref = %5d\tsin_d_ref = %.6f\n\n", sin_i_ref, sin_d_ref);

    printf("CORDIC rotation output (scaled by gain A[n]):\n");
    printf("x_i_calc = %5d\tx_d_calc = %.6f\n", x_i, x_d);
    printf("y_i_calc = %5d\ty_d_calc = %.6f\n", y_i, y_d);
    printf("z_i_calc = %5d\tz_d_calc = %.6f (should approach 0)\n\n", z_i, z_d);

    printf("After gain correction (divide by %.6f):\n", CORDIC_GAIN_FLOAT);
    printf("cos_est  = %.6f\tcos_err = %.6e\n", cos_est, cos_err);
    printf("sin_est  = %.6f\tsin_err = %.6e\n", sin_est, sin_err);
}

int main(void)
{   //angle from the lecture slides in radians
    const double theta_rad = 0.729556;
    int theta_i_init;
    int cos_i_ref;
    int sin_i_ref;
    int x_i;
    int y_i;
    int z_i;

    // convert the angle to Q15 scaled integer representation and compute reference cosine and sine values
    theta_i_init = (int)lround(theta_rad * CORDIC_ONE_Q15);
    cos_i_ref = (int)lround(cos(theta_rad) * CORDIC_ONE_Q15);
    sin_i_ref = (int)lround(sin(theta_rad) * CORDIC_ONE_Q15);

    x_i = CORDIC_ONE_Q15;
    y_i = 0;
    z_i = theta_i_init;

    //call the CORDIC rotation function and verify the results
    printf("Rotation-mode CORDIC:\n\n");
    cordic_R_fixed_point(&x_i, &y_i, &z_i);
    verify(theta_i_init, cos_i_ref, sin_i_ref, x_i, y_i, z_i);

    return 0;
}