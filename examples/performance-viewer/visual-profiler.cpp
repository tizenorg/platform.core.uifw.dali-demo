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


// CLASS HEADER
#include "visual-profiler.h"

#include "profile-client.h"
#include <stdio.h>
namespace
{

const unsigned int PIXELS_PER_MILLI_SECOND = 8.0f;
const unsigned int GRAPH_WIDTH  = 1900;   // 1 pixel per milli-second
const unsigned int GRAPH_HEIGHT = 1200;
const unsigned int FOUR_BYTES_PER_PIXEL = 4;
const unsigned int GRAPH_BYTE_COUNT = ( GRAPH_WIDTH * GRAPH_HEIGHT * FOUR_BYTES_PER_PIXEL);

  // calculate how long a single row is, in seconds
  // e.g. if GraphWidth = 1000, and PixelPerMillisecond = 1,  then 1 row = 1 second
const float SECONDS_PER_ROW = static_cast<float>(GRAPH_WIDTH) / static_cast<float>( PIXELS_PER_MILLI_SECOND  * 1000 );
const unsigned int TIMELINE_HEIGHT = 30;          // 30 pixel height for each timeline
const unsigned int GRAPH_UPDATE_FPS_RATE  = 60;  // FPS
const float TIMER_RATE = 1000.f/static_cast<float>(GRAPH_UPDATE_FPS_RATE);

  // Chart Colors
const unsigned int UPDATE_BUFFER_0_COLOR = 0x99FF66;
const unsigned int UPDATE_BUFFER_1_COLOR = 0x7ACC52;
const unsigned int RENDER_BUFFER_0_COLOR = 0xDDFFFF; // lighter for buffer zero
const unsigned int RENDER_BUFFER_1_COLOR = 0xAACCFF;
const unsigned int SWAP_BUFFERS_COLOR  =  0xFFCEE6;
const unsigned int GL_CLEAR_COLOR = 0xfdffdd;//0xcecfff;//0xff9400;//0xec373b;
const unsigned int WL_DISPATCH_QUEUE_COLOR = 0xFFFFFF;

const unsigned int FULL_ALPHA= 0xFF000000;

struct GraphLine
{
    unsigned int mColor;
    unsigned int mYPosition;
    unsigned int mStartX;
    unsigned int mStartY;

};



/**
 * Given a time in seconds, work out the x,y the graph to plot an event.
 * e.g.
 * time scans across, then moves down a row when it hits then end. E.g.
 *
 * 0-----> 0.5-----> 1
 * 1-----> 1.5-----> 2
 * 2-----> 2.5-----> 3
 * 3-----> 3.5-----> 4
 * 4-----> 4.5-----> 5
 *
 */
void GetTimeXYPosition( float secondsDiff, unsigned int& xPos,unsigned int& yPos )
{
  // get y position to plot
  // e.g. if graph width == 1 second
  // and our time is 4 seconds, then plot at y == 4
  // we multiple by TimeLineHeight because we use multiple lines per timeline (update/render/vsync lines)

  unsigned int column= floor( secondsDiff / SECONDS_PER_ROW );
  yPos = column * TIMELINE_HEIGHT;


  // Get x position
  // Each col (horizontal line) represents a second (or whatever is in SecondsPerRow)
  // We need to minus num columns * SecondPerRow  to find the fractional part of the second
  float secondsDone = (column * SECONDS_PER_ROW);
  float x = secondsDiff - secondsDone;

  // xPos = value between 0 .. SECONDS PER ROW
  // multiply by graph width and divide by seconds per row
  xPos = x * ((GRAPH_WIDTH /** PIXELS_PER_MILLI_SECOND*/) / SECONDS_PER_ROW );

}

}

Profiler::Profiler()
:mConnected( false ),
 mGraphInitialised( false )
{

  mPixelBuffer = new unsigned char[ GRAPH_BYTE_COUNT ];



}
void Profiler::Start()
{
  if(! mGraph )
  {
    mGraph = Dali::BufferImage::New(mPixelBuffer,
                           GRAPH_WIDTH,
                           GRAPH_HEIGHT,
                           Dali::Pixel::RGBA8888);

    ClearImage();
  }
  if( ! mTimer )
  {
    mTimer = Dali::Timer::New(TIMER_RATE);

    mTimer.TickSignal().Connect( this, &Profiler::TimerCallback );
  }

  mTimer.Start();
}
void Profiler::Stop()
{
  mTimer.Stop();
}

bool Profiler::TimerCallback()
{
  // Get a list of time stamped markers from DALi via a socket.
  mProfileClient.GetTimeMarkers( mTimeMarkers );

  // update the graph
  UpdateGraph();

  return true;
}


void Profiler::PlotPixel( unsigned int color, unsigned int xPos,unsigned int yPos )
{
  if(( xPos >= GRAPH_WIDTH) || ( yPos >= GRAPH_HEIGHT ))
  {
    return;
  }
  unsigned int y = yPos * GRAPH_WIDTH *FOUR_BYTES_PER_PIXEL;
  unsigned int x = y + xPos*FOUR_BYTES_PER_PIXEL;

  // switch R / B
  mPixelBuffer[ x + 2 ] = color & 0xFF;        // RED
  mPixelBuffer[ x + 1 ] = (color >> 8) & 0xFF;  // GREEN
  mPixelBuffer[ x + 0 ] = (color >> 16 )& 0xFF; // BLUE
  mPixelBuffer[ x + 3 ] = (color >> 24 )& 0xFF; // ALPHA
}

void Profiler::DrawLine(  unsigned int color,
                                unsigned int startX,
                                unsigned int startY,
                                unsigned int endX,
                                unsigned int endY )
{

  // if it wraps, the split into two lines
  if( startY != endY )
  {
    DrawLine( color, startX, startY, GRAPH_WIDTH, startY );
    DrawLine( color, 0, endY, endX, endY);
    return;
  }
  if(( endY >= GRAPH_HEIGHT ) ||( endX >= GRAPH_WIDTH ))
  {
    endX= GRAPH_WIDTH -1;
    endY = GRAPH_HEIGHT -1;
  }
  for( unsigned int x = startX; x < endX; x++ )
  {
    PlotPixel( color, x, startY );
  }
}
unsigned int Profiler::GetColor(PerformanceMarker::MarkerType type, unsigned int bufferId )
{

  switch( type )
  {
  case PerformanceMarker::UPDATE_START:
  case PerformanceMarker::UPDATE_END:
  {
    if (bufferId == 0)
    {
      return (FULL_ALPHA | UPDATE_BUFFER_0_COLOR);
    }
    else
    {
      return (FULL_ALPHA | UPDATE_BUFFER_1_COLOR);
    }

  }
  case PerformanceMarker::RENDER_START:
  case PerformanceMarker::RENDER_END:
  {
    // BGR
    if (bufferId == 0)
    {
      return (FULL_ALPHA |  RENDER_BUFFER_0_COLOR ); //red
    }
    else
    {
      return (FULL_ALPHA |  RENDER_BUFFER_1_COLOR ); //green
    }
  }
  case PerformanceMarker::SWAP_END:
  case PerformanceMarker::SWAP_START:
  {
    // BGR
      return (FULL_ALPHA | SWAP_BUFFERS_COLOR ); //red
  }

  case PerformanceMarker::GL_CLEAR_START:
  case PerformanceMarker::GL_CLEAR_END:
  {
    // BGR
      return (FULL_ALPHA | GL_CLEAR_COLOR);
  }
  case PerformanceMarker::WL_DISPATCH_QUEUE_START:
  case PerformanceMarker::WL_DISPATCH_QUEUE_END:
  {
    return  (FULL_ALPHA |WL_DISPATCH_QUEUE_COLOR);
  }

  default:
     return 0xFFFFFFFF; //blue
  }
}

void Profiler::PlotMarker(PerformanceMarker::MarkerType type, unsigned int bufferId, unsigned int xPos,unsigned int yPos )
{
  if( type ==PerformanceMarker::VSYNC )
  {
      // draw a white line
      for( unsigned int y = 0; y< TIMELINE_HEIGHT; y++ )
      {
        if (( yPos+y) < GRAPH_HEIGHT )
        {
          PlotPixel( 0xFFFFFFAA, xPos, yPos+y );
        }
      }
      return;
  }

  static unsigned int UpdateStartXPos(0);
  static unsigned int UpdateStartYPos(0);

  static unsigned int RenderStartXPos(0);
  static unsigned int RenderStartYPos(0);

  static unsigned int SwapStartXPos(0);
  static unsigned int SwapStartYPos(0);

  static unsigned int glClearStartXPos(0);
  static unsigned int glClearStartYPos(0);

  static unsigned int wlDispatchQueueStartXPos(0);
  static unsigned int wlDispatchQueueStartYPos(0);

  // Record the start positions of the line

  if( type ==PerformanceMarker::UPDATE_START )
  {
    UpdateStartXPos = xPos;
    UpdateStartYPos = yPos;
    return;
  }
  else if( type == PerformanceMarker::RENDER_START )
    {
      RenderStartXPos = xPos;
      RenderStartYPos = yPos;
      return;
  }
  else if( type == PerformanceMarker::SWAP_START )
  {
    SwapStartXPos = xPos;
    SwapStartYPos = yPos;
    return;
  }
  else if( type == PerformanceMarker::GL_CLEAR_START)
  {
    glClearStartXPos = xPos;
    glClearStartYPos = yPos;
    return;
  }
  else if( type == PerformanceMarker::WL_DISPATCH_QUEUE_START)
  {
    wlDispatchQueueStartXPos = xPos;
    wlDispatchQueueStartYPos = yPos;
    return;
  }

    const unsigned int updatePos = 8;
    const unsigned int clearPos =12;
    const unsigned int wlDispatchQueuePos = 16;
    const unsigned int renderPos = 20;
    const unsigned int swapPos = 24;


    if( type ==PerformanceMarker::UPDATE_END )
    {
      if( (xPos - UpdateStartXPos) == 0 )
      {
        xPos+=2; // make sure it's at least 1 pixel wide
      }


      // draw a rounded line
      DrawLine( GetColor(type, bufferId), UpdateStartXPos, UpdateStartYPos+updatePos, xPos, updatePos+yPos );
      DrawLine( GetColor(type, bufferId), UpdateStartXPos, UpdateStartYPos+updatePos+1, xPos, updatePos+yPos+1 );
      return;
    }
    if( type ==PerformanceMarker::RENDER_END )
    {
      if( (xPos - RenderStartXPos) == 0 )
       {
         xPos+=2; // make sure it's at least 1 pixel wide
       }

      // draw a rounded line
      DrawLine( GetColor(type, bufferId),  RenderStartXPos, renderPos+RenderStartYPos, xPos, renderPos+yPos );
      DrawLine( GetColor(type, bufferId),  RenderStartXPos, renderPos+RenderStartYPos+1, xPos, renderPos+yPos +1);
      return;
    }

    if( type ==PerformanceMarker::SWAP_END )
    {
      if( (xPos - SwapStartXPos) == 0 )
       {
         xPos+=1; // make sure it's at least 1 pixel wide
       }


      // draw a thinner line for swap buffers
      DrawLine( GetColor(type, bufferId), SwapStartXPos, swapPos+SwapStartYPos, xPos, swapPos+SwapStartYPos );
      DrawLine( GetColor(type, bufferId), SwapStartXPos, swapPos+SwapStartYPos+1, xPos, swapPos+SwapStartYPos+1 );


      return;
    }
    if( type ==PerformanceMarker::WL_DISPATCH_QUEUE_END )
    {
      if( (xPos - wlDispatchQueueStartXPos) == 0 )
       {
         xPos+=1;
       }

      DrawLine( GetColor(type, bufferId), wlDispatchQueueStartXPos, wlDispatchQueuePos+wlDispatchQueueStartYPos, xPos, wlDispatchQueuePos+yPos );
      DrawLine( GetColor(type, bufferId), wlDispatchQueueStartXPos, wlDispatchQueuePos+wlDispatchQueueStartYPos+1, xPos, wlDispatchQueuePos+yPos+1 );


      return;
    }
    if( type ==PerformanceMarker::GL_CLEAR_END )
    {
      RenderStartXPos = xPos;
      RenderStartYPos = yPos;

      if( (xPos - glClearStartXPos) == 0 )
       {
         xPos+=1; // use 2 pixels minimum
       }

      // draw a thinner line for swap buffers
      DrawLine( GetColor(type, bufferId), glClearStartXPos, clearPos+glClearStartYPos, xPos, clearPos+yPos );
      DrawLine( GetColor(type, bufferId), glClearStartXPos, clearPos+glClearStartYPos+1, xPos, clearPos+yPos+1 );
      return;
    }
}

void Profiler::ClearImage()
{
  //fill the buffer
  for( unsigned int i = 0; i < GRAPH_BYTE_COUNT; i+=4)
  {
    mPixelBuffer[i] = 0x0;
    mPixelBuffer[i+1] = 0x0;
    mPixelBuffer[i+2] = 0x0;
    mPixelBuffer[i+3] = 0xFF;
  }

  // Draw the timelines
  for( unsigned int y = 0; y < GRAPH_HEIGHT; y+=TIMELINE_HEIGHT)
  {
    for( unsigned int x =0; x< GRAPH_WIDTH; x++ )
    {
      PlotPixel(0xFF00FFFF,x,y );
    }
  }
  printf(" clear image\n");


  // buffer updated
  mGraph.Update();
}
void Profiler::UpdateGraph()
{
  static PerformanceMarker::FrameTime startTime(0,0,0,0);

  if( mTimeMarkers.empty() )
  {
    return;
  }


  if( mGraphInitialised == false )
  {
    PerformanceMarker marker = *(mTimeMarkers.begin());
    startTime =  marker.mTimeStamp;
    mGraphInitialised = true;
    // wait for a v-sync
  }



  // we have a list of time markers....
  // plot marker at a time
  for( MarkerList::iterator i = mTimeMarkers.begin(); i != mTimeMarkers.end();  i++)
  {
    PerformanceMarker marker = (*i) ;

    PerformanceMarker::FrameTime diffTime = PerformanceMarker::FrameTime::Diff( startTime, marker.mTimeStamp);

    // diffTime is the difference in time from start time, to the current marker time
    float secondsDiff = diffTime.GetTime();

    //printf(" second diff %f \n",secondsDiff);
    unsigned int xPos,yPos;

    GetTimeXYPosition(secondsDiff, xPos, yPos );

    if( yPos > GRAPH_HEIGHT )
    {
      // start again
      ClearImage();
      mGraphInitialised = false;
      return;
    }

    PlotMarker( marker.mType,marker.mTimeStamp.mBuffer, xPos,yPos );
  }

  mTimeMarkers.clear();

  // update the image
  mGraph.Update();
}


bool Profiler::Connect( const std::string& address, unsigned int port )
{
  if(! mConnected )
  {
    mConnected = mProfileClient.Connect( address, port );
  }
  Start();
  return mConnected;
}

Dali::BufferImage Profiler::GetGraph()
{
  return mGraph;
}



