#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>  

#define N 100000000
#define INI 1500

int p[N / 10];
int pp;
pthread_mutex_t afegirPrimer = PTHREAD_MUTEX_INITIALIZER;  // Mutex per protegir l'acces a pp

typedef struct {
    int inici;      // determina el inici de l'interval
    int fi;
    int contador;  // contador de numeros primers en cada thread
} DadesThread;

// Funció per cercar primers per cada thread
void* cercar_primers(void* args) {
    DadesThread* dades = (DadesThread*)args;
    int inici = dades->inici;
    int fi = dades->fi;
    
    if((inici % 2) == 0) inici++; // Si es parell, s'incrementa (a partir del 2 tots els primers son imparells)

    for (int num = inici; num <= fi; num += 2) {
        int div = 0; // Comprovar si num te algun divisor diferent de 1 i d'ell mateix
        
       // Revisar la divisibilitat pels primers trobats fins ara
       for (int i=1; p[i]*p[i] <= num && !div;i++){
            div = div || !(num % p[i]);
        }

        
        if (!div) { // Si no te divisors, es primer
            pthread_mutex_lock(&afegirPrimer);
            p[pp++] = num;
            pthread_mutex_unlock(&afegirPrimer);
            
            dades->contador++;   // Incrementar el comptador de primers en aquest thread
        }  

    }
    pthread_exit(NULL);
}

int comparar(const void* a, const void* b) {
    int numA = *(int*)a;  // punter a int per obtenir el valor
    int numB = *(int*)b;  // punter a int per obtenir el valor

    if (numA < numB) return -1;  // Si numA és més petit que numB
    if (numA > numB) return 1;   // Si numA és més gran que numB
    return 0;                    // Si són iguals
}

int main(int na, char* arg[]) {
    int i;
    int nn, num;
    int n_threads, primerNum, ultimNum;

    assert(na == 3);    // nombre d'arguments
    nn = atoi(arg[1]);       
    n_threads = atoi(arg[2]); 

    assert(n_threads > 0);
    printf("Tots els primers fins a %d amb %d threads\n", nn, n_threads);

    p[0] = 2;
    p[1] = 3;
    pp = 2;
    num = 5;
    
    while (pp < INI) {
        for (i = 1; p[i] * p[i] <= num; i++) {
            if (num % p[i] == 0) break;
        }
        if (p[i] * p[i] > num) p[pp++] = num;
        num += 2;
    }
    
    int mida_total = nn-num;
    int mida_thread = (nn - num)/n_threads;

    int primersSobrants = mida_total % n_threads;
    pthread_t threads[n_threads];
    DadesThread dades_thread[n_threads];
    pthread_mutex_init(&afegirPrimer, NULL);
    
    primerNum=num;
    // Crear els threads
    for (i = 0; i < n_threads; i++) {
        ultimNum = primerNum + mida_thread -1;
        if(i< primersSobrants) ultimNum++;
        
        dades_thread[i].inici = primerNum;
        dades_thread[i].fi = ultimNum;
        dades_thread->contador = 0;
        pthread_create(&threads[i], NULL, cercar_primers, &dades_thread[i]);
        primerNum = ultimNum + 1;
    }

    // Esperar a que tots els threads acabin
    for (i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&afegirPrimer);

    /* ORDENAR PRIMERS PER VEURE EL DARRER CORRECTE
    (Provoca quasi 1 segon de retard) */
    qsort(p, pp, sizeof(int), comparar);

    printf("Hi ha %d primers\n", pp - 1);
    printf("Darrer primer trobat %d\n", p[pp - 1]);
    //for(i=0;i<pp;i++) printf("%d\n",p[i]);

    return 0;
}