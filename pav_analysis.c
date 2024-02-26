#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N)
{
    // Calculate the squared sum of the first N values in the array
    float squared_sum = 0.0;
    for (unsigned int i = 0; i < N; ++i)
    {
        squared_sum += x[i] * x[i];
    }

    // Divide the squared sum by N
    float average = squared_sum / N;

    // Convert the average to dB using 10 * log10()
    float result_dB = 10.0 * log10(average);

    return result_dB;
}

float compute_hamming_power(const float *x, const float *hamming, unsigned int N)
{
    // Initialize the numerator and denominator sums
    float numerator_sum = 0.0;
    float denominator_sum = 0.0;

    // Perform the summation over the first N elements of the arrays
    for (unsigned int n = 0; n < N; ++n)
    {
        float term = x[n] * hamming[n];
        numerator_sum += term * term;
        denominator_sum += hamming[n] * hamming[n];
    }

    // Calculate the final result using 10 * log10()
    float result = 10.0 * log10(numerator_sum / denominator_sum);

    return result;
}

float compute_am(const float *x, unsigned int N)
{
    // Calculate the sum of absolute values of the first N elements in the array
    float absolute_sum = 0.0;
    for (unsigned int i = 0; i < N; ++i)
    {
        absolute_sum += fabs(x[i]);
    }

    // Divide the absolute sum by N
    float result = absolute_sum / N;

    return result;
}

float compute_zcr(const float *x, unsigned int N, float fm)
{
    // Initialize the sum of sign changes
    int sign_change_sum = 0;

    // Calculate the sum of sign changes for n going from 0 to N-1
    for (unsigned int n = 1; n < N; ++n)
    {
        if ((x[n] >= 0 && x[n - 1] < 0) || (x[n] < 0 && x[n - 1] >= 0))
        {
            // If the sign of consecutive values is different, increment the sum
            sign_change_sum++;
        }
    }

    // Calculate the final result
    float result = (float)sign_change_sum * fm / (2 * (N - 1));

    return result;
}
