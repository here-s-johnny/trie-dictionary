/** @file
  Testy słownika.
  Testy operacji na drzewie znajdują sie w pliku trie_test.c.
  @ingroup dictionary
  @author Jan Gajl <jg332124@students.mimuw.edu.pl>
  @date 2015-06-06
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <cmocka.h>
#include <locale.h>
#include "dictionary.h"

/// Podmienia funkcje malloc na _test_malloc z biblioteki cmocka
#ifdef malloc
#undef malloc
#endif /* malloc */
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
/// Podmienia funkcje free na _test_free z biblioteki cmocka
#ifdef free
#undef free
#endif /* free */
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)

/// Funkcja biblioteczna cmocka
void* _test_malloc(const size_t size, const char* file, const int line);
/// Funkcja biblioteczna cmocka
void _test_free(void* const ptr, const char* file, const int line);

/* Atrapy funkcji z interfejsów trie i word_list */

void init(struct tNode **n) { }

int insert(struct tNode **n, wchar_t *word) {return 1;}

bool search(struct tNode **n, const wchar_t* word) {return true;}

int delete(struct tNode *n, const wchar_t *word) {return 1;}

void tree_free(struct tNode **n) { }

void word_list_init(struct word_list *list) { }

void word_list_done(struct word_list *list) { }

int word_list_add(struct word_list *list, const wchar_t *word){return 1;}

int save(struct tNode *n, FILE* stream) {return 1;}

int load(FILE* stream, struct tNode *n, wchar_t *alphabet, int alphabetCount) {return 1;}



/// Test funkcji dictionary_new i dictionary_done
static void dictionary_new_and_done_test(void **state)
{
	struct dictionary *dict = dictionary_new();
	dictionary_done(dict);
}

/// Test funkcji dictionary_insert
static void dictionary_insert_test(void **state)
{
	struct dictionary *dict = dictionary_new();
	const wchar_t *word = L"test";
	assert_int_equal(dictionary_insert(dict, word), 1);
	dictionary_done(dict);
}

/// Test funkcji dictionary_find
static void dictionary_find_test(void **state)
{
	struct dictionary *dict = dictionary_new();
	assert_false(dictionary_find(dict, L"test"));
	dictionary_done(dict);
}

/// Test funkcji dictionary_delete
static void dictionary_delete_test(void **state)
{
	struct dictionary *dict = dictionary_new();
	assert_int_equal(dictionary_delete(dict, L"test"), 0);
	dictionary_done(dict);
}

/// Test funkcji dictionary_hints
static void dictionary_hints_test(void **state)
{
	struct dictionary *dict = dictionary_new();
	struct word_list list;
	dictionary_hints(dict, L"test", &list);
	dictionary_done(dict);
}

/// Funkcja główna modułu
int main(void)
{
	setlocale(LC_ALL, "pl_PL.UTF-8");
	const struct CMUnitTest tests[] =
	{
		cmocka_unit_test(dictionary_new_and_done_test),
		cmocka_unit_test(dictionary_insert_test),
		cmocka_unit_test(dictionary_find_test),
		cmocka_unit_test(dictionary_delete_test),
		cmocka_unit_test(dictionary_hints_test),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}

/**@}*/
