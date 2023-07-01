/*

posprintf - a condensed version of sprintf for Thumb, esp. GBA
Copyright (C) 2003 Dan Posluns

The person or persons who have associated work with this document (the "Dedicator" or "Certifier") hereby either (a)
certifies that, to the best of his knowledge, the work of authorship identified is in the public domain of the country
from which the work is published, or (b) hereby dedicates whatever copyright the dedicators holds in the work of
authorship identified below (the "Work") to the public domain. A certifier, moreover, dedicates any copyright interest
he may have in the associated work, and for these purposes, is described as a "dedicator" below.

A certifier has taken reasonable steps to verify the copyright status of this work. Certifier recognizes that his good
faith efforts may not shield him from liability if in fact the work certified is not in the public domain.

Dedicator makes this dedication for the benefit of the public at large and to the detriment of the Dedicator's heirs and
successors. Dedicator intends this dedication to be an overt act of relinquishment in perpetuity of all present and
future rights under copyright law, whether vested or contingent, in the Work. Dedicator understands that such
relinquishment of all rights includes the relinquishment of all rights to enforce (by lawsuit or otherwise) those
copyrights in the Work.

Dedicator recognizes that, once placed in the public domain, the Work may be freely reproduced, distributed,
transmitted, used, modified, built upon, or otherwise exploited by anyone for any purpose, commercial or non-commercial,
and in any way, including by methods that have not yet been invented or conceived.

Author contact e-mail: dan at danposluns dot com


INSTRUCTIONS:

To call: posprintf(char *dest, const char *src[, param1[, param2[, ... paramN]]]);

- src must be a valid zero-terminated C string.
- dest must point to a sufficiently large block of memory to contain the result string.

The following format specifiers are valid:

%% - print a '%' symbol
%s - print a string; parameter must point to a valid zero-terminated C string
%d - print a 16-bit (short) integer; must be within [-65,535 .. 65,535]
%l - print a 29-bit integer; approximate range [-500,000,000 .. 500,000,000]
%x - print a hexadecimal number (lowercase digits)
%X - print a hexadecimal number (uppercase digits)

The specifiers %d, %l, %x and %X may be modified as follows:

- Digits 1 through 9 indicate number of leading spaces to print, eg.
        %5d would print the number 123 as "  123"
        %5d would print the number 123456 as "123456" (no leading spaces)
- When above digit is prefixed with 0, leading zeros are printed instead of spaces
        %05d would print the number 123 as "00123"
        %04d would print the number 12345 as "12345" (no leading zeros)
- Negative sign consumes a leading space, eg.
        %05d would print the number -123 as "-0123"
        (Hexadecimal numbers are considered unsigned)

IF YOU WANT MORE FUNCTIONALITY THAN THIS, YOU CAN FEEL FREE TO MODIFY THE CODE,
BUT THEN I WOULD SUGGEST MOVING TO A MORE SOPHISTICATED LIBRARY ANYWAY.

*** CAUTION IF NOT USED ON GAMEBOY ADVANCE ***
Although this is mostly written as general Thumb code, the %l (29-bit print)
specifier code currently uses a software interrupt (SWI) specific to the
Gameboy Advance to perform a division. If you wish to port this to other ARM
machines, you may need to alter this code. I believe that most ARM machines
support SWI 7 as an alternative software divide, although you will have to
swap the numerator/denominator registers (r0 and r1).
*** END CAUTION ***

My contact e-mail is: dan at danposluns dot com

*/

/*
posprintf Documentation

What is it?

posprintf is a function you can call in your Gameboy Advance programs to generate strings. It is basically a somewhat
amputated version of the standard C library function, sprintf.

Why use it?

Probably the most significant use of it is to print out and format numbers for display on the screen. In order to do
this, the numbers need to be converted from their internal representation (base 2) to decimal (base 10). posprintf uses
an extremely efficient algorithm scammed off of the 'net to do this, and it is written entirely in Thumb assembler to
run very quickly (without consuming the internal memory needed to run efficiently in ARM mode).

Other reasons to use it include that it's fast, extremely flexible for most GBA applications, and absolutely free. I
wouldn't say no to a credit if it is used in an application, though, commercial or otherwise.

How do I use it?

    Include the file posprintf.h in your C source code that you want to call the function. (ie. #include "posprintf.h")

    Call it the same way you would call sprintf, keeping in mind the differences between the two (see the next section).

    Make sure to link the file posprintf.o with your project. Generally this just means including posprintf.o on the
command line when you call gcc.

That's all there is to it.

How do I call the function?

The syntax is as follows:

    posprintf(char *dest, const char *src[, param1[, param2[, ... paramN]]]);

    dest is the address of the string to be printed out to. IMPORTANT: There must be sufficient memory allocated to the
buffer to hold the string, including the terminating zero.

    src is the address of the source string to print. This absolutely must be a valid, zero-terminated C string.

    The optional parameters accompany any format specifiers used in the source string.

Format Specifiers

The following are valid format specifiers for use with posprintf:

    %% - print a % character
    %s - print a string. The accompanying parameter must be a valid, zero-terminated C string.
    %d - print an integer. Important: unlike sprintf, this is only accurate for 16 bits of data, or numbers ranging from
-65,535 to 65,535. %l - print a long integer. Important: unlike sprintf this is only accurate for approximately 29 bits
of data, or numbers ranging from about -500 million to 500 million. %x - print in hexadecimal format. Provided mostly as
a programming tool, since few user applications would probably ever want to print hexadecimal numbers. %X - same as %x,
but prints capital letters for the digits 'a' through 'f'.

Output Control

The output of %d, %l, %x and %X tags may optionally be influenced by the following prefixes:

    Digits 1 through 9: Indicate how many leading spaces to include. Examples:

        %5d would print the number 123 as " 123"
        %3d would print the number 12345 as "12345" (no leading spaces)

    Digit 0: When prefixing the above number, indicates to print leading zeros instead of leading spaces. Examples:

        %05d would print the number 123 as "00123"
        %03d would print the number 12345 as "12345" (no leading zeros)

Note: If the number printed is negative, then the negative sign will consume a leading space/zero, if there are any.

Other Notes

Optimized for GBA development

One of the big realizations in understanding this code is that few GBA games need to display numbers larger than 65,535,
and when they do these are the minority of numbers, not the majority. The %d specifier is highly optimized to work with
16 bits, and you should use it whenever possible.

For the minority of occasions when you need to display more than 16 bits worth of numerical data, %l works fine to
numbers well beyond the plus-or-minus 500 million mark. (The actual limit is +/- 655,359,999.) It takes about twice as
long to run as %d, though, and includes a slow SWI call to split the number into two chunks, so you should still use %d
instead whenever you can.

If you need to display numbers larger than 500 million, then you will either have to heavily alter the code, use a
different utility, or break your number up into sizeable chunks. If you do need to display larger numbers than 500
million for a GBA game, though, you may want to rethink your game design.

What about other ARM machines?

Although posprintf is mostly written as generic Thumb code, the %l (29-bit print) specifier code currently uses a
software interrupt (SWI) specific to the Gameboy Advance to perform a division. If you wish to port this to other ARM
machines, you may need to alter this code. I believe that most ARM machines support SWI 7 as an alternative software
divide, although you will have to swap the numerator/denominator registers (r0 and r1).

Can't you optimize out the SWI call? Use inverse multiplication, register shifting, or somesuch?

Not easily. Not in Thumb mode, anyway, which doesn't have the 64-bit multiply instruction. The number has many
significant digits, and is being divided by 10,000, which requires two registers to keep things accurate. My best advice
is to avoid it in the first place by keeping your numbers small and using %d instead, but also not to fret over using %l
as the SWI is only issued once in the entire routine, and everything else is still quite zippy. It is not like you will
have to call it a bazillion times per frame, and it's still a helluva lot faster than what the standard library sprintf
does. If anyone does feel like optimizing this, though, please send me the results.

If it's displaying 16-bit numbers and they are signed, shouldn't they only be in the range +/- 32767?

The conversion to base 10 is actually done on the number unsigned, so you get a full range up to 65,535. The sign is
taken from the extension of the number to 32 bits, which happens automatically regardless of whether you pass a short or
not. The same applies to %l.

What are the terms and restrictions to using posprintf?

posprintf no longer has a license; it is freely available to use as part of the public domain.

Why "posprintf"?

I think the "po" stood for something originally, but now the "pos" is just a play on my last name. Please, no P.O.S.
jokes. :)
*/

#ifndef _PSPRINTF_HEADER_
#define _PSPRINTF_HEADER_

extern void posprintf(char*, const char*, ...);

#endif
