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


#include <dali.h>
#include "profile-client.h"

/**
 *
 * Connects to DALi via a TCP socket and processes time stamps information,
 * and renderes it to a bitmap image for display
 *
 */
class Profiler : public Dali::ConnectionTracker
{


public:

  Profiler();

  bool Connect( const std::string& address, unsigned int port  );

  void Start();

  void Stop();

  Dali::BufferImage GetGraph();

private:

  bool TimerCallback();

  void UpdateGraph();

  void ClearImage();

  void PlotPixel( unsigned int color, unsigned int xPos,unsigned int yPos );
  void PlotMarker(PerformanceMarker::MarkerType type, unsigned int bufferId, unsigned int xPos,unsigned int yPos );
  unsigned int GetColor(PerformanceMarker::MarkerType type, unsigned int bufferId );

  void DrawLine(  unsigned int color,
                                  unsigned int startX,
                                  unsigned int startY,
                                  unsigned int endX,
                                  unsigned int endY );
  MarkerList mTimeMarkers;

  ProfileClient mProfileClient;
  Dali::BufferImage mGraph;
  Dali::Timer mTimer;
  unsigned char* mPixelBuffer;
  bool mConnected;
  bool mGraphInitialised;

};
