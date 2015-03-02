/* ui_preferences_dialog_cb.h
 *
 * Part of amide - Amide's a Medical Image Dataset Examiner
 * Copyright (C) 2001-2002 Andy Loening
 *
 * Author: Andy Loening <loening@ucla.edu>
 */

/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
  02111-1307, USA.
*/




/* external functions */
void ui_preferences_dialog_cb_roi_width(GtkWidget * widget, gpointer data);
void ui_preferences_dialog_cb_line_style(GtkWidget * widget, gpointer data);
void ui_preferences_dialog_cb_layout(GtkWidget * widget, gpointer data);
void ui_preferences_dialog_cb_save_on_exit(GtkWidget * widget, gpointer data);
void ui_preferences_dialog_cb_apply(GtkWidget* widget, gint page_number, gpointer data);
void ui_preferences_dialog_cb_help(GnomePropertyBox *preferences_dialog, gint page_number, gpointer data);
gboolean ui_preferences_dialog_cb_close(GtkWidget* widget, gpointer data);