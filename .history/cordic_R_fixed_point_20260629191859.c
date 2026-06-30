/*
 *rotation mode CORDIC algorithm in fixed point Q15 format
* inputs: x - A[n]*cos(theta), y - A[n]*sin(theta), z - theta (desired rotation angle)
 * Outputs: x - A[n]*cos(theta), y - A[n]*sin(theta), z - 0
 *
 * sigma[i] = +1 if z[i] >= 0, else -1
 */
void cordic_R_fixed_point(int *x, int *y, int *z)
{
    //copy input values into local variables
    int x_temp = *x;
    int y_temp = *y;
    int z_temp = *z;

    //store new x and y values after each iteration
    int x_next;
    int y_next;
    int i;

    //perform CORDIC iterations, 0 to 14
    for (i = 0; i < CORDIC_ITERATIONS; i++) {
        //if remaining angle is positive, rotate clockwise(positive direction) and subtract current angle
        if (z_temp >= 0) {
            x_next = x_temp - (y_temp >> i);
            y_next = y_temp + (x_temp >> i);
            z_temp -= z_table[i];
        } else {//remaining angle is negative so go in opposite direction and add the current angle back to z_temp
            x_next = x_temp + (y_temp >> i);
            y_next = y_temp - (x_temp >> i);
            z_temp += z_table[i];
        }
        //update coordinates for next iteration
        x_temp = x_next;
        y_temp = y_next;
    }

    //final output values are stored back into the input pointers
    *x = x_temp;
    *y = y_temp;
    *z = z_temp;
}
