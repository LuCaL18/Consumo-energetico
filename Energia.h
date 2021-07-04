#ifndef ENERGIA_H_INCLUDED
#define ENERGIA_H_INCLUDED

#define ENERGIA_MIN 5000//i valori di energia sono scelti in base ai valori di esempio nel file 
#define ENERGIA_MAX 7200//situazione ideale, energia massima per tutti
#define INTERVALLO_TEMPO 60//valore in minuti 

typedef struct Edificio
{
    double max_corrente;
    double corrente_cons;
}Edificio;


void assegnazionevalore(double disponibile,double totale,double*array_dei_valori,int max,double*corrente_per_elemento);
void recuperodati(Edificio**array,int max,int*edifici,double* somme_di_energia);
double regolazioneconsumo(Edificio**array,int quartiere,int edifici,double max_per_quartiere,double energia_disponibile,double differenza);
void memorizzadati(double energia,int numero_registrazioni);

#endif