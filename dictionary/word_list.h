/** @file
    Interfejs listy słów.

    @ingroup dictionary
    @author Jan Gajl <jg332124@students.mimuw.edu.pl>
    @date 2015-05-31
 */

#ifndef __WORD_LIST_H__
#define __WORD_LIST_H__

#include <wchar.h>

/**
  Maksymalna liczba znaków w słowie
 */
#define MAX_W_LENGTH 10000

/**
  Struktura przechowująca listę słów.
 */
struct word_list
{
    /// Liczba słów w tablicy
    int wordCount;
    /// Rozmiar tablicy
    size_t size;
    /// Tablica słów
    wchar_t **array;
};

/**
  Inicjuje listę słów.
  @param[in,out] list Lista słów.
  */
void word_list_init(struct word_list *list);

/**
  Destrukcja listy słów.
  @param[in,out] list Lista słów.
  */
void word_list_done(struct word_list *list);

/**
  Dodaje słowo do listy.
  @param[in,out] list Lista słów.
  @param[in] word Dodawane słowo.
  @return 1 jeśli się udało, 0 w p.p.
  */
int word_list_add(struct word_list *list, const wchar_t *word);

/**
  Wypisuje elementy listy.
  @param[in] list Lista słów.
 */
void word_list_print(struct word_list *list);

/**
  Zwraca liczę słów w liście.
  @param[in] list Lista słów.
  @return Liczba słów w liście.
  */
static inline
size_t word_list_size(const struct word_list *list)
{
    return (size_t)list->wordCount;
}

/**
  Zwraca tablicę słów w liście.
  @param[in] list Lista słów.
  @return Tablica słów.
  */
static inline
const wchar_t * const * word_list_get(const struct word_list *list)
{
    const wchar_t * const * arr = (const wchar_t * const *)list->array;
    return arr;
}

#endif /* __WORD_LIST_H__ */
