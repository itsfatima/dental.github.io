/* idl_data_import.c
 *
 * Part of amide - Amide's a Medical Image Dataset Examiner
 * Copyright (C) 2000-2002 Andy Loening
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


#include "amide_config.h"
#include <gnome.h>
#include <sys/stat.h>
#include "raw_data_import.h"
#include "idl_data_import.h"



/* function to load in volume data from an IDL data file */
AmitkDataSet * idl_data_import(const gchar * idl_data_filename) {

  AmitkDataSet * ds;
  FILE * file_pointer;
  void * file_buffer = NULL;
  size_t bytes_read;
  size_t bytes_to_read;
  guint8 * bytes;
  guint32 * uints;
  guint dimensions;
  AmitkVoxel dim;
  gchar * name;
  gchar ** frags;
  guint file_offset = 35; /* the idl header is 35 bytes */
  AmitkAxes new_axes;

  /* we need to read in the first 35 bytes of the IDL data file (the header) */
  if ((file_pointer = fopen(idl_data_filename, "r")) == NULL) {
    g_warning("couldn't open idl data file %s",idl_data_filename);
    return NULL;
  }
    
  /* read in the header of the file */
  bytes_to_read = file_offset; /* this is the size of the header */
  file_buffer = (void *) g_malloc(bytes_to_read);
  bytes_read = fread(file_buffer, 1, bytes_to_read, file_pointer);
  if (bytes_read != bytes_to_read) {
    g_warning("read wrong number of elements from idl data file:\n\t%s\n\texpected %d\tgot %d", 
	      idl_data_filename, bytes_to_read, bytes_read);
    g_free(file_buffer);
    fclose(file_pointer);
    return NULL;
  }
  fclose(file_pointer);

  /* interperate the header, note, it's in big endian format...

     bytes         content
     -------------------------------------
     0             number of dimensions
     1-4           x dimension
     5-8           y dimension
     9-12          z dimension
     13-16         number of frames?
     17-35         ?

  */
  bytes = file_buffer;

  dimensions = bytes[0];
  
  uints = file_buffer+1;

  dim.x = GUINT32_FROM_BE(uints[0]); /* bytes 1-4 */
  dim.y = GUINT32_FROM_BE(uints[1]); /* bytes 5-8 */
  dim.z = GUINT32_FROM_BE(uints[2]); /* bytes 9-12 */
  dim.t = GUINT32_FROM_BE(uints[3]); /* bytes 13-16 */

#ifdef AMIDE_DEBUG
  g_print("IDL File %s\n",idl_data_filename);
  g_print("\tdimensions %d\n",dimensions);
  g_print("\tdim\tx %d\ty %d\tz %d\tframes %d\n",dim.x,dim.y,dim.z, dim.t);
#endif

  /* now that we've figured out the header info, read in the rest of the idl data file as raw data*/
  ds = amitk_data_set_import_raw_file(idl_data_filename, IDL_RAW_DATA_FORMAT,dim, file_offset);
  if (ds == NULL) {
    g_warning("couldn't read in idl data set");
    return NULL;
  }

  /* figure out an initial name for the data */
  name = g_strdup(g_basename(idl_data_filename));
  /* remove the extension of the file */
  g_strreverse(name);
  frags = g_strsplit(name, ".", 2);
  g_free(name);
  g_strreverse(frags[1]);
  amitk_object_set_name(AMITK_OBJECT(ds), frags[1]);
  g_strfreev(frags); /* free up now unused strings */

  /* set the axis such that transverse/coronal/sagittal match up correctly */
  /* this is all derived empirically */
  new_axes[0].x = -1.0;
  new_axes[0].y = 0.0;
  new_axes[0].z = 0.0;
  new_axes[1].x = 0.0;
  new_axes[1].y = 0.0;
  new_axes[1].z = 1.0;
  new_axes[2].x = 0.0;
  new_axes[2].y = 1.0;
  new_axes[2].z = 0.0;
  amitk_space_set_axes(AMITK_SPACE(ds), new_axes, zero_point); 

  /* reset the center, needed as we've reset the axis */
  amitk_volume_set_center(AMITK_VOLUME(ds), zero_point);

  /* garbage collection */
  g_free(file_buffer);

  return ds; 

}









