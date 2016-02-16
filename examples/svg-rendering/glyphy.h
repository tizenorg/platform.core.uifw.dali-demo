/*
 * Copyright 2012 Google, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Google Author(s): Behdad Esfahbod, Maysum Panju
 */

#ifndef GLYPHY_H
#define GLYPHY_H

#include <vector>
#include "glyphy-type.h"

#define GLYPHY_PASTE_ARGS(prefix, name) prefix ## name
#define GLYPHY_PASTE(prefix, name) GLYPHY_PASTE_ARGS (prefix, name)

void
glyphy_extents_clear (glyphy_extents_t *extents);

glyphy_bool_t
glyphy_extents_is_empty (const glyphy_extents_t *extents);

void
glyphy_extents_add (glyphy_extents_t     *extents,
		    const glyphy_point_t *p);

void
glyphy_extents_extend (glyphy_extents_t       *extents,
		       const glyphy_extents_t *other);

glyphy_bool_t
glyphy_extents_includes (const glyphy_extents_t *extents,
			 const glyphy_point_t   *p);

void
glyphy_extents_scale (glyphy_extents_t *extents,
		      double            x_scale,
		      double            y_scale);

glyphy_bool_t
glyphy_extents_overlap(const glyphy_extents_t *extents1,
              const glyphy_extents_t *extents2);



/*
 * Circular arcs
 */


typedef struct {
  glyphy_point_t p0;
  glyphy_point_t p1;
  double d;
} glyphy_arc_t;


/* Build from a conventional arc representation */
void
glyphy_arc_from_conventional (const glyphy_point_t *center,
			      double                radius,
			      double                angle0,
			      double                angle1,
			      glyphy_bool_t         negative,
			      glyphy_arc_t         *arc);

/* Convert to a conventional arc representation */
void
glyphy_arc_to_conventional (glyphy_arc_t    arc,
			    glyphy_point_t *center /* may be NULL */,
			    double         *radius /* may be NULL */,
			    double         *angle0 /* may be NULL */,
			    double         *angle1 /* may be NULL */,
			    glyphy_bool_t  *negative /* may be NULL */);

void
glyphy_arc_extents (glyphy_arc_t      arc,
		    glyphy_extents_t *extents);



/*
 * Approximate single pieces of geometry to/from one arc
 */


void
glyphy_arc_from_line (const glyphy_point_t *p0,
		      const glyphy_point_t *p1,
		      glyphy_arc_t         *arc);

void
glyphy_arc_from_conic (const glyphy_point_t *p0,
		       const glyphy_point_t *p1,
		       const glyphy_point_t *p2,
		       glyphy_arc_t         *arc,
		       double               *error);

void
glyphy_arc_from_cubic (const glyphy_point_t *p0,
		       const glyphy_point_t *p1,
		       const glyphy_point_t *p2,
		       const glyphy_point_t *p3,
		       glyphy_arc_t         *arc,
		       double               *error);

void
glyphy_arc_to_cubic (const glyphy_arc_t *arc,
		     glyphy_point_t     *p0,
		     glyphy_point_t     *p1,
		     glyphy_point_t     *p2,
		     glyphy_point_t     *p3,
		     double             *error);



/*
 * Approximate outlines with multiple arcs
 */


typedef struct {
  glyphy_point_t p;
  double d;
} glyphy_arc_endpoint_t;

typedef glyphy_bool_t (*glyphy_arc_endpoint_accumulator_callback_t) (glyphy_arc_endpoint_t *endpoint,
								     void                  *user_data);


typedef struct glyphy_arc_accumulator_t glyphy_arc_accumulator_t;

glyphy_arc_accumulator_t *
glyphy_arc_accumulator_create (void);

void
glyphy_arc_accumulator_destroy (glyphy_arc_accumulator_t *acc);

glyphy_arc_accumulator_t *
glyphy_arc_accumulator_reference (glyphy_arc_accumulator_t *acc);


void
glyphy_arc_accumulator_reset (glyphy_arc_accumulator_t *acc);


/* Configure accumulator */

void
glyphy_arc_accumulator_set_tolerance (glyphy_arc_accumulator_t *acc,
				      double                    tolerance);

double
glyphy_arc_accumulator_get_tolerance (glyphy_arc_accumulator_t *acc);

void
glyphy_arc_accumulator_set_callback (glyphy_arc_accumulator_t *acc,
				     glyphy_arc_endpoint_accumulator_callback_t callback,
				     void                     *user_data);

void
glyphy_arc_accumulator_get_callback (glyphy_arc_accumulator_t  *acc,
				     glyphy_arc_endpoint_accumulator_callback_t *callback,
				     void                     **user_data);

void
glyphy_arc_accumulator_set_d_metrics (glyphy_arc_accumulator_t *acc,
				      double                    max_d,
				      double                    d_bits);

void
glyphy_arc_accumulator_get_d_metrics (glyphy_arc_accumulator_t *acc,
				      double                   *max_d,
				      double                   *d_bits);


/* Accumulation results */

unsigned int
glyphy_arc_accumulator_get_num_endpoints (glyphy_arc_accumulator_t *acc);

double
glyphy_arc_accumulator_get_error (glyphy_arc_accumulator_t *acc);

glyphy_bool_t
glyphy_arc_accumulator_successful (glyphy_arc_accumulator_t *acc);


/* Accumulate */

void
glyphy_arc_accumulator_move_to (glyphy_arc_accumulator_t *acc,
				const glyphy_point_t *p0);

void
glyphy_arc_accumulator_line_to (glyphy_arc_accumulator_t *acc,
				const glyphy_point_t *p1);

void
glyphy_arc_accumulator_conic_to (glyphy_arc_accumulator_t *acc,
				 const glyphy_point_t *p1,
				 const glyphy_point_t *p2);

void
glyphy_arc_accumulator_cubic_to (glyphy_arc_accumulator_t *acc,
				 const glyphy_point_t *p1,
				 const glyphy_point_t *p2,
				 const glyphy_point_t *p3);

void
glyphy_arc_accumulator_arc_to (glyphy_arc_accumulator_t *acc,
			       const glyphy_point_t *p1,
			       double                d);

void
glyphy_arc_accumulator_close_path (glyphy_arc_accumulator_t *acc);

void
glyphy_arc_list_extents (const glyphy_arc_endpoint_t *endpoints,
			 unsigned int                 num_endpoints,
			 glyphy_extents_t            *extents);

/*
 * Encode an arc outline into binary blob for fast SDF calculation
 */


glyphy_bool_t
glyphy_arc_list_encode_blob (const glyphy_arc_endpoint_t *endpoints,
			     unsigned int                 num_endpoints,
                 std::vector<unsigned char>&  blob,
			     double                       faraway,
			     unsigned int                *nominal_width,  /* 6bit */
			     unsigned int                *nominal_height, /* 6bit */
			     glyphy_extents_t            *extents);


/*
 * Calculate signed-distance-field from (encoded) arc list
 */


double
glyphy_df_from_arc_list (const glyphy_arc_endpoint_t *endpoints,
			  unsigned int                 num_endpoints,
			  const glyphy_point_t        *p);

/*
 * new add, for better winding number counting
 */
void
cell_winding_count( const glyphy_arc_endpoint_t *endpoints,
            unsigned int                 num_endpoints,
            const glyphy_extents_t *extents,
            const int grid_w, const int grid_h,
            int* counts, int* pivots);

#endif /* GLYPHY_H */
