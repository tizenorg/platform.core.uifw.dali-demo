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

#include <math.h>

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

const char * const WORLDMAP_IMAGE( DALI_IMAGE_DIR "worldmap.png" );

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication(application)
  , mRadius(150.f)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  Mesh CreateMesh()
  {
    // We create the meshdata for the metaballs
    Stage stage = Stage::GetCurrent();
    //Size stageSize = stage.GetSize();

    //float aspect = (float)stageSize.y / (float)stageSize.x;

    // Create vertices and specify their color
    float fPI   = (float) M_PI;
    int rings   = 50;
    int sectors = 50;
    float R     = 1.0f / (float) (rings - 1);
    float S     = 1.0f / (float) (sectors - 1);

    MeshData::VertexContainer vertices(rings * sectors);

    for (int r = 0; r < rings; r++)
    {
      for (int s = 0; s < sectors; s++)
      {
        float y = (float) sinf(-fPI * 0.5f + fPI * r * R);
        float x = (float) cosf(2 * fPI * s * S) * (float) sinf(fPI * r * R);
        float z = (float) sinf(2 * fPI * s * S) * (float) sinf(fPI * r * R);

        vertices[r * sectors + s] = MeshData::Vertex(Vector3(x * mRadius * -1, y * mRadius * -1, z * mRadius), // x, y, z
                                                     Vector2((float) s * S, (float) r * R), // u, v
                                                     Vector3(x, y, z)); // nx, ny, nz

        //latlong.put(2 * fPI * s * S - fPI);
        //latlong.put(fPI * r * R - fPI/2);
      }
    }

    MeshData::FaceIndices faces(rings * sectors * 6);
    for (int r = 0; r < rings - 1; r++)
    {
      for (int s = 0; s < sectors - 1; s++)
      {
        faces.push_back((short) (r * sectors + s));
        faces.push_back((short) (r * sectors + (s + 1)));
        faces.push_back((short) ((r + 1) * sectors + s));

        faces.push_back((short) ((r + 1) * sectors + s));
        faces.push_back((short) (r * sectors + (s + 1)));
        faces.push_back((short) ((r + 1) * sectors + (s + 1)));
      }
    }

    Material baseMaterial = Material::New("Material1");
    baseMaterial.SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    mWorldmapImage = ResourceImage::New(WORLDMAP_IMAGE);
    baseMaterial.SetDiffuseTexture(mWorldmapImage);

    // Create the mesh data from the vertices and faces
    MeshData meshData;
    meshData.SetMaterial( baseMaterial );
    meshData.SetVertices( vertices );
    meshData.SetFaceIndices( faces );
    meshData.SetHasTextureCoords( true );
    meshData.SetHasNormals( true );

    // Create a mesh from the data
    Dali::Mesh mesh = Mesh::New( meshData );
    return mesh;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor(Color::BLACK);

    Actor actor = MeshActor::New(CreateMesh());
    actor.SetName("mesh actor");
    actor.SetParentOrigin(ParentOrigin::CENTER);
    actor.SetAnchorPoint(AnchorPoint::CENTER);
    actor.SetScale(1.0f);
    //actor.SetSize(100.f, 100.f);
    stage.Add(actor);

    Animation anim = Animation::New(5.f);
    anim.SetLooping(true);
    anim.AnimateBy(Property(actor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360.f)), Vector3::YAXIS));
    anim.Play();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;

  float         mRadius;
  Image         mWorldmapImage;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
