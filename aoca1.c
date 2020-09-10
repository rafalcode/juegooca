#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h> 

#define GBUF 2
#define MAXVSZ 36
#define MAXISZ 5 // max size of the integer
#define boolean unsigned char

#define CONDREALLOC(x, b, c, a, t); \
    if((x)>=((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
    }

// following is hard coded for oca_t and it has its own iterative index, k9 for fun and uniqueness.
#define CONDREALLOC2(x, b, c, a); \
    int k9; \
    if((x)>=((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(oca_t)); \
            for(k9=(b) - (c); k9<(b);++k9) { \
                aoca[k9].ocibf=(c); \
                aoca[k9].ocinda=malloc(aoca[k9].ocibf*sizeof(int)); \
                aoca[k9].ocisz=0; \
            } \
    }

/* Rough work column:
aoca[k9].uo=-1; // no speculative assigning so this wasn't necessary
*/

typedef struct /* oca_t OCcurence Array type */
{
    int uo; /* the unique occurence this array entry refers to */
    int *ocinda; /* the indices corresponding to the uo unique occurence. */
    int ocisz; /* Ocurrence Index Array size */
    int ocibf; /* Ocurrence Index Array buffer */
} oca_t;

int cmpocabyoc(const void *a, const void *b) /* compare aoca by occurence */
{
    oca_t *oca = (oca_t*)a; /* cast our void! */
    oca_t *ocb = (oca_t*)b; /* cast our void! */
    return oca->uo  - ocb->uo; /* integer comparison: returns positive if b > a and nagetive if a > b: i.e. highest values first */
}

int cmpocabyoasz(const void *a, const void *b) /* compare aoca by occurence */
{
    oca_t *oca = (oca_t*)a; /* cast our void! */
    oca_t *ocb = (oca_t*)b; /* cast our void! */
    return ocb->ocisz  - oca->ocisz; /* integer comparison: returns positive if b > a and nagetive if a > b: i.e. highest values first */
}

void prtaoca(oca_t *aoca, int aocasz)
{
    int i, j;
    for(j=0; j<aocasz;++j) {
        printf("val %i (sz=%i): ", aoca[j].uo, aoca[j].ocisz);
        for(i=0;i<aoca[j].ocisz;++i) 
            printf("%u ", aoca[j].ocinda[i]);
        putchar('\n');
    }
}

oca_t *uniquevals(int *v, int vsz, int *aocasz_)
{
    unsigned char new;
    unsigned i, j;
    unsigned aocabuf=GBUF;
    int aocasz=0;
    oca_t *aoca=malloc(aocabuf*sizeof(oca_t));
    for(i=0;i<aocabuf;++i) {
        aoca[i].ocibf=GBUF;
        aoca[i].ocinda=malloc(aoca[i].ocibf*sizeof(int));
        aoca[i].ocisz=0;
    }

    for(i=0; i<vsz;++i) {
        new=1;
        for(j=0; j<aocasz;++j) {
            if(aoca[j].uo == v[i]) {
                CONDREALLOC(aoca[j].ocisz, aoca[j].ocibf, GBUF, aoca[j].ocinda, int);
#ifdef DBG
                printf("APPENDING aocaind %i (ou=%i) @sz %i @buf %i adding vind %i\n", j, v[i], aoca[j].ocisz, aoca[j].ocibf, i);
#endif
                aoca[j].ocinda[aoca[j].ocisz] = i;
                aoca[j].ocisz++;
                new=0;
                break;
            }
        }
        if(new) {
            aocasz++;
            CONDREALLOC2(aocasz, aocabuf, GBUF, aoca);
#ifdef DBG
                printf("NEW aocaind %i (ou=%i) @sz %i @buf %i adding vind %i\n", j, v[i], aoca[j].ocisz, aoca[j].ocibf, i);
#endif
            aoca[aocasz-1].uo = v[i];
            aoca[aocasz-1].ocisz++;
            aoca[aocasz-1].ocinda[aoca[j].ocisz-1] = i;
        }
    }

    // normalizing
    for(i=aocasz;i<aocabuf;++i)
        free(aoca[i].ocinda);
    aoca=realloc(aoca, aocasz*sizeof(oca_t));

    /* order */
    // qsort(aoca, aocasz, sizeof(oca_t), cmpocabyoc);
    qsort(aoca, aocasz, sizeof(oca_t), cmpocabyoasz);
#ifdef DBG
    printf("number of different values: %i\n", aocasz);
    for(j=0; j<aocasz;++j) {
        printf("%i (%u): ", aoca[j].uo, aoca[j].ocisz);
        for(i=0;i<aoca[j].ocisz;++i) 
            printf("%i ", aoca[j].ocinda[i]);
        printf("\n"); 
    }
#endif
    *aocasz_ = aocasz;
    return aoca;
}

void prtusage(char *progname)
{
    printf("Help for prog \"%s\":\n", progname);
    printf("Just one arg for the time being\n");
}

int main(int argc, char *argv[])
{
    /* argument accounting: remember argc, the number of arguments, _includes_ the executable */
    if(argc!=1) {
        prtusage(argv[0]);
        exit(EXIT_FAILURE);
    }
    int i;

    int vsz=(int)(MAXVSZ*(float)random()/RAND_MAX); // what will be the size of our integer vector (up to a certain max)?
    int *v=malloc(vsz*sizeof(int));
    for(i=0;i<vsz;++i) 
        v[i] = (int)(MAXISZ*(float)random()/RAND_MAX); // each element a random integer (up to a certain max).

    // let's see what we got:
    printf("Indices: "); 
    for(i=0;i<vsz; ++i) 
        printf("%3i ", i);
    putchar('\n');
    printf(" Values: "); 
    for(i=0;i<vsz; ++i) 
        printf("%3i ", v[i]);
    putchar('\n');

    int aocasz;
    oca_t *aoca = uniquevals(v, vsz, &aocasz);
    printf("aocasz = %i \n", aocasz); 
    prtaoca(aoca, aocasz);

    for(i=0;i<aocasz;++i)
        free(aoca[i].ocinda);
    free(aoca);

    free(v);
    return 0;
}
