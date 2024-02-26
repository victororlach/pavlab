#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "pav_analysis.h"
#include "fic_wave.h"

int main(int argc, char *argv[])
{
    float durTrm = 0.010;
    float fm;
    int N;
    int trm;
    float *x;
    short *buffer;
    FILE *fpWave;

    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Empleo: %s inputfile [outputfile]\n", argv[0]);
        return -1;
    }

    if ((fpWave = abre_wave(argv[1], &fm)) == NULL)
    {
        fprintf(stderr, "Error al abrir el fichero WAVE de entrada %s (%s)\n", argv[1], strerror(errno));
        return -1;
    }

    // N = durTrm * fm; <-- N for non extra task
    if ((buffer = malloc(N * sizeof(*buffer))) == 0 ||
        (x = malloc(N * sizeof(*x))) == 0)
    {
        fprintf(stderr, "Error al ubicar los vectores (%s)\n", strerror(errno));
        return -1;
    }

    // beginning of actual code
    trm = 0;
    // while (lee_wave(buffer, sizeof(*buffer), N, fpWave) == N)
    // {
    //     for (int n = 0; n < N; n++)
    //         x[n] = buffer[n] / (float)(1 << 15);

    //     printf("%d\t%f\t%f\t%f\n", trm, compute_power(x, N),
    //            compute_am(x, N),
    //            compute_zcr(x, N, fm));
    //     trm += 1;
    // }

    //***EJERCICIO AMPLIACION CODIGO***

    float T_Long = 0.02;                 // duration of hamming window
    N = T_Long * fm;                     // duration in samples of hamming window
    unsigned int M_displacement = N / 2; // displacement in samples of hamming window
    float *hamming_window;
    printf("line 57");
    // hamming_window generation
    for (unsigned int n = 0; n < N; ++n)
    {
        hamming_window[n] = 0.54 - 0.46 * cos(6.28 * n / (N - 1)); // 6.28 = 2*pi (aprox)
    }

    while (lee_wave(buffer, sizeof(*buffer), N, fpWave) == N)
    {
        // if its first iteration, extract new N block, otherwise work with what we got
        if (trm == 0)
        {
            for (int n = 0; n < N; n++)
                x[n] = buffer[n] / (float)(1 << 15);

            printf("%d\t%f\t%f\t%f\n", trm, compute_hamming_power(x, hamming_window, N),
                   compute_am(x, N),
                   compute_zcr(x, N, fm));
            trm += 1;
        }
        else
        {
            int iteration = 0;

            // rerun 2 times
            while (iteration < 2)
            {

                // flip the x array
                float aux[N / 2];

                for (int i = 0; i < N / 2; i++)
                {
                    aux[i] = x[N / 2 + i];
                }

                // update x by shifting last N/2 elements down to first N/2 elements
                memcpy(x, &aux, sizeof(aux) / sizeof(float)); // sizeof(aux) / sizeof(float) = size of array

                for (int n = 0; n < N / 2; n++)
                    x[n + N / 2] = buffer[n * (iteration * N / 2)] / (float)(1 << 15);

                printf("%d\t%f\t%f\t%f\n", trm, compute_hamming_power(x, hamming_window, N),
                       compute_am(x, N),
                       compute_zcr(x, N, fm));
                trm += 1;
                iteration++;
            }
        }
    }

    //***FIN EJERCICIO AMPLIACIÓN CÓDIGO***

    cierra_wave(fpWave);
    free(buffer);
    free(x);

    return 0;
}
