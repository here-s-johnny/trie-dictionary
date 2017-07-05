/** @file
  Interfejs drzewa trie.

  @ingroup dictionary
  @author Jan Gajl <jg332124@students.mimuw.edu.pl>
  @date 2015-06-06
 */


#include <stdio.h>
#include <wchar.h>
  
/**
  Struktura przechowująca węzeł w drzewie.
 */
struct tNode
{
	/// litera przechowywana w wierzchołku
    wchar_t value;
    /// ilość dzieci wierzchołka
    int childrenCount;
    /// długośc tablicy przechowującej dzieci
    int childrenLength;
    /// flaga mowiąca czy dany wierzchołek jest słowem
    int ifWord;
    /// tablica dzieci wierzchołka
    struct tNode **children;
};

/**
  Inicjalizuje komórkę drzewa.
  Będziemy używać tej funkcji do inicjalizacji korzenia.
  @param[in] n komórka.
*/
void init(struct tNode **n);

/**
  Czyszczenie pamięci drzewa
  @param[in,out] n korzeń drzewa
 */
void tree_free(struct tNode *n);

/**
  Wstawianie slowa do drzewa.
  @param[in] n, komórka, od której zaczynamy wstawianie (root)
  @param[in] word słowo do wstawienia.
  @return 0 jeśli słowo było już w słowniku, 1 jeśli udało się wstawić.
*/
int insert(struct tNode **n, wchar_t *word);

/**
  Usuwanie słowa z drzewa.
  @param[in,out] n komórka, od której zaczynamy wstawianie (root).
  @param[in] word Słowo, które należy usunąć ze słownika.
  @return 0 jeśli słowa nie było w słowniku, 1 jeśli udało się usunąć.
*/
int delete(struct tNode *n, wchar_t *word);

/**
  Szuka podanego słowa w drzewie.
  @param[in] n komórka, od której zaczynamy przeszukiwanie (root).
  @param[in] word Słowo, którego szukamy w słowniku.
  @return 0 jeśli słowa nie było w słowniku, 1 jeśli było.
  */
int search(struct tNode **n, wchar_t *word);

/**
  Zapisuje drzewo do strumienia.
  @param[in] n komórka, od której zaczynamy zapisywanie (root).
  @param[in,out] stream strumień, w którym zapiszemy drzewo.
  @return >=0 jeśli operacja się nie powiedzie, <0 w p.p.
 */
int save(struct tNode *n, FILE* stream);

/**
  Wczytuje drzewo ze strumienia.
  @param[in] stream strumień, z którego wczytane zostanie drzewo.
  @param[in,out] dict slownik, ktory zostanie utwozony.
  @param[in,out] alphabet alfabet.
  @param[in,out] alphabetCount ilość liter w alfabecie.
  @return >0 jeśli udało się wczytać, <= 0 w.p.p.
 */
int load(FILE* stream, struct tNode *n, wchar_t **alphabet, int *alphabetCount);