/* gbfs.h
   access object in a GBFS file

Copyright 2002 Damian Yerrick

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

*/

/* Dependency on prior include files

Before you #include "gbfs.h", you should define the following types:
  typedef (unsigned 16-bit integer) u16;
  typedef (unsigned 32-bit integer) u32;
Your gba.h should do this for you.
*/

//\{

//\}

/*! \name Volatile types
*	Volatile types for registers
*/
//\{

//\}

/*! \name Const types
*	Const types for const function aprameters
*/
//\{

//\}

//! 8-word type for fast struct-copies

//! Type for consting a string as well as the pointer than points to it.

extern (C):

alias uchar = ubyte;
alias echar = ubyte;
alias hword = ushort;
alias eshort = ushort;
alias word = uint;
alias eint = uint;
alias vu8 = ubyte;
alias vu16 = ushort;
alias vu32 = uint;
alias vu64 = ulong;
alias vs8 = byte;
alias vs16 = short;
alias vs32 = int;
alias vs64 = long;
alias cu8 = const ubyte;
alias cu16 = const ushort;
alias cu32 = const uint;
alias cu64 = const ulong;
alias cs8 = const byte;
alias cs16 = const short;
alias cs32 = const int;
alias cs64 = const long;

struct BLOCK
{
    uint[8] data;
}

alias CSTR = const char*;

/* to make a 300 KB space called samples do GBFS_SPACE(samples, 300) */

enum GBFS_ENTRY_SIZE = 24;

struct GBFS_FILE
{
    char[16] magic; /* "PinEightGBFS\r\n\032\n" */
    uint total_len; /* total length of archive */
    ushort dir_off; /* offset in bytes to directory */
    ushort dir_nmemb; /* number of files */
    char[8] reserved; /* for future use */
}

struct GBFS_ENTRY
{
    char[GBFS_ENTRY_SIZE] name; /* filename, nul-padded */
    uint len; /* length of object in bytes */
    uint data_offset; /* in bytes from beginning of file */
}

const(GBFS_FILE)* find_first_gbfs_file (const(void)* start);
const(void)* skip_gbfs_file (const(GBFS_FILE)* file);
const(void)* gbfs_get_obj (const(GBFS_FILE)* file, const(char)* name, uint* len);
const(void)* gbfs_get_nth_obj (const(GBFS_FILE)* file, size_t n, char* name, uint* len);
void* gbfs_copy_obj (void* dst, const(GBFS_FILE)* file, const(char)* name);
size_t gbfs_count_objs (const(GBFS_FILE)* file);

