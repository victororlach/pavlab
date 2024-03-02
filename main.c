#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include "pav_analysis.h"
#include "fic_wave.h"

void stereo_compute(FILE *output_file, float *x, float *hamming_window, float *left_samples, float *right_samples, int N, int *trm);
void mono_compute(FILE *output_file, float *x, float *hamming_window, int N, int *trm);

int main(int argc, char *argv[])
{
    float T_Long = 0.02;  // duration of hamming window (donat per l'enunciat)
    float durTrm = 0.010; // valor arbitrari donat ja d'entrada
    float fm;             // sample rate (s'ha de llegir del .wav)
    int trm = 0;          // initial block number
    float *x;             // array del bloc a analitzar
    short *buffer;        // array on es depositen els valors llegits del .wav
    FILE *fpWave;         // punter al fitxer .wav donat d'entrada al CLI
    FILE *output_file;    // només s'inicialitza si l'usuari crida l'executable amb 2 arguments
    bool stereo;          // are we reading in mono or stereo mode

    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Empleo: %s inputfile [outputfile]\n", argv[0]);
        return -1;
    }

    // check whether output file has been given as input (and create it if yes)
    if (argv[2])
    {
        // file already exists, remove & create new one
        remove("data_ampliacion.txt");                   // should return 0 if operation is succesful
        output_file = fopen("data_ampliacion.txt", "w"); // create the same file but this time empty
    }

    if ((fpWave = abre_wave(argv[1], &fm)) == NULL)
    {
        fprintf(stderr, "Error al abrir el fichero WAVE de entrada %s (%s)\n", argv[1], strerror(errno));
        return -1;
    }

    int N = T_Long * fm; // block size (formula document)
    // comentar origen de la formula
    unsigned int M_displacement = N / 2; // displacement in samples of hamming window -> if N=T_Long * fm and M = T_desp * fm and T_desp = T_long/2
    // a simple substitution leads to the equaliyu that M = N/2, so that's how its hard-coded (formulas extracted from documentation)
    float hamming_window[N]; // hamming window array
    // hamming_window generation
    for (unsigned int n = 0; n < N; ++n)
    {
        hamming_window[n] = 0.54 - 0.46 * cos(6.28 * n / (N - 1)); // 6.28 = 2*pi (aprox) <-- coefficients from the internet
    }
    /*origin of coefficients:

        the general formula of the Hamming window is as follows:

            w[n] = a0 - (1-a0)cos((2pin/(N-1)))

        and setting a0 = 0.54 yields the above equation*/

    if ((buffer = malloc(N * sizeof(*buffer))) == 0 ||
        (x = malloc(N * sizeof(*x))) == 0)
    {
        fprintf(stderr, "Error al ubicar los vectores (%s)\n", strerror(errno));
        return -1;
    }

    // beginning of actual code

    /*EXPLICACIO DEL CODI A CONTINUACIO

    Hem d'implementar l'algoritme que calculi la potencia promitg pero ara desplaçant-li una finestra de Hamming tota l'estona. La finestra de
    Hamming té duracio N i desplaçament continu cap a la dreta de N/2. Per tant, a l'inici, agafem un bloc sencer de tamany N, i l'apliquem sobre
    els primers N elements del senyal. Aixo es fa en el primer if (if(trm==0)...). Despres, ja no tornarem a visitar aquest if(), sino que
    entrarem en els else. Agafem un nou bloc de tamany N del senyal, i desplacem el finestra de Hamming N/2 cap a la dreta. Per tant,
    ara la primera meitat de la finestra de Hamming està situada sobre els ultims N/2 elements del bloc anterior i l'altre meitat superior esta
    situada sobre els primers N/2 elements del nou bloc de tamany N. Tot aixo s'implementa amb les linies 81-87. Un cop hem fet el calcul de les
    metriques amb aquests N elements, NO TORNEM A AGAFAR UN NOU BLOC DE N ELEMENTS perquè encara no fa falta. Ara la finestra de Hamming se situa a sobre,
    i de forma sencera, els N elements del bloc de tamany N que hem agafat, i tornem a calcular les metriques. Ara bé, tornem a desplacar la puta finestra, per tant
    ara sí surtim del while(iteration < 2) i tornem a agafar un nou bloc de N elements. El proces es torna a repetir del tot. Espero que s'entengui :)*/

    // Create arrays to store the even samples (left_samples) and the odd samples (right_samples)
    float *left_samples = malloc(N / 2 * sizeof(float));
    float *right_samples = malloc(N / 2 * sizeof(float));
    stereo = true;
    output_file = NULL;
    printf("%d", trm);
    while (lee_wave(buffer, sizeof(*buffer), N, fpWave) == N)
    {
        // if its first iteration, extract new N block, otherwise work with what we got
        if (trm == 0)
        {
            // apply normalization
            for (unsigned int i = 0; i < N; ++i)
            {
                x[i] = buffer[i] / (float)(1 << 15);
            }

            if (stereo)
            {
                printf("%d", trm);
                stereo_compute(output_file, x, hamming_window, left_samples, right_samples, N, &trm); // used when working with stereo
            }
            else
            {
                mono_compute(output_file, x, hamming_window, N, &trm); // used when working with mono
            }
            trm++;
        }
        break;
        // else
        // {
        //     int iteration = 0;

        //     // rerun 2 times
        //     while (iteration < 2)
        //     {
        //         // flip the x array
        //         float aux[M_displacement];

        //         for (int i = 0; i < M_displacement; i++)
        //         {
        //             aux[i] = x[M_displacement + i];
        //         }

        //         // update x by shifting last N/2 elements down to first N/2 elements
        //         memcpy(x, aux, sizeof(aux) / sizeof(float)); // sizeof(aux) / sizeof(float) = size of array

        //         for (int n = 0; n < M_displacement; n++)
        //             x[n + M_displacement] = buffer[n + (iteration * M_displacement)] / (float)(1 << 15);

        //         if (stereo)
        //         {
        //             // do stereo stuff
        //             stereo_compute(output_file, x, hamming_window, left_samples, right_samples, N, &trm);
        //             // printf(stereo ? "yes" : "no");
        //         }
        //         else
        //         {
        //             // do mono stuff
        //             mono_compute(output_file, x, hamming_window, N, &trm);
        //         }
        //         trm++;
        //         iteration++;
        //     }
        // }
    }

    cierra_wave(fpWave);
    free(buffer);
    free(x);

    // free memory
    free(left_samples);
    free(right_samples);

    return 0;
}

/*computes the left and right channel power values & prints them to file to CLI*/
void stereo_compute(FILE *output_file, float *x, float *hamming_window, float *left_samples, float *right_samples, int N, int *trm)
{

    //  parse x into split arrays depending on mono or stereo
    for (unsigned int i = 0; i < N / 2; ++i)
    {
        left_samples[i] = x[2 * i];
        right_samples[i] = x[2 * i + 1];
    }
    // PROBLEM IS HERE
    if (output_file != NULL) //.txt printing
    {
        // create 2 floats to store each channel's power
        float left_channel_power;
        float right_channel_power;

        compute_hamming_power(hamming_window, N, 2, left_samples, right_samples, &left_channel_power, &right_channel_power);
        fprintf(output_file, "%d\t%f\t%f\n", *trm, left_channel_power, right_channel_power);
    }
    else // CLI priting
    {
        // printf("%d", *trm);
        //  create 2 floats to store each channel's power
        float left_channel_power;
        float right_channel_power;
        compute_hamming_power(hamming_window, N, 2, left_samples, right_samples, &left_channel_power, &right_channel_power);
        // printf("%d\t%f\t%f\n", *trm, left_channel_power, right_channel_power);
    }
}

void mono_compute(FILE *output_file, float *x, float *hamming_window, int N, int *trm)
{

    if (output_file != NULL) //.txt printing
    {
        fprintf(output_file, "%d\t%f\n", *trm, compute_hamming_power(hamming_window, N, 1, x));
    }
    else // CLI priting
    {
        printf("%d\t%f\n", *trm, compute_hamming_power(hamming_window, N, 1, x));
    }
}
