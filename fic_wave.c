#include <stdio.h>
#include "fic_wave.h"

// need to extract the actual sound data, which begins at offset 44 in WAVE file
FILE *abre_wave(const char *ficWave, float *fm)
{
    FILE *fpWave;

    if ((fpWave = fopen(ficWave, "r")) == NULL)
        return NULL;

    //****Audio Format + Number of channels extraction Start****
    if (fseek(fpWave, 20, SEEK_SET) < 0) // position 20, 4 bytes
        return NULL;

    // variable to store number of channels (2 bytes)
    u_int32_t metadata;

    // fread 4 bytes of fpWave
    fread(&metadata, 2, 2, fpWave);

    // break <metadata> into variable 1 (Audio Format) & variable 2 (Number of Channels)
    u_int16_t audio_format = (u_int16_t)(metadata >> 16);                                                                       // <-- stick with upper 2 bytes
    u_int16_t num_channels = (u_int16_t)(metadata & 0x00FF);                                                                    // stick with lower 2 bytes
    printf("Audio Format: %u\nNumber of channels: %u\n", audio_format, num_channels);                                           //<-- checking purposes (can delete)
    printf("Size of audio format variable: %lu\nSize of channels variable: %lu\n", sizeof(audio_format), sizeof(num_channels)); // <-- checking purposes (can delete)

    // error checking
    if (audio_format != (u_int16_t)(1) || num_channels != (u_int16_t)(1))
    {
        fprintf(stderr, "Either the audio format or the number of channels is incompatible with the program.\n");
        return NULL;
    }
    // //****Audio Format + Number of Channels extraction ends****

    //****Sample Rate Extraction Start****
    unsigned int buffer;

    if (fseek(fpWave, 24, SEEK_SET) < 0)
        return NULL; // position reading cursos to fm chunk

    fread(&buffer, 4, 1, fpWave);
    // printf("%lu\n%d\n%08X", buffer, sizeof(buffer), buffer);
    if (fseek(fpWave, 44, SEEK_SET) < 0)
        // printf("fuck");
        return NULL; // <--- aixo peta
    // assign buffer to fm pointer
    // printf("not ok");
    *fm = buffer;
    //****Sample Rate Extraction End****
    // printf("ok");
    return fpWave;
}

size_t lee_wave(void *x, size_t size, size_t nmemb, FILE *fpWave)
{
    return fread(x, size, nmemb, fpWave);
}

void cierra_wave(FILE *fpWave)
{
    fclose(fpWave);
}
