#include <stdio.h>
#include <stdbool.h>
#include "fic_wave.h"

FILE *abre_wave(const char *ficWave, float *fm, bool *stereo)
{
    /*---INPUTS---:

       (1) *ficWave -> (és un string o char[]) punter al nom del fitxer que hem de llegir
        (2) *fm -> (float) punter a la variable de p1.c que guarda el sample rate (freqüència de mostratge)

    ---OUTPUTS---:

        (1) En cas d'exit -> punter al fitxer PAV_P1.wav (apuntant ja a la posició d'inici de les dades de so (offset 44))
        (2) En cas d'error -> NULL

    ---DESCRICPIÓ---

    Extreure diversos valors de metadata del fitxer amb nom *ficWave (Format d'Audio (PCM, ...) # de canals (mono, stereo, ...) & sampling rate (fm))

        Totes les accions comentades son comprovades per possibles errors, és a dir, existeix control d'errors

        (1) Comenca obrint el fitxer, posicionant el cursor de lectura a la  posició 20.

        (2) Llegeix el primers 4 bytes, per extreure el Audio Format i # de canals

        (3) Comprova que aquests  paràmetres siguin del format esperat (pot canviar aquesta funcionalitat en el futur)

        (4) Extreu el fm, posicionant el cursor a la posicio 24 (offset 24) i llegeix el pròxims 4 bytes, guardant-ho en la variable buffer

        (5) Acaba posant el cursor del punter FILE al offset 44 (on comencen les dades del senyal), assigna a *fm el valor de buffer (per accedir
        al valor desde p1.c) i retorna el punter *fpWave per també poder usar en p1.c*/

    FILE *fpWave;

    // check if file opening was successful
    if ((fpWave = fopen(ficWave, "r")) == NULL)
    {
        return NULL;
    }

    //****Audio Format + Number of channels extraction Start****
    if (fseek(fpWave, 20, SEEK_SET) < 0)
    { // position 20, 4 bytes
        return NULL;
    }

    // variable to store audio format (2 bytes) & number of channels (2 bytes)
    u_int32_t metadata;

    // fread 4 bytes of fpWave
    if (fread(&metadata, 4, 1, fpWave) != 1)
    {
        fprintf(stderr, "Reading metadata failed\n");
        return NULL;
    }

    // break <metadata> into variable 1 (Audio Format) & variable 2 (Number of Channels)
    u_int16_t audio_format = (u_int16_t)(metadata >> 16);  // <-- stick with upper 2 bytes
    u_int16_t num_channels = (u_int16_t)(metadata & 0xFF); // stick with lower 2 bytes

    // error checking (CANVIAR FUNCIONAMENT EN FUNCIO DEL QUE VULGUEM QUE PASSI)
    if (audio_format != (u_int16_t)(1) || (num_channels != (u_int16_t)(1) && num_channels != (u_int16_t)(2)))
    {
        fprintf(stderr, "Either the audio format or the number of channels is incompatible with the program.\n");
        return NULL;
    }

    // check whether to update *stereo
    if (num_channels == 1)
    {
        *stereo = false;
    }
    else
    {
        *stereo = true;
    }
    //****Audio Format + Number of Channels extraction ends****

    //****Sample Rate Extraction Start****

    // variable to store the sample rate (fm)
    unsigned int buffer;

    // place cursor at offset 24
    if (fseek(fpWave, 24, SEEK_SET) < 0)
    {
        fprintf(stderr, "Seeking to line 24 has failed.\n");
        return NULL;
    }

    // read sample rate (4 bytes)
    if (fread(&buffer, 4, 1, fpWave) != 1)
    {
        fprintf(stderr, "Reading buffer failed.\n");
        return NULL;
    }
    //****Sample Rate Extraction End****

    //****16 bits per samples checking**** (offset 34 / 2 bytes) START****

    // Read the first 2 bytes
    unsigned short bits_per_sample;
    // cursor positioning and byte extraction
    if (fseek(fpWave, 34, SEEK_SET) < 0 && fread(&bits_per_sample, 2, 1, fpWave) != 1 && bits_per_sample != 16)
    {
        fprintf(stderr, "Seeking to line 34, reading operation has failed or bits/sample != 16.\n");
        return NULL;
    }
    //****16 bits per samples checking**** (offset 34 / 2 bytes) END****

    // place cursor at offset 44 (wave data)
    if (fseek(fpWave, 44, SEEK_SET) < 0) // <--- aixo peta
    {
        fprintf(stderr, "Seeking to line 44 failed");
        return NULL;
    }

    // assign buffer to fm pointer
    *fm = buffer;

    return fpWave;
}

size_t lee_wave(void *x, size_t size, size_t nmemb, FILE *fpWave)
{
    /*---INPUTS---

        (1) *x -> punter a on guardar els bytes llegits
        (2) size -> tamany en bytes de cada un dels elements que volem llegir
        (3) nmemb -> quantitat d'elements a extreure
        (4) *fpWave -> punter fitxer del qual llegir

    ---OUTPUTS---

        (1) Nombre d'elements extrets correctament (hauria de ser igual a size (input) * nmemb (input))

    ---DESCRICIO---

        Fa només una simple crida a la funció pre-implementada fread() que bàsicament llegeix <nmemb> elements de tamany <size> (bytes) cadascun
        del fitxer fpWave i ho guarda en la variable <x>
    */
    return fread(x, size, nmemb, fpWave);
}

void cierra_wave(FILE *fpWave)
{
    /*---INPUTS---

        (1) *fpWave -> punter FILE del fitxer del qual volem tancar el seu stream

    ---OUTPUTS---

        (1) void

    --DESCRIPCIO---
        Simple crida al mètode pre-implementat en C, fclose(), que tanca el stream del fitxer que es passsa com argument
    */
    fclose(fpWave);
}
