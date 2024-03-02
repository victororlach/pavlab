#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include "pav_analysis.h"
#include "fic_wave.h"

int main(int argc, char *argv[])
{
    // inicialitzacio variables
    float durTrm = 0.010; // valor arbitrari donat ja d'entrada
    float fm;             // sample rate (s'ha de llegir del .wav)
    int N;                // tamany dels blocs sobre els que operarem
    int trm;              // nombre del bloc
    bool *stereo;         // whether .wav is mono or stereo (true for stereo)
    float *x;             // array del bloc a analitzar
    short *buffer;        // array on es depositen els valors llegits del .wav
    FILE *fpWave;         // punter al fitxer .wav donat d'entrada al CLI
    FILE *output_file;    // només inicialitzat quan s'executa el programa amb 2 arguments, el nom del fitxer de sortida on aniran guardades
    // totes les mètriques

    // comprovem que ens han donat el nom dels fitxers necessaris
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Empleo: %s inputfile [outputfile]\n", argv[0]);
        return -1;
    }

    // check whether output file has been given as input (and create it if yes)
    if (argv[2] && (fopen("data_metrics.txt", "w") != NULL))
    {
        // file already exists, remove & create new one
        remove("data_metrics.txt");                   // should return 0 if operation is succesful
        output_file = fopen("data_metrics.txt", "w"); // create the same file but this time empty
    }

    // extreure el valor de fm del .wav
    if ((fpWave = abre_wave(argv[1], &fm)) == NULL)
    {
        fprintf(stderr, "Error al abrir el fichero WAVE de entrada %s (%s)\n", argv[1], strerror(errno));
        return -1;
    }

    N = durTrm * fm; //<-- assignacio de valor a N
    printf("valor de N: %d", N);

    // assignacio de lloc en memoria de <buffer> i <x>
    if ((buffer = malloc(N * sizeof(*buffer))) == 0 ||
        (x = malloc(N * sizeof(*x))) == 0)
    {
        fprintf(stderr, "Error al ubicar los vectores (%s)\n", strerror(errno));
        return -1;
    }

    // beginning of actual code
    trm = 0;                                                  // el primer bloc
    while (lee_wave(buffer, sizeof(*buffer), N, fpWave) == N) // mentre sempre anem extreient N valors del .wav, podem anar fent blocs sencers
    {
        // traspas dels valor de <buffer> a <x>
        for (int n = 0; n < N; n++)
            x[n] = buffer[n] / (float)(1 << 15);

        // check whether program has been confingured for CLI writing or .txt writing
        if (argv[2])
        {
            // we want to write to output_file, which has already been created
            fprintf(output_file, "%d\t%f\t%f\t%d\n", trm, compute_power(x, N),
                    compute_am(x, N),
                    (int)compute_zcr(x, N, fm));
        }
        else // otherwise just CLI print
        {
            printf("%d\t%f\t%f\t%f\n", trm, compute_power(x, N),
                   compute_am(x, N),
                   compute_zcr(x, N, fm));
        }
        trm += 1; // seguent bloc
    }

    // material clean-up
    cierra_wave(fpWave);
    free(buffer);
    free(x);

    // close the .txt if has been created
    // if (output_file != NULL)
    // {
    //     fclose(output_file);
    // }

    return 0;
}
