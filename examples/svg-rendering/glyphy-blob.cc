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
 * Google Author(s): Behdad Esfahbod, Maysum Panju, Wojciech Baranowski
 */

#include <iostream>

#include "glyphy-common.hh"
#include "glyphy-geometry.hh"

#define GRID_SIZE 30

using namespace GLyphy::Geometry;


#define UPPER_BITS(v,bits,total_bits) ((v) >> ((total_bits) - (bits)))
#define LOWER_BITS(v,bits,total_bits) ((v) & ((1 << (bits)) - 1))

#define MAX_X 4095
#define MAX_Y 4095

static inline glyphy_rgba_t
arc_endpoint_encode (unsigned int ix, unsigned int iy, double d)
{
  glyphy_rgba_t v;

  /* 12 bits for each of x and y, 8 bits for d */
  assert (ix <= MAX_X);
  assert (iy <= MAX_Y);
  unsigned int id;
  if (isinf (d))
    id = 0;
  else {
    assert (fabs (d) <= GLYPHY_MAX_D);
    id = 128 + lround (d * 127 / GLYPHY_MAX_D);
  }
  assert (id < 256);

  v.r = id;
  v.g = LOWER_BITS (ix, 8, 12);
  v.b = LOWER_BITS (iy, 8, 12);
  v.a = ((ix >> 8) << 4) | (iy >> 8);
  return v;
}

static inline glyphy_rgba_t
arc_list_encode (unsigned int offset, unsigned int num_points, int pivot, int count)
{
  glyphy_rgba_t v;
  int winding = count % 2;
  v.r = pivot<<1 | LOWER_BITS (winding, 1, 8);
  v.g = UPPER_BITS (offset, 8, 16);
  v.b = LOWER_BITS (offset, 8, 16);
  v.a = LOWER_BITS (num_points, 8, 15);
  return v;
}

double
glyphy_df_from_arc_list (const glyphy_arc_endpoint_t *endpoints,
              unsigned int                 num_endpoints,
              const glyphy_point_t        *p)
{
  Point c = *p;
  Point p0 (0, 0);
  double min_dist = GLYPHY_INFINITY;
  double dist;
  for (unsigned int i = 0; i < num_endpoints; i++)
  {
    const glyphy_arc_endpoint_t &endpoint = endpoints[i];
    if (endpoint.d == GLYPHY_INFINITY)
    {
      p0 = endpoint.p;
      continue;
    }

    Arc arc (p0, endpoint.p, endpoint.d);
    p0 = endpoint.p;

    if (arc.wedge_contains_point (c))
    {
      dist = fabs(arc.distance_to_point (c) );
    }
    else
    {
      dist = std::min ((arc.p0 - c).len (), (arc.p1 - c).len ());
    }

    if (dist < min_dist)
    {
      min_dist = dist;
    }
  }

  return min_dist;
}

/* Given a cell, fills the vector closest_arcs with arcs that may be closest to some point in the cell.
 * Uses idea that all close arcs to cell must be ~close to center of cell.
 */
static void
closest_arcs_to_cell (Point c0, Point c1, /* corners */
		      double faraway,
		      const glyphy_arc_endpoint_t *endpoints,
		      unsigned int num_endpoints,
		      std::vector<glyphy_arc_endpoint_t> &near_endpoints)
{
  // Find distance between cell center
  Point c = c0.midpoint (c1);
  double min_dist = glyphy_df_from_arc_list (endpoints, num_endpoints, &c );
  min_dist = min_dist < faraway*0.5f+1. ? min_dist : faraway*0.5f+1.;

  std::vector<Arc> near_arcs;

  // If d is the distance from the center of the square to the nearest arc, then
  // all nearest arcs to the square must be at most almost [d + half_diagonal] from the center.
  double half_diagonal = (c - c0).len ();
  double radius_squared = pow (min_dist + half_diagonal, 2);
  if (min_dist - half_diagonal <= faraway)
  {
    Point p0 (0, 0);
    for (unsigned int i = 0; i < num_endpoints; i++)
    {
      const glyphy_arc_endpoint_t &endpoint = endpoints[i];
      if (endpoint.d == GLYPHY_INFINITY)
      {
	    p0 = endpoint.p;
	    continue;
      }
      Arc arc (p0, endpoint.p, endpoint.d);
      p0 = endpoint.p;

      if (arc.squared_distance_to_point (c) <= radius_squared)
        near_arcs.push_back (arc);
    }
  }

  Point p1 = Point (0, 0);
  for (unsigned i = 0; i < near_arcs.size (); i++)
  {
    Arc arc = near_arcs[i];

    if (i == 0 || p1 != arc.p0) {
      glyphy_arc_endpoint_t endpoint = {arc.p0, GLYPHY_INFINITY};
      near_endpoints.push_back (endpoint);
      p1 = arc.p0;
    }

    glyphy_arc_endpoint_t endpoint = {arc.p1, arc.d};
    near_endpoints.push_back (endpoint);
    p1 = arc.p1;
  }
}


glyphy_bool_t
glyphy_arc_list_encode_blob (const glyphy_arc_endpoint_t *endpoints,
			     unsigned int                 num_endpoints,
			     glyphy_rgba_t               *blob,
			     unsigned int                 blob_size,
			     double                       faraway,
			     double                       avg_fetch_desired,
			     double                      *avg_fetch_achieved,
			     unsigned int                *output_len,
			     unsigned int                *nominal_width,  /* 8bit */
			     unsigned int                *nominal_height, /* 8bit */
			     glyphy_extents_t            *pextents)
{
  glyphy_extents_t extents;
  glyphy_extents_clear (&extents);

  glyphy_arc_list_extents (endpoints, num_endpoints, &extents);

  if (glyphy_extents_is_empty (&extents)) {
    *pextents = extents;
    if (!blob_size)
      return false;
    *blob = arc_list_encode (0, 0, +1,0);
    *avg_fetch_achieved = 1;
    *output_len = 1;
    *nominal_width = *nominal_height = 1;
    return true;
  }

  /* Add antialiasing padding */
  extents.min_x -= faraway;
  extents.min_y -= faraway;
  extents.max_x += faraway;
  extents.max_y += faraway;

  double glyph_width = extents.max_x - extents.min_x;
  double glyph_height = extents.max_y - extents.min_y;
  double unit = std::max (glyph_width, glyph_height);

  unsigned int num_size = sqrt(num_endpoints);
  unsigned int grid_size = GRID_SIZE < num_size ? GRID_SIZE : num_size;
  unsigned int grid_w = grid_size;
  unsigned int grid_h = grid_size;

  if (glyph_width > glyph_height) {
    while ((grid_h - 1) * unit / grid_w > glyph_height)
      grid_h--;
    glyph_height = grid_h * unit / grid_w;
    extents.max_y = extents.min_y + glyph_height;
  } else {
    while ((grid_w - 1) * unit / grid_h > glyph_width)
      grid_w--;
    glyph_width = grid_w * unit / grid_h;
    extents.max_x = extents.min_x + glyph_width;
  }

  std::vector<int> counts;
  counts.resize(grid_w * grid_h);
  std::vector<int> pivots;
  pivots.resize(grid_w * grid_h);
  cell_winding_count(endpoints, num_endpoints, &extents, grid_w, grid_h, &counts[0], &pivots[0]);

  double cell_unit = unit / std::max (grid_w, grid_h);

  std::vector<glyphy_rgba_t> tex_data;
  std::vector<glyphy_arc_endpoint_t> near_endpoints;

  unsigned int header_length = grid_w * grid_h;
  unsigned int offset = header_length;
  tex_data.resize (header_length);
  Point origin = Point (extents.min_x, extents.min_y);
  unsigned int total_arcs = 0;

  for (unsigned int row = 0; row < grid_h; row++)
    for (unsigned int col = 0; col < grid_w; col++)
    {
      Point cp0 = origin + Vector ((col + 0) * cell_unit, (row + 0) * cell_unit);
      Point cp1 = origin + Vector ((col + 1) * cell_unit, (row + 1) * cell_unit);
      near_endpoints.clear ();

      closest_arcs_to_cell (cp0, cp1, faraway, endpoints, num_endpoints, near_endpoints);

#define QUANTIZE_X(X) (lround (MAX_X * ((X - extents.min_x) / glyph_width )))
#define QUANTIZE_Y(Y) (lround (MAX_Y * ((Y - extents.min_y) / glyph_height)))
#define DEQUANTIZE_X(X) (double (X) / MAX_X * glyph_width  + extents.min_x)
#define DEQUANTIZE_Y(Y) (double (Y) / MAX_Y * glyph_height + extents.min_y)
#define SNAP(P) (Point (DEQUANTIZE_X (QUANTIZE_X ((P).x)), DEQUANTIZE_Y (QUANTIZE_Y ((P).y))))

      /* If the arclist is two arcs that can be combined in encoding if reordered,
       * do that. */
      if (near_endpoints.size () == 4 &&
	  isinf (near_endpoints[2].d) &&
	  near_endpoints[0].p.x == near_endpoints[3].p.x &&
	  near_endpoints[0].p.y == near_endpoints[3].p.y)
      {
    	  glyphy_arc_endpoint_t e0, e1, e2;
        e0 = near_endpoints[2];
        e1 = near_endpoints[3];
        e2 = near_endpoints[1];
        near_endpoints.resize (0);
        near_endpoints.push_back (e0);
        near_endpoints.push_back (e1);
        near_endpoints.push_back (e2);
      }

      for (unsigned i = 0; i < near_endpoints.size (); i++)
      {
        glyphy_arc_endpoint_t &endpoint = near_endpoints[i];
	    tex_data.push_back (arc_endpoint_encode (QUANTIZE_X(endpoint.p.x), QUANTIZE_Y(endpoint.p.y), endpoint.d));
      }

      unsigned int current_endpoints = tex_data.size () - offset;

      if (current_endpoints)
      {
        // See if we can fulfill this cell by using already-encoded arcs
        const glyphy_rgba_t *needle = &tex_data[offset];
        unsigned int needle_len = current_endpoints;
        const glyphy_rgba_t *haystack = &tex_data[header_length];
        unsigned int haystack_len = offset - header_length;

        bool found = false;
        while (haystack_len >= needle_len) {
          // Trick: we don't care about first endpoint's d value, so skip one
          // byte in comparison.  This works because arc_encode() packs the
          // d value in the first byte.
          if (0 == memcmp (1 + (const char *) needle,
              1 + (const char *) haystack,
              needle_len * sizeof (*needle) - 1)) {
            found = true;
            break;
          }
          haystack++;
          haystack_len--;
        }
        if (found) {
          unsigned int new_offset = haystack - &tex_data[0];
          tex_data.resize (offset);
          haystack = needle = NULL; // Invalidated by the resize.
          offset = new_offset;
        }
      }
      else
        offset = 0;

      tex_data[row * grid_w + col] = arc_list_encode (offset, current_endpoints, pivots[row*grid_w+col], counts[row*grid_w+col] );
      offset = tex_data.size ();

      total_arcs += current_endpoints;
    }

  if (avg_fetch_achieved)
    *avg_fetch_achieved = 1 + double (total_arcs) / (grid_w * grid_h);

  *pextents = extents;

  if (tex_data.size () > blob_size)
    return false;

  memcpy (blob, &tex_data[0], tex_data.size () * sizeof(tex_data[0]));
  *output_len = tex_data.size ();
  *nominal_width = grid_w;
  *nominal_height = grid_h;

  return true;
}
