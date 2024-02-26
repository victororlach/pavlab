#include <stdio.h>
#include "fic_wave.h"

FILE *abre_wave(const char *ficWave, float *fm)
{
    FILE *fpWave;

    if ((fpWave = fopen(ficWave, "r")) == NULL)
        return NULL;
    if (fseek(fpWave, 24, SEEK_SET) < 0)
        return NULL;
    // guardem a fm, 4 bytes,
    __uint32_t buffer;
    __uint32_t fm_from_big_to_little_hexa;
    if (fread(&buffer, sizeof(__uint32_t), 1, fpWave) != 1)
    {
        fclose(fpWave); // Si hay un error, cerrar el archivo y devolver NULL
        return NULL;
    }
    // Convertir el orden de bytes de little-endian a big-endian
    fm_from_big_to_little_hexa = ((buffer & 0x000000FF) << 24) |
                                 ((buffer & 0x0000FF00) << 8) |
                                 ((buffer & 0x00FF0000) >> 8) |
                                 ((buffer & 0xFF000000) >> 24);

    // Imprimir el valor en formato hexadecimal y decimal
    printf("Formato hexadecimal: %08X\n", buffer);                     // el treu en big endian
    printf("Formato hexadecimal: %08X\n", fm_from_big_to_little_hexa); // el treu en little endian endian
    printf("Valor en punto flotante: %f\n", *((float *)&fm));          // Interpretar los bytes como un float
    printf("Valor de fm: %f\n", *fm);
    printf("Valor en punto flotante: %f\n", *((float *)&fm_from_big_to_little_hexa)); // Interpretar los bytes como un float
    printf("Valor de fm: %f\n", (float)fm_from_big_to_little_hexa);
    printf("%lu", (unsigned long)fm_from_big_to_little_hexa); // <-- Da 2151546... que no está bien
    printf("%lu\n", (unsigned long)buffer);                   // <-- Da 16000, que parece ser lo más normal

    // *****IGNORAR ESTOS COMENTARIOS*****
    // sacar los datos de metadata de canonical wave file format
    // hacer casos para cada idea quer podamos tener
    // Sacar los datos de la metadata de la cabecera (encontrar la real)
    // http://soundfile.sapp.org/doc/WaveFormat/
    // MIDI I XIFF I RIFF SON IDEM PERO AMB PEITES VARIACIONS DE BYTES EN METADATA
    // little endian    ->    RIFF
    // big endian ->    XIFF
    // usaremos bless para analizar los ficheros .wav en formato binario/hexadecimal
    // Confirmado el programa es mas potente que la castaÃ±a de hexdump
    // treball amb t_int32 i t_int16
    // por ultimo falta sox i la de mate matefile
    // pdf con todas las tareas
    //*fm = 16000;

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
