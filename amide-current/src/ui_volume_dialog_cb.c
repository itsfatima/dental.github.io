/* ui_volume_dialog_cb.c
 *
 * Part of amide - Amide's a Medical Image Dataset Examiner
 * Copyright (C) 2001 Andy Loening
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

#include "config.h"
#include <gnome.h>
#include <math.h>
#include "study.h"
#include "amitk_threshold.h"
#include "ui_study.h"
#include "ui_volume_dialog.h"
#include "ui_volume_dialog_cb.h"
#include "ui_time_dialog.h"
#include "../pixmaps/PET.xpm"
#include "../pixmaps/SPECT.xpm"
#include "../pixmaps/CT.xpm"
#include "../pixmaps/MRI.xpm"
#include "../pixmaps/OTHER.xpm"


/* function called when the name of the volume has been changed */
void ui_volume_dialog_cb_change_name(GtkWidget * widget, gpointer data) {

  volume_t * volume_new_info = data;
  gchar * new_name;
  GtkWidget * volume_dialog;

  /* get the contents of the name entry box and save it */
  new_name = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);
  volume_set_name(volume_new_info, new_name);
  g_free(new_name);

  /* tell the volume_dialog that we've changed */
  volume_dialog =  gtk_object_get_data(GTK_OBJECT(widget), "volume_dialog");
  gnome_property_box_changed(GNOME_PROPERTY_BOX(volume_dialog));

  return;
}

/* function called when the scan date of the volume has been changed */
void ui_volume_dialog_cb_change_scan_date(GtkWidget * widget, gpointer data) {

  volume_t * volume_new_info = data;
  gchar * new_date;
  GtkWidget * volume_dialog;

  /* get the contents of the name entry box and save it */
  new_date = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);
  volume_set_scan_date(volume_new_info, new_date);
  g_free(new_date);

  /* tell the volume_dialog that we've changed */
  volume_dialog =  gtk_object_get_data(GTK_OBJECT(widget), "volume_dialog");
  gnome_property_box_changed(GNOME_PROPERTY_BOX(volume_dialog));

  return;
}

/* when a numerical entry of the volume has been changed, 
   used for the coordinate frame offset, */
void ui_volume_dialog_cb_change_entry(GtkWidget * widget, gpointer data) {

  volume_t * volume_new_info = data;
  gchar * str;
  gint error;
  gdouble temp_val;
  floatpoint_t scale;
  which_entry_widget_t which_widget;
  realpoint_t old_center, new_center, shift;
  GtkWidget * volume_dialog;
  GtkWidget * entry;
  guint i;
  gboolean aspect_ratio=TRUE;
  gboolean update_size_x = FALSE;
  gboolean update_size_y = FALSE;
  gboolean update_size_z = FALSE;
  
  /* initialize the center variables based on the old volume info */
  old_center = new_center = volume_calculate_center(volume_new_info); /* in real coords */

  /* figure out which widget this is */
  which_widget = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "type")); 

  /* get the pointer ot the volume dialog and get any necessary info */
  volume_dialog =  gtk_object_get_data(GTK_OBJECT(widget), "volume_dialog");
  aspect_ratio =  GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(volume_dialog), "aspect_ratio"));


  /* get the contents of the name entry box */
  str = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);

  /* convert to a floating point */
  error = sscanf(str, "%lf", &temp_val);
  g_free(str);

  if ((error == EOF))  /* make sure it's a valid number */
    return;
  
  /* and save the value until it's applied to the actual volume */
  switch(which_widget) {
  case VOXEL_SIZE_X:
    if (temp_val > SMALL) { /* can't be having negative/very small numbers */
      if (aspect_ratio) {
	scale = temp_val/volume_new_info->voxel_size.x;
	volume_new_info->voxel_size.y = scale*volume_new_info->voxel_size.y;
	volume_new_info->voxel_size.z = scale*volume_new_info->voxel_size.z;
	update_size_y = update_size_z = TRUE;
      }
      volume_new_info->voxel_size.x = temp_val;
    }
    break;
  case VOXEL_SIZE_Y:
    if (temp_val > SMALL) { /* can't be having negative/very small numbers */
      if (aspect_ratio) {
	scale = temp_val/volume_new_info->voxel_size.y;
	volume_new_info->voxel_size.x = scale*volume_new_info->voxel_size.x;
	volume_new_info->voxel_size.z = scale*volume_new_info->voxel_size.z;
	update_size_x = update_size_z = TRUE;
      }
      volume_new_info->voxel_size.y = temp_val;
    }
    break;
  case VOXEL_SIZE_Z:
    if (temp_val > SMALL) { /* can't be having negative/very small numbers */
      if (aspect_ratio) {
	scale = temp_val/volume_new_info->voxel_size.z;
	volume_new_info->voxel_size.y = scale*volume_new_info->voxel_size.y;
	volume_new_info->voxel_size.x = scale*volume_new_info->voxel_size.x;
	update_size_x = update_size_y = TRUE;
      }
      volume_new_info->voxel_size.z = temp_val;
    }
    break;
  case CENTER_X:
    new_center.x = temp_val;
    break;
  case CENTER_Y:
    new_center.y = temp_val;
    break;
  case CENTER_Z:
    new_center.z = temp_val;
    break;
  case SCALING_FACTOR:
    if (fabs(temp_val) > CLOSE) /* avoid zero... */
      volume_set_scaling(volume_new_info, temp_val);
    break;
  case SCAN_START:
    volume_new_info->scan_start = temp_val;
    break;
  case FRAME_DURATION:
    if (temp_val > CLOSE) {/* avoid zero and negatives */
      i = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "frame"));
      volume_new_info->frame_duration[i] = temp_val;
    }
    break;
  default:
    break; /* do nothing */
  }
  
  /* recalculate the volume's offset based on the new center */
  shift = rp_sub(new_center, old_center);

  /* and save any changes to the coord frame */
  rs_set_offset(&volume_new_info->coord_frame, rp_add(rs_offset(volume_new_info->coord_frame), shift));

  /* update the entry widgets as necessary */
  if (update_size_x) {
    entry =  gtk_object_get_data(GTK_OBJECT(volume_dialog), "voxel_size_x");
    gtk_signal_handler_block_by_func(GTK_OBJECT(entry),
    				     GTK_SIGNAL_FUNC(ui_volume_dialog_cb_change_entry), 
    				     volume_new_info);
    str = g_strdup_printf("%f", volume_new_info->voxel_size.x);
    gtk_entry_set_text(GTK_ENTRY(entry), str);
    g_free(str);
    gtk_signal_handler_unblock_by_func(GTK_OBJECT(entry), 
    				       GTK_SIGNAL_FUNC(ui_volume_dialog_cb_change_entry), 
    				       volume_new_info);
  }
  if (update_size_y) {
    entry =  gtk_object_get_data(GTK_OBJECT(volume_dialog), "voxel_size_y");
    gtk_signal_handler_block_by_func(GTK_OBJECT(entry),
    				     GTK_SIGNAL_FUNC(ui_volume_dialog_cb_change_entry), 
    				     volume_new_info);
    str = g_strdup_printf("%f", volume_new_info->voxel_size.y);
    gtk_entry_set_text(GTK_ENTRY(entry), str);
    g_free(str);
    gtk_signal_handler_unblock_by_func(GTK_OBJECT(entry), 
    				       GTK_SIGNAL_FUNC(ui_volume_dialog_cb_change_entry), 
    				       volume_new_info);
  }
  if (update_size_z) {
    entry =  gtk_object_get_data(GTK_OBJECT(volume_dialog), "voxel_size_z");
    gtk_signal_handler_block_by_func(GTK_OBJECT(entry),
    				     GTK_SIGNAL_FUNC(ui_volume_dialog_cb_change_entry), 
    				     volume_new_info);
    str = g_strdup_printf("%f", volume_new_info->voxel_size.z);
    gtk_entry_set_text(GTK_ENTRY(entry), str);
    g_free(str);
    gtk_signal_handler_unblock_by_func(GTK_OBJECT(entry), 
    				       GTK_SIGNAL_FUNC(ui_volume_dialog_cb_change_entry), 
    				       volume_new_info);
  }



  /* now tell the volume_dialog that we've changed */
  gnome_property_box_changed(GNOME_PROPERTY_BOX(volume_dialog));

  return;
}


/* function called when the aspect ratio button gets clicked */
void ui_volume_dialog_cb_aspect_ratio(GtkWidget * widget, gpointer data) {

  GtkWidget * volume_dialog;
  gboolean state;

  /* get the state of the button */
  state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

  /* record the change */
  volume_dialog =  gtk_object_get_data(GTK_OBJECT(widget), "volume_dialog");
  gtk_object_set_data(GTK_OBJECT(volume_dialog), "aspect_ratio", GINT_TO_POINTER(state));

  return;
}

/* function called when the modality type of the volume gets changed */
void ui_volume_dialog_cb_change_modality(GtkWidget * widget, gpointer data) {

  volume_t * volume_new_info = data;
  modality_t i_modality;
  GtkWidget * volume_dialog;

  /* figure out which menu item called me */
  for (i_modality=0;i_modality<NUM_MODALITIES;i_modality++)       
    if (GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget),"modality")) == i_modality)
      volume_new_info->modality = i_modality;  /* save the new modality until it's applied */

  /* now tell the volume_dialog that we've changed */
  volume_dialog =  gtk_object_get_data(GTK_OBJECT(widget), "volume_dialog");
  gnome_property_box_changed(GNOME_PROPERTY_BOX(volume_dialog));

  return;
}



/* function called when rotating the volume around an axis */
void ui_volume_dialog_cb_change_axis(GtkAdjustment * adjustment, gpointer data) {

  ui_study_t * ui_study;
  volume_t * volume_new_info = data;
  view_t i_view;
  floatpoint_t rotation;
  GtkWidget * volume_dialog;
  realpoint_t center, temp;

  /* we need the current view_axis so that we know what we're rotating around */
  ui_study = gtk_object_get_data(GTK_OBJECT(adjustment), "ui_study"); 

  /* saving the center, as we're rotating the volume around it's own center */
  center = volume_calculate_center(volume_new_info); 

  /* figure out which scale widget called me */
  i_view = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(adjustment),"view"));

  rotation = (adjustment->value/180)*M_PI; /* get rotation in radians */

  /* compensate for sagittal being a left-handed coordinate frame */
  if (i_view == SAGITTAL)
    rotation = -rotation; 

  /* rotate the axis */
  realspace_rotate_on_axis(&volume_new_info->coord_frame,
			   realspace_get_view_normal(study_coord_frame_axis(ui_study->study), i_view),
			   rotation);
  
  /* recalculate the offset of this volume based on the center we stored */
  REALPOINT_CMULT(-0.5,volume_new_info->corner,temp);
  rs_set_offset(&volume_new_info->coord_frame, center);
  rs_set_offset(&volume_new_info->coord_frame, 
		realspace_alt_coord_to_base(temp, volume_new_info->coord_frame));

  /* return adjustment back to normal */
  adjustment->value = 0.0;
  gtk_adjustment_changed(adjustment);


  /* now tell the volume_dialog that we've changed */
  volume_dialog =  gtk_object_get_data(GTK_OBJECT(adjustment), "volume_dialog");
  ui_volume_dialog_set_axis_display(volume_dialog);
  gnome_property_box_changed(GNOME_PROPERTY_BOX(volume_dialog));

  return;
}

/* function to reset the volume's axis back to the default coords */
void ui_volume_dialog_cb_reset_axis(GtkWidget* widget, gpointer data) {

  volume_t * volume_new_info = data;
  GtkWidget * volume_dialog;
  realpoint_t center, temp;

  /* saving the center, as we're rotating the volume around it's own center */
  center = volume_calculate_center(volume_new_info); 

  /* reset the axis */
  rs_set_axis(&volume_new_info->coord_frame, default_axis);

  /* recalculate the offset of this volume based on the center we stored */
  REALPOINT_CMULT(-0.5,volume_new_info->corner,temp);
  rs_set_offset(&volume_new_info->coord_frame, center);
  rs_set_offset(&volume_new_info->coord_frame, 
		realspace_alt_coord_to_base(temp, volume_new_info->coord_frame));

  /* now tell the volume_dialog that we've changed */
  volume_dialog =  gtk_object_get_data(GTK_OBJECT(widget), "volume_dialog");
  ui_volume_dialog_set_axis_display(volume_dialog);
  gnome_property_box_changed(GNOME_PROPERTY_BOX(volume_dialog));
  
  return;
}

/* function called when we hit the apply button */
void ui_volume_dialog_cb_apply(GtkWidget* widget, gint page_number, gpointer data) {
  
  ui_volume_list_t * ui_volume_list_item = data;
  ui_study_t * ui_study;
  volume_t * volume_new_info;
  amide_data_t scale;
  guint i;
  GtkWidget * label;
  GtkWidget * pixmap;
  GdkPixmap * icon;
  
  /* we'll apply all page changes at once */
  if (page_number != -1)
    return;

  /* get a pointer to ui_study so we can redraw the volume's */
  ui_study = gtk_object_get_data(GTK_OBJECT(ui_volume_list_item->dialog), "ui_study"); 

  /* get the new info for the volume */
  volume_new_info = gtk_object_get_data(GTK_OBJECT(ui_volume_list_item->dialog),"volume_new_info");

  /* sanity check */
  if (volume_new_info == NULL) {
    g_warning("%s, volume_new_info inappropriately null....", PACKAGE);
    return;
  }

  /* copy the needed new info on over */
  ui_volume_list_item->volume->modality = volume_new_info->modality;
  ui_volume_list_item->volume->coord_frame = volume_new_info->coord_frame;
  ui_volume_list_item->volume->voxel_size = volume_new_info->voxel_size;
  volume_set_name(ui_volume_list_item->volume, volume_new_info->name);

  /* apply any changes in the scaling factor */
  if (!FLOATPOINT_EQUAL(ui_volume_list_item->volume->external_scaling, volume_new_info->external_scaling)) {
    scale = (volume_new_info->external_scaling)/(ui_volume_list_item->volume->external_scaling);
    volume_set_scaling(ui_volume_list_item->volume, volume_new_info->external_scaling);
    ui_volume_list_item->volume->max = scale *  ui_volume_list_item->volume->max;
    ui_volume_list_item->volume->min = scale *  ui_volume_list_item->volume->min;
    ui_volume_list_item->volume->threshold_max = scale *  ui_volume_list_item->volume->threshold_max;
    ui_volume_list_item->volume->threshold_min = scale *  ui_volume_list_item->volume->threshold_min;
    amitk_threshold_update(AMITK_THRESHOLD(ui_volume_list_item->threshold));
    /* note, the threshold bar graph does not need to be redrawn as it's values
       are all relative anyway */
  }

  /* reset the far corner */
  volume_recalc_far_corner(ui_volume_list_item->volume);

  /* apply any time changes, and recalculate the frame selection
     widget in case any timing information in this volume has changed */
  ui_volume_list_item->volume->scan_start = volume_new_info->scan_start;
  for (i=0;i<ui_volume_list_item->volume->data_set->dim.t;i++)
    ui_volume_list_item->volume->frame_duration[i] = volume_new_info->frame_duration[i];
  ui_time_dialog_set_times(ui_study);


  /* apply any changes to the name of the volume */
  label = gtk_object_get_data(GTK_OBJECT(ui_volume_list_item->tree_leaf), "text_label");
  gtk_label_set_text(GTK_LABEL(label), ui_volume_list_item->volume->name);


  /* change the modality icon */
  pixmap = gtk_object_get_data(GTK_OBJECT(ui_volume_list_item->tree_leaf), "pixmap");
  switch (volume_new_info->modality) {
  case SPECT:
    icon = gdk_pixmap_create_from_xpm_d(gtk_widget_get_parent_window(ui_study->tree),
					NULL,NULL,SPECT_xpm);
    break;
  case MRI:
    icon = gdk_pixmap_create_from_xpm_d(gtk_widget_get_parent_window(ui_study->tree),
					NULL,NULL,MRI_xpm);
    break;
  case CT:
    icon = gdk_pixmap_create_from_xpm_d(gtk_widget_get_parent_window(ui_study->tree),
					NULL,NULL,CT_xpm);
    break;
  case OTHER:
    icon = gdk_pixmap_create_from_xpm_d(gtk_widget_get_parent_window(ui_study->tree),
					NULL,NULL,OTHER_xpm);
    break;
  case PET:
  default:
    icon = gdk_pixmap_create_from_xpm_d(gtk_widget_get_parent_window(ui_study->tree),
					NULL,NULL,PET_xpm);
    break;
  }
  gtk_pixmap_set(GTK_PIXMAP(pixmap), icon, NULL);


  /* redraw the volume */
  if (ui_volume_list_includes_volume(ui_study->current_volumes, ui_volume_list_item->volume))
      ui_study_update_canvas(ui_study, NUM_VIEWS, UPDATE_ALL);

  return;
}

/* callback for the help button */
void ui_volume_dialog_cb_help(GnomePropertyBox *volume_dialog, gint page_number, gpointer data) {

  GnomeHelpMenuEntry help_ref={PACKAGE,"basics.html#VOLUME-DIALOG-HELP"};
  GnomeHelpMenuEntry help_ref_0 = {PACKAGE,"basics.html#VOLUME-DIALOG-HELP-BASIC"};
  GnomeHelpMenuEntry help_ref_1 = {PACKAGE,"basics.html#VOLUME-DIALOG-HELP-CENTER-DIMENSIONS"};
  GnomeHelpMenuEntry help_ref_2 = {PACKAGE,"basics.html#VOLUME-DIALOG-HELP-ROTATE"};
  GnomeHelpMenuEntry help_ref_3 = {PACKAGE,"basics.html#VOLUME-DIALOG-HELP-COLORMAP_THRESHOLD"};
  GnomeHelpMenuEntry help_ref_4 = {PACKAGE,"basics.html#VOLUME-DIALOG-HELP-TIME"};
  GnomeHelpMenuEntry help_ref_5 = {PACKAGE,"basics.html#VOLUME-DIALOG-HELP-IMMUTABLES"};


  switch (page_number) {
  case 0:
    gnome_help_display (0, &help_ref_0);
    break;
  case 1:
    gnome_help_display (0, &help_ref_1);
    break;
  case 2:
    gnome_help_display (0, &help_ref_2);
    break;
  case 3:
    gnome_help_display (0, &help_ref_3);
    break;
  case 4:
    gnome_help_display (0, &help_ref_4);
    break;
  case 5:
    gnome_help_display (0, &help_ref_5);
    break;
  default:
    gnome_help_display (0, &help_ref);
    break;
  }

  return;
}

/* function called to destroy the volume dialog */
gboolean ui_volume_dialog_cb_close(GtkWidget* widget, gpointer data) {

  ui_volume_list_t * ui_volume_list_item = data;
  volume_t * volume_new_info;

  /* trash collection */
  volume_new_info = gtk_object_get_data(GTK_OBJECT(widget), "volume_new_info");
#if AMIDE_DEBUG
  { /* little something to make the debugging messages make sense */
    gchar * temp_string;
    temp_string = g_strdup_printf("Copy of %s",volume_new_info->name);
    volume_set_name(volume_new_info, temp_string);
    g_free(temp_string);

  }
#endif
  volume_new_info = volume_free(volume_new_info);

  /* make sure the pointer in the ui_volume_list_item is nulled */
  ui_volume_list_item->dialog = NULL;

  return FALSE;
}

