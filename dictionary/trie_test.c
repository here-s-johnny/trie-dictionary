/** @file
  Testy drzewa.

  @ingroup dictionary
  @author Jan Gajl <jg332124@students.mimuw.edu.pl>
  @date 2015-06-06
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include <locale.h>
#include "trie.h"
// #include "trie.c"

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

extern int if_child_present(wchar_t word, struct tNode *n);
extern void increase_array_cap(struct tNode ***arr, int size);

/// Test binsearchu, zakłada, że działa init i insert
static void if_child_present_test(void **state)
{
	struct tNode *trie;
	init(&trie);
	insert(&trie, L"a");
	insert(&trie, L"b");
	insert(&trie, L"c");
	insert(&trie, L"d");
	insert(&trie, L"e");
	assert_int_equal(if_child_present(L'b', trie), 1);
	assert_int_equal(if_child_present(L'k', trie), -1);
	assert_int_equal(if_child_present(L'a', trie), 0);
	assert_int_equal(if_child_present(L'd', trie), 3);
	assert_int_equal(if_child_present(L'z', trie), -1);
	tree_free(trie);
}

/// Test podwajania tablicy, zakładamy, że działa init
static void increase_array_cap_test(void **state)
{
	struct tNode *trie;
	init(&trie);
	assert_null(trie->children[0]);
	assert_int_equal(trie->childrenLength, 1);
	increase_array_cap(&trie->children, 1);
	assert_null(trie->children[1]);
	increase_array_cap(&trie->children, 2);
	assert_null(trie->children[2]);
	assert_null(trie->children[3]);
	tree_free(trie);
}

/// Test inicjacji korzenia
static void init_test(void **state)
{
	struct tNode *trie;
	init(&trie);
	assert_non_null(trie);
	assert_non_null(trie->children);
	assert_true(trie->value == '\0');
	assert_int_equal(trie->ifWord, 0);
	assert_int_equal(trie->childrenCount, 0);
	assert_int_equal(trie->childrenLength, 1);
	tree_free(trie);
}

/// Test zwalniania drzewa z pamięci
static void tree_free_test(void **state)
{
	struct tNode *trie;	
	init(&trie);
	tree_free(trie);
}

/** Test wstawiania słowa do drzewa,
    testuje wszystkie możliwości wstawiania słowa. */
static void insert_test(void **state)
{
	struct tNode *trie;
	init(&trie);
	int size = 0;
	wchar_t w0[] = L"test";
	assert_int_equal(insert(&trie, w0), 1);
	wchar_t w1[] = L"teścik";
	assert_int_equal(insert(&trie, w1), 1);
	wchar_t w2[] = L"tester";
	assert_int_equal(insert(&trie, w2), 1);
	wchar_t w3[] = L"atest";
	assert_int_equal(insert(&trie, w3), 1);
	wchar_t w4[] = L"tes";
	assert_int_equal(insert(&trie, w4), 1);	
	assert_int_equal(insert(&trie, w0), 0);
	tree_free(trie);
}

/// Test funkcji search
static void search_test(void **state)
{
	struct tNode *trie;
	init(&trie);
	int size = 0;
	wchar_t w0[] = L"test";
	assert_false(search(&trie, w0));
	assert_int_equal(insert(&trie, w0), 1);
	assert_true(search(&trie, w0));
	wchar_t w1[] = L"teścik";
	assert_false(search(&trie, w1));
	assert_int_equal(insert(&trie, w1), 1);
	assert_true(search(&trie, w1));
	wchar_t w2[] = L"tester";
	assert_false(search(&trie, w2));
	assert_int_equal(insert(&trie, w2), 1);
	assert_true(search(&trie, w2));
	wchar_t w3[] = L"atest";
	assert_false(search(&trie, w3));
	assert_int_equal(insert(&trie, w3), 1);
	assert_true(search(&trie, w3));
	wchar_t w4[] = L"tes";
	assert_false(search(&trie, w4));
	assert_int_equal(insert(&trie, w4), 1);
	assert_true(search(&trie, w4));
	assert_true(search(&trie, w0));
	assert_int_equal(insert(&trie, w0), 0);
	assert_true(search(&trie, w0));
	tree_free(trie);
}

/** Test usuwania słowa z drzewa,
    testuje wszystkie przypadki. */
static void delete_test(void **state)
{
	struct tNode *trie;
	init(&trie);
	int size = 0;
	wchar_t w0[] = L"test";
	insert(&trie, w0);
	wchar_t w1[] = L"teścik";
	insert(&trie, w1);
	wchar_t w2[] = L"tester";
	insert(&trie, w2);
	wchar_t w3[] = L"atest";
	insert(&trie, w3);
	wchar_t w4[] = L"tes";
	insert(&trie, w4);
	assert_true(search(&trie, w4));
	delete(trie, w4);
	assert_false(search(&trie, w4));
	assert_true(search(&trie, w1));
	delete(trie, w1);
	assert_false(search(&trie, w1));
	assert_true(search(&trie, w2));
	delete(trie, w2);
	assert_false(search(&trie, w2));
	assert_true(search(&trie, w0));
	delete(trie, w0);
	assert_false(search(&trie, w0));
	tree_free(trie);
}

/// Funkcja główna modułu.
int main(void)
{
	setlocale(LC_ALL, "pl_PL.UTF-8");
	const struct CMUnitTest tests[] =
	{
		cmocka_unit_test(if_child_present_test),
		cmocka_unit_test(increase_array_cap_test),
		cmocka_unit_test(init_test),
		cmocka_unit_test(tree_free_test),
		cmocka_unit_test(insert_test),
		cmocka_unit_test(search_test),
		cmocka_unit_test(delete_test),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}

/**@}*/
