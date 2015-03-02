/* rendering.h
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

#ifdef AMIDE_LIBVOLPACK_SUPPORT

#ifndef __RENDERING_H__
#define __RENDERING_H__

/* header files that are always needed with this file */
#include <volpack.h>
#include "volume.h"
#include "roi.h"

/* -------------- structures and such ------------- */

typedef gshort rendering_normal_t;
typedef guchar rendering_density_t;
typedef guchar rendering_gradient_t;
typedef enum {DENSITY_CLASSIFICATION, GRADIENT_CLASSIFICATION, NUM_CLASSIFICATIONS} classification_t;
typedef enum {HIGHEST, HIGH, FAST, FASTEST, NUM_QUALITIES} rendering_quality_t;
typedef enum {OPACITY, GRAYSCALE, NUM_PIXEL_TYPES} pixel_type_t;
typedef enum {CURVE_LINEAR, CURVE_SPLINE, NUM_CURVE_TYPES} curve_type_t;

typedef struct {        /*   contents of a voxel */
  rendering_normal_t normal;        /*   encoded surface normal vector */
  rendering_density_t density;	/*   original density */
  rendering_gradient_t gradient;	/*   original gradient */
} rendering_voxel_t;


/* dummy variable used in some macros below */
rendering_voxel_t * dummy_voxel;  


/* ----------- defines ------------- */

#define RENDERING_BYTES_PER_VOXEL	sizeof(rendering_voxel_t)/* voxel size in bytes */
#define RENDERING_VOXEL_FIELDS	        3	/* number of fields in voxel */
#define RENDERING_SHADE_FIELDS	        2	/* number of fields used for shading
						   (normal and density); must be the
						   1st fields of RawVoxel */
#define RENDERING_CLSFY_FIELDS	        2	/* number of fields used for classifying
						   (density and gradient); can be any fields
						   in the RawVoxel */


#define RENDERING_NORMAL_FIELD  	0
#define RENDERING_NORMAL_OFFSET 	vpFieldOffset(dummy_voxel, normal)
#define RENDERING_NORMAL_SIZE   	sizeof(rendering_normal_t)
#define RENDERING_NORMAL_MAX    	VP_NORM_MAX /*7923 last time I checked */


#define RENDERING_DENSITY_FIELD 	1
#define RENDERING_DENSITY_OFFSET	vpFieldOffset(dummy_voxel, density)
#define RENDERING_DENSITY_SIZE  	sizeof(rendering_density_t)
#define RENDERING_DENSITY_MAX   	VP_SCALAR_MAX /*255 last time I checked */

#define RENDERING_GRADIENT_FIELD	2
#define RENDERING_GRADIENT_OFFSET	vpFieldOffset(dummy_voxel, gradient)
#define RENDERING_GRADIENT_SIZE 	sizeof(rendering_gradient_t)
#define RENDERING_GRADIENT_MAX  	VP_GRAD_MAX /* 221 last time I checked */

#define RENDERING_DENSITY_PARAM		0      /* classification parameter */
#define RENDERING_GRADIENT_PARAM	1      /* classification parameter */

/* initial density and gradient ramps */
//#define RENDERING_DENSITY_RAMP_X {0.0, 20, RENDERING_DENSITY_MAX}
//#define RENDERING_DENSITY_RAMP_Y {0.0, 1.0, 1.0}
//#define RENDERING_DENSITY_RAMP_POINTS 3
#define RENDERING_DENSITY_RAMP_X {0.0, RENDERING_DENSITY_MAX}
#define RENDERING_DENSITY_RAMP_Y {0.0, 1.0}
#define RENDERING_DENSITY_RAMP_POINTS 2

#define RENDERING_GRADIENT_RAMP_X {0.0, RENDERING_GRADIENT_MAX}
#define RENDERING_GRADIENT_RAMP_Y {0.0, 1.0}
#define RENDERING_GRADIENT_RAMP_POINTS 2

#define RENDERING_OCTREE_DENSITY_THRESH 	4
#define RENDERING_OCTREE_GRADIENT_THRESH	4
#define RENDERING_OCTREE_BASE_NODE_SIZE 	4

#define RENDERING_DEFAULT_ZOOM 1.0
#define RENDERING_DEFAULT_QUALITY HIGHEST
#define RENDERING_DEFAULT_PIXEL_TYPE GRAYSCALE
#define RENDERING_DEFAULT_DEPTH_CUEING FALSE
#define RENDERING_DEFAULT_FRONT_FACTOR 1.0
#define RENDERING_DEFAULT_DENSITY 1.0

/* ------------ some more structures ------------ */

typedef struct _rendering_t {
  vpContext * vpc;      /*  VolPack rendering Context */
  object_t type;
  volume_t * volume;
  roi_t * roi;
  gchar * name;
  color_table_t color_table;
  amide_time_t start;
  amide_time_t duration;
  realspace_t current_coord_frame;
  realspace_t initial_coord_frame;
  rendering_voxel_t * rendering_vol;
  realpoint_t voxel_size;
  voxelpoint_t dim; /* dimensions of our rendering_vol and image */
  guchar * image;
  pixel_type_t pixel_type;
  gfloat shade_table[RENDERING_NORMAL_MAX+1];	/* shading lookup table */
  gfloat density_ramp[RENDERING_DENSITY_MAX+1]; /* opacity as a function */
  gfloat gradient_ramp[RENDERING_GRADIENT_MAX+1]; /* opacity as a function */
  gint * ramp_x[NUM_CLASSIFICATIONS];
  gfloat * ramp_y[NUM_CLASSIFICATIONS];
  guint num_points[NUM_CLASSIFICATIONS];
  curve_type_t curve_type[NUM_CLASSIFICATIONS];
  guint reference_count;
} rendering_t;


/* a list of rendering contexts */
typedef struct _rendering_list_t rendering_list_t;
struct _rendering_list_t {
  rendering_t * rendering_context;
  guint reference_count;
  rendering_list_t * next;
};




/* external functions */
rendering_t * rendering_context_free(rendering_t * context);
rendering_t * rendering_context_volume_init(volume_t * volume, 
					    const realspace_t render_coord_frame, 
					    const realpoint_t render_far_corner, 
					    const floatpoint_t min_voxel_size, 
					    const amide_time_t start, 
					    const amide_time_t duration, 
					    const interpolation_t interpolation);
void rendering_context_reload_object(rendering_t * rendering_context, 
				     const amide_time_t new_start,
				     const amide_time_t new_duration, 
				     const interpolation_t interpolation);
void rendering_context_load_object(rendering_t * rendering_context, 
				   const interpolation_t interpolation);
void rendering_context_set_rotation(rendering_t * context, axis_t dir, gdouble rotation);
void rendering_context_reset_rotation(rendering_t * context);
void rendering_context_set_quality(rendering_t * context, rendering_quality_t quality);
void rendering_context_set_image(rendering_t * context, pixel_type_t pixel_type, gdouble zoom);
void rendering_context_set_depth_cueing(rendering_t * context, gboolean state);
void rendering_context_set_depth_cueing_parameters(rendering_t * context, 
						   gdouble front_factor, gdouble density);
void rendering_context_render(rendering_t * context);
rendering_list_t * rendering_list_free(rendering_list_t * rendering_list);
rendering_list_t * rendering_list_init(volume_list_t * volumes, roi_list_t * rois,
				       realspace_t render_coord_frame, const amide_time_t start, 
				       const amide_time_t duration, const interpolation_t interpolation);
void rendering_list_reload_objects(rendering_list_t * rendering_list, const amide_time_t start, 
				   const amide_time_t duration, const interpolation_t interpolation);
void rendering_list_set_rotation(rendering_list_t * contexts, axis_t dir, gdouble rotation);
void rendering_list_reset_rotation(rendering_list_t * contexts);
void rendering_list_set_quality(rendering_list_t * renderling_list, rendering_quality_t quality);
void rendering_list_set_image(rendering_list_t * rendering_list, pixel_type_t pixel_type, gdouble zoom);
void rendering_list_set_depth_cueing(rendering_list_t * rendering_list, gboolean state);
void rendering_list_set_depth_cueing_parameters(rendering_list_t * rendering_list, 
						gdouble front_factor, gdouble density);
void rendering_list_render(rendering_list_t * rendering_list);
guint rendering_list_count(rendering_list_t * rendering_list);

/* external variables */
extern gchar * rendering_quality_names[];
extern gchar * pixel_type_names[];





/* external funnctions from render.c */
//gboolean render_set_material_shinyness(gint material_num, gdouble shinyness);

#endif /*  __RENDERING_H__ */
#endif /* AMIDE_LIBVOLPACK_SUPPORT */
