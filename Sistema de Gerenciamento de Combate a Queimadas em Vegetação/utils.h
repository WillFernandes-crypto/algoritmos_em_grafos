#ifndef UTILS_H
#define UTILS_H

// Limpa o buffer do teclado
void clear_input_buffer();

// Lê uma string do usuário
void read_string(const char* prompt, char* buffer, int max_len);

// Lê um float do usuário (não permite negativo)
float read_positive_float(const char* prompt);

// Lê um inteiro do usuário (não permite negativo)
int read_positive_int(const char* prompt);

// Lê um float do usuário (permite negativo)
float read_float(const char* prompt);

// Lê um inteiro do usuário (permite negativo)
int read_int(const char* prompt);

// Valida se a string está no formato DD/MM/AAAA
int validate_date(const char* data);

#endif // UTILS_H

