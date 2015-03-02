/* Do not edit this file, it is generated automatically from variable_type.m4 */

	
	
	



	
	

/* volume_SBYTE_0D_SCALING.h 
 *
 * generated from the following file:                 */

/* volume_variable_type.h - used to generate the different volume_*.h files
 *
 * Part of amide - Amide's a Medical Image Data Examiner
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


#ifndef __VOLUME_SBYTE_0D_SCALING__
#define __VOLUME_SBYTE_0D_SCALING__

/* header files that are always needed with this file */
#include "volume.h"


/* defines */

/* translates to the contents of the voxel specified by voxelpoint i */
#define VOLUME_SBYTE_0D_SCALING_CONTENTS(volume,i) \
 (*(DATA_SET_FLOAT_0D_SCALING_POINTER((volume)->current_scaling, (i))) * \
  ((amide_data_t) (*(DATA_SET_SBYTE_POINTER((volume)->data_set,(i))))))


/* function declarations */
void volume_SBYTE_0D_SCALING_recalc_max_min(volume_t * volume);
void volume_SBYTE_0D_SCALING_generate_distribution(volume_t * volume);
volume_t * volume_SBYTE_0D_SCALING_get_slice(const volume_t * volume,
								  const amide_time_t start,
								  const amide_time_t duration,
								  const realpoint_t  requested_voxel_size,
								  const realspace_t slice_coord_frame,
								  const realpoint_t far_corner,
								  const interpolation_t interpolation,
								  const gboolean need_calc_max_min);


#endif /* __VOLUME_SBYTE_0D_SCALING__ */















