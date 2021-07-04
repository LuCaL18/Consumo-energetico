#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<Windows.h>
#include "Energia.h"

void assegnazionevalore(double energia_disponibile,double totale,double*array_dei_valori,int max,double* corrente_per_quartiere)
{
    double rapporto=energia_disponibile/totale;

    for(int i=0;i<max;i++)
    {
        array_dei_valori[i]=(corrente_per_quartiere[i]*rapporto);//mettiamo un ulteriore limiti per evitare errori in fase di calcolo per la presenza di valori in float
    }
}


void recuperodati(Edificio**array,int max,int*edifici,double*somme_di_energia)//simula il consumo di ciascun edificio dando valori random
{
    srand(time(NULL));
    for(int i=0;i<max;i++)
    {
        for(int j=0;j<edifici[i];j++)
        {
            array[i][j].corrente_cons=(150+rand())%((int)(array[i][j].max_corrente));
            somme_di_energia[i]+=array[i][j].corrente_cons;
        }
    }

}


double regolazioneconsumo(Edificio**array,int quartiere,int edifici,double max_per_quartiere,double energia_disponibile,double differenza)
{
    double* media_consumo=(double*)malloc(edifici*sizeof(double));//array contenente le medie dei consumi per ogni edificio di un quartiere
    int*indici=(int*)malloc(edifici*sizeof(int));
    double*eccessi=(double*)malloc(edifici*sizeof(double));
    double nuovo_consumo=0;

    int cont=0;

    double eccesso_totale=0;

    assegnazionevalore(energia_disponibile,max_per_quartiere,media_consumo,edifici,&array[quartiere]->max_corrente);//viene calcolata la media del consumo per ogni edificio

    for(int i=0;i<edifici;i++)
    {
        if(array[quartiere][i].corrente_cons>media_consumo[i])//se il consumo supera la media 
        {
           cont++;
           eccesso_totale+=array[quartiere][i].corrente_cons-media_consumo[i];//calcolo dell'eccesso totale
           indici[cont-1]=i;
           eccessi[cont-1]=array[quartiere][i].corrente_cons-media_consumo[i];//viene memorizzato l'eccesso
        }
        else
            nuovo_consumo+=array[quartiere][i].corrente_cons;
    }

    double rapporto_eccessi=differenza/eccesso_totale;

    for(int i=0;i<cont;i++)
    {
        eccessi[i]=eccessi[i]*rapporto_eccessi;
        printf("Riduzione consumo EDIFICIO %d QUARTIERE %d da %lfV",indici[i],quartiere,array[quartiere][indici[i]].corrente_cons);
        array[quartiere][indici[i]].corrente_cons-=eccessi[i];//il consumo viene diminuito in rapporto al suo eccesso
        printf(" a %lfV\n",array[quartiere][indici[i]].corrente_cons);
        Sleep(350);
        nuovo_consumo+=array[quartiere][indici[i]].corrente_cons;
    }
    

    free(media_consumo);
    media_consumo=NULL;
    free(indici);
    indici=NULL;
    free(eccessi);
    eccessi=NULL;

    return nuovo_consumo;
}


void memorizzadati(double energia,int numero_registrazioni)//salva i dati riguardanti il consumo medio di energia elettrica
{
    FILE*ptr;
    static int tempo=INTERVALLO_TEMPO;
    double media;

    media=energia/numero_registrazioni;

    ptr=fopen("Risultati.txt","a");

    fprintf(ptr,"%d %lf\n",tempo,media);

    fclose(ptr);

    tempo+=INTERVALLO_TEMPO;

}


