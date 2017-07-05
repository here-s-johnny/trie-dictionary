/** @file
  Implementacja słownika.
  Słownik jest w postaci drzewa trie.

  @ingroup dictionary
  @author Jan Gajl <jg332124@students.mimuw.edu.pl>
  @date 2015-05-31
 */

#include "dictionary.h"
#include "word_list.h"
#include "trie.h"
#include "conf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wctype.h>    
#include <dirent.h>
#include <argz.h>
#include <sys/stat.h>

#define _GNU_SOURCE

/// maksymalna ilość liter w alfabecie
#define MAX_ALPHABET_CAP 50

/**
  Struktura przechowująca słownik.
  Alfabet przechowuje wszystkie litery jakie pojawiły się w słowniku
 */
struct dictionary
{
    /// korzeń drzewa
    struct tNode *root;
    /// alfabet
    wchar_t alphabet[MAX_ALPHABET_CAP];
    /// ilość liter w alfabecie
    int alphabetCount;
};

/** @name Funkcje pomocnicze
  @{
 */

/**
  Podwaja rozmiar tablicy
  @param[in,out] arr tablica, którą powiększamy.
  @param[in] size dotychczasowy rozmiar.
*/
void increase_array_capacity(struct tNode ***arr, int size)
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
  Wypisywanie słownika
  Wypisanie słownika, przydatne przy debuggowaniu
*/
void dictionary_print(struct tNode *n)
{
    if (n != NULL)
    {
        printf("%lc %i %i\n", n->value, n->ifWord, n->childrenCount);
        int i;
        for (i = 0; i < n->childrenLength; ++i)
        {
            dictionary_print(n->children[i]);
        }
    }
}  

/**
  Wstawia nowe litery do alfabetu.
  @param[in,out] dict Słownik.
  @param[in] word Słowo, którego litery przeglądamy.
  */
void update_alphabet(struct dictionary *dict, wchar_t *word)
{
    int i;
    int j;
    int toInsert;
    for (i = 0; i < wcslen(word); ++i)
    {
        toInsert = 1;
        for (j = 0; j < dict->alphabetCount && toInsert == 1; ++j)
        {
            if (compare(word[i], dict->alphabet[j]) == 0)
            {
                toInsert = 0;
            }
        }
        if (toInsert == 1)
        {
            if (dict->alphabet == NULL)
            {
                dict->alphabet[0] = word[i];
                dict->alphabetCount = 1;
            } else 
            {
                dict->alphabet[dict->alphabetCount] = word[i];
                dict->alphabetCount++;
            }
        }
    }
}

/**@}*/
/** @name Elementy interfejsu 
  @{
 */
struct dictionary* dictionary_new()
{
    struct dictionary *dict =
        (struct dictionary *) malloc(sizeof(struct dictionary));
    dict->root = NULL;
    int i;
    for (i = 1; i < MAX_ALPHABET_CAP; ++i)
    {
        dict->alphabet[i] = '\0';
    }
    dict->alphabetCount = 0;
    return dict;
}

void dictionary_done(struct dictionary *dict)
{
    if (dict != NULL && dict->root != NULL)
    {
        tree_free(dict->root);
        free(dict);
    } else if (dict != NULL)
    {
        free(dict);
    }
}

int dictionary_insert(struct dictionary *dict, const wchar_t *word)
{
    wchar_t *tmpWord = (wchar_t *)word;

    update_alphabet(dict,tmpWord);

    if (dict->root == NULL)
    {
        init(&dict->root);
        insert(&dict->root,tmpWord);
        return 1;
    } else
    {
        int res = insert(&dict->root,tmpWord);
        return res;
    }
}

int dictionary_delete(struct dictionary *dict, const wchar_t *word)
{
    wchar_t *tmpWord = (wchar_t *)word;
    if (dict->root == NULL)
    {
        return 0;
    } else
    {
        int res = delete(dict->root,tmpWord);
        return res;
    }
}

bool dictionary_find(const struct dictionary *dict, const wchar_t* word)
{
    wchar_t *tmpWord = (wchar_t *)word;
    struct dictionary *tmpDict = (struct dictionary *)dict;
    if (dict == NULL || dict->root == NULL)
    {
        return 0;
    }
    return (search(&tmpDict->root, tmpWord) == 1);
}

int dictionary_save(const struct dictionary *dict, FILE* stream)
{
    if (dict == NULL || dict->root == NULL)
    {
        return -1;
    }
    int k;
    k = save(dict->root,stream);
    return (k >= 0) ? 0 : -1;
}

struct dictionary * dictionary_load(FILE* stream)
{
    int g;
    struct dictionary *dict = dictionary_new();
    init(&dict->root);
 
    void load(FILE *stream, struct tNode *n)
    {
        int k;
        int i;
        g = fscanf(stream, "%d", &k);
        n->childrenCount = k;
        for (i = 0; i < k && g > 0; ++i)
        {
            if (n->childrenCount > n->childrenLength)
            {
                increase_array_capacity(&n->children, n->childrenLength);
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
            for (j = 0; j < dict->alphabetCount && toInsert == 1; ++j)
            {
                if (compare(dict->alphabet[j], c) == 0)
                {
                    toInsert = 0;
                }
            }
            if (toInsert == 1)
            {
                dict->alphabet[dict->alphabetCount] = c;
                dict->alphabetCount++;
            }
 
 
            load(stream,n->children[i]);
        }
    }
 
    load(stream,dict->root);
    if (g <= 0)
    {
        dictionary_done(dict);
    }
    return (g > 0) ? dict : NULL;
}

void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
        struct word_list *list)
{
    struct word_list tmpList;
    word_list_init(&tmpList);

    // faza pierwsza - dodajemy siebie do listy
    word_list_add(&tmpList,word);

    // faza druga - dodajemy modyfikacje przez zamianę
    int i;
    int j;
    wchar_t *tmpWord = (wchar_t *)word;
    for (i = 0; i < wcslen(tmpWord); ++i)
    {
        for (j = 0; j < dict->alphabetCount; ++j)
        {
            wchar_t *tmp = malloc(MAX_W_LENGTH);
            int k;
            for (k = 0; k < wcslen(tmpWord); ++k)
            {
                tmp[k] = tmpWord[k];
            }
            tmp[i] = dict->alphabet[j];
            tmp[wcslen(tmpWord)] = '\0';
            word_list_add(&tmpList,tmp); 
            free(tmp);
        } 
    }

    // faza trzecia - dodajemy modyfikacje przez usunięcie
    for (i = 0; i < wcslen(tmpWord); ++i)
    {
        wchar_t *tmp = malloc(MAX_W_LENGTH);
        for (j = 0; j < wcslen(tmpWord); ++j)
        {
            tmp[j] = tmpWord[j];
        }
        for (j = i; j < wcslen(tmpWord) - 1; ++j)
        {
            tmp[j] = tmp[j+1];
        }
        tmp[wcslen(tmpWord) - 1] = '\0';
        word_list_add(&tmpList,tmp);
        free(tmp);
    }

    // faza czwarta - dodajemy modyfikacje przez dodanie
    for (i = 0; i <= wcslen(tmpWord); ++i)
    {
        for (j = 0; j < dict->alphabetCount; ++j)
        {
            wchar_t *tmp = malloc(MAX_W_LENGTH);
            int k;
            for (k = 0; k < wcslen(tmpWord); ++k)
            {
                tmp[k] = tmpWord[k];
            }
            for (k = wcslen(tmpWord) - 1; k >= i; --k)
            {
                tmp[k+1] = tmp[k];
            }
            tmp[i] = dict->alphabet[j];
            tmp[wcslen(tmpWord) + 1] = '\0';
            word_list_add(&tmpList,tmp);
            free(tmp);
        }
    }

    struct word_list hintList;
    word_list_init(&hintList);
    for (i = 0; i < tmpList.wordCount; ++i)
    {
        if (dictionary_find(dict,tmpList.array[i]))
        {
            word_list_add(&hintList,tmpList.array[i]);
        }
    }
    (*list) = hintList;
    word_list_done(&tmpList);
}

int dictionary_lang_list(char **list, size_t *size) 
{
    DIR *dir = opendir(CONF_PATH);
    if (dir == NULL) 
    {
        *list = malloc(sizeof(char));
        *size = 0;
        return -1;
    }
    *list = NULL;
    *size = 0;
    struct dirent *dirent;
    while ((dirent = readdir(dir)) != NULL) 
    {
        if (strcmp(dirent->d_name, ".") != 0 &&
                strcmp(dirent->d_name, "..") != 0)
        {
            error_t error = argz_add(list, size, dirent->d_name);
            if (error != 0)
            {
                free(*list);
                *list = NULL;
                *size = 0;
                return -1;
            }
        }
    }
    closedir(dir);
    return 0;
}

int dictionary_save_lang(const struct dictionary *dict, const char *lang)
{
    if (mkdir(CONF_PATH, 0755) == -1)
    {
        if (EEXIST != errno)
        {
            return -1;
        }
    }
    char directory[1000];
    strcpy(directory,CONF_PATH);
    strcat(directory,"/");
    strcat(directory,lang);
    FILE *f = fopen(directory, "w");
    int k = dictionary_save(dict,f);
    fclose(f);
    return k;
}

struct dictionary *dictionary_load_lang(const char *lang)
{

    char directory[1000];
    strcpy(directory,CONF_PATH);
    strcat(directory,"/");
    strcat(directory,lang);
    FILE *f = fopen(directory, "r");
    if (f == NULL)
    {
        fclose(f);
        return NULL;
    } else
    {
        struct dictionary *new_dict = dictionary_load(f);
        fclose(f);
        return new_dict;
    }
}


/**@}*/
