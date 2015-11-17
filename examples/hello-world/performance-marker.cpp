/*
Copyright (c) 2000-2015 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

// INTERNAL INCLUDES
#include "performance-marker.h"

#include <strings.h>
#include <stdio.h>
#include <unistd.h>

#include <string.h>



const unsigned int TYPE_SIZE = 1;
const unsigned int SERILIZED_SIZE =  TYPE_SIZE + sizeof( PerformanceMarker::FrameTime );

struct NamePair
{
  PerformanceMarker::MarkerType type;
  const char* const name;
};

const NamePair MARKER_LOOKUP[] =
{
    // timed event names must be postfixed with with _START and _END
    // this is to allow tracers to extract the event name by removing the _START, _END strings
    //
    { PerformanceMarker::VSYNC       ,         "V_SYNC"               },
    { PerformanceMarker::UPDATE_START ,        "UPDATE_START"         },
    { PerformanceMarker::UPDATE_END   ,        "UPDATE_END"           },
    { PerformanceMarker::RENDER_START ,        "RENDER_START"         },
    { PerformanceMarker::RENDER_END   ,        "RENDER_END"           },
    { PerformanceMarker::SWAP_START   ,        "SWAP_START"           },
    { PerformanceMarker::SWAP_END     ,        "SWAP_END"             },
    { PerformanceMarker::PROCESS_EVENTS_START, "PROCESS_EVENT_START"  },
    { PerformanceMarker::PROCESS_EVENTS_END,   "PROCESS_EVENT_END"    },
    { PerformanceMarker::PAUSED       ,        "PAUSED"               },
    { PerformanceMarker::RESUME       ,        "RESUMED"              },
    { PerformanceMarker::START        ,        "START"                },
    { PerformanceMarker::END          ,        "END"                  },
    { PerformanceMarker::GL_CLEAR_START,       "GL_CLEAR_START"       },
    { PerformanceMarker::GL_CLEAR_END ,        "GL_CLEAR_END"         },
    { PerformanceMarker::WL_FLUSH_START,       "WL_FLUSH_START"       },
    { PerformanceMarker::WL_FLUSH_END,           "WL_FLUSH_END"       },
    { PerformanceMarker::WL_DISPATCH_QUEUE_START,"WL_DISPATCH_QUEUE_START"},
    { PerformanceMarker::WL_DISPATCH_QUEUE_END,  "WL_DISPATCH_QUEUE_END"},
    { PerformanceMarker::WL_DISPATCH_START,      "WL_DISPATCH_START"},
    { PerformanceMarker::WL_DISPATCH_END,        "WL_DISPATCH_END"}

};
const unsigned int MARKER_TABLE_ENTRIES = sizeof( MARKER_LOOKUP ) / sizeof( NamePair );

PerformanceMarker::MarkerType PerformanceMarker::GetMarkerType( const char* description )
{
  for( unsigned int i = 0; i < MARKER_TABLE_ENTRIES; ++i )
  {
    const NamePair& namePair(MARKER_LOOKUP[i]);

    if(strcmp( namePair.name , description) == 0 )
    {
      return namePair.type;
    }
  }
  return PerformanceMarker::UNKNOWN;
}


PerformanceMarker::PerformanceMarker( MarkerType type, PerformanceMarker::FrameTime time )
{
  mTimeStamp = time;
  mType = type;
}

bool PerformanceMarker::Serialize( unsigned char* buffer, unsigned int& numBytes )
{
  if( numBytes < SERILIZED_SIZE )
   {
     return false;
   }

  unsigned char *ptr = buffer;

  ptr[ 0 ] = mType;

  // shift the ptr along by the size of type size
  ptr++;

  memcpy( ptr,  &mTimeStamp, sizeof(PerformanceMarker::FrameTime));

  return true;
}
unsigned int PerformanceMarker::GetSerializedSize()
{
  return SERILIZED_SIZE;
}

bool PerformanceMarker::DeSerialize( unsigned char* buffer, unsigned int numBytes )
{
  if( numBytes < SERILIZED_SIZE )
  {
    return false;
  }
  unsigned char type = buffer[0];

  mType = static_cast<MarkerType>( type);
  buffer++;

  memcpy( &mTimeStamp, buffer, sizeof(PerformanceMarker::FrameTime));

  return true;
}


