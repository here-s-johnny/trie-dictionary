/** @file
  Testy inputu/outputu drzewa.
  @ingroup dictionary
  @author Jan Gajl <jg332124@students.mimuw.edu.pl>
  @date 2015-06-06
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include "trie.h"
#include "dictionary.h"
#include "io_utils.h"
#include <string.h>
#include <locale.h>


/// Maksymalny rozmiar bufora
#define TEMPORARY_BUFFER_SIZE 1000

/// Bufor symujujący wyjście
static wchar_t temporary_buffer[TEMPORARY_BUFFER_SIZE];
/// Aktualny rozmiar bufora
static int it;

/// Atrapa funkcji ferror
int example_test_ferror(FILE* const file) {return 0;}

/// Atrapa funkcji fprintf zapisująca wyjście do temporary_buffer
int example_test_fprintf(FILE* const file, const char *format, wchar_t wc)
{
	if (wc == 0)
		temporary_buffer[it++] = L'0';
	else if (wc == 1)
		temporary_buffer[it++] = L'1';
	else if (wc == 2)
		temporary_buffer[it++] = L'2';	
	else if (wc == 3)
		temporary_buffer[it++] = L'3';	
	else if (wc == 4)
		temporary_buffer[it++] = L'4';	
	else if (wc == 5)
		temporary_buffer[it++] = L'5';	
	else if (wc == 6)
		temporary_buffer[it++] = L'6';	
	else if (wc == 7)
		temporary_buffer[it++] = L'7';	
	else if (wc == 8)
		temporary_buffer[it++] = L'8';	
	else if (wc == 9)
		temporary_buffer[it++] = L'9';
	else 
		temporary_buffer[it++] = wc;
	return 1;
}

/// Atrapa funkci fgetwc
wchar_t example_test_fgetwc(FILE* const file)
{
	return temporary_buffer[it++];
}

/// Atrapa funkcji fscanf
int example_test_fscanf(FILE* const file, const char *format, wchar_t wc)
{
	if (temporary_buffer[it++] == L'0')
		wc = 0;
	else if (temporary_buffer[it++] == L'1')
		wc = 1;	
	else if (temporary_buffer[it++] == L'2')
		wc = 2;	
	else if (temporary_buffer[it++] == L'3')
		wc = 3;	
	else if (temporary_buffer[it++] == L'4')
		wc = 4;	
	else if (temporary_buffer[it++] == L'5')
		wc = 5;	
	else if (temporary_buffer[it++] == L'6')
		wc = 6;	
	else if (temporary_buffer[it++] == L'7')
		wc = 7;
	else if (temporary_buffer[it++] == L'8')
		wc = 8;
	else if (temporary_buffer[it++] == L'9')
		wc = 9;
	else wc = temporary_buffer[it++];
	return 1;
}

/// Atrapa funkcji ungetwc
void example_test_ungetwc(wchar_t wc, FILE* const file)
{
	it--;
}

/// Pomocnicza funkcja sprawdzająca poprawność wygenerowanego słownika
void compare_output(const wchar_t *expected_output)
{
	assert_true(it == wcslen(expected_output));
	for (int i = 0; i < it; ++i)
	{
		assert_true(temporary_buffer[i] == expected_output[i]);
	}
}

/// Funkcja porównująca drzewa
bool trie_check(struct tNode *load, struct tNode *test)
{
	assert_true(load != NULL);
	assert_true(test != NULL);
	assert_true(load->value == test->value);
	assert_int_equal(load->ifWord, test->ifWord);
	printf("%d  %d\n", load->childrenCount, test->childrenCount);
	assert_int_equal(load->childrenCount, test->childrenCount);
	for (int i = 0; i < load->childrenCount; ++i)
	{
		if (!trie_check(load->children[i], test->children[i]))
			return false;
	}
	return true;
}

/// Test funkcji save.
static void save_test(void **state)
{
	it = 0;
	struct tNode *root;
	init(&root);
	insert(&root, L"lannister");
	insert(&root, L"tyrell");
	insert(&root, L"baratheon");
	insert(&root, L"martell");
	insert(&root, L"targaryen");
	insert(&root, L"snow");
	insert(&root, L"tully");
	insert(&root, L"stark");
	insert(&root, L"greyjoy");
	insert(&root, L"frey");
	insert(&root, L"bolton");
	assert_int_equal(save(root, stderr), 0);
	compare_output(L"7b2a1r1a1t1h1e1o1N0o1l1t1o1N0f1r1e1Y0g1r1e1y1j1o1Y0l1a1n1n1i1s1t1e1R0m1a1r1t1e1l1L0s2n1o1W0t1a1r1K0t3a1r1g1a1r1y1e1N0u1l1l1Y0y1r1e1l1L0");
	tree_free(root);
}

/// Test funkcji save z wcharami.
static void save_wchar_test(void **state)
{
	it = 0;
	struct tNode *root;
	init(&root);
	insert(&root, L"pięść");
	insert(&root, L"pnąć");
	insert(&root, L"kiść");
	insert(&root, L"łódź");
	assert_int_equal(save(root, stderr), 0);
	compare_output(L"3k1i1ś1Ć0ł1ó1d1Ź0p2i1ę1ś1Ć0n1ą1Ć0");
	tree_free(root);

}

// /// Test funkcji load.
// static void load_test(void **state)
// {
// 	it = 0;
// 	struct tNode *root;
// 	init(&root);
// 	wchar_t alphabet[50];
// 	int i;
// 	for (i = 0; i < 50; ++i)
// 	{
// 		alphabet[i] = '\0';
// 	} 
// 	int alphabetCount = 0;
// 	insert(&root, L"lannister");
// 	insert(&root, L"tyrell");
// 	insert(&root, L"baratheon");
// 	insert(&root, L"martell");
// 	insert(&root, L"targaryen");
// 	insert(&root, L"snow");
// 	insert(&root, L"tully");
// 	insert(&root, L"stark");
// 	insert(&root, L"greyjoy");
// 	insert(&root, L"frey");
// 	insert(&root, L"bolton");
// 	save(root, stderr);
// 	it = 0;
// 	struct tNode *root2;
// 	init(&root2);
// 	wchar_t alphabet2[50];
// 	for (i = 0; i < 50; ++i)
// 	{
// 		alphabet2[i] = '\0';
// 	} 
// 	int alphabetCount2 = 0;

// 	assert_true(load(stderr, root2, alphabet2, alphabetCount2));
// 	printf("%lc\n", root2->children[0]->value);
// 	assert_true(trie_check(root2, root));
// 	tree_free(root);
// 	tree_free(root2);
// }

// /// Test funkcji load z wcharami.
// static void load_wchar_test(void **state)
// {
// 	it = 0;
// 	struct tNode *root;
// 	init(&root);
// 	wchar_t alphabet[50];
// 	int i;
// 	for (i = 0; i < 50; ++i)
// 	{
// 		alphabet[i] = '\0';
// 	} 
// 	int alphabetCount = 0;
// 	insert(&root, L"pięść");
// 	insert(&root, L"pnąć");
// 	insert(&root, L"kiść");
// 	insert(&root, L"łódź");
// 	save(root, stderr);
// 	it = 0;
// 	struct tNode *root2;
// 	init(&root2);
// 	assert_true(load(stderr, root, alphabet, alphabetCount));
// 	assert_true(trie_check(root, root2));
// 	tree_free(root);
// 	tree_free(root2);
// }

/// Funkcja główna modułu
int main(void) {
	setlocale(LC_ALL, "pl_PL.UTF-8");
    const struct CMUnitTest tests[] = {
		cmocka_unit_test(save_test),
		cmocka_unit_test(save_wchar_test),
        // cmocka_unit_test(load_test),
        // cmocka_unit_test(load_wccdhar_test),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
