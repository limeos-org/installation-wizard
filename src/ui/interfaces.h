#pragma once
#include "../all.h"

/**
 * Column alignment options.
 */
typedef enum {
    TABLE_ALIGN_LEFT,
    TABLE_ALIGN_RIGHT,
    TABLE_ALIGN_CENTER
} TableAlign;

/**
 * Defines a single table column.
 */
typedef struct {
    const char *header;
    int width;
    TableAlign align;
} TableColumn;

/**
 * Represents a table row with cells.
 */
typedef struct {
    char cells[6][64];
    int cell_count;
} TableRow;

/**
 * Form field for spinner-type input.
 */
typedef struct {
    const char *label;
    const char **options;
    int option_count;
    int current;
    int readonly;
} FormField;

/**
 * Result of form input handling.
 */
typedef enum {
    FORM_CONTINUE,
    FORM_SUBMIT,
    FORM_CANCEL
} FormResult;

/**
 * Renders a vertical scrollbar with track and thumb indicator.
 *
 * @param window The ncurses window to draw in.
 * @param y Y coordinate of the top of the scrollbar track.
 * @param x X coordinate of the scrollbar.
 * @param height Height of the scrollbar track in characters.
 * @param offset Current scroll offset (first visible item index).
 * @param visible Number of visible items.
 * @param total Total number of items.
 */
void render_scrollbar(
    WINDOW *window, int y, int x, int height, int offset, int visible, int total
);

/**
 * Prints bold text at the specified position.
 *
 * @param window The ncurses window to draw in.
 * @param y Y coordinate.
 * @param x X coordinate.
 * @param format Format string.
 */
void print_bold(WINDOW *window, int y, int x, const char *format, ...);

/**
 * Prints dimmed (gray) text at the specified position. Uses color pair 3.
 *
 * @param window The ncurses window to draw in.
 * @param y Y coordinate.
 * @param x X coordinate.
 * @param format Format string.
 */
void print_dim(WINDOW *window, int y, int x, const char *format, ...);

/**
 * Prints warning (orange) text at the specified position. Uses color pair 7
 * with bold.
 *
 * @param window The ncurses window to draw in.
 * @param y Y coordinate.
 * @param x X coordinate.
 * @param format Format string.
 */
void print_warning(WINDOW *window, int y, int x, const char *format, ...);

/**
 * Prints text with the selected indicator style (bold dodger blue). Uses
 * color pair 6 with bold.
 *
 * @param window The ncurses window to draw in.
 * @param y Y coordinate.
 * @param x X coordinate.
 * @param format Format string.
 */
void print_selected(WINDOW *window, int y, int x, const char *format, ...);

/**
 * Renders a data table with header, scrolling rows, and optional scrollbar.
 *
 * @param window The ncurses window to draw in.
 * @param y Top Y coordinate for header row.
 * @param x Left edge X coordinate.
 * @param columns Array of column definitions.
 * @param column_count Number of columns.
 * @param rows Array of data rows.
 * @param row_count Total number of data rows.
 * @param selected Index of selected row (-1 for none).
 * @param scroll_offset Index of first visible row.
 * @param max_visible Maximum visible rows (excluding header).
 */
void render_table(
    WINDOW *window, int y, int x, TableColumn *columns, int column_count,
    TableRow *rows, int row_count, int selected, int scroll_offset,
    int max_visible
);

/**
 * Renders a form with multiple fields.
 *
 * @param window The ncurses window to draw in.
 * @param y Top Y coordinate.
 * @param x Left edge X coordinate.
 * @param label_width Width reserved for labels.
 * @param fields Array of form fields.
 * @param field_count Number of fields.
 * @param focused Index of currently focused field.
 */
void render_form(
    WINDOW *window, int y, int x, int label_width, FormField *fields,
    int field_count, int focused
);

/**
 * Handles keyboard input for form navigation.
 *
 * @param key The key code from wgetch().
 * @param fields Array of form fields.
 * @param field_count Number of fields.
 * @param out_focused Pointer to focused field index (will be modified).
 *
 * @return Form result indicating action to take.
 */
FormResult handle_form_key(
    int key, FormField *fields, int field_count, int *out_focused
);
