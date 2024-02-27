#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N)
{
    /*---INPUTS---

        (1) *x -> float array del qual extreiem els float values per poder calcular la potencia

        (2) N -> integer positiu que ens diu el nombre de valors que extreure i multiplicar del float array x

    ---OUTPUTS---

        (1) el valor de potència mitjana en dBs (com un float)

    ---DESCRIPCIO---

        (1) Multiplica tots els elements (almenys el N primers) del float array x amb sí mateixos (operació d'elevar al quadrat), fa el sumatori
        i ho passa a dBs. Normalment els valors que treu aquesta funció, almenys amb el PAV_P1.wav d'aquesta pràctica, haurien de rondar
        entre els -40 dB -- -90 dB (sobretot ser negatius)
    */
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
    /*---INPUTS---

        (1) *x -> float array del senyal principal

        (2) *hamming -> float array dels coeficients de la finestra de Hamming

        (3) N -> Integer positiu que representa el nombre de multiplicacions a fer

    ---OUTPUTS---

        (1) valor de potència mitjana resultant en dBs

    ---DESCRIPCIO---

        Implementació de la funció expressa en el document d'exercisi 2 d'ampliació de la pràctica
    */
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
    /*---INPUTS---

        (1) *x -> float array que conté els valors del senyal sobre el que operar

        (2) N -> Integer positiu que representa el nombre de multiplicacions a fer

    ---OUTPUTS---

        (1) resultat de la operacio (valor mitja del valor absolut del senyal x)

    ---DESCRIPCIO---

        Implementacio de la segona funcio (calcul del valor mitja del valor absolut del senyal) de les funcions del document de pràctiques
    */
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
    /*--INPUTS---

        1) *x -> float array que conté els valors del senyal sobre el que operar

        (2) N -> Integer positiu que representa el nombre de multiplicacions a fer

        (3) fm -> valor float utilitzat en la operacio (sample rate)

    --OUTPUTS---

        (1) resultat de la operacio (nombre de 0 crossings trobats en x)

    ---DESCRIPTION---

        Impleemntacio de la tercera funcio del document de practiques
    */
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
