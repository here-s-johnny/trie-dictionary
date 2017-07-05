/** @file
    Plug-in do menu edytora
    @author Jan Gajl <jg332124@students.mimuw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-09-06
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include "editor.h"
#include "word_list.h"
#include "dictionary.h"

static struct dictionary *dict = NULL;

static char *dict_name = NULL;

/** @name Funkcje pomocnicze
  @{
 */

/**
  Zamienia słowo wchar_t na takie samo pisane małymi literami.
  @param[in] word Słowo do zmiany
  @return zmienione słowo.
  */
wchar_t * to_lowercase(const wchar_t *word)
{
  wchar_t *res = malloc(sizeof(wchar_t) * (wcslen(word) + 1));
  res[wcslen(word)] = L'\0';
  for(int i = 0; i < wcslen(word); i++)
    res[i] = towlower(word[i]);
  return res;
}

/**
  Ustawia aktywny słownik.
  @param[in] name Nazwa słownika, który ustawiamy jako aktywny.
  */
void set_active_dictionary(char *name) {
  char *new_dict_name = malloc(sizeof(char) * (strlen(name) + 1));
  for (int i = 0; i <= strlen(name); i++)
    new_dict_name[i] = name[i];
  struct dictionary *new_dict = dictionary_load_lang(new_dict_name);
  if (new_dict != NULL) {
    free(dict_name);
    dict_name = new_dict_name;
    if (dict != NULL)
      dictionary_done(dict);
    dict = new_dict;
  }
  else
    free(new_dict_name);
  if (dict == NULL) {
    if (dict_name == NULL) {
      dict_name = malloc(sizeof(char) * (strlen(name) + 1));
      for (int i = 0; i <= (int)strlen(name); i++)
        dict_name[i] = name[i];
    }
    dict = dictionary_new();
    dictionary_save_lang(dict, dict_name);
  }
}

/**@}*/
/** @name Elementy menu
  @{
 */

/**
  Ustawia język (słownik).
  @param[in] item Parametr używany przez GTK.
  @param[in] data Parametr używany przez GTK.
  */
static void set_language(GtkMenuItem *item, gpointer data) {
  GtkWidget *dialog = gtk_dialog_new_with_buttons("Słownik", NULL, 0,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  NULL);
  GtkWidget *vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *label = gtk_label_new("Wybierz słownik:");
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);
  char *list;
  size_t size;
  int res = dictionary_lang_list(&list, &size);
  if (size == 0) {
    GtkWidget *dialog2 = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, 
                                    GTK_BUTTONS_OK,
                                    "Nie znaleziono słowników, zostanie utworzony nowy, pusty słownik");
    gtk_dialog_run(GTK_DIALOG(dialog2));
    gtk_widget_destroy(dialog2);
    dict_name = "default";
    dict = dictionary_new();
    dictionary_save_lang(dict, dict_name);
  } else {
    GtkWidget *combo = gtk_combo_box_text_new();
    for (int i = -1; i < (int)size - 1; i++)
      if (i == -1 || list[i] == '\0')
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), list + i + 1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 1);
    gtk_widget_show(combo);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
      char *new_active_dict =
        gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
      if (new_active_dict != NULL) {
        set_active_dictionary(new_active_dict);
        g_free(new_active_dict);
        }
    }
  }
  gtk_widget_destroy(dialog);
  free(list);
}



void show_about () {
  GtkWidget *dialog = gtk_about_dialog_new();

  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Text Editor");
  //gtk_window_set_title(GTK_WINDOW(dialog), "About Text Editor");
  
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
     "Text Editor for IPP exercises\n");
    
  gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);
}

void show_help (void) {
  GtkWidget *help_window;
  GtkWidget *label;
  char help[5000];

  help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (help_window), "Help - Text Editor");
  gtk_window_set_default_size(GTK_WINDOW(help_window), 300, 300);
 
  strcpy(help,
         "\nAby podłączyć usługę spell-checkera do programu trzeba:\n\n"
         "Dołączyć ją do menu 'Spell' w menubar.\n\n"
         "Pobrać zawartość bufora tekstu z edytora: całą lub fragment,\n"
         "  zapamiętując pozycję.\n\n");
  strcat(help, "\0");

  label = gtk_label_new(help);
    
  gtk_container_add(GTK_CONTAINER(help_window), label); 

  gtk_widget_show_all(help_window);
}

/**
  Sprawdza pojedyncze słowo i podejmuje akcje na podstawie wyboru użytkownika.
  @param[in] item Parametr używany przez GTK.
  @param[in] data Parametr używany przez GTK.
  */

static void WhatCheck (GtkMenuItem *item, gpointer data) {
  GtkWidget *dialog;
  GtkTextIter start, end;
  char *word;
  gunichar *wword;

  if (dict == NULL) {
    dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR, 
                                    GTK_BUTTONS_OK,
                                    "Najpierw wybierz słownik");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
  } else {

    // Znajdujemy pozycję kursora
    gtk_text_buffer_get_iter_at_mark(editor_buf, &start,
                                     gtk_text_buffer_get_insert(editor_buf));  

    // Jeśli nie wewnątrz słowa, kończymy
    if (!gtk_text_iter_inside_word(&start)) {
      dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                      GTK_BUTTONS_OK,
                                      "Kursor musi być w środku słowa");
      gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
      return;
    }  

    // Znajdujemy początek i koniec słowa, a potem samo słowo 
    end = start;
    // gtk_text_buffer_get_iter_at_mark(editor_buf, &start,
                                     // gtk_text_buffer_get_insert(editor_buf));
    gtk_text_iter_forward_word_end(&end); 
    gtk_text_iter_backward_word_start(&start);
    word = gtk_text_iter_get_text(&start, &end);  

    // Zamieniamy na wide char (no prawie)
    wword = g_utf8_to_ucs4_fast(word, -1, NULL); 

    // Sprawdzamy
    if (dictionary_find(dict, to_lowercase((wchar_t *)wword))) {
      dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                      "Wszystko w porządku,\nśpij spokojnie");
      gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
    }
    else {
      // Czas korekty
      GtkWidget *vbox, *label, *combo, *add_word_button;
      struct word_list hints;
      int i;
      wchar_t **words;  

      dictionary_hints(dict, to_lowercase((wchar_t *)wword), &hints);
      words = (wchar_t **)word_list_get(&hints);
      dialog = gtk_dialog_new_with_buttons("Korekta", NULL, 0, 
                                           GTK_STOCK_OK,
                                           GTK_RESPONSE_ACCEPT,
                                           "Dodaj słowo",
                                           GTK_RESPONSE_APPLY,
                                           GTK_STOCK_CANCEL,
                                           GTK_RESPONSE_REJECT,
                                           NULL);

      // W treści dialogu dwa elementy
      vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
      // Tekst
      label = gtk_label_new("Coś nie tak, mam kilka propozycji");
      gtk_widget_show(label);
      gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);  

      // Spuszczane menu
      combo = gtk_combo_box_text_new();
      for (i = 0; i < word_list_size(&hints); i++) {
        // Combo box lubi mieć Gtk
        char *uword = g_ucs4_to_utf8((gunichar *)words[i], -1, NULL, NULL, NULL);  

        // Dodajemy kolejny element
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), uword);
        g_free(uword);
      }
      gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
      gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 1);
      gtk_widget_show(combo);  

      if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *korekta =
          gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

        // Usuwamy stare
        gtk_text_buffer_delete(editor_buf, &start, &end);
        // Wstawiamy nowe
        char n_char = '\0';
        if (korekta == NULL)
          korekta = &n_char;
        gtk_text_buffer_insert(editor_buf, &start, korekta, -1);
        if (korekta != &n_char)
          g_free(korekta);
      } else if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_APPLY) {

        dictionary_insert(dict, to_lowercase((wchar_t *)wword));
        dictionary_save_lang(dict, dict_name);

        gtk_text_buffer_get_start_iter(editor_buf, &end);
        while (!gtk_text_iter_is_end(&end)) {
          gtk_text_iter_forward_word_end(&end);
          GtkTextIter oldStart = start;
          start = end;
          gtk_text_iter_backward_word_start(&start);
          if (gtk_text_iter_equal(&oldStart, &start))
            break;
          char *new_word = gtk_text_iter_get_text(&start, &end);
          wchar_t *new_wword = (wchar_t *)g_utf8_to_ucs4_fast(new_word, -1, NULL);
          if (wcscmp(to_lowercase(new_wword),to_lowercase(wword)) == 0) 
            gtk_text_buffer_remove_tag(editor_buf,
              gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(editor_buf),
              "red_fg"), &start, &end);
          g_free(new_word);
        }
      }
      gtk_widget_destroy(dialog);
    }
    g_free(word);
    g_free(wword);
  }
}

/**
  Koloruje na czerwono wszystkie słowa spoza słownika.
  @param[in] item Parametr używany przez GTK.
  @param[in] data Parametr używany przez GTK.
  */
void colour(GtkMenuItem *item, gpointer data) {
  GtkWidget *dialog;

  if (dict == NULL) {
    dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR, 
                                    GTK_BUTTONS_OK,
                                    "Najpierw wybierz słownik");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
  } else {
    if (gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(editor_buf), "red_fg") == NULL)
      gtk_text_buffer_create_tag(editor_buf, "red_fg",
        "foreground", "red", "weight", PANGO_WEIGHT_BOLD, NULL);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(editor_buf, &end);
    while (!gtk_text_iter_is_end(&end)) {
      gtk_text_iter_forward_word_end(&end);
      GtkTextIter oldStart = start;
      start = end;
      gtk_text_iter_backward_word_start(&start);
      if (gtk_text_iter_equal(&oldStart, &start))
        break;
      char *word = gtk_text_iter_get_text(&start, &end);
      wchar_t *wword = to_lowercase((wchar_t *)g_utf8_to_ucs4_fast(word, -1, NULL));

      if (!dictionary_find(dict, wword))
        gtk_text_buffer_apply_tag_by_name(editor_buf, "red_fg",
              &start, &end);
      free(wword);
      g_free(word);
      }
  }

}

/**
  Odkolorowuje zakolorowane słowa.
  @param[in] item Parametr używany przez GTK.
  @param[in] data Parametr używany przez GTK.
  */
void uncolour(GtkMenuItem *item, gpointer data) {
  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(editor_buf, &end);
  while (!gtk_text_iter_is_end(&end)) {
    gtk_text_iter_forward_word_end(&end);
    GtkTextIter oldStart = start;
    start = end;
    gtk_text_iter_backward_word_start(&start);
    if (gtk_text_iter_equal(&oldStart, &start))
      break;
    gtk_text_buffer_remove_tag(editor_buf,
      gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(editor_buf),
      "red_fg"), &start, &end);
    }
}
/**@}*/

/**
  Dołącza wszystkie powyższe funkcjonalności do menu.
  @param[in] item Parametr używany przez GTK.
  @param[in] data Parametr używany przez GTK.
  */
// Tutaj dodacie nowe pozycje menu

void extend_menu (GtkWidget *menubar) {
  GtkWidget *spell_menu_item, *spell_menu, *check_item, 
            *language_choice_menu_item, *language_choice_menu, *language_item,
            *colour_item, *uncolour_item;

  spell_menu_item = gtk_menu_item_new_with_label("Spell");
  spell_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(spell_menu_item), spell_menu);
  gtk_widget_show(spell_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), spell_menu_item);

  language_choice_menu_item = gtk_menu_item_new_with_label("Language");
  language_choice_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(language_choice_menu_item),
                            language_choice_menu);
  gtk_widget_show(language_choice_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), language_choice_menu_item);

  language_item = gtk_menu_item_new_with_label("Choose language");
  g_signal_connect(G_OBJECT(language_item), "activate", 
                   G_CALLBACK(set_language), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(language_choice_menu), language_item);
  gtk_widget_show(language_item);

  check_item = gtk_menu_item_new_with_label("Check Word");
  g_signal_connect(G_OBJECT(check_item), "activate", 
                   G_CALLBACK(WhatCheck), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), check_item);
  gtk_widget_show(check_item);

  colour_item = gtk_menu_item_new_with_label("Colour words not in dictionary");
  g_signal_connect(G_OBJECT(colour_item), "activate",
                   G_CALLBACK(colour), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), colour_item);
  gtk_widget_show(colour_item);

  uncolour_item = gtk_menu_item_new_with_label("Deactivate colour");
  g_signal_connect(G_OBJECT(uncolour_item), "activate",
                   G_CALLBACK(uncolour), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), uncolour_item);
  gtk_widget_show(uncolour_item);
}

/*EOF*/
