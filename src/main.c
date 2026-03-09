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

typedef enum State {
    IDLE,
    FIRST_OPERAND_SET,
    SECOND_OPERAND_INPUT
} State;

static State state = IDLE;
static float firstOperand = 0.0f;
static float secondOperand = 0.0f;
static char operator = 0;

static void activate( GtkApplication *app, gpointer user_data );
static void onButtonClicked( GtkButton *button, gpointer userData );
static gboolean onWindowKeyPressed( GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer userData );
static void concatNewInput( const char *currentText, const char *textToAppend, GtkEntry *entry );
static bool applyOperator( float a, float b, char op, char *out, int outSize );
static void showErrorDialog( GtkEntry *entry, const char *message );

static void doDigitButtonAction( GtkEntry *entry, const char *current, const char *value );
static void doDotButtonAction( GtkEntry *entry, const char *current, const char *value );
static void doEqualsButtonAction( GtkEntry *entry, const char *current );
static void doOperatorButtonAction( GtkEntry *entry, char op, const char *current );
static void doClearButtonAction( GtkEntry *entry );

int main( int argc, char **argv ) {

    GtkApplication *app;
    int status;

    app = gtk_application_new( "br.com.davidbuzatto.learning-gtk", G_APPLICATION_DEFAULT_FLAGS );
    g_signal_connect( app, "activate", G_CALLBACK( activate ), NULL );
    status = g_application_run( G_APPLICATION( app ), argc, argv );
    g_object_unref( app );

    return status;

}

static void activate( GtkApplication *app, gpointer user_data ) {

    GtkWidget *window;

    window = gtk_application_window_new( app );
    gtk_window_set_title ( GTK_WINDOW( window ), "GTK4 Calc" );
    gtk_window_set_default_size( GTK_WINDOW( window ), 200, 200 );

    // css styles
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(
        provider,
        ".entry-result {"
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

    GtkWidget *vbox = gtk_box_new( GTK_ORIENTATION_VERTICAL, 8 );
    gtk_widget_set_margin_top( vbox, 12 );
    gtk_widget_set_margin_bottom( vbox, 12 );
    gtk_widget_set_margin_start( vbox, 12 );
    gtk_widget_set_margin_end( vbox, 12 );
    gtk_window_set_child( GTK_WINDOW(window), vbox );

    GtkWidget *entryResult = gtk_entry_new();
    gtk_widget_set_focusable( entryResult, FALSE );
    gtk_widget_set_can_focus( entryResult, FALSE );
    gtk_widget_set_focus_on_click( entryResult, FALSE );
    gtk_entry_set_placeholder_text( GTK_ENTRY(entryResult), "0" );
    gtk_entry_set_alignment( GTK_ENTRY(entryResult), 1.0 );
    gtk_editable_set_editable( GTK_EDITABLE(entryResult), FALSE );
    gtk_widget_set_hexpand( entryResult, TRUE );

    gtk_widget_add_css_class( entryResult, "entry-result" );
    gtk_box_append( GTK_BOX(vbox), entryResult );
    
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
        { "C", 3, 0, 1 }, { "0", 3, 1, 1 }, { ".", 3, 2, 1 }, { "+", 3, 3, 1 },
        { "=", 4, 0, 4 } 
    };

    int n = sizeof( buttons ) / sizeof( buttons[0] );

    for ( int i = 0; i < n; i++ ) {

        GtkWidget *btn = gtk_button_new_with_label( buttons[i].label );
        gtk_widget_set_focusable( btn, FALSE );
        gtk_widget_set_can_focus( btn, FALSE );
        gtk_widget_set_focus_on_click( btn, FALSE );
        gtk_widget_set_hexpand( btn, TRUE );
        gtk_widget_set_vexpand( btn, TRUE );
        gtk_widget_set_size_request( btn, 64, 64 );
        gtk_widget_add_css_class( btn, "btn" );

        g_signal_connect( btn, "clicked", G_CALLBACK(onButtonClicked), entryResult );
        gtk_grid_attach( 
            GTK_GRID(grid),
            btn,
            buttons[i].col,
            buttons[i].row,
            buttons[i].colspan,
            1
        );

    }

    GtkEventController *keyController = gtk_event_controller_key_new();
    g_signal_connect( keyController, "key-pressed", G_CALLBACK(onWindowKeyPressed), entryResult );
    gtk_widget_add_controller( window, keyController );

    gtk_window_present( GTK_WINDOW(window) );

}

static void concatNewInput( const char *currentText, const char *textToAppend, GtkEntry *entry ) {
    char *newText = g_strconcat( currentText, textToAppend, NULL );
    gtk_editable_set_text( GTK_EDITABLE(entry), newText );
    g_free( newText );
}

static bool applyOperator( float a, float b, char op, char *out, int outSize ) {

    float result;

    if ( op == '/' && b == 0.0f ) {
        // division by zero
        return false;
    }

    switch ( op ) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/': result = a / b; break;
        default:  result = b;     break;
    }

    // removing trailing zeros
    // exemple:
    //     3.0000 turns into 3
    //     3.5000 turns into 3.5
    snprintf( out, outSize, "%.4f", result );
    char *dot = strchr( out, '.' );
    if ( dot ) {
        char *end = out + strlen( out ) - 1;
        while ( end > dot && *end == '0' ) {
            *end-- = '\0';
        }
        if ( *end == '.' ) {
            *end = '\0';
        }
    }

    return true;

}

static void onButtonClicked( GtkButton *button, gpointer userData ) {

    GtkEntry *entry = GTK_ENTRY( userData );
    const char *label = gtk_button_get_label( button );
    const char *current = gtk_editable_get_text( GTK_EDITABLE(entry) );
    const char c = label[0];

    if ( c >= '0' && c <= '9' ) {
        doDigitButtonAction( entry, current, label );
    } else if ( c == '.' ) {
        doDotButtonAction( entry, current, label );
    } else if ( c == 'C' ) { // clear
        doClearButtonAction( entry );
    } else if ( c == '=' ) {
        doEqualsButtonAction( entry, current );
    } else { // operator
        doOperatorButtonAction( entry, c, current );
    }

}

static void doDigitButtonAction( GtkEntry *entry, const char *current, const char *value ) {

    if ( state == IDLE ) {
        concatNewInput( current, value, entry );
    } else if ( state == FIRST_OPERAND_SET ) {
        gtk_editable_set_text( GTK_EDITABLE(entry), value );
        state = SECOND_OPERAND_INPUT;
    } else if ( state == SECOND_OPERAND_INPUT ) {
        concatNewInput( current, value, entry );
    }

}

static void doDotButtonAction( GtkEntry *entry, const char *current, const char *value ) {
    if ( strlen( current ) > 0 && strchr( current, '.' ) == NULL ) {
        concatNewInput( current, value, entry );
    }
}

static void doClearButtonAction( GtkEntry *entry ) {
    state = IDLE;
    operator = 0;
    firstOperand = 0;
    secondOperand = 0;
    gtk_editable_set_text( GTK_EDITABLE(entry), "" );
}

static void doEqualsButtonAction( GtkEntry *entry, const char *current ) {

    if ( state == FIRST_OPERAND_SET ) {

        char result[64];

        if ( !applyOperator( firstOperand, secondOperand, operator, result, sizeof(result) ) ) {
            showErrorDialog( entry, "Division by zero!" );
            doClearButtonAction( entry );
        } else {
            firstOperand = atof( result );
            gtk_editable_set_text( GTK_EDITABLE(entry), result );
        }

    } else if ( state == SECOND_OPERAND_INPUT ) {

        secondOperand = atof( current );
        char result[64];

        if ( !applyOperator( firstOperand, secondOperand, operator, result, sizeof(result) ) ) {
            showErrorDialog( entry, "Division by zero!" );
            doClearButtonAction( entry );
        } else {
            firstOperand = atof( result );
            gtk_editable_set_text( GTK_EDITABLE(entry), result );
            state = FIRST_OPERAND_SET;
        }

    }

}

static void doOperatorButtonAction( GtkEntry *entry, char op, const char *current ) {

    if ( state == IDLE ) {

        firstOperand = strlen( current ) > 0 ? atof( current ) : 0.0f;
        operator = op;
        state = FIRST_OPERAND_SET;

    } else if ( state == FIRST_OPERAND_SET ) {
        operator = op;
    } else if ( state == SECOND_OPERAND_INPUT ) {

        secondOperand = atof( current );
        char result[64];

        if ( !applyOperator( firstOperand, secondOperand, operator, result, sizeof(result) ) ) {
            showErrorDialog( entry, "Division by zero!" );
            doClearButtonAction( entry );
        } else {
            firstOperand = atof( result );
            gtk_editable_set_text( GTK_EDITABLE(entry), result );
            operator = op;
            state = FIRST_OPERAND_SET;
        }

    }

}

static gboolean onWindowKeyPressed( GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer userData ) {

    GtkEntry *entry = GTK_ENTRY( userData );
    const char *current = gtk_editable_get_text( GTK_EDITABLE(entry) );

    if ( keyval >= GDK_KEY_0 && keyval <= GDK_KEY_9 ) {
        int v = keyval - GDK_KEY_0;
        char value[4];
        snprintf( value, 2, "%d", v );
        doDigitButtonAction( entry, current, value );
        return TRUE;
    } else if ( keyval >= GDK_KEY_KP_0 && keyval <= GDK_KEY_KP_9 ) {
        unsigned int v = keyval - GDK_KEY_KP_0;
        char value[4];
        snprintf( value, 2, "%d", v );
        doDigitButtonAction( entry, current, value );
        return TRUE;
    } else if ( keyval == GDK_KEY_period || keyval == GDK_KEY_KP_Decimal ) {
        doDotButtonAction( entry, current, "." );
        return TRUE;
    } else if ( keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter ) {
        doEqualsButtonAction( entry, current );
        return TRUE;
    } else if ( keyval == GDK_KEY_plus || keyval == GDK_KEY_KP_Add ) {
        doOperatorButtonAction( entry, '+', current );
        return TRUE;
    } else if ( keyval == GDK_KEY_minus || keyval == GDK_KEY_KP_Subtract ) {
        doOperatorButtonAction( entry, '-', current );
        return TRUE;
    } else if ( keyval == GDK_KEY_asterisk || keyval == GDK_KEY_KP_Multiply ) {
        doOperatorButtonAction( entry, '*', current );
        return TRUE;
    } else if ( keyval == GDK_KEY_slash || keyval == GDK_KEY_KP_Divide ) {
        doOperatorButtonAction( entry, '/', current );
        return TRUE;
    } else if ( keyval == GDK_KEY_Escape ) {
        doClearButtonAction( entry );
        return TRUE;
    } 

    return FALSE; // bubbles the event
}

static void showErrorDialog( GtkEntry *entry, const char *message ) {
    GtkWindow *window = GTK_WINDOW( gtk_widget_get_root( GTK_WIDGET( entry ) ) );
    GtkAlertDialog *dialog = gtk_alert_dialog_new( "ERROR" );
    gtk_alert_dialog_set_detail( dialog, message );
    gtk_alert_dialog_show( dialog, window );
    g_object_unref( dialog );
}
