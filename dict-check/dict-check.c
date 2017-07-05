/** @defgroup dict-check Moduł dict-check
    Program parsujący tekst wejściowy.
    Sprawdza, które słowa z wejścia nie znajdują się
    w słowniku i wypisuje dla nich podpowiedzi.
  */

/** @file
  Dict-check sprawdza które słowa z wejścia 
  znajdują się we wczytanym słowniku.

  @ingroup dict-check
  @author Jan Gajl <jg332124@students.mimuw.edu.pl>
  @date 2015-05-31
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include "dictionary.h"
#include "word_list.h"

/**
  Dodaje literke do przetwarzanego słowa
  @param[in,out] str słowo, do którego dodajemy literę.
  @param[in] length długość słowa, do którego dodajemy literę.
 */

void add_letter(wchar_t **str, wchar_t c, int length) 
{
	wchar_t *result = malloc((length+1)*sizeof(wchar_t));
	int i;
	for (i = 0; i < length - 1; i++) {
		result[i] = (*str)[i];
	}
	result[length - 1] = c;
	result[length] = L'\0';
	free(*str);
	(*str) = result;
}

/**
  Usuwa słowo
  @param[in] str słowo do usunięcia.
 */
void destroy_word(wchar_t **str) 
{
	free((*str));
	(*str) = malloc(sizeof(wchar_t));
	(*str)[0] = L'\0';
}

/**
  Funkcja main.
  Główna funkcja programu do parsowania i przetwarzania inputu. 
 */
int main (int argc, char *argv[]) 
{
	setlocale(LC_ALL, "pl_PL.UTF-8");
	FILE *f = NULL;
	if (argc == 2)
	{
		f = fopen(argv[1], "r");
	} else if (argc == 3)
	{
		f = fopen(argv[2], "r");
	} else
	{
		printf("Invalid arguments!");
		exit(1);
	}

	struct dictionary *dict;
	if (f)
		dict = dictionary_load(f);
	if (!f || !(dict))
	{
		printf("Failed to load dictionary\n");
		exit(1);
	}

	fclose(f);
	wchar_t c;
	int character = 1;
	int line = 1;
	int in_word = 0;
	int length = 1;
	wchar_t *word = malloc(sizeof(wchar_t));
	word[0] = L'\0';
	int go_to_next_line = 0;
	do 
	{
		c = fgetwc(stdin);
		if (c == '\n') 
		{
			go_to_next_line = 1;
			character++;
		} else 
		{
			character++;
		}
		if (iswalpha(c)) 
		{
			add_letter(&word, c, length);
			length++;
			if (!in_word)
			 {
				in_word = 1;
			}
		} else 
		{
			if (in_word) 
			{
				wchar_t *lWord = malloc(wcslen(word) * sizeof(wchar_t) + 1);
				int i;
				for (i = 0; i < wcslen(word); ++i)
				{
					lWord[i] = towlower(word[i]);
				}
				lWord[wcslen(word)] = '\0';
				if (dictionary_find(dict, lWord)) 
				{
					printf("%ls", word);
				}  else {
					printf("#%ls", word);
					if (argc == 3 && strcmp(argv[1],"-v") == 0)
					{
						fprintf(stderr, "%d,%d %ls: ", line, character - length, word);
						struct word_list list;
						dictionary_hints(dict, lWord, &list);
              			const wchar_t * const *a = word_list_get(&list);
              			for (size_t i = 0; i < word_list_size(&list); ++i)
              			{
              			    if (i)
              			    { 
                               	fprintf(stderr, " ");
                            }
                  			fprintf(stderr, "%ls", a[i]);
             				}
             				fprintf(stderr, "\n");
             			word_list_done(&list);
					}
					
				}
				in_word = 0;
				destroy_word(&word);
				destroy_word(&lWord);
				length = 1;
			}
			printf("%lc", c);
		}
		if (go_to_next_line == 1)
		{
			line++;
			character = 1;
			go_to_next_line = 0;
		}
	} while (c != EOF);
	free(word);
	dictionary_done(dict);
	return 0;
}
