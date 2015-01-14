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
 * Google Author(s): Behdad Esfahbod
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "demo-buffer.h"

demo_buffer_t *
demo_buffer_create (void)
{
  demo_buffer_t *buffer = (demo_buffer_t *) calloc (1, sizeof (demo_buffer_t));
  buffer->refcount = 1;

  buffer->vertices = new std::vector<glyph_vertex_t>;

  return buffer;
}

demo_buffer_t *
demo_buffer_reference (demo_buffer_t *buffer)
{
  if (buffer) buffer->refcount++;
  return buffer;
}

void
demo_buffer_destroy (demo_buffer_t *buffer)
{
  if (!buffer || --buffer->refcount)
    return;

  delete buffer->vertices;
  free (buffer);
}


void
demo_buffer_clear (demo_buffer_t *buffer)
{
  buffer->vertices->clear ();
  glyphy_extents_clear (&buffer->ink_extents);
  glyphy_extents_clear (&buffer->logical_extents);
  buffer->dirty = true;
}

void
demo_buffer_extents (demo_buffer_t    *buffer,
		     glyphy_extents_t *ink_extents,
		     glyphy_extents_t *logical_extents)
{
  if (ink_extents)
    *ink_extents = buffer->ink_extents;
  if (logical_extents)
    *logical_extents = buffer->logical_extents;
}

void
demo_buffer_move_to (demo_buffer_t        *buffer,
		     const glyphy_point_t *p)
{
  buffer->cursor = *p;
}

void
demo_buffer_current_point (demo_buffer_t  *buffer,
			   glyphy_point_t *p)
{
  *p = buffer->cursor;
}

void
demo_buffer_add_text (demo_buffer_t        *buffer,
		      const char           *utf8,
		      demo_font_t          *font,
		      double                font_size,
		      double                lineSpacing)
{
  FT_Face face = demo_font_get_face (font);
  glyphy_point_t top_left = buffer->cursor;
  buffer->cursor.y += font_size /* * font->ascent */;
  unsigned int unicode;
  for (const unsigned char *p = (const unsigned char *) utf8; *p; p++) {
    if (*p < 128) {
      unicode = *p;
    } else {
      unsigned int j;
      if (*p < 0xE0) {
	unicode = *p & ~0xE0;
	j = 1;
      } else if (*p < 0xF0) {
	unicode = *p & ~0xF0;
	j = 2;
      } else {
	unicode = *p & ~0xF8;
	j = 3;
	continue;
      }
      p++;
      for (; j && *p; j--, p++)
	unicode = (unicode << 6) | (*p & ~0xC0);
      p--;
    }

    if (unicode == '\n') {
      buffer->cursor.y += font_size+lineSpacing;
      buffer->cursor.x = top_left.x;
      continue;
    }

    unsigned int glyph_index = FT_Get_Char_Index (face, unicode);
    if( glyph_index == 0 )
    {
    	printf("missing glyphy for %c with unicode of %d !\n", *p, unicode);
    }
    glyph_info_t gi;
    demo_font_lookup_glyph (font, glyph_index, &gi);

    /* Update ink extents */
    glyphy_extents_t ink_extents;
    demo_add_glyph_vertices (buffer->cursor, font_size, &gi, buffer->vertices, &ink_extents);
    glyphy_extents_extend (&buffer->ink_extents, &ink_extents);

    /* Update logical extents */
    glyphy_point_t corner;
    corner.x = buffer->cursor.x;
    corner.y = buffer->cursor.y - font_size;
    glyphy_extents_add (&buffer->logical_extents, &corner);
    corner.x = buffer->cursor.x + font_size * gi.advance;
    corner.y = buffer->cursor.y;
    glyphy_extents_add (&buffer->logical_extents, &corner);

    buffer->cursor.x += font_size * gi.advance;
  }

  buffer->dirty = true;
}

static unsigned int
glyph_encode (unsigned int atlas_x ,  /* 7 bits */
	      unsigned int atlas_y,   /* 7 bits */
	      unsigned int corner_x,  /* 1 bit */
	      unsigned int corner_y,  /* 1 bit */
	      unsigned int nominal_w, /* 6 bits */
	      unsigned int nominal_h  /* 6 bits */)
{
  assert (0 == (atlas_x & ~0x7F));
  assert (0 == (atlas_y & ~0x7F));
  assert (0 == (corner_x & ~1));
  assert (0 == (corner_y & ~1));
  assert (0 == (nominal_w & ~0x3F));
  assert (0 == (nominal_h & ~0x3F));

  unsigned int x = (((atlas_x << 6) | nominal_w) << 1) | corner_x;
  unsigned int y = (((atlas_y << 6) | nominal_h) << 1) | corner_y;

  return (x << 16) | y;
}

static void
glyph_vertex_encode (double x, double y,
		     unsigned int corner_x, unsigned int corner_y,
		     const glyph_info_t *gi,
		     glyph_vertex_t *v)
{
  unsigned int encoded = glyph_encode (gi->atlas_x, gi->atlas_y,
				       corner_x, corner_y,
				       gi->nominal_w, gi->nominal_h);
  v->x = x;
  v->y = y;
  v->g16hi = encoded >> 16;
  v->g16lo = encoded & 0xFFFF;
}

void
demo_add_glyph_vertices (const glyphy_point_t        &p,
				double                       font_size,
				glyph_info_t                *gi,
				std::vector<glyph_vertex_t> *vertices,
				glyphy_extents_t            *extents)
{
  if (gi->is_empty)
    return;

  glyph_vertex_t v[4];

#define ENCODE_CORNER(_cx, _cy) \
  do { \
    double _vx = p.x + font_size * ((1-_cx) * gi->extents.min_x + _cx * gi->extents.max_x); \
    double _vy = p.y - font_size * ((1-_cy) * gi->extents.min_y + _cy * gi->extents.max_y); \
    glyph_vertex_encode (_vx, _vy, _cx, _cy, gi, &v[_cx * 2 + _cy]); \
  } while (0)
  ENCODE_CORNER (0, 0);
  ENCODE_CORNER (0, 1);
  ENCODE_CORNER (1, 0);
  ENCODE_CORNER (1, 1);
#undef ENCODE_CORNER

  vertices->push_back (v[0]);
  vertices->push_back (v[1]);
  vertices->push_back (v[2]);

  vertices->push_back (v[1]);
  vertices->push_back (v[2]);
  vertices->push_back (v[3]);

  if (extents) {
    glyphy_extents_clear (extents);
    for (unsigned int i = 0; i < 4; i++) {
      glyphy_point_t p = {v[i].x, v[i].y};
      glyphy_extents_add (extents, &p);
    }
  }
}

