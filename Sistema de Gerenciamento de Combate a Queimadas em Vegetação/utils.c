#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

// Lê um float do usuário (permite negativo)
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

// Lê um inteiro do usuário (permite negativo)
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

// Lê um float do usuário (não permite negativo)
float read_positive_float(const char* prompt) {
    float value;
    do {
        value = read_float(prompt);
        if (value < 0)
            printf("Valor não pode ser negativo. Tente novamente.\n");
    } while (value < 0);
    return value;
}

// Lê um inteiro do usuário (não permite negativo)
int read_positive_int(const char* prompt) {
    int value;
    do {
        value = read_int(prompt);
        if (value < 0)
            printf("Valor não pode ser negativo. Tente novamente.\n");
    } while (value < 0);
    return value;
}

// Valida se a string está no formato DD/MM/AAAA
int validate_date(const char* data) {
    if (strlen(data) != 10)
        return 0;
    if (!isdigit(data[0]) || !isdigit(data[1]) || data[2] != '/' ||
        !isdigit(data[3]) || !isdigit(data[4]) || data[5] != '/' ||
        !isdigit(data[6]) || !isdigit(data[7]) || !isdigit(data[8]) || !isdigit(data[9]))
        return 0;
    int dia = (data[0] - '0') * 10 + (data[1] - '0');
    int mes = (data[3] - '0') * 10 + (data[4] - '0');
    int ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100 + (data[8] - '0') * 10 + (data[9] - '0');
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1900)
        return 0;
    return 1;
}

