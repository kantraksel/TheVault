#include "StringUtils.h"

static char buff[16];
constexpr int max = sizeof(buff) - 1;

char* StringUtils::ToStringNoAlloc(int n)
{
    bool sign = false;
    if (n < 0)
    {
        sign = true;
        n = 0 - n;
    }

    char* ptr = buff + max;
    *ptr = 0;

    do
    {
        --ptr;
        *ptr = '0' + n % 10;
        n /= 10;
    } while (n != 0);

    if (sign)
        *--ptr = '-';
    return ptr;
}
