//
// Created by root on 7/10/23.
//
#include "../include/stdarg.h"
#include "../include/string.h"
#include "../include/linux/kernel.h"

// This is not necessary. ⚠️
// A compiler directive used in many IDEs (Integrated Development Environments)
// to suppress specific warnings or errors.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"

/* we use this so that we can do without the ctype library */
#define is_digit(c)    ((c) >= '0' && (c) <= '9')

// The `skip_atoi` function is a kind of string to integer function which ignores non-numeric characters
static int skip_atoi(const char **s) {
    // The `**s` is used because `s` is a pointer to a pointer.
    // This often happens when you pass the address of a pointer to a function,
    // intending to modify the original pointer.
    int i = 0;

    while (is_digit(**s))
        // The expression `*((*s)++)` gets the current character that s is pointing to
        // and then increments `s` to the next character
        i = i * 10 + *((*s)++) - '0';

    return i;
}

/*******************************************************************************************************
 * 💎A complete format combination: %[flags][width][.precision][length]specifier
 * 🌞flags: They can be `-`, `+`, ` `, `#`, and `0`.
 *      🌟The `-` flag left-justifies the output of a primitive data type. By default, the output is right-justified
 *      🌟The `+` flag forces the output of signed numeric types to be prefixed with a + or -.
 *          By default, only negative numbers are prefixed with -.
 *      🌟The space flag inserts a space before the output of positive signed numeric types.
 *          Negative numbers are still prefixed with a -. This is ignored if the + flag is also specified.
 *      🌟The `#` flag modifies the output of o, x, X, e, E, f, F, g, G format specifiers.
 *          For o, x, X it prefixes the output with 0, 0x, 0X respectively.
 *      🌟The `0` flag pads the output of numeric types with leading zeros.
 *          It is ignored if the - flag is also specified.
 *
 * 🌞width: Specifies the minimum number of characters to output
 *      🌟The width can be either a number or an asterisk (*).
 *          If you use * instead of a number, like `%*d`,
 *          then the function expects an int argument to be provided which specifies the width.
 *          This can be useful if the width needs to be calculated or is variable for some reason.
 *
 * 🌞precision: Specifies the number of digits after the decimal point for floating point numbers,
 *      or maximum output length for string
 *
 * 🌞length: Modifies the length of the data type. These can be hh, h, l, ll, j, z, t, L.
 *
 * 🌞specifier: Specifies the type of data
 *      🌟%d or %i:    Represents an integer
 *      🌟%u:          Represents an unsigned integer
 *      🌟%f:          Represents a floating point number
 *      🌟%c:          Represents a character
 *      🌟%s:          Represents a string
 *      🌟%p:          Represents a pointer
 *      🌟%%:          Represents a literal percent sign
 *
 * ******************************************************************************************************/

#define ZEROPAD     1       /* pad with zero */
#define SIGN        2       /* unsigned/signed long */
#define PLUS        4       /* show plus */
#define SPACE       8       /* space if plus */
#define LEFT        16      /* left justified */
#define SPECIAL     32      /* 0x */
#define SMALL       64      /* use 'abcdef' instead of 'ABCDEF' */

// The do_div macro performs division and modulus using assembly language commands.
// The macro performs the division of `n` by `base`,
// updating `n` with the quotient and returning the remainder.
// 1. `"div %4"` is the assembly instruction.
//      div is an assembly language instruction for unsigned division.
//      %4 refers to the 4th operand, which is (`base`).
// 2. `"=a" (n),"=d" (__res)` are output operands.
//      The `"=a"` and `"=d"` means that the eax (accumulator) and edx (data) registers are to be used for output.
//      (n) will hold the quotient and (__res) will hold the remainder after the operation
// 3. `"0" (n),"1" (0),"r" (base)` are input operands.
//      `"0"` and `"1"` are matching constraints which tie (n) and (0) to the same registers
//      as the output operands at those indices.
//      In other words, the input (n) goes into the same register as output (n),
//      and input (0) goes into the same register as output (__res).
//      `"r" (base)` means the divisor can be placed in any general-purpose register.
// The division is performed, and then `__res` is returned.
// `__res` will contain the remainder of the division,
// because the div instruction places the quotient in the eax register and the remainder in the edx register.
// After the operation, `n` is the quotient and `__res` is the remainder.
#define do_div(n, base) ({ \
int __res; \
__asm__("div %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
__res; })


// This function is designed to convert a number `num` to a string `str` based on a certain base (2-36),
// with various formatting options controlled by the type parameter.
// `str` is a pointer to a character array where the resulting string will be placed.
// `num` is the number to be converted.
// `base` is the numerical base for conversion, can be between 2 and 36 inclusive.
// `size` is the minimum width of the field to be printed.
// `precision`
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
            *str++ = digits[33]; // X or x
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

    return str;
}

int vsprintf(char *buf, const char *fmt, va_list args) {
    // These are various variables that will be used in the function.
    int len;
    int i;
    char *str;
    char *s;
    int *ip;

    //  These variables are used to interpret the format string.
    int flags;          /* flags to number() */
    int field_width;    /* width of output field */
    int precision;      /* min number of digits for integers; max number of chars for from string */
    int qualifier;      /* 'h', 'l', or 'L' for integer fields */

    // `str` is set to the start of the buffer,
    // and `fmt` is incremented each iteration.
    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            // If the current character is not a '%', it is copied to the buffer
            *str++ = *fmt;

            continue;
        }

        /* process flags */
        flags = 0;
        repeat:
        ++fmt;        /* this also skips first '%' */
        switch (*fmt) {
            case '-':
                flags |= LEFT;
                goto repeat;

            case '+':
                flags |= PLUS;
                goto repeat;

            case ' ':
                flags |= SPACE;
                goto repeat;

            case '#':
                flags |= SPECIAL;
                goto repeat;

            case '0':
                flags |= ZEROPAD;
                goto repeat;
        }

        /* get field width */
        field_width = -1;

        if (is_digit(*fmt))
            field_width = skip_atoi(&fmt);

        else if (*fmt == '*') {
            /* it's the next argument */
            field_width = va_arg(args, int);

            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;

        if (*fmt == '.') {
            ++fmt;
            if (is_digit(*fmt))
                precision = skip_atoi(&fmt);

            else if (*fmt == '*') {
                /* it's the next argument */
                precision = va_arg(args, int);
            }

            if (precision < 0)
                precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
            qualifier = *fmt;
            ++fmt;
        }

        switch (*fmt) {
            case 'c':
                if (!(flags & LEFT))
                    while (--field_width > 0)
                        *str++ = ' ';

                *str++ = (unsigned char) va_arg(args, int);

                while (--field_width > 0)
                    *str++ = ' ';

                break;

            case 's':
                s = va_arg(args, char *);
                len = strlen(s);

                if (precision < 0)
                    precision = len;

                else if (len > precision)
                    len = precision;

                if (!(flags & LEFT))
                    while (len < field_width--)
                        *str++ = ' ';

                for (i = 0; i < len; ++i)
                    *str++ = *s++;

                while (len < field_width--)
                    *str++ = ' ';

                break;

            case 'o':
                str = number(str, va_arg(args, unsigned long), 8, field_width, precision, flags);

                break;

            case 'p':
                if (field_width == -1) {
                    field_width = 8;
                    flags |= ZEROPAD;
                }

                str = number(str, (unsigned long) va_arg(args, void *), 16, field_width, precision, flags);

                break;

            case 'x':
                flags |= SMALL;

            case 'X':
                str = number(str, va_arg(args, unsigned long), 16, field_width, precision, flags);

                break;

            case 'd':
            case 'i':
                flags |= SIGN;

            case 'u':
                str = number(str, va_arg(args, unsigned long), 10, field_width, precision, flags);

                break;

            case 'n':
                ip = va_arg(args, int *);

                *ip = (str - buf);

                break;

            default:
                if (*fmt != '%')
                    *str++ = '%';

                if (*fmt)
                    *str++ = *fmt;

                else
                    --fmt;

                break;
        }

    }

    // At the end of the function, the buffer is null-terminated
    // and the number of characters written is returned.
    *str = '\0';

    return str - buf;
}

#pragma clang diagnostic pop