#ifndef PROFILE_CLIENT
#define PROFILE_CLIENT

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

// EXTERNAL INCLUDES
#include <string>
#include <arpa/inet.h>

// INTERNAL INCLUDES
#include "performance-marker.h"

class ProfileClient
{


public:

  ProfileClient();

  ~ProfileClient();

  bool Connect( const std::string& address, unsigned int port  );

  void Disconnect();

  void GetTimeMarkers( MarkerList& markers );


private:

  void SendData( const unsigned char* Data, unsigned int DataLength  );


  void ReadSocketData(  unsigned int& length );

  void ProcessCmd( const char* cmdStart, MarkerList& markers);


  int                 mSocketFileDescriptor;
  struct sockaddr_in  mClientAddress;
  bool                mConnected;
  char*               mRxBuffer;

};
#endif
