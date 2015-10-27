/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>

#define NUM_OF_ACTORS 300
#define NUM_OF_HIDE_ACTOR 5000
#define ACTOR_WIDTH  25
#define ACTOR_HEIGHT 25
#define SPACING 5

#define USE_32BIT
#define USE_CONTROL

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to create and display Hello World! using a simple TextActor
//
class TestFps : public ConnectionTracker
{
public:

  TestFps( Application& application )
  : mApplication( application )
  {
    std::cout << "TestFps::TestFps" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TestFps::Create );
  }

  ~TestFps()
  {
    // Nothing to do here;
  }

  Actor CreateActor(int i, bool isShow)
  {

#ifdef USE_CONTROL
    Vector4 color((float)i/NUM_OF_ACTORS,.5,.8, 1);

    Control actor = Control::New();
    actor.SetSize(ACTOR_WIDTH, ACTOR_HEIGHT);
    actor.SetBackgroundColor(color);
#else
    Vector3 color((float)i/NUM_OF_ACTORS,.5,.8);

    MeshData::VertexContainer vertices(4);
    vertices[0] = MeshData::Vertex(Vector3(0,0,0), Vector2::ZERO, color);
    vertices[1] = MeshData::Vertex(Vector3(ACTOR_WIDTH,0,0), Vector2::ZERO, color);
    vertices[2] = MeshData::Vertex(Vector3(0,ACTOR_HEIGHT,0), Vector2::ZERO, color);
    vertices[3] = MeshData::Vertex(Vector3(ACTOR_WIDTH,ACTOR_HEIGHT,0), Vector2::ZERO, color);

    MeshData::FaceIndices faces;
    faces.reserve(6);
    faces.push_back(0); faces.push_back(3); faces.push_back(1);
    faces.push_back(0); faces.push_back(2); faces.push_back(3);

    MeshData meshData;
    meshData.SetMaterial(Material::New("Material"));
    meshData.SetVertices(vertices);
    meshData.SetFaceIndices(faces);
    meshData.SetHasColor(true);

    MeshActor actor = MeshActor::New(Mesh::New(meshData));
    actor.SetAffectedByLighting(false);
    //actor.SetAnchorPoint(Vector3(1,1,.5));
#endif

    actor.SetVisible(isShow);

    return actor;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "TestFps::Create" << std::endl;

    Stage stage = Stage::GetCurrent();
    //stage.SetSize(1920, 1080);
    stage.SetBackgroundColor(Vector4(1,1,1,1));

    stage.GetRootLayer().TouchedSignal().Connect( this, &TestFps::OnTouch );

    int numPerRow = ((int)(stage.GetSize().x/(ACTOR_WIDTH+SPACING)));

    for(int i=0; i<NUM_OF_ACTORS; i++)
    {
      Actor actor = CreateActor(i,true);
      actor.SetPosition(Vector3((i%numPerRow)*(ACTOR_WIDTH+SPACING),
                                (i/numPerRow)*(ACTOR_WIDTH+SPACING), 0));
      stage.Add(actor);
    }

    for(int i=0; i<NUM_OF_HIDE_ACTOR; i++)
    {
      Actor actor = CreateActor(i,false);
      actor.SetPosition(Vector3((i%numPerRow)*(ACTOR_WIDTH+SPACING),
                                (i/numPerRow)*(ACTOR_WIDTH+SPACING), 0));
      stage.Add(actor);
    }

    Animation rotAni = Animation::New(4.);
    rotAni.SetLooping(true);
    //rotAni.RotateBy(stage.GetRootLayer().GetChildAt(100), Degree(360), Vector3(0,0,1));
    rotAni.Play();
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    //std::cout<<Stage::GetCurrent().GetRootLayer().GetChildAt(100).GetCurrentAnchorPoint()<<std::endl;
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  TestFps test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
#ifdef USE_32BIT
  Application application = Application::New( &argc, &argv, "test-transparent-tv-example", Application::TRANSPARENT );
#else
  Application application = Application::New( &argc, &argv );
#endif

  RunTest( application );

  return 0;
}
