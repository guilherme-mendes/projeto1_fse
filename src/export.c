#include <stdio.h>
#include <time.h>
#include <export.h>
#include <stdlib.h>

void add_file(float TI, float TE, float TR)
{
    FILE *file_csv;
    time_t rawtime;
    struct tm *timeinfo;
    char buf[81];

    file_csv = fopen("measures.csv", "a+");

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buf, 81, "%F %X", timeinfo);

    fprintf(file_csv, "%s, %.3f, %.3f, %.3f\n", buf, TI, TE, TR);

    fclose(file_csv);
}

void create_csv()
{
    FILE *file_csv;

    file_csv = fopen("measures.csv", "w");
    fprintf(file_csv, "tempo, temperatura_interna, temperatura_externa, temperatura_referencia\n");
    fclose(file_csv);
}