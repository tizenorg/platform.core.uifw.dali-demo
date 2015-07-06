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

#ifndef GLYPHY_ARCS_BEZIER_HH
#define GLYPHY_ARCS_BEZIER_HH

#include "glyphy-common.hh"
#include "glyphy-geometry.hh"
#include "glyphy-arc-bezier.hh"

namespace GLyphy {
namespace ArcsBezier {

using namespace Geometry;
using namespace ArcBezier;

template <class ArcBezierApproximator>
class ArcsBezierApproximatorSpringSystem
{
  static inline double calc_arcs (const Bezier &b,
				double start,
				double end,
				const ArcBezierApproximator &appx,
				std::vector<Arc > &arcs)
  {
    Bezier segment = b.segment (start, end);
    double error;
    arcs.push_back (appx.approximate_bezier_with_arc (segment, &error));

    return error;
  }

  public:
  static void approximate_bezier_with_arcs (const Bezier &b,
					    double tolerance,
					    const ArcBezierApproximator &appx,
					    std::vector<Arc> &arcs,
					    double *perror){

    // find inflection points
    double roots[2];
    int count = inflection( b, roots );
    if(count>0){
      approximate_bezier_with_arcs (b,tolerance,0.0,roots[0],appx,arcs);
      if(count>1){
        approximate_bezier_with_arcs (b,tolerance,roots[0],roots[1],appx,arcs);
      }
      approximate_bezier_with_arcs (b,tolerance,roots[count-1],1.0,appx,arcs);
    }else{
      approximate_bezier_with_arcs (b,tolerance,0.0,1.0,appx,arcs);
    }

    if (perror)
      *perror = tolerance;

  }

  private:
  static void approximate_bezier_with_arcs (const Bezier &b,
                        double tolerance,
                        double start,
                        double end,
                        const ArcBezierApproximator &appx,
                        std::vector<Arc> &arcs)
  {
    double error = calc_arcs (b, start,end, appx, arcs);
    if(  error > tolerance){
        arcs.pop_back();
        approximate_bezier_with_arcs (b,tolerance,start,(end+start)*0.5,appx,arcs);
        approximate_bezier_with_arcs (b,tolerance,(end+start)*0.5,end,appx,arcs);
    }

  }

  //http://www.caffeineowl.com/graphics/2d/vectorial/cubic-inflexion.html#formula
  private:
  static int inflection( const Bezier &bez,
                          double* roots ){
    double Ax, Bx, Cx, Ay, By, Cy;
    double b2ac, t;

    Ax = bez.p1.x - bez.p0.x;
    Ay = bez.p1.y - bez.p0.y;
    Bx = bez.p2.x - bez.p1.x - Ax;
    By = bez.p2.y - bez.p1.y - Ay;
    Cx = bez.p3.x - bez.p2.y - Ax - 2.0*Bx;
    Cy = bez.p3.y - bez.p2.y - Ay - 2.0*By;

    double a, b, c;
    a = Ax*By - Ay*Bx;
    b = ( Ax*Cy - Ay*Cx ) / 2.0;
    c = Bx*Cy - By*Cx;

    int count = 0;
    if (fabs(a) < GLYPHY_EPSILON) {
      if (fabs(b) > GLYPHY_EPSILON) {
        t = -c / b;
        if (t > GLYPHY_EPSILON && t < 1.0-GLYPHY_EPSILON)
          roots[count++] = t;
      }
    } else {
      b2ac = b*b - 4.0*c*a;
      if (b2ac > GLYPHY_EPSILON) {
        t = (-b + sqrt(b2ac)) / (2.0 * a);
        if (t > GLYPHY_EPSILON && t < 1.0-GLYPHY_EPSILON)
          roots[count++] = t;
        t = (-b - sqrt(b2ac)) / (2.0 * a);
        if (t > GLYPHY_EPSILON && t < 1.0-GLYPHY_EPSILON)
          roots[count++] = t;
      }
    }

    if( count == 2 && roots[0] > roots[1] ){
      double temp = roots[0];
      roots[0] = roots[1];
      roots[1] = temp;
    }

    return count;
  }
};

} /* namespace ArcsBezier */
} /* namespace GLyphy */

#endif /* GLYPHY_ARCS_BEZIER_HH */
