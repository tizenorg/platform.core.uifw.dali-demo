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
#include "profile-client.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>


// INTERNAL INCLUDES
#include "performance-marker.h"


namespace
{
const unsigned int RECIEVED_BUFFER_LENGTH = 1024; // 32KB recieve buffer
const unsigned char START_CMD[] = "set_marker 255\n"; // 255 = 0xFF all  marker bits set
const unsigned char STOP_CMD[] = "Stop";

}


ProfileClient::ProfileClient()
:mSocketFileDescriptor( -1 ),
 mConnected(false)
{
  mRxBuffer = new char[ RECIEVED_BUFFER_LENGTH ];
}

ProfileClient::~ProfileClient()
{
  delete []mRxBuffer;
}

bool ProfileClient::Connect( const std::string& address, unsigned int port )
{

  struct sockaddr_in serverAddress; // IP address of local host

  bzero(&serverAddress, sizeof(serverAddress));

  char   server[30] = {0};
  strcpy(server, address.c_str() );


  mSocketFileDescriptor = socket( AF_INET,        // Internet Protocol address
                                  SOCK_STREAM,    // required by TCP
                                  IPPROTO_TCP);   // TCP

  if( mSocketFileDescriptor  == -1 )
  {
    printf("Client : Socket() failed %s:%d\n", address.c_str(), port);
    DALI_ASSERT_ALWAYS(0 && " failed to open socket");
  }
  else
  {
    printf("Client : Socket() successful %s:%d\n", address.c_str(), port);
  }

  socklen_t recieveBufferSize = RECIEVED_BUFFER_LENGTH;
  setsockopt(mSocketFileDescriptor, SOL_SOCKET, SO_RCVBUF, &recieveBufferSize, (int)sizeof(recieveBufferSize));

  serverAddress.sin_family = AF_INET;

  serverAddress.sin_port = htons( port );  //  host-to-net short (16-bit) translation

  printf(" server address = %s \n", server );
  serverAddress.sin_addr.s_addr = inet_addr(server);

  int ret = connect( mSocketFileDescriptor,
                  (struct sockaddr* ) &serverAddress,
                  sizeof(serverAddress));
  if( ret < 0)
  {
     DALI_ASSERT_ALWAYS(0 && "socket connect failed");
  }
  else
  {
    printf("Client : connect() successful\n");
  }

  SendData(START_CMD,sizeof( START_CMD)-1); // Don't sent the null terminator

  mConnected = true;
  return true;

}

void ProfileClient::SendData( const unsigned char* Data, unsigned int DataLength  )
{
  // send the data to the client
  int ret = send(mSocketFileDescriptor,
                   Data,
                   DataLength ,0 );
  printf("Client: Sent %d bytes, send API returned %d \n",DataLength, ret);

  if (ret < 0)
  {
    DALI_ASSERT_ALWAYS(0 && "sendto() failed");
 }
}

void ProfileClient::ReadSocketData( unsigned int& length )
{
  length = 0;

  if( ! mConnected)
  {
    return;
  }
  bzero( mRxBuffer, RECIEVED_BUFFER_LENGTH );


  // perform a blocking read from the socket
  int bytesRead = recv( mSocketFileDescriptor,
                        mRxBuffer,
                        RECIEVED_BUFFER_LENGTH, MSG_DONTWAIT);


  printf(" trying to read %d bytes: Read %d \n", RECIEVED_BUFFER_LENGTH, bytesRead );
  if( bytesRead < 0 )
  {
    length = 0;
    //  perror("read ");
    return;
  }

  mRxBuffer[bytesRead]=0;

  length = bytesRead;
  return;
}

void ProfileClient::ProcessCmd( const char* cmdStart,  MarkerList& markers  )
{
  unsigned int seconds;
  unsigned int microseconds;

  char cmdString[128];

  // Make sure this format matches the format in NetworkPerformanceClient::TransmitMarker()
  int numItems = sscanf( cmdStart,"%d.%d (seconds), %s", &seconds, &microseconds, cmdString);

  if( numItems != 3)
  {
    printf(" not a marker \n");
    return;
  }

  PerformanceMarker::MarkerType type = PerformanceMarker::GetMarkerType( cmdString );

  if( type == PerformanceMarker::UNKNOWN )
  {
    printf("unknown marker%s \n", cmdString);
    return;
  }

  PerformanceMarker::FrameTime frameTime( 0 /*Frame ignored*/, seconds, microseconds, 0 /*buffer ignored */);
  PerformanceMarker marker( type, frameTime );

  markers.push_back( marker );

  printf(" command = %s, time stamp = %d.%d \n", cmdString, seconds, microseconds);

}

void ProfileClient::GetTimeMarkers( MarkerList& markers )
{
  unsigned int length(0);

 // printf("reading socket data \n");
  ReadSocketData( length );

  if( length == 0)
  {
    return;
  }

   // this data will contain 1 or more strings of
  for( unsigned int i = 0; i < length; i++ )
  {
    const char* cmdStart = &mRxBuffer[i];
    const char* cmdEnd= &mRxBuffer[i];
    for( ; *cmdEnd !='\n' && i < length ; cmdEnd++, i++ )
    {
    }
    if( i < length )
    {
      ProcessCmd( cmdStart, markers);
    }
  }

}


