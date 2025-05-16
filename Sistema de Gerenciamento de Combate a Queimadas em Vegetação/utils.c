#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

// Limpa o buffer do teclado (stdin)
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Lê uma string do usuário com tamanho máximo
void read_string(const char* prompt, char* buffer, int max_len) {
    printf("%s", prompt);
    if (fgets(buffer, max_len, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        else
            clear_input_buffer();
    }
}

// Lê um float do usuário
float read_float(const char* prompt) {
    float value;
    printf("%s", prompt);
    while (scanf("%f", &value) != 1) {
        printf("Valor inválido. Tente novamente: ");
        clear_input_buffer();
    }
    clear_input_buffer();
    return value;
}

// Lê um inteiro do usuário
int read_int(const char* prompt) {
    int value;
    printf("%s", prompt);
    while (scanf("%d", &value) != 1) {
        printf("Valor inválido. Tente novamente: ");
        clear_input_buffer();
    }
    clear_input_buffer();
    return value;
}

