#include "resources.hpp"
#include <stdio.h>
#include <memory.h>
#include <string>

char *cstring(const uint8_t *res, size_t length)
{
    char *string = (char *)malloc(length);
    memcpy(string, res, length);
    return string;
}
#define CSTRING(res) cstring(res, sizeof(res))

int main()
{
    char string[sizeof(Resources::test_txt) + 1] = {0};
    memcpy(string, Resources::test_txt, sizeof(Resources::test_txt));
    printf("%s\n", string);
    printf("%s\n", cstring(Resources::test2_txt, sizeof(Resources::test2_txt)));
    printf("%s\n", CSTRING(Resources::folder::file_txt));
    printf("%s\n", CSTRING(Resources::folder::folder_with_space::file_with_space_txt));
} 