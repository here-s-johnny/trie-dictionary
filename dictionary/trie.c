/** @file
  Implementacja drzewa trie.

  @ingroup dictionary
  @author Jan Gajl <jg332124@students.mimuw.edu.pl>
  @date 2015-06-06
 */

#include <stdio.h>
#include <stdlib.h>
#include <wctype.h>
#include <wchar.h>
#include <assert.h>
#include "trie.h"
#include "io_utils.h"


/** @name Funkcje pomocnicze
 @{
 */

/**
  Porównanie pojedynczych liter funkcją wcscoll
  @param[in] a pierwsza litera.
  @param[in] b druga litera.
  @return 0 jeśli równe, >0 pierwsza większa, <0 druga większa.
  */
static int compare(wchar_t a, wchar_t b)
{
    return wcscoll((wchar_t[2]){a,0}, (wchar_t[2]){b,0});
}

/**
  Sprawdza czy dany node posiada już danego syna
  Sprawdzanie odbywa się wyszukiwaniem binarnym.
  @param[in] word litera, której szukamy.
  @param[in] n node, którego synów przeszukujemy.
*/
int if_child_present(wchar_t word, struct tNode *n)
{
    if (n->children[0] == NULL)
        return -1;
    int low = 0;
    int high = n->childrenCount - 1;
    while (low < high)
    {
        int mid = ((low + high) / 2);
        wchar_t midVal = n->children[mid]->value; 
        if (compare(word,midVal) > 0)
        {
            low = mid + 1;
        } else if (compare(word,midVal) <= 0)
        {
            high = mid;
        }
    } 

    if (compare(word,n->children[low]->value) == 0)
    {
        return low;

    } else 
    {
        return -1;
    } 
}

/**
  Podwaja rozmiar tablicy
  @param[in,out] arr tablica, którą powiększamy.
  @param[in] size dotychczasowy rozmiar.
*/
void increase_array_cap(struct tNode ***arr, int size)
{
    struct tNode **newArr;
    newArr = malloc(2*size*sizeof(struct tNode));
    int i;
    for (i = 0; i < 2 *size; ++i)
    {
        if (i < size)
        {
            newArr[i] = (*arr)[i];
        } else
        {
            newArr[i] = NULL;
        }
    }       
    struct tNode **tmp = *arr;
    *arr = newArr;
    free(tmp);
}

/**@}*/
/** @name Elementy interfejsu 
  @{
 */
void init(struct tNode **n)
{
    (*n) = (struct tNode *) malloc(sizeof(struct tNode));
    (*n)->children = (struct tNode **) malloc(sizeof(struct tNode *));
    (*n)->childrenCount = 0;
    (*n)->childrenLength = 1;
    (*n)->value = '\0';
    (*n)->ifWord = 0;
    (*n)->children[0] = NULL; 
}

void tree_free(struct tNode *n)
{
    if (n != NULL)
    {
        int i;
        for (i = 0; i < n->childrenLength; ++i)
        {
            tree_free(n->children[i]);
        }
        free(n->children);
        free(n);
    }
}

int insert(struct tNode **n, wchar_t *word)
{
    int i = 0;
    struct tNode *previous = (*n);
    struct tNode *current = (*n)->children[0];

    // schodzimy w dół aż skończy się albo drzewo albo słowo
    while (i < wcslen(word) && current != NULL)
    {
        if (previous->childrenCount == previous->childrenLength)
        {
            increase_array_cap(&previous->children, previous->childrenLength);
            previous->childrenLength = 2 * previous->childrenLength;
        }

        int j = 0;
        j = if_child_present(word[i], previous);
     
        if (j >= 0)
        {
            current = previous->children[j];
        } else
        {
            current = previous->children[previous->childrenCount];
        }

        if (current != NULL && word[i] == current->value)
        {
            previous = current;
            current = previous->children[0];
            ++i;
        }

    }

    // jeśli skończyły się jednocześnie
    if (current == NULL && i == wcslen(word))
    {
            return 0;

    // jeśli skończyło się drzewo, ale nie słowo
    } else if (current == NULL)
    {
        int k;
        for (k = i; k < wcslen(word); ++k)
        {
            current = malloc(sizeof(struct tNode));
            current->children = malloc(sizeof(struct tNode *));
            current->value = word[k];
            current->ifWord = (k == wcslen(word) - 1);
            current->childrenLength = 1;
            current->childrenCount = 0;
            current->children[0] = NULL;
            int l = 0;
            while (previous->children[l] != NULL &&
                   compare(previous->children[l]->value, word[k]) < 0) 
            {
                l++;
            }
            int m;
            for (m = previous->childrenCount; m > l; --m)
            {
                struct tNode *tmp = previous->children[m];
                previous->children[m] = previous->children[m-1];
                previous->children[m-1] = tmp;
            }
            previous->children[l] = current;
            previous->childrenCount++;
            previous = current;
            current = current->children[0];
        }
        return 1;

    // jeśli skończyło się słowo, ale nie drzewo
    } else if (i == wcslen(word))
    {
        if (previous->ifWord == 1)
        {
            return 0;
        } else 
        {
            previous->ifWord = 1;
            return 1;
        }
    } 
}

int delete(struct tNode *n, wchar_t *word)
{
    int i = 0;
    struct tNode *last = n;
    int k;    
    struct tNode *previous = n;
    struct tNode *current = n->children[0];
    while (i < wcslen(word) && current != NULL) 
    {
        int j = if_child_present(word[i], previous);
        if (j < 0)
        {
            return 0;
        } else
        {
            current = previous->children[j];
            // zapamiętujemy ostatnią komórkę, która albo ma 2 synów
            // albo ma jednego syna, który jest słowem
            if (current->childrenCount > 1 || 
                (current->children[0] != NULL && 
                    current->children[0]->ifWord == 1 && i < wcslen(word) - 2))
            {
                last = current;
                k = if_child_present(word[i+1], current);
            }
        }

        if (current != NULL && word[i] == current->value)
        {
            previous = current;
            current = previous->children[0];
            ++i;
        }
    }

    if (last == n)
    {
        k = if_child_present(word[0],last);
    }

    if (current == NULL && i == wcslen(word))
    {
        if (last->children[k]->ifWord == 1)
        {
            if (last->children[k]->children[0] == NULL)
            {
                tree_free(last->children[k]);
                last->children[k] = NULL;
            } else 
            {
                tree_free(last->children[k]->children[0]);
                last->children[k]->children[0] = NULL;
            }
        } else 
        {
            tree_free(last->children[k]);
            last->children[k] = NULL;
        }
        int m;
        for (m = k+1; m < last->childrenCount; ++m)
        {
            struct tNode *tmp;
            tmp = last->children[m]; 
            last->children[m] = last->children[m-1];
            last->children[m-1] = tmp;
        }
        last->childrenCount--;
    } else if (current == NULL)
    {
        return 0;
    } else if (i == wcslen(word))
    {
        if (previous->ifWord == 1)
        {
            previous->ifWord = 0;
        } else
        {
            return 0;
        }
    }
    return 1;
}

int search(struct tNode **n, wchar_t *word)
{
    int i = 0;
    struct tNode *previous = (*n);
    struct tNode *current = (*n)->children[0];
    while (current != NULL && i < wcslen(word))
    {
        int j = if_child_present(word[i], previous);
        if (j < 0)
        {
            return 0;
        } else
        {
            current = previous->children[j];
        }

        if (current != NULL && word[i] == current->value)
        {
            previous = current;
            current = previous->children[0];
            ++i;
        }
    }
    if (current == NULL && i == wcslen(word))
    {
        return 1;
    }
    else if (current == NULL)
    {
        return 0;
    } else if (i == wcslen(word))
    {
        if (previous->ifWord == 1)
        {
            return 1;
        } else
        {
            return 0;
        }
    }
}

int save(struct tNode *n, FILE* stream)
{
    int k = 0;
    void save_rec(struct tNode *n, FILE *stream)
    { 
        if (n->childrenCount > 0)
        {
            k = fprintf(stream, "%d", n->childrenCount);
        } else
        {
            fprintf(stream, "%d", 0);  
        }
        int i;
        for (i = 0; i < n->childrenCount && k >= 0; ++i)
        {
            wchar_t c;
            if (n->children[i]->ifWord == 1)
            {
                c = towupper(n->children[i]->value);
            } else
            {
                c = (n->children[i]->value);
            }
            k  = fprintf(stream, "%lc", c);
            save_rec(n->children[i], stream);
        }
    }

    save_rec(n,stream);
    return (k >= 0) ? 0 : -1;
}

int load(FILE* stream, struct tNode *n, wchar_t **alphabet, int *alphabetCount)
{
    int g;

    void load_rec(FILE *stream, struct tNode *n)
    {
        int k;
        int i;
        g = fscanf(stream, "%d", &k);
        n->childrenCount = k;
        for (i = 0; i < k && g > 0; ++i)
        {
            if (n->childrenCount > n->childrenLength)
            {
                increase_array_cap(&n->children, n->childrenLength);
                n->childrenLength = 2 * n->childrenLength;
            }

            n->children[i] = malloc(sizeof(struct tNode));
            n->children[i]->children = malloc(sizeof(struct tNode *));
            n->children[i]->children[0] = NULL;
            n->children[i]->childrenCount = 0;
            n->children[i]->childrenLength = 1;

            wchar_t c;
            g = fscanf(stream, "%lc", &c);
            if (iswupper(c))
            {
                n->children[i]->ifWord = 1;
                wchar_t d = towlower(c);
                n->children[i]->value = d;
                c = d;
            } else
            {
                n->children[i]->ifWord = 0;
                n->children[i]->value = c;
            }

            int j;
            int toInsert = 1;
            for (j = 0; j < (*alphabetCount) && toInsert == 1; ++j)
            {
                if (compare((*alphabet)[j], c) == 0)
                {
                    toInsert = 0;
                }
            }
            if (toInsert == 1)
            {
                (*alphabet)[*alphabetCount] = c;
                (*alphabetCount)++;
            }


            load_rec(stream,n->children[i]);
        }
    }

    load_rec(stream, n);
    return g;
}

/**@}*/
