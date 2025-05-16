#ifndef UTILS_H
#define UTILS_H

// Limpa o buffer do teclado
void clear_input_buffer();

// Lê uma string do usuário
void read_string(const char* prompt, char* buffer, int max_len);

// Lê um float do usuário
float read_float(const char* prompt);

// Lê um inteiro do usuário
int read_int(const char* prompt);

#endif // UTILS_H

