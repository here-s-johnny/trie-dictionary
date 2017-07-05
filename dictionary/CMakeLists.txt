# dodajemy bibliotekę dictionary, stworzoną na podstawie pliku dictionary.c
# biblioteka będzie dołączana statycznie (czyli przez linkowanie pliku .o)

add_library (dictionary dictionary.c word_list.c trie.c)


if (CMOCKA)
    # dodajemy plik wykonywalny z testem    
    add_executable (word_list_test word_list.c word_list_test.c)

    # i linkujemy go z biblioteką do testowania
    target_link_libraries (word_list_test ${CMOCKA})

    # wreszcie deklarujemy, że to test
    add_test (word_list_unit_test word_list_test)

    add_executable (dictionary_test dictionary.c dictionary_test.c)
    set_target_properties(
        dictionary_test
        PROPERTIES
        COMPILE_DEFINITIONS UNIT_TESTING=1)
    target_link_libraries (dictionary_test ${CMOCKA})
    add_test (dictionary_unit_test dictionary_test)

    add_executable (trie_test trie.c trie_test.c)
    target_link_libraries (trie_test ${CMOCKA})
    add_test (trie_unit_test trie_test)

    add_executable (io_test trie.c word_list.c io_test.c)
    set_target_properties(
        io_test
        PROPERTIES
        COMPILE_DEFINITIONS IO_TEST=1)
    target_link_libraries (io_test ${CMOCKA})
    add_test (io_unit_test io_test)
endif (CMOCKA)
