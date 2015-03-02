/* ui_preferences_dialog.c
 *
 * Part of amide - Amide's a Medical Image Dataset Examiner
 * Copyright (C) 2001-2003 Andy Loening
 *
 * Author: Andy Loening <loening@alum.mit.edu>
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


#include "amide_config.h"
#ifndef AMIDE_WIN32_HACKS
#include <libgnome/libgnome.h>
#endif
#include "ui_study.h"
#include "ui_preferences_dialog.h"
#include "pixmaps.h"
#include "amitk_canvas.h"
#include "amitk_color_table_menu.h"


// static gchar * line_style_names[] = {
//  "Solid",
//  "On/Off",
//  "Double Dash"
//};

static void roi_width_cb(GtkWidget * widget, gpointer data);
//static void line_style_cb(GtkWidget * widget, gpointer data);
static void layout_cb(GtkWidget * widget, gpointer data);
static void save_on_exit_cb(GtkWidget * widget, gpointer data);
static void save_xif_as_directory_cb(GtkWidget * widget, gpointer data);
static void response_cb (GtkDialog * dialog, gint response_id, gpointer data);
static gboolean delete_event_cb(GtkWidget* widget, GdkEvent * event, gpointer data);
static void maintain_size_cb(GtkWidget * widget, gpointer data);
static void target_empty_area_cb(GtkWidget * widget, gpointer data);



/* function called when the roi width has been changed */
static void roi_width_cb(GtkWidget * widget, gpointer data) {

  ui_study_t * ui_study = data;
  gint new_roi_width;
  GnomeCanvasItem * roi_item;
  AmitkView i_view;
  AmitkViewMode i_view_mode;

  g_return_if_fail(ui_study->study != NULL);

  new_roi_width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

  /* sanity checks */
  if (new_roi_width < UI_STUDY_MIN_ROI_WIDTH) new_roi_width = UI_STUDY_MIN_ROI_WIDTH;
  if (new_roi_width > UI_STUDY_MAX_ROI_WIDTH) new_roi_width = UI_STUDY_MAX_ROI_WIDTH;

  if (ui_study->roi_width != new_roi_width) {
    ui_study->roi_width = new_roi_width;

#ifndef AMIDE_WIN32_HACKS
    gnome_config_push_prefix("/"PACKAGE"/");
    gnome_config_set_int("ROI/Width",ui_study->roi_width);
    gnome_config_pop_prefix();
    gnome_config_sync();
#endif

    for (i_view_mode=0; i_view_mode <= AMITK_STUDY_VIEW_MODE(ui_study->study); i_view_mode++) 
      for (i_view=0; i_view<AMITK_VIEW_NUM; i_view++)
	if (ui_study->canvas[i_view_mode][i_view] != NULL)
	  amitk_canvas_set_roi_width(AMITK_CANVAS(ui_study->canvas[i_view_mode][i_view]), 
				     ui_study->roi_width);
    
    /* update the roi indicator */
    roi_item = g_object_get_data(G_OBJECT(ui_study->preferences_dialog), "roi_item");
    gnome_canvas_item_set(roi_item, "width_pixels", new_roi_width, NULL);
  }

  return;
}


/* function to change the line style */
//static void line_style_cb(GtkWidget * widget, gpointer data) {
//
//  ui_study_t * ui_study=data;
//  GdkLineStyle new_line_style;
//  GnomeCanvasItem * roi_item;
//  AmitkView i_view;
//  AmitkViewMode i_view_mode;
//
//  g_return_if_fail(ui_study->study != NULL);
//
//  /* figure out which menu item called me */
//  new_line_style = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget),"line_style"));
//
//  if (ui_study->line_style != new_line_style) {
//    ui_study->line_style = new_line_style;
//
//#ifndef AMIDE_WIN32_HACKS
//    gnome_config_push_prefix("/"PACKAGE"/");
//    gnome_config_set_int("ROI/LineStyle",ui_study->line_style);
//    gnome_config_pop_prefix();
//    gnome_config_sync();
//#endif
//
//    for (i_view_mode=0; i_view_mode <= AMITK_STUDY_VIEW_MODE(ui_study->study); i_view_mode++)
//      for (i_view=0; i_view<AMITK_VIEW_NUM; i_view++)
//	if (ui_study->canvas[i_view_mode][i_view] != NULL)
//	  amitk_canvas_set_line_style(AMITK_CANVAS(ui_study->canvas[i_view_mode][i_view]), 
//				      ui_study->line_style);
//
//    /* update the roi indicator */
//    roi_item = g_object_get_data(G_OBJECT(ui_study->preferences_dialog), "roi_item");
//    gnome_canvas_item_set(roi_item, "line_style", new_line_style, NULL);
//  }
//
//  return;
//}


/* function called to change the layout */
static void layout_cb(GtkWidget * widget, gpointer data) {

  ui_study_t * ui_study = data;
  AmitkLayout new_layout;

  new_layout = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "layout"));

  if (ui_study->canvas_layout != new_layout) {
    ui_study->canvas_layout = new_layout;

#ifndef AMIDE_WIN32_HACKS
    gnome_config_push_prefix("/"PACKAGE"/");
    gnome_config_set_int("CANVAS/Layout",ui_study->canvas_layout);
    gnome_config_pop_prefix();
    gnome_config_sync();
#endif

    ui_study_update_layout(ui_study);
  }

  return;
}

static void maintain_size_cb(GtkWidget * widget, gpointer data) {

  ui_study_t * ui_study = data;
  gboolean canvas_maintain_size;
  AmitkView i_view;
  AmitkViewMode i_view_mode;

  g_return_if_fail(ui_study->study != NULL);

  canvas_maintain_size = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

  if (ui_study->canvas_maintain_size != canvas_maintain_size) {
    ui_study->canvas_maintain_size = canvas_maintain_size;

#ifndef AMIDE_WIN32_HACKS
    gnome_config_push_prefix("/"PACKAGE"/");
    gnome_config_set_int("CANVAS/MinimizeSize",
			 !ui_study->canvas_maintain_size);
    gnome_config_pop_prefix();
    gnome_config_sync();
#endif

    for (i_view_mode=0; i_view_mode <= AMITK_STUDY_VIEW_MODE(ui_study->study); i_view_mode++)
      for (i_view=0; i_view<AMITK_VIEW_NUM; i_view++)
	if (ui_study->canvas[i_view_mode][i_view] != NULL)
	  amitk_canvas_set_general_properties(AMITK_CANVAS(ui_study->canvas[i_view_mode][i_view]), 
					      ui_study->canvas_maintain_size);
  }



  return;
}


/* function called when the roi width has been changed */
static void target_empty_area_cb(GtkWidget * widget, gpointer data) {

  ui_study_t * ui_study = data;
  gint new_target_empty_area;
  AmitkView i_view;
  AmitkViewMode i_view_mode;

  g_return_if_fail(ui_study->study != NULL);

  new_target_empty_area = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

  /* sanity checks */
  if (new_target_empty_area < 0) new_target_empty_area = 0;
  if (new_target_empty_area > UI_STUDY_MAX_TARGET_EMPTY_AREA) 
    new_target_empty_area = UI_STUDY_MAX_TARGET_EMPTY_AREA;

  if (ui_study->canvas_target_empty_area != new_target_empty_area) {
    ui_study->canvas_target_empty_area = new_target_empty_area;

#ifndef AMIDE_WIN32_HACKS
    gnome_config_push_prefix("/"PACKAGE"/");
    gnome_config_set_int("CANVAS/TargetEmptyArea",ui_study->canvas_target_empty_area);
    gnome_config_pop_prefix();
    gnome_config_sync();
#endif

    for (i_view_mode=0; i_view_mode <= AMITK_STUDY_VIEW_MODE(ui_study->study); i_view_mode++) 
      for (i_view=0; i_view<AMITK_VIEW_NUM; i_view++)
	if (ui_study->canvas[i_view_mode][i_view] != NULL)
	  amitk_canvas_set_target_properties(AMITK_CANVAS(ui_study->canvas[i_view_mode][i_view]), 
					     ui_study->canvas_target_empty_area);
  }

  return;
}


static void save_on_exit_cb(GtkWidget * widget, gpointer data) {

  ui_study_t * ui_study = data;
  gboolean dont_prompt_for_save_on_exit;

  dont_prompt_for_save_on_exit = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

  if (ui_study->dont_prompt_for_save_on_exit != dont_prompt_for_save_on_exit) {
    ui_study->dont_prompt_for_save_on_exit = dont_prompt_for_save_on_exit;

#ifndef AMIDE_WIN32_HACKS
    gnome_config_push_prefix("/"PACKAGE"/");
    gnome_config_set_int("MISC/DontPromptForSaveOnExit",
			 ui_study->dont_prompt_for_save_on_exit);
    gnome_config_pop_prefix();
    gnome_config_sync();
#endif
  }



  return;
}

static void save_xif_as_directory_cb(GtkWidget * widget, gpointer data) {

  ui_study_t * ui_study = data;
  gboolean save_as_directory;

  save_as_directory = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

  if (ui_study->save_xif_as_directory != save_as_directory) {
    ui_study->save_xif_as_directory = save_as_directory;

#ifndef AMIDE_WIN32_HACKS
    gnome_config_push_prefix("/"PACKAGE"/");
    gnome_config_set_int("MISC/SaveXifAsDirectory",
			 ui_study->save_xif_as_directory);
    gnome_config_pop_prefix();
    gnome_config_sync();
#endif
  }



  return;
}



/* changing the color table of a rendering context */
static void color_table_cb(GtkWidget * widget, gpointer data) {

  ui_study_t * ui_study=data;
  AmitkModality modality;
  AmitkColorTable color_table;
  gchar * temp_string;

  modality = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "modality"));
  color_table = gtk_option_menu_get_history(GTK_OPTION_MENU(widget));

  if (ui_study->default_color_table[modality] != color_table) {
    ui_study->default_color_table[modality] = color_table;

    temp_string = g_strdup_printf("DATASETS/DefaultColorTable%s", 
				  amitk_modality_get_name(modality));
#ifndef AMIDE_WIN32_HACKS
    gnome_config_push_prefix("/"PACKAGE"/");
    gnome_config_set_int(temp_string, color_table);
    gnome_config_pop_prefix();
    g_free(temp_string);

    gnome_config_sync();
#endif

  }


  return;
}

static void response_cb (GtkDialog * dialog, gint response_id, gpointer data) {
  
  gint return_val;

  switch(response_id) {

  case GTK_RESPONSE_HELP:
#ifndef AMIDE_WIN32_HACKS
    if (!gnome_help_display("amide.xml", "preferences-dialog", NULL)) 
      g_warning("Failed to load help");
#else
    g_warning("Help is unavailable in the Windows version. Please see the help documentation online at http://amide.sf.net");
#endif
    break;

  case GTK_RESPONSE_CLOSE:
    g_signal_emit_by_name(G_OBJECT(dialog), "delete_event", NULL, &return_val);
    if (!return_val) gtk_widget_destroy(GTK_WIDGET(dialog));
    break;

  default:
    break;
  }

  return;
}

/* function called to destroy the preferences dialog */
gboolean delete_event_cb(GtkWidget* widget, GdkEvent * event, gpointer data) {

  ui_study_t * ui_study = data;

  ui_study->preferences_dialog = NULL;

  return FALSE;
}




/* function that sets up the preferences dialog */
GtkWidget * ui_preferences_dialog_create(ui_study_t * ui_study) {
  
  GtkWidget * dialog;
  gchar * temp_string = NULL;
  GtkWidget * packing_table;
  GtkWidget * label;
  GtkObject * adjustment;
  GtkWidget * spin_button;
  //  GtkWidget * option_menu;
  //  GtkWidget * menuitem;
  //  GdkLineStyle i_line_style;
  GtkWidget * menu;
  GtkWidget * check_button;
  GtkWidget * radio_button1;
  GtkWidget * radio_button2;
  GdkPixbuf * pixbuf;
  GtkWidget * image;
  GdkColormap * colormap;
  GtkWidget * notebook;

  AmitkModality i_modality;
  GnomeCanvas * roi_indicator;
  GnomeCanvasItem * roi_item;
  guint table_row;
  GnomeCanvasPoints * roi_line_points;
  rgba_t outline_color;

  /* sanity checks */
  g_return_val_if_fail(ui_study != NULL, NULL);
  g_return_val_if_fail(ui_study->preferences_dialog == NULL, NULL);
    
  temp_string = g_strdup_printf(_("%s: Preferences Dialog"), PACKAGE);
  dialog = gtk_dialog_new_with_buttons (temp_string,  GTK_WINDOW(ui_study->app),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_HELP, GTK_RESPONSE_HELP,
					GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
					NULL);
  gtk_window_set_title(GTK_WINDOW(dialog), temp_string);
  g_free(temp_string);

  /* setup the callbacks for the dialog */
  g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(response_cb), ui_study);
  g_signal_connect(G_OBJECT(dialog), "delete_event", G_CALLBACK(delete_event_cb), ui_study);

  notebook = gtk_notebook_new();
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), notebook);

  /* ---------------------------
     ROI Drawing page 
     --------------------------- */


  /* start making the widgets for this dialog box */
  packing_table = gtk_table_new(4,5,FALSE);
  label = gtk_label_new(_("ROI Drawing"));
  table_row=0;
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), packing_table, label);

  /* widgets to change the roi's size */
  label = gtk_label_new(_("Width (pixels)"));
  gtk_table_attach(GTK_TABLE(packing_table), label, 
		   0,1, table_row, table_row+1,
		   0, 0, X_PADDING, Y_PADDING);

  adjustment = gtk_adjustment_new(ui_study->roi_width,
				  UI_STUDY_MIN_ROI_WIDTH,
				  UI_STUDY_MAX_ROI_WIDTH,1.0, 1.0, 1.0);
  spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1.0, 0);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin_button),FALSE);
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spin_button), TRUE);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spin_button), TRUE);
  gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(spin_button), GTK_UPDATE_ALWAYS);

  g_signal_connect(G_OBJECT(spin_button), "changed",  G_CALLBACK(roi_width_cb), ui_study);
  gtk_table_attach(GTK_TABLE(packing_table), spin_button, 1,2, 
		   table_row, table_row+1, GTK_FILL, 0, X_PADDING, Y_PADDING);
  table_row++;

  /* widgets to change the roi's line style */
  // Anti-aliased canvas doesn't yet support this */
  // also need to uncomment relevant lines in amitk_canvas_object.c */
  //  label = gtk_label_new(_("Line Style:"));
  //  gtk_table_attach(GTK_TABLE(packing_table), label, 0,1,
  //		   table_row, table_row+1, 0, 0, X_PADDING, Y_PADDING);

  //  option_menu = gtk_option_menu_new();
  //  menu = gtk_menu_new();

  //  for (i_line_style=0; i_line_style<=GDK_LINE_DOUBLE_DASH; i_line_style++) {
  //    menuitem = gtk_menu_item_new_with_label(line_style_names[i_line_style]);
  //    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
  //    g_object_set_data(G_OBJECT(menuitem), "line_style", GINT_TO_POINTER(i_line_style)); 
  //    g_signal_connect(G_OBJECT(menuitem), "activate",  G_CALLBACK(line_style_cb), ui_study);
  //  }
  
  //  gtk_option_menu_set_menu(GTK_OPTION_MENU(option_menu), menu);
  //  gtk_option_menu_set_history(GTK_OPTION_MENU(option_menu), ui_study->line_style);
  //  gtk_widget_set_size_request (option_menu, 125, -1);
  //  gtk_table_attach(GTK_TABLE(packing_table),  option_menu, 1,2, 
  //		   table_row,table_row+1, GTK_FILL, 0,  X_PADDING, Y_PADDING);
  //  table_row++;


  /* a little canvas indicator thingie to show the user who the new preferences will look */
  roi_indicator = GNOME_CANVAS(gnome_canvas_new_aa());
  gtk_widget_set_size_request(GTK_WIDGET(roi_indicator), 100, 100);
  gnome_canvas_set_scroll_region(roi_indicator, 0.0, 0.0, 100.0, 100.0);
  gtk_table_attach(GTK_TABLE(packing_table),  GTK_WIDGET(roi_indicator), 
		   2,4,0,2,
		   GTK_FILL, 0,  X_PADDING, Y_PADDING);
  /* the x axis */
  roi_line_points = gnome_canvas_points_new(5);
  roi_line_points->coords[0] = 25.0; /* x1 */
  roi_line_points->coords[1] = 25.0; /* y1 */
  roi_line_points->coords[2] = 75.0; /* x2 */
  roi_line_points->coords[3] = 25.0; /* y2 */
  roi_line_points->coords[4] = 75.0; /* x3 */
  roi_line_points->coords[5] = 75.0; /* y3 */
  roi_line_points->coords[6] = 25.0; /* x4 */
  roi_line_points->coords[7] = 75.0; /* y4 */
  roi_line_points->coords[8] = 25.0; /* x4 */
  roi_line_points->coords[9] = 25.0; /* y4 */

  if (AMITK_IS_DATA_SET(ui_study->active_object))
    outline_color = 
      amitk_color_table_outline_color(AMITK_DATA_SET_COLOR_TABLE(ui_study->active_object), TRUE);
  else
    outline_color = amitk_color_table_outline_color(AMITK_COLOR_TABLE_BW_LINEAR, TRUE);
  roi_item = gnome_canvas_item_new(gnome_canvas_root(roi_indicator), 
				   gnome_canvas_line_get_type(),
				   "points", roi_line_points, 
				   "fill_color_rgba", amitk_color_table_rgba_to_uint32(outline_color), 
  				   "width_pixels", ui_study->roi_width,
    				   "line_style", ui_study->line_style, NULL);
  gnome_canvas_points_unref(roi_line_points);
  g_object_set_data(G_OBJECT(dialog), "roi_item", roi_item);

  /* ---------------------------
     Canvas Setup
     --------------------------- */


  /* start making the widgets for this dialog box */
  packing_table = gtk_table_new(4,5,FALSE);
  label = gtk_label_new(_("View Setup"));
  table_row=0;
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), packing_table, label);

  label = gtk_label_new(_("Layout:"));
  gtk_table_attach(GTK_TABLE(packing_table), label, 
		   0,1, table_row, table_row+1,
		   0, 0, X_PADDING, Y_PADDING);

  /* the radio buttons */
  radio_button1 = gtk_radio_button_new(NULL);
  colormap = gtk_widget_get_colormap(dialog);
  pixbuf = gdk_pixbuf_new_from_xpm_data(linear_layout_xpm);
  image = gtk_image_new_from_pixbuf(pixbuf);
  g_object_unref(pixbuf);
  gtk_container_add(GTK_CONTAINER(radio_button1), image);
  gtk_table_attach(GTK_TABLE(packing_table), radio_button1,
  		   1,2, table_row, table_row+1,
  		   0, 0, X_PADDING, Y_PADDING);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button1), 
			       (ui_study->canvas_layout == AMITK_LAYOUT_LINEAR));
  g_object_set_data(G_OBJECT(radio_button1), "layout", GINT_TO_POINTER(AMITK_LAYOUT_LINEAR));

  radio_button2 = gtk_radio_button_new(NULL);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radio_button2), 
			     gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button1)));
  pixbuf = gdk_pixbuf_new_from_xpm_data(orthogonal_layout_xpm);
  image = gtk_image_new_from_pixbuf(pixbuf);
  g_object_unref(pixbuf);
  gtk_container_add(GTK_CONTAINER(radio_button2), image);
  gtk_table_attach(GTK_TABLE(packing_table), radio_button2, 2,3, table_row, table_row+1,
  		   0, 0, X_PADDING, Y_PADDING);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button2), 
			       (ui_study->canvas_layout == AMITK_LAYOUT_ORTHOGONAL));
  g_object_set_data(G_OBJECT(radio_button2), "layout", GINT_TO_POINTER(AMITK_LAYOUT_ORTHOGONAL));

  g_signal_connect(G_OBJECT(radio_button1), "clicked", G_CALLBACK(layout_cb), ui_study);
  g_signal_connect(G_OBJECT(radio_button2), "clicked", G_CALLBACK(layout_cb), ui_study);

  table_row++;


  /* do we want the size of the canvas to not resize */
  label = gtk_label_new(_("Maintain view size constant:"));
  gtk_table_attach(GTK_TABLE(packing_table), label, 
		   0,1, table_row, table_row+1, 0, 0, X_PADDING, Y_PADDING);

  check_button = gtk_check_button_new();
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), 
			       ui_study->canvas_maintain_size);
  gtk_table_attach(GTK_TABLE(packing_table), check_button, 
		   1,2, table_row, table_row+1, 0, 0, X_PADDING, Y_PADDING);
  g_signal_connect(G_OBJECT(check_button), "toggled", G_CALLBACK(maintain_size_cb), ui_study);
  table_row++;


  /* widgets to change the amount of empty space in the center of the target */
  label = gtk_label_new(_("Target Empty Area (pixels)"));
  gtk_table_attach(GTK_TABLE(packing_table), label, 
		   0,1, table_row, table_row+1, 0, 0, X_PADDING, Y_PADDING);

  adjustment = gtk_adjustment_new(ui_study->canvas_target_empty_area,
				  0, UI_STUDY_MAX_TARGET_EMPTY_AREA,1.0, 1.0, 1.0);
  spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1.0, 0);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin_button),FALSE);
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spin_button), TRUE);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spin_button), TRUE);
  gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(spin_button), GTK_UPDATE_ALWAYS);

  g_signal_connect(G_OBJECT(spin_button), "changed",  G_CALLBACK(target_empty_area_cb), ui_study);
  gtk_table_attach(GTK_TABLE(packing_table), spin_button, 1,2, 
		   table_row, table_row+1, GTK_FILL, 0, X_PADDING, Y_PADDING);

  /* ----------------------
     Default color tables 
     ---------------------- */
  packing_table = gtk_table_new(4,2,FALSE);
  label = gtk_label_new(_("Default Color Tables"));
  table_row=0;
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), packing_table, label);

  for (i_modality=0; i_modality < AMITK_MODALITY_NUM; i_modality++) {

    /* color table selector */
    temp_string = g_strdup_printf(_("default %s color table:"), 
				  amitk_modality_get_name(i_modality));
    label = gtk_label_new(temp_string);
    g_free(temp_string);
    gtk_table_attach(GTK_TABLE(packing_table), label, 0,1, table_row,table_row+1,
		     X_PACKING_OPTIONS | GTK_FILL, 0, X_PADDING, Y_PADDING);
    gtk_widget_show(label);

    menu = amitk_color_table_menu_new();
    gtk_table_attach(GTK_TABLE(packing_table), menu, 1,2, table_row,table_row+1,
		     X_PACKING_OPTIONS | GTK_FILL, 0, X_PADDING, Y_PADDING);
    gtk_option_menu_set_history(GTK_OPTION_MENU(menu),
				ui_study->default_color_table[i_modality]);
    g_object_set_data(G_OBJECT(menu), "modality", GINT_TO_POINTER(i_modality));
    g_signal_connect(G_OBJECT(menu), "changed", 
		     G_CALLBACK(color_table_cb), ui_study);
    gtk_widget_show(menu);

    table_row++;
  }


  /* ---------------------------
     Miscellaneous stuff
     --------------------------- */


  /* start making the widgets for this dialog box */
  packing_table = gtk_table_new(4,5,FALSE);
  label = gtk_label_new(_("Miscellaneous"));
  table_row=0;
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), packing_table, label);

  check_button = gtk_check_button_new_with_label(_("Don't Prompt for \"Save Changes\" on Exit:"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), 
			       ui_study->dont_prompt_for_save_on_exit);
  gtk_table_attach(GTK_TABLE(packing_table), check_button, 
		   0,1, table_row, table_row+1,
		   GTK_FILL, 0, X_PADDING, Y_PADDING);
  table_row++;
  g_signal_connect(G_OBJECT(check_button), "toggled", G_CALLBACK(save_on_exit_cb), ui_study);


  check_button = gtk_check_button_new_with_label(_("Save .XIF file as directory:"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), 
			       ui_study->save_xif_as_directory);
  gtk_table_attach(GTK_TABLE(packing_table), check_button, 
		   0,1, table_row, table_row+1,
		   GTK_FILL, 0, X_PADDING, Y_PADDING);
  table_row++;
  g_signal_connect(G_OBJECT(check_button), "toggled", G_CALLBACK(save_xif_as_directory_cb), ui_study);


  /* and show all our widgets */
  gtk_widget_show_all(dialog);

  return dialog;
}











