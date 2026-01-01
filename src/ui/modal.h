#pragma once
#include "../all.h"

/**
 * Default modal window width.
 */
#define MODAL_WIDTH 60

/**
 * Default modal window height.
 */
#define MODAL_HEIGHT 15

/**
 * Content area positioning (inside modal border).
 */
#define MODAL_CONTENT_X 3
#define MODAL_CONTENT_Y 2

/**
 * Footer position (inside modal).
 */
#define MODAL_FOOTER_Y (MODAL_HEIGHT - 2)

/**
 * Scrollbar position (relative to modal).
 */
#define MODAL_SCROLLBAR_X (MODAL_WIDTH - 2)

/**
 * Maximum visible options in selection lists.
 */
#define MODAL_MAX_VISIBLE (MODAL_HEIGHT - 9)

/**
 * Maximum visible rows in partition table.
 */
#define MODAL_TABLE_ROWS 3

/**
 * Table header Y position.
 */
#define MODAL_TABLE_HEADER_Y 6

/**
 * Table content start Y position.
 */
#define MODAL_TABLE_START_Y 7

/**
 * Creates a centered modal window with a title and border.
 *
 * @param title The title to display at the top of the modal.
 *
 * @return The created modal window.
 */
WINDOW *create_modal(const char *title);

/**
 * Clears the content area of a modal window, preserving the border.
 *
 * @param modal The modal window to clear.
 */
void clear_modal(WINDOW *modal);

/**
 * Destroys a modal window and frees resources.
 *
 * @param modal The modal window to destroy.
 */
void destroy_modal(WINDOW *modal);
