/** @file
  Nagłówek testowy modułu io_test.c.
  @ingroup dictionary
  @author Jan Gajl <jk332124@students.mimuw.edu.pl>
  @date 2015-06-06
 */

#ifdef IO_TEST

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#ifdef ferror
#undef ferror
#endif /* ferror */
#define ferror example_test_ferror
int example_test_ferror(FILE* const file);

#ifdef fprintf
#undef fprintf
#endif /* fprintf */
#define fprintf example_test_fprintf
int example_test_fprintf(FILE* const file, const char *format, wchar_t wc);

#ifdef fscanf
#undef fscanf
#endif /* fscanf */
#define fscanf example_test_fscanf
int example_test_fscanf(FILE* const file, const char *format, wchar_t wc);

#ifdef fgetwc
#undef fgetwc
#endif /* fgetwc */
#define fgetwc example_test_fgetwc
wchar_t example_test_fgetwc(FILE* const file);

#ifdef ungetwc
#undef ungetwc
#endif /* ungetwc */
#define ungetwc example_test_ungetwc
void example_test_ungetwc(wchar_t wc, FILE* const file);

#endif /* IO_TEST */

/**@}*/