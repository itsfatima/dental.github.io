/* realspace.h
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

#ifndef __REALSPACE_H__
#define __REALSPACE_H__

/* header files that are always associated with this header file */
#include "amide.h"

typedef enum {XAXIS, YAXIS, ZAXIS, NUM_AXIS} axis_t;

/* canvas point is a point in canvas (real) 2D space */
typedef struct canvaspoint_t {
  floatpoint_t x;
  floatpoint_t y;
} canvaspoint_t;

/* pixel point is a point in pixel (integer) 2D space */
typedef struct pixelpoint_t {
  intpoint_t x;
  intpoint_t y;
} pixelpoint_t;

/* voxel point is a point in voxel (integer) 4D space */
typedef struct voxelpoint_t {
  intpoint_t x;
  intpoint_t y;
  intpoint_t z;
  intpoint_t t;
} voxelpoint_t;


/* realpoint is a point in real (float) 3D space */
typedef struct realpoint_t {
  floatpoint_t x;
  floatpoint_t y;
  floatpoint_t z;
} realpoint_t;

/* a realspace is the description of an alternative coordinate frame
   wrt to the base coordinate space.  Offset is in the base coordinate
   frame.  All of these items should be accessed using the proper functions */
typedef struct realspace_t {
  realpoint_t offset;
  realpoint_t axis[NUM_AXIS];
  guint ref_count;
} realspace_t;

/* constants */
#define CLOSE 0.00001 /* what's close enough to be equal.... */
#define SMALL 0.01 /* in milimeter's, used as a lower limit on some dimensions */


/* convenience functions */

/* returns the offset of a realspace */
#define rs_offset(rs) ((rs)->offset)
#define rs_specific_axis(rs, which) ((rs)->axis[(which)])
#define rs_all_axis(rs) ((rs)->axis)

/* returns the boolean value of fp1==fp2 (within a factor of CLOSE) */
#define FLOATPOINT_EQUAL(fp1,fp2) (((fp1) > ((1.00-CLOSE)*(fp2)-CLOSE)) && \
				   ((fp1) < ((1.00+CLOSE)*(fp2)+CLOSE)))

#define EQUAL_ZERO(fp1) (FLOATPOINT_EQUAL((fp1), 0.0))

/* returns the boolean value of rp1==rp2 (within a factor of CLOSE) */
#define REALPOINT_EQUAL(rp1,rp2) (FLOATPOINT_EQUAL(((rp1).x),((rp2).x)) && \
				  FLOATPOINT_EQUAL(((rp1).y),((rp2).y)) && \
				  FLOATPOINT_EQUAL(((rp1).z),((rp2).z)))

#define VOXELPOINT_EQUAL(vp1,vp2) (((vp1).x == (vp2).x) && \
				   ((vp1).y == (vp2).y) && \
				   ((vp1).z == (vp2).z) && \
				   ((vp1).t == (vp2).t))

/* figure out the real point that corresponds to the voxel coordinates */
#define VOXEL_TO_REALPOINT(vox, vox_size, real) (((real).x = (((floatpoint_t) (vox).x)+0.5) * (vox_size).x), \
					         ((real).y = (((floatpoint_t) (vox).y)+0.5) * (vox_size).y), \
					         ((real).z = (((floatpoint_t) (vox).z)+0.5) * (vox_size).z))

#define REALPOINT_TO_VOXEL(real, vox_size, frame, vox) (((vox).x = floor((real).x/(vox_size).x)), \
						        ((vox).y = floor((real).y/(vox_size).y)), \
						        ((vox).z = floor((real).z/(vox_size).z)), \
							((vox).t = (frame)))

/* returned the maximum of rp1 */
#define REALPOINT_MAX(rp1) (MAX( MAX((rp1).x, (rp1).y), (rp1).z))

/* returns rp1 dot rp2" */
#define REALPOINT_DOT_PRODUCT(rp1,rp2) ((rp1).x*(rp2).x+(rp1).y*(rp2).y+(rp1).z*(rp2).z)

/* returns sqrt(rp1 dot rp1) */
#define REALPOINT_MAGNITUDE(rp) (sqrt(REALPOINT_DOT_PRODUCT((rp), (rp))))

/* returns rp2 = abs(rp1)" */
#define REALPOINT_ABS(rp1,rp2) ((rp2).x = fabs((rp1).x), \
				(rp2).y = fabs((rp1).y), \
				(rp2).z = fabs((rp1).z)) 

/* does rp3=rp1+rp2 for realpoint structures */
#define REALPOINT_ADD(rp1,rp2,rp3) (((rp3).x = (rp1).x+(rp2).x), \
				    ((rp3).y = (rp1).y+(rp2).y), \
		                    ((rp3).z = (rp1).z+(rp2).z))
/* does rp3=rp1-rp2 for realpoint structures */
#define REALPOINT_SUB(rp1,rp2,rp3) (((rp3).x = (rp1).x-(rp2).x), \
				    ((rp3).y = (rp1).y-(rp2).y), \
		                    ((rp3).z = (rp1).z-(rp2).z))

/* does rp3=rp1.*rp2 for realpoint structures */
#define REALPOINT_MULT(rp1,rp2,rp3) (((rp3).x = (rp1).x*(rp2).x), \
				     ((rp3).y = (rp1).y*(rp2).y), \
		                     ((rp3).z = (rp1).z*(rp2).z))

/* does rp3=rp1./rp2 for realpoint structures */
#define REALPOINT_DIV(rp1,rp2,rp3) (((rp3).x = (rp1).x/(rp2).x), \
				     ((rp3).y = (rp1).y/(rp2).y), \
		                     ((rp3).z = (rp1).z/(rp2).z))


/* does rp3=fabs(rp1-rp2) for realpoint structures */
#define REALPOINT_DIFF(rp1,rp2,rp3) (((rp3).x = fabs((rp1).x-(rp2).x)), \
				     ((rp3).y = fabs((rp1).y-(rp2).y)), \
		                     ((rp3).z = fabs((rp1).z-(rp2).z)))

/* does rp3=cm*rp1 for realpoint structures */
#define REALPOINT_CMULT(cm,rp1,rp3) (((rp3).x = (cm)*(rp1).x), \
				    ((rp3).y = (cm)*(rp1).y), \
				    ((rp3).z = (cm)*(rp1).z))

/* does rp3=cm*rp1+dm*rp2 for realpoint structures */
#define REALPOINT_MADD(cm,rp1,dm,rp2,rp3) (((rp3).x = cm*(rp1).x+dm*(rp2).x), \
					   ((rp3).y = cm*(rp1).y+dm*(rp2).y), \
					   ((rp3).z = cm*(rp1).z+dm*(rp2).z)) 

#define realspace_alt_coord_to_alt(in, in_frame, out_frame) (realspace_base_coord_to_alt(realspace_alt_coord_to_base((in),(in_frame)),(out_frame)))

/* external functions */

/* note! the equivalent defines above are faster and should be used in any time critical spots */
inline realpoint_t rp_abs(const realpoint_t rp1);
inline realpoint_t rp_neg(const realpoint_t rp1);
inline realpoint_t rp_add(const realpoint_t rp1, const realpoint_t rp2);
inline realpoint_t rp_sub(const realpoint_t rp1, const realpoint_t rp2);
inline realpoint_t rp_mult(const realpoint_t rp1, const realpoint_t rp2);
inline realpoint_t rp_div(const realpoint_t rp1, const realpoint_t rp2);
inline realpoint_t rp_diff(const realpoint_t rp1, const realpoint_t rp2);
inline realpoint_t rp_cmult(const floatpoint_t cmult, const realpoint_t rp1);
inline realpoint_t rp_cross_product(const realpoint_t rp1, const realpoint_t rp2);
inline floatpoint_t rp_dot_product(const realpoint_t rp1, const realpoint_t rp2);
inline floatpoint_t rp_mag(const realpoint_t rp1);
inline floatpoint_t rp_min_dim(const realpoint_t rp1);
inline floatpoint_t rp_max_dim(const realpoint_t rp1);

inline canvaspoint_t cp_diff(const canvaspoint_t cp1,const canvaspoint_t cp2);
inline canvaspoint_t cp_sub(const canvaspoint_t cp1,const canvaspoint_t cp2);
inline canvaspoint_t cp_add(const canvaspoint_t cp1,const canvaspoint_t cp2);
inline floatpoint_t cp_dot_product(const canvaspoint_t cp1, const canvaspoint_t cp2);
inline floatpoint_t cp_mag(const canvaspoint_t cp1);

inline voxelpoint_t vp_add(const voxelpoint_t vp1,const voxelpoint_t vp2);
inline voxelpoint_t vp_sub(const voxelpoint_t vp1,const voxelpoint_t vp2);
inline gboolean vp_equal(const voxelpoint_t vp1, const voxelpoint_t vp2);
inline floatpoint_t vp_max_dim(const voxelpoint_t vp1);

gboolean rp_in_box(const realpoint_t p,
		   const realpoint_t p0,
		   const realpoint_t p1);
gboolean rp_in_elliptic_cylinder(const realpoint_t p,
				 const realpoint_t center,
				 const floatpoint_t height,
				 const realpoint_t radius);
gboolean rp_in_ellipsoid(const realpoint_t p,
			 const realpoint_t center,
			 const realpoint_t radius);
void rp_print(gchar * message, const realpoint_t rp);
void rs_print(gchar * message, const realspace_t * coord_frame);
realspace_t * rs_unref(realspace_t * rs);
realspace_t * rs_init(void);
realspace_t * rs_ref(realspace_t * src_rs);
realspace_t * rs_copy(const realspace_t * src_rs);
void rs_set_offset(realspace_t * rs, const realpoint_t new_offset);
void rs_set_axis(realspace_t * rs, const realpoint_t new_axis[]);
void rs_set_coord_frame(realspace_t * dest_rs, const realspace_t * src_rs);
void rs_invert_axis(realspace_t * rs, const axis_t which_axis);
void realspace_get_enclosing_corners(const realspace_t * in_coord_frame, const realpoint_t in_corner[], 
				     const realspace_t * out_coord_frame, realpoint_t out_corner[] );
void realspace_rotate_on_axis(realspace_t * rs, 
			      const realpoint_t axis, 
			      const floatpoint_t theta);
realpoint_t rs_get_orthogonal_view_axis(const realspace_t * rs,
					const view_t view,
					const layout_t layout,
					const axis_t ax);
realpoint_t rs_get_view_normal(const realspace_t * rs, const view_t view);
realspace_t * realspace_get_view_coord_frame(const realspace_t * in_coord_frame,
					     const view_t view,
					     const layout_t layout);
inline realpoint_t realspace_alt_coord_to_base(const realpoint_t in,
					       const realspace_t * in_alt_coord_frame);
inline realpoint_t realspace_base_coord_to_alt(realpoint_t in,
					       const realspace_t * out_alt_coord_frame);
inline realpoint_t realspace_alt_dim_to_base(const realpoint_t in,
					     const realspace_t * in_alt_coord_frame);
inline realpoint_t realspace_base_dim_to_alt(const realpoint_t in,
					     const realspace_t * out_alt_coord_frame);
inline realpoint_t realspace_alt_dim_to_alt(const realpoint_t in,
					    const realspace_t * in_alt_coord_frame,
					    const realspace_t * out_alt_coord_frame);

/* external variables */
extern const gchar * axis_names[];
extern const realpoint_t default_axis[NUM_AXIS];
extern const realspace_t default_coord_frame;
extern const realpoint_t zero_rp;
extern const realpoint_t one_rp;
extern const realpoint_t ten_rp;
extern const voxelpoint_t zero_vp;
extern const voxelpoint_t one_vp;


#endif /* __REALSPACE_H__ */




