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

#ifndef GLYPHY_TYPE_H
#define GLYPHY_TYPE_H

typedef int glyphy_bool_t;

typedef struct {
  double x;
  double y;
} glyphy_point_t;

/*
 * Geometry extents
 */

typedef struct {
  double min_x;
  double min_y;
  double max_x;
  double max_y;
} glyphy_extents_t;


/*
 * Encode an arc outline into binary blob for fast SDF calculation
 */
typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} glyphy_rgba_t;

#endif /* GLYPHY_TYPE_H */
