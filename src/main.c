/*
 * Contador de números primos multithread
 *
 * Rafael Sartori, 186154
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>

/** Número de threads utilizadas */
#define NUM_THREADS     4
/** Número de inteiros que poderão ser armazenados */
#define NUM_INTEIROS    100 * 1024


typedef struct {
    uint32_t *numeros;
    uint32_t numero_total;
    uint32_t numero_atual;
    uint32_t num_primos;
    pthread_mutex_t trava;
} primos_thread_t;


/**
 * Retorna 1 se o inteiro dado é primo.
 */
int is_primo(uint32_t inteiro) {
    /* Conferimos se é possível existir primo */
    if (inteiro <= 1) {
        return 0;
    }

    /* Procuramos um divisor entre 2 e a metade do número que temos */
    // OBS: podemos fazer sqrt(inteiro)
    for (uint32_t i = 2; i <= (inteiro / 2); i++) {
        /* Se é divisor, não é um primo, saímos */
        if ((inteiro % i) == 0) {
            return 0;
        }
    }

    return 1;
}

/**
 * Cada thread irá pegar a próxima posição do vetor incrementando o índice.
 */
void *run_thread(void *argumento_) {
    primos_thread_t *argumento = (primos_thread_t *) argumento_;

    int anterior_primo = 0;
    do {
        /* Região crítica: pegar o próximo número primo, salvar se o anterior
         * era primo */
        pthread_mutex_lock(&(argumento->trava));
        /* Salvamos o resultado anterior */
        argumento->num_primos += anterior_primo;
        /* Pegamos o próximo ou saímos se acabamos */
        uint32_t numero;
        if (argumento->numero_atual < argumento->numero_total) {
            /* Pegamos uma cópia do próximo e incrementamos */
            numero = argumento->numeros[argumento->numero_atual++];
            pthread_mutex_unlock(&(argumento->trava));
        } else {
            pthread_mutex_unlock(&(argumento->trava));
            break;
        }

        /* Fora da região crítica, testamos se o número é primo */
        anterior_primo = is_primo(numero);
    } while(1);
    return NULL;
}

int main() {
    /* Vetor de threads utilizadas */
    pthread_t threads[NUM_THREADS];

    /* Inicializamos o argumento que será passado às threads (não há
     * concorrência aqui) */
    primos_thread_t argumento;
    argumento.num_primos = 0;
    argumento.numeros = malloc3(NUM_INTEIROS * sizeof(uint32_t));
    argumento.numero_atual = 0;
    argumento.numero_total = 0;
    pthread_mutex_init(&(argumento.trava), NULL);

    /* Escrevemos os números da entrada enquanto ela não acabou */
    int escrito;
    while (scanf(" %d ", &escrito) != EOF) {
        /* Colocamos no vetor mais um número que falta (não há concorrência
         * aqui) */
        argumento.numeros[argumento.numero_total++] = escrito;
    }

    /* Inicializamos as threads */
    for (uint32_t i = 0; i < NUM_THREADS; i++) {
        /* Criamos a thread atual com a função que criamos */
        pthread_create(&(threads[i]), NULL, run_thread, (void *) &argumento);
    }

    /* Aguardamos as threads acabarem */
    for (uint32_t i = 0; i < NUM_THREADS; i++) {
        /* Aguardamos as threads acabarem */
        pthread_join(threads[i], NULL);
    }

    /* Informamos quantos primos existem (não há concorrência aqui) */
    printf("%d\n", argumento.num_primos);

    /* Encerramos */
    return 0;
}
