/* ui_roi_analysis_dialog.h
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

#ifndef __UI_ROI_ANALYSIS_DIALOG_H__
#define __UI_ROI_ANALYSIS_DIALOG_H__


/* header files always needed with this one */
#include "analysis.h"
#include "ui_study.h"

/* external functions */
void ui_roi_analysis_dialog_export(gchar * save_filename, analysis_roi_t * roi_analyses);
void ui_roi_analysis_dialog_create(ui_study_t * ui_study, gboolean all);


#endif /* __UI_ROI_ANALYSIS_DIALOG_H__ */




