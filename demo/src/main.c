#include <stddef.h>

char beef(char* g) { 
    return g[0];
}

int main() {
    char* str_a = "cheese flakes";
    char* str_b = "applesauce";
    char* str_c = "cupcake";

    char a = beef(str_a); // 13
    char b = beef(str_b); // 10
    char c = beef(str_c); // 7
}