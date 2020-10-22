#include <stddef.h>

size_t bleh(char* a) { return strlen(a); }

size_t yeh(char* b) { return strlen(b); }

int main() {
    char* str_a = "cheese flakes";
    char* str_b = "applesauce";
    char* str_c = "cupcake";

    size_t b = bleh(str_b); // 10
    size_t a = yeh(str_a);  // 13
    size_t c = yeh(str_c);  // 7
}