#ifndef PERFORMANCE_MARKER
#define PERFORMANCE_MARKER


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

#include <list>


const unsigned int MICROSECONDS_PER_SECOND = 1000000; ///< microseconds per second



// todo add a base class to support other marker types
// with a virtual overload for GetBytes() and GetLength()
class PerformanceMarker
{
public:

  /**
   * enum for difference performance markers.
   * Please modify the name lookup table in performance-interface.cpp
   * file if adding new markers (the order must match one to one).
   */
  enum MarkerType
  {
    VSYNC     = 0,        ///< V-Sync
    UPDATE_START ,        ///< Update start
    UPDATE_END   ,        ///< Update end
    RENDER_START ,        ///< Render start
    RENDER_END   ,        ///< Render end
    SWAP_START   ,        ///< SwapBuffers Start
    SWAP_END     ,        ///< SwapBuffers End
    PROCESS_EVENTS_START, ///< Process events start (e.g. touch event)
    PROCESS_EVENTS_END,   ///< Process events end
    PAUSED       ,        ///< Pause start
    RESUME       ,        ///< Resume start
    GL_CLEAR_START,
    GL_CLEAR_END,
    WL_FLUSH_START,       // wayland flush wl_flush
    WL_FLUSH_END,
    WL_DISPATCH_QUEUE_START,  // wayland dispatch queue start ( called by TLP / egl)
    WL_DISPATCH_QUEUE_END,    // wayland dispatch queue start
    WL_DISPATCH_START,        // wayland dispatch start (not called by anything)
    WL_DISPATCH_END,         // wayland dispatch queue start

    START        ,        ///< The start of custom tracking
    END          ,        ///< The end of custom tracking
    UNKNOWN
  };


  struct FrameTime
  {
    FrameTime() {}
    FrameTime(unsigned int frame, unsigned int seconds,unsigned int microSeconds, bool buffer)
    : mFrame( frame ),
      mSeconds( seconds ),
      mMicroSeconds( microSeconds ),
      mBuffer( buffer )
    {}

    unsigned int mFrame;
    unsigned int mSeconds;
    unsigned int mMicroSeconds;

    float GetTime()
    {
      if( !mMicroSeconds )
      {
        return mSeconds;
      }
      else
      {
        // switch to doubles if too much micro-second information gets lots
        return static_cast<float>(mSeconds) + static_cast<float>(mMicroSeconds)/ 1000000.f;
      }
    }

    static FrameTime Diff( const FrameTime& t1, const FrameTime& t2  )
    {
      unsigned int secondsDiff = t2.mSeconds - t1.mSeconds;
      int microSecondsDiff =  t2.mMicroSeconds - t1.mMicroSeconds;
      if( microSecondsDiff < 0 )
      {
        secondsDiff--;
        microSecondsDiff+= MICROSECONDS_PER_SECOND;
      }
      return FrameTime(t1.mBuffer,secondsDiff, microSecondsDiff, t1.mFrame );
    }

    bool  mBuffer;
  };


  static MarkerType GetMarkerType( const char* description );

  PerformanceMarker() {};
  PerformanceMarker(MarkerType type,  FrameTime time);

  static unsigned int GetSerializedSize();
  bool Serialize( unsigned char* buffer, unsigned int& numBytes );
  bool DeSerialize( unsigned char* buffer, unsigned int numBytes );

  MarkerType    mType;                ///< Time type
  FrameTime     mTimeStamp;           ///< Time stamp

};
typedef std::list< PerformanceMarker > MarkerList;

#endif
