/** @file
  Implementacja listy słów.

  @ingroup dictionary
  @author Jan Gajl <jg332124@students.mimuw.edu.pl>
  @date 2015-05-31
 */

#include "word_list.h"
#include <stdlib.h>
#include <stdio.h>

/** @name Funkcje pomocnicze
   @{
 */

/**
  Podwaja wielkość tablicy słów
  @param[in,out] arr tablica.
  @param[in] size dotychczasowa wielkość tablicy.
  */    
static void enlarge_array(wchar_t ***arr, int size)
{
    wchar_t **newArr;
    newArr = malloc(2*size*sizeof(wchar_t *));
    int i;
    for (i = 0; i < size; ++i)
    {
        newArr[i] = malloc(MAX_W_LENGTH);
        wcscpy(newArr[i],(*arr)[i]);
        free((*arr)[i]);
    }
    for (i = size; i < 2 * size; ++i)
    {
        newArr[i] = NULL;
    }

    wchar_t **tmp = *arr;
    *arr = newArr;
    free(tmp);
}

/**@}*/

/** @name Elementy interfejsu 
   @{
 */

void word_list_init(struct word_list *list)
{
    (*list).array = (wchar_t **) malloc(sizeof(wchar_t *));
    (*list).array[0] = NULL;
    (*list).wordCount = 0;
    (*list).size = 1;
}

void word_list_done(struct word_list *list)
{
    int i;
    for (i = 0; i < list->wordCount; ++i)
    {
        free(list->array[i]);
    }
    free(list->array);
}

int word_list_add(struct word_list *list, const wchar_t *word)
{
    wchar_t *tmpWord = (wchar_t *)word;
    int i;
    for (i = 0; i < list->wordCount; ++i)
    {
        if (wcscoll(list->array[i], tmpWord) == 0)
        {
            return 0;
        }
    }

    if (list->size == list->wordCount)
    {
        enlarge_array(&list->array, list->size);
        list->size = 2 * list->size;
    }

    list->array[list->wordCount] = malloc(MAX_W_LENGTH);
    wcscpy(list->array[list->wordCount], tmpWord);
    list->wordCount++;

    int done = 0;
    for (i = list->wordCount -1;  i > 0 && done == 0; --i)
    {
        if (wcscoll(list->array[i], list->array[i-1]) < 0)
        {
            wchar_t *tmp = list->array[i];
            list->array[i] = list->array[i-1];
            list->array[i-1] = tmp;
        } else
        {
            done = 1;
        }
    }
    return 1;
}

void word_list_print(struct word_list *list)
{
    int i;
    for (i = 0; i < list->wordCount; ++i)
    {
        printf("%ls\n", list->array[i]);
    }
}

/**@}*/
