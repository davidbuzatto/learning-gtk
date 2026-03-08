/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Program entry point.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <gtk-4.0/gtk/gtk.h>

static void on_button_clicked( GtkButton *button, gpointer user_data ) {

    GtkEntry *entry_resultado = GTK_ENTRY( user_data );
    const char *label = gtk_button_get_label( button );
    const char *current = gtk_editable_get_text( GTK_EDITABLE(entry_resultado) );

    if ( g_strcmp0( label, "C" ) == 0 ) {
        gtk_editable_set_text( GTK_EDITABLE(entry_resultado), "" );
    } else if ( g_strcmp0( label, "=" ) == 0 ) {
        gtk_editable_set_text( GTK_EDITABLE(entry_resultado), "resultado..." );
    } else {
        char *new_text = g_strconcat( current, label, NULL );
        gtk_editable_set_text( GTK_EDITABLE(entry_resultado), new_text );
        g_free( new_text );
    }

}

static void activate( GtkApplication *app, gpointer user_data ) {

    GtkWidget *window;

    window = gtk_application_window_new( app );
    gtk_window_set_title ( GTK_WINDOW( window ), "Calculadora GTK4" );
    gtk_window_set_default_size( GTK_WINDOW( window ), 200, 200 );

    // estilos
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(
        provider,
        ".entry-resultado {"
        "    color: black;"
        "    font-size: 28px;"
        "    padding: 12px;"
        "}"
        ".btn {"
        "    color: black;"
        "    font-size: 28px;"
        "    padding: 12px;"
        "}"
    );

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    // caixa vertical
    GtkWidget *vbox = gtk_box_new( GTK_ORIENTATION_VERTICAL, 8 );
    gtk_widget_set_margin_top( vbox, 12 );
    gtk_widget_set_margin_bottom( vbox, 12 );
    gtk_widget_set_margin_start( vbox, 12 );
    gtk_widget_set_margin_end( vbox, 12 );
    gtk_window_set_child( GTK_WINDOW(window), vbox );

    GtkWidget *entry_resultado = gtk_entry_new();
    gtk_entry_set_placeholder_text( GTK_ENTRY(entry_resultado), "0" );
    gtk_entry_set_alignment( GTK_ENTRY(entry_resultado), 1.0 );
    gtk_editable_set_editable( GTK_EDITABLE(entry_resultado), FALSE );
    gtk_widget_set_hexpand( entry_resultado, TRUE );

    gtk_widget_add_css_class( entry_resultado, "entry-resultado" );
    gtk_box_append( GTK_BOX(vbox), entry_resultado );

    // grade para os botões
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing( GTK_GRID(grid), 6 );
    gtk_grid_set_column_spacing( GTK_GRID(grid), 6 );
    gtk_box_append( GTK_BOX(vbox ), grid );

    struct { 
        const char *label;
        int row;
        int col;
        int colspan;
    } buttons[] = {
        { "7", 0, 0, 1 }, { "8", 0, 1, 1 }, { "9", 0, 2, 1 }, { "/", 0, 3, 1 }, 
        { "4", 1, 0, 1 }, { "5", 1, 1, 1 }, { "6", 1, 2, 1 }, { "*", 1, 3, 1 }, 
        { "1", 2, 0, 1 }, { "2", 2, 1, 1 }, { "3", 2, 2, 1 }, { "-", 2, 3, 1 }, 
        { "C", 3, 0, 1 }, { "0", 3, 1, 1 }, { ".", 3, 2, 1 }, { "=", 3, 3, 1 } 
    };

    int n = sizeof( buttons ) / sizeof( buttons[0] );

    for ( int i = 0; i < n; i++ ) {

        GtkWidget *btn = gtk_button_new_with_label( buttons[i].label );
        gtk_widget_set_hexpand( btn, TRUE );
        gtk_widget_set_vexpand( btn, TRUE );
        gtk_widget_set_size_request( btn, 64, 64 );
        gtk_widget_add_css_class( btn, "btn" );

        g_signal_connect( btn, "clicked", G_CALLBACK(on_button_clicked), entry_resultado );
        gtk_grid_attach( 
            GTK_GRID(grid),
            btn,
            buttons[i].col,
            buttons[i].row,
            buttons[i].colspan,
            1
        );

    }

    gtk_window_present( GTK_WINDOW(window) );

}

int main( int argc, char **argv ) {

    GtkApplication *app;
    int status;

    app = gtk_application_new( "br.com.davidbuzatto.learning-gtk", G_APPLICATION_DEFAULT_FLAGS );
    g_signal_connect( app, "activate", G_CALLBACK( activate ), NULL );
    status = g_application_run( G_APPLICATION( app ), argc, argv );
    g_object_unref( app );

    return status;

}