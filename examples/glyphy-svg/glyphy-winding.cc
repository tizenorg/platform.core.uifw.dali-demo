/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>

#include "glyphy-common.hh"
#include "glyphy-geometry.hh"

using namespace GLyphy::Geometry;

void line_intersection_Y( const Point& p0, const Point& p1, double y, bool &found, double &x )
{
  found = false;
  x = 0.f;

  double a = p1.y - p0.y;

  if( ! iszero( a ) )
  {
     float b = y - p0.y;
     float t = b / a;
     if( t>= 0.0 && t<=1.0 )
     {
       x = p0.x + t*( p1.x - p0.x);
       found = true;
     }
  }
}

void arc_intersection_Y( const Arc& arc, double y, bool &found1, bool& found2, double &x1, double &x2 )
{
  found1 = false;
  found2 = false;
  x1 = 0.0;
  x2 = 0.0;

  glyphy_extents_t arcBbox;
  arc.extents(arcBbox);

  if( y >= arcBbox.min_y && y<= arcBbox.max_y )
  {
    Point center = arc.center();
    double radius = arc.radius();
    double m = sqrt( radius*radius - ( y-center.y )*(y-center.y) );
    x1 = center.x - m;
    x2 = center.x + m;
    Point p1(x1,y), p2(x2,y);
    if( arc.wedge_contains_point(p1) ) found1 = true;
    if( arc.wedge_contains_point(p2) ) found2 = true;
  }
}

void
cell_winding_count( const glyphy_arc_endpoint_t *endpoints,
            unsigned int                 num_endpoints,
            const glyphy_extents_t *extents,
            const int grid_w, const int grid_h,
            int* counts, int* pivots)
{
  double cell_width = (extents->max_x - extents->min_x) / grid_w;
  double cell_height = (extents->max_y - extents->min_y) / grid_h;

  int num_cell = grid_w * grid_h;
  for( int i = 0; i<num_cell; i++  )
  {
    counts[i] = 0;
    pivots[i] = 0;
  }

  double threshold = 0.01;

  for( int row = 0; row<grid_h; row++ )
  {
    Point p0(0.0,0.0);
    double y = extents->min_y + (row+0.4) * cell_height;
    for( unsigned int i=0; i<num_endpoints; i++ )
    {
      const glyphy_arc_endpoint_t &endpoint = endpoints[i];
      if (endpoint.d == GLYPHY_INFINITY) {
        p0 = endpoint.p;
        continue;
      }
      Arc arc (p0, endpoint.p, endpoint.d);
      p0 = endpoint.p;

      if( iszero (arc.d) ) // line
      {
        bool found;
        double x;
        line_intersection_Y( arc.p0, arc.p1, y, found, x );
        if( found )
        {
          float location = (x-extents->min_x) / cell_width  + 0.6;
          int divide = int(location);
          if( fabs( location - round( location ) )< threshold ){
            pivots[ row*grid_w + int(round( location ))-1 ] = 1;
          }
          for( int col = 0; col < divide; col++ )
          {
            counts[ row*grid_w + col ] += 1;
          }
        }
      }
      else // arc
      {
        bool found1, found2;
        double x1, x2;
        arc_intersection_Y(arc, y, found1, found2, x1, x2);
        if( found1 )
        {
          float location = (x1-extents->min_x) / cell_width  + 0.6;
          int divide = int(location);
          if( fabs( location - round( location ) ) < threshold  ){
            pivots[ row*grid_w + int(round( location ))-1 ] = 1;
          }
          for( int col = 0; col < divide; col++ )
          {
            counts[ row*grid_w + col ] += 1;
          }
        }
        if( found2 )
        {
          float location = (x2-extents->min_x) / cell_width  + 0.6;
          int divide = int(location);
          if( fabs( location - round( location ) ) < threshold  ){
            pivots[ row*grid_w + int(round( location ))-1 ] = 1;
          }
          for( int col = 0; col < divide; col++ )
          {
            counts[ row*grid_w + col ] += 1;
          }
        }
      }
    }
  }



  for( int row = 0; row<grid_h; row++ )
    for( int col = 0; col<grid_w; col++ )
    {
      int idx = row*grid_w + col;
      if( pivots[idx] )
      {
        Point p0(0.0,0.0);
        int count = 0;

        double y = extents->min_y + (row+0.8) * cell_height;
        for( unsigned int i=0; i<num_endpoints; i++ ){
          const glyphy_arc_endpoint_t &endpoint = endpoints[i];
          if (endpoint.d == GLYPHY_INFINITY)
          {
            p0 = endpoint.p;
            continue;
          }
          Arc arc (p0, endpoint.p, endpoint.d);
          p0 = endpoint.p;

          Point current(extents->min_x+(col+0.4)*cell_width,  y);
          if( fabs(arc.distance_to_point( current )) < threshold*cell_width )
          {
            pivots[ idx ] = 2;
            break;
          }

          if( iszero (arc.d) ) // line
          {
            bool found;
            double x;
            line_intersection_Y( arc.p0, arc.p1, y, found, x );
            if( found )
            {
              float location = (x-extents->min_x) / cell_width  + 0.6;
              if(col < int(location) )
              {
                count++;
              }
            }
          }
          else // arc
          {
            bool found1, found2;
            double x1, x2;
            arc_intersection_Y(arc, y, found1, found2, x1, x2);
            if( found1 )
            {
              float location = (x1-extents->min_x) / cell_width  + 0.6;
              if(col < int(location) )
              {
                count++;
              }
            }
            if( found2 )
            {
              float location = (x2-extents->min_x) / cell_width  + 0.6;
              if(col < int(location) )
              {
                count++;
              }
            }
          }
          counts[idx] = count;
        }
      }
    }

  for( int row = 0; row<grid_h; row++ )
    for( int col = 0; col<grid_w; col++ )
    {
      int idx = row*grid_w + col;
      if( pivots[idx] > 1)
      {
        Point p0(0.0,0.0);
        int count = 0;

        double y = extents->min_y + (row+0.4) * cell_height;
        for( unsigned int i=0; i<num_endpoints; i++ ){
          const glyphy_arc_endpoint_t &endpoint = endpoints[i];
          if (endpoint.d == GLYPHY_INFINITY)
          {
            p0 = endpoint.p;
            continue;
          }
          Arc arc (p0, endpoint.p, endpoint.d);
          p0 = endpoint.p;

          Point current(extents->min_x+(col+0.8)*cell_width,  y);
          if( fabs(arc.distance_to_point( current )) < threshold*cell_width  )
          {
            pivots[ idx] = 3;
            counts[idx] = 0;
            break;
          }

          if( iszero (arc.d) ) // line
          {
            bool found;
            double x;
            line_intersection_Y( arc.p0, arc.p1, y, found, x );
            if( found )
            {
              float location = (x-extents->min_x) / cell_width  + 0.2;
              if(col < int(location) )
              {
                count++;
              }
            }
          }
          else // arc
          {
            bool found1, found2;
            double x1, x2;
            arc_intersection_Y(arc, y, found1, found2, x1, x2);
            if( found1 )
            {
              float location = (x1-extents->min_x) / cell_width  + 0.2;
              if(col < int(location) )
              {
                count++;
              }
            }
            if( found2 )
            {
              float location = (x2-extents->min_x) / cell_width  + 0.2;
              if(col < int(location) )
              {
                count++;
              }
            }
          }
          counts[idx] = count;
        }
      }
    }

  for( int row = 0; row<grid_h; row++ )
    for( int col = 0; col<grid_w; col++ )
    {
      int idx = row*grid_w + col;
      if( pivots[idx] > 2)
      {
        std::cout<<"cross the center!!!---again---and again---"<<std::endl;
      }
    }
}
