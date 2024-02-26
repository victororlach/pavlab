#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pav_analysis.h"

int main()
{
    unsigned int N = 5;
    float x[6] = {1.0, -2.0, 3.0, -4.0, 5.0, 6.0};
    float fm = 16000;
    float answer = compute_zcr(x, N, fm);

    printf("%f", answer);
    return 0;
}
