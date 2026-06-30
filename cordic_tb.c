#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define Q15_FRAC_BITS 15
#define Q15_SCALE     (1 << Q15_FRAC_BITS)
#define Q15_ONE       (1 << Q15_FRAC_BITS)
#define CORDIC_ITERS  15
#define PASS_LIMIT    200
#define CORDIC_GAIN   1.6467603

//struct to store name and angle for one test case
typedef struct {
    const char *label;
    double theta_rad;
} CordicTestCase;

//conver floating point value to q15 scaled
static int q15_from_real(double x)
{
    return (int)lround(x * Q15_ONE);
}

// convert q15 scaled integer back to floating point
static double q15_to_double(int x)
{
    return (double)x / Q15_ONE;
}

//build the angle table for CORDIC iterations
static void build_angle_table(int table[CORDIC_ITERS])
{
    int i;

    for (i = 0; i < CORDIC_ITERS; i++) {
        table[i] = q15_from_real(atan(ldexp(1.0, -i)));
    }
}

//perform CORDIC rotation in Q15 format
static void reference_rotate_q15(double theta_rad, int *cos_q15, int *sin_q15)
{
    *cos_q15 = q15_from_real(cos(theta_rad));
    *sin_q15 = q15_from_real(sin(theta_rad));
}

//perform CORDIC rotation in Q15 format
static void cordic_rotate_demo_q15(int theta_q15, int *x_out, int *y_out, int *z_out)
{
    int z_table[CORDIC_ITERS];
    int x;
    int y;
    int z;
    int i;

    //generate micro rotation angle table for CORDIC iterations
    build_angle_table(z_table);

    //init the starting values for CORDIC rotation
    x = Q15_ONE;
    y = 0;
    z = theta_q15;

    //perform CORDIC iterations
    for (i = 0; i < CORDIC_ITERS; i++) {
        //right shift x and y by i to compute the micro-rotation values
        int x_shift = x >> i;
        int y_shift = y >> i;
        int x_next;
        int y_next;

        //rotate counterclockwise if remaining angle is positive, else rotate clockwise
        if (z >= 0) {
            x_next = x - y_shift;
            y_next = y + x_shift;
            z -= z_table[i];
        } else {
            //clockwise rotation if remaining angle is negative
            x_next = x + y_shift;
            y_next = y - x_shift;
            z += z_table[i];
        }

        //return the updated values for the next iteration
        x = x_next;
        y = y_next;
    }

    *x_out = x;
    *y_out = y;
    *z_out = z;
}

//print the cordic angle table
static void print_angle_table(void)
{
    int z_table[CORDIC_ITERS];
    int i;

    build_angle_table(z_table);

    printf("Angle table z[i] = atan(2^-i) in Q15:\n");
    for (i = 0; i < CORDIC_ITERS; i++) {
        printf("z[%2d] = %6d\n", i, z_table[i]);
    }
    printf("\n");
}

//main function to run the test cases
int main(void)
{
    CordicTestCase tests[] = {
        {"zero",          0.0},
        {"small_pos",     0.125},
        {"lecture_angle", 0.729556},
        {"neg_angle",    -0.500000},
        {"pi_over_4",     0.7853981633974483}
    };

    //calculate the number of tests in the array
    int num_tests = (int)(sizeof(tests) / sizeof(tests[0]));
    
    int pass_count = 0;
    int i;

    printf("Rotation-mode CORDIC reference testbench (Q15)\n\n");
    printf("Initial x value used by CORDIC: %d\n", Q15_ONE);
    printf("CORDIC gain correction: divide by %.6f\n\n", CORDIC_GAIN);

    print_angle_table();

    //test result headings
    printf("%-14s %-10s %-8s %-8s %-8s %-8s %-8s %-8s\n","test", "theta_q15", "exp_cos", "act_cos","err_cos", "exp_sin", "act_sin", "err_sin");

    for (i = 0; i < num_tests; i++) {
        int theta_q15;
        int exp_cos;
        int exp_sin;
        int raw_x;
        int raw_y;
        int raw_z;
        int act_cos;
        int act_sin;
        int err_cos;
        int err_sin;

        double cos_est;
        double sin_est;

        //convert the test angle to Q15 scaled integer representation
        theta_q15 = q15_from_real(tests[i].theta_rad);

        //calculate expected cosine and sine values using math.h, then run the CORDIC rotation and compare results
        reference_rotate_q15(tests[i].theta_rad, &exp_cos, &exp_sin);

        //run the CORDIC rotation function and get the raw output values
        cordic_rotate_demo_q15(theta_q15, &raw_x, &raw_y, &raw_z);

        //convert the raw output values back to floating point and divide by the CORDIC gain to get the estimated cosine and sine values
        cos_est = q15_to_double(raw_x) / CORDIC_GAIN;
        sin_est = q15_to_double(raw_y) / CORDIC_GAIN;

        //convert the estimated cosine and sine values back to Q15 scaled integers for error calculation
        act_cos = q15_from_real(cos_est);
        act_sin = q15_from_real(sin_est);

        //calculate the absolute error between the expected and actual values
        err_cos = abs(exp_cos - act_cos);
        err_sin = abs(exp_sin - act_sin);

        //check if the errors are within the acceptable limit and count the number of passed tests
        if (err_cos <= PASS_LIMIT && err_sin <= PASS_LIMIT) {
            pass_count++;
        }

        printf("%-14s %-10d %-8d %-8d %-8d %-8d %-8d %-8d\n",
               tests[i].label, theta_q15, exp_cos, act_cos,
               err_cos, exp_sin, act_sin, err_sin);
    }

    printf("\nPassed %d/%d tests using absolute error limit %d.\n",
           pass_count, num_tests, PASS_LIMIT);

    printf("\nReference note:\n");
    printf("This version matches the main-file style: start with x = 1.0 in Q15,\n");
    printf("run CORDIC, then divide the outputs by the CORDIC gain.\n");

    return 0;
}