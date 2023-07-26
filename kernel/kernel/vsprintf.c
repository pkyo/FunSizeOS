//
// Created by root on 7/10/23.
//


/* we use this so that we can do without the ctype library */
#define is_digit(c)    ((c) >= '0' && (c) <= '9')

#pragma ide diagnostic ignored "OCDFAInspection"

// The `skip_atoi` function is a kind of string to integer function which ignores non-numeric characters
static int skip_atoi(const char **s) {
    // The **s is used because s is a pointer to a pointer.
    // This often happens when you pass the address of a pointer to a function,
    // intending to modify the original pointer.
    int i = 0;

    while (is_digit(**s))
        // The expression *((*s)++) gets the current character that s is pointing to
        // and then increments s to the next character
        i = i * 10 + *((*s)++) - '0';

    return i;
}

#define ZEROPAD     1       /* pad with zero */
#define SIGN        2       /* unsigned/signed long */
#define PLUS        4       /* show plus */
#define SPACE       8       /* space if plus */
#define LEFT        16      /* left justified */
#define SPECIAL     32      /* 0x */
#define SMALL       64      /* use 'abcdef' instead of 'ABCDEF' */

// The do_div macro performs division and modulus using assembly language commands.
// The macro performs the division of n by base,
// updating n with the quotient and returning the remainder.
// 1. `"div %4"` is the assembly instruction.
//      div is an assembly language instruction for unsigned division.
//      %4 refers to the 4th operand, which is (base).
// 2. `"=a" (n),"=d" (__res)` are output operands.
//      The `"=a"` and `"=d"` means that the a (accumulator) and d (data) registers are to be used for output.
//      (n) will hold the quotient and (__res) will hold the remainder after the operation
// 3. `"0" (n),"1" (0),"r" (base)` are input operands.
//      `"0"` and `"1"` are matching constraints which tie (n) and (0) to the same registers
//      as the output operands at those indices.
//      In other words, the input (n) goes into the same register as output (n),
//      and input (0) goes into the same register as output (__res).
//      `"r" (base)` means the divisor can be placed in any general-purpose register.
// The division is performed, and then __res is returned.
// __res will contain the remainder of the division,
// because the div instruction places the quotient in the eax register and the remainder in the edx register.
// After the operation, n is the quotient and __res is the remainder.
#define do_div(n, base) ({ \
int __res; \
__asm__("div %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
__res; })


// This function is designed to convert a number num to a string str based on a certain base (2-36),
// with various formatting options controlled by the type parameter.
// `str` is a pointer to a character array where the resulting string will be placed.
// `num` is the number to be converted.
// `base` is the numerical base for conversion, can be between 2 and 36 inclusive.
// `size` is the minimum width of the field to be printed.
// `precision` is the minimum number of digits to be printed.
// `type` is a bit mask that controls various aspects of formatting.
static char *number(char *str, int num, int base, int size, int precision, int type) {
    char c, sign, tmp[36];
    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;

    // This line of code is checking if the `type` includes the `SMALL` flag.
    // If it does, then digits is set to a string that includes lowercase letters.
    // Otherwise, digits will remain its default value, which includes uppercase letters.
    if (type & SMALL) digits = "0123456789abcdefghijklmnopqrstuvwxyz";

    // Checks if the LEFT flag is set in type
    // This effectively clears the ZEROPAD flag from type.
    if (type & LEFT) type &= ~ZEROPAD;

    if (base < 2 || base > 36)
        // Return null if the base is invalid
        return 0;

    c = (type & ZEROPAD) ? '0' : ' ';

    if ((type & SIGN) && num < 0) {
        // If the `type` includes `SIGN` and the number is negative,
        // set `sign` to '-' and make the number positive
        sign = '-';
        num = -num;
    } else {
        sign = (type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);
    }

    if (sign) size--;

    if (type & SPECIAL) {
        // If the `type` includes SPECIAL,
        // the field width size is decreased by 2 for hexadecimal
        // and by 1 for octal,
        // to account for '0x' and '0' prefixes respectively.
        if (base == 16) size -= 2;
        else if (base == 8) size--;
    }

    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    else
        while (num != 0)
            // Fills the `tmp` array with the digits of `num` in reverse order.
            tmp[i++] = digits[do_div(num, base)];

    // Updates the precision and size according to the actual number of digits
    if (i > precision) precision = i;
    size -= precision;

    if (!(type & (ZEROPAD + LEFT)))
        // checking if neither the ZEROPAD nor LEFT flags are set in the `type`.
        // If this is true, then it enters a loop where it fills in the string `str` with spaces
        // until the size is decremented to 0
        while (size-- > 0)
            *str++ = ' ';

    if (sign)
        *str++ = sign;

    if (type & SPECIAL) {
        // A prefix is added: '0' for octal and '0x' or '0X' for hexadecimal
        if (base == 8)
            *str++ = '0';
        else if (base == 16) {
            *str++ = '0';
            *str++ = digits[33]; // X x
        }
    }

    if (!(type & LEFT))
        // Adds padding on the left if needed ('if not left-justified').
        while (size-- > 0)
            *str++ = c;

    while (i < precision--)
        *str++ = '0';

    while (i-- > 0)
        *str++ = tmp[i];

    while (size-- > 0)
        *str++ = ' ';

    return 0;
}