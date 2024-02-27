#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "pav_analysis.h"
#include "fic_wave.h"

int main(int argc, char *argv[])
{

    float T_Long = 0.02;  // duration of hamming window
    float durTrm = 0.010; // valor arbitrari donat ja d'entrada
    float fm;             // sample rate (s'ha de llegir del .wav)
    int trm = 0;          // initial block number
    float *x;             // array del bloc a analitzar
    short *buffer;        // array on es depositen els valors llegits del .wav
    FILE *fpWave;         // punter al fitxer .wav donat d'entrada al CLI

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

    int N = T_Long * fm;                 // block size
    unsigned int M_displacement = N / 2; // displacement in samples of hamming window
    float hamming_window[N];             // hamming window array
    // hamming_window generation
    for (unsigned int n = 0; n < N; ++n)
    {
        hamming_window[n] = 0.54 - 0.46 * cos(6.28 * n / (N - 1)); // 6.28 = 2*pi (aprox) <-- from the internet
    }

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
    while (lee_wave(buffer, sizeof(*buffer), N, fpWave) == N)
    {
        // if its first iteration, extract new N block, otherwise work with what we got
        if (trm == 0)
        {
            // copy buffer into x
            for (int n = 0; n < N; n++)
                x[n] = buffer[n] / (float)(1 << 15);

            printf("%d\t%f\n", trm, compute_hamming_power(x, hamming_window, N));
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
                memcpy(x, aux, sizeof(aux) / sizeof(float)); // sizeof(aux) / sizeof(float) = size of array

                for (int n = 0; n < N / 2; n++)
                    x[n + N / 2] = buffer[n + (iteration * N / 2)] / (float)(1 << 15);

                printf("%d\t%f\n", trm, compute_hamming_power(x, hamming_window, N));
                trm += 1;
                iteration++;
            }
        }
    }

    cierra_wave(fpWave);
    free(buffer);
    free(x);

    return 0;
}
