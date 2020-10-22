#include <stddef.h>

char beef(char* g) { 
    return g[0];
}

int main() {
    char* str_a = "apples";
    char* str_b = "beans";
    char* str_c = "cakes";

    char a = 'x';
    char b = 'x';
    char c = 'x';

    a = beef(str_a);
    b = beef(str_b);
    c = beef(str_c);
}