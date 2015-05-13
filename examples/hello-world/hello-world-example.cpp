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

#if 0
static final String VERTEX_SHADER =
            "attribute vec3 SGPositions;\n"+
            "attribute vec3 SGNormals;\n"+
            "attribute vec2 SGTextureCoords;\n"+
            "attribute vec2 LatLong;"+

            "uniform mat4 MyWorldViewProjection;\n"+
            "uniform vec3 viewDirec;\n"+
            "uniform vec2 timezoneLatLong;"+

            "varying vec2 vTextureCoords1;\n"+
            "varying vec2 vTextureCoords2;\n"+
            "varying vec3 normal;"+
            "varying float brightness;"+
            "varying float inRangeForTZHighlight;"+

            "void main(){\n" +
            "   vTextureCoords1 = vec2(SGTextureCoords.x, SGTextureCoords.y/2.0);\n"+
            "   vTextureCoords2 = vec2(vTextureCoords1.x, vTextureCoords1.y+0.5);\n"+
            "   gl_Position = MyWorldViewProjection * vec4(SGPositions, 1.0);\n"+
            "   normal = SGNormals;"+
            "   brightness = pow(1.0 - dot(viewDirec, SGPositions), 3.0);"+

            "	float angleDelta = mod(abs(timezoneLatLong.x - LatLong.x), 6.28);"+
            "	inRangeForTZHighlight = step(3.14/2.0, min(angleDelta, 6.28-angleDelta));"+
            "}\n";

    static final String FRAGMENT_SHADER =
            "precision mediump float;\n" +

            "uniform sampler2D dayTexture;\n" +
            "uniform vec3 lightDirec;\n"+
            "uniform vec3 timezoneColorUpper;"+
            "uniform vec3 timezoneColorLower;"+
            "uniform float timezoneVisibility;"+
            "uniform float cameraDistance;"+
            "uniform float overlayCrossOver;"+

            "varying vec2 vTextureCoords1;\n" +
            "varying vec2 vTextureCoords2;\n"+
            "varying vec3 normal;"+
            "varying float brightness;"+
            "varying float inRangeForTZHighlight;"+

            "void main(){\n" +
            "   vec4 dayColorUpper = texture2D(dayTexture, vTextureCoords1);\n" +
            "   vec4 dayColorLower = texture2D(dayTexture, vTextureCoords2);\n" +
            "   vec3 stepUpper = step(0.5, dayColorUpper.rgb);"+
            "   vec3 stepLower = step(0.5, dayColorLower.rgb);"+

            "   vec3 dayColorUpperQuantized = abs(stepUpper - timezoneColorUpper);\n" +
            "   vec3 dayColorLowerQuantized = abs(stepLower - timezoneColorLower);\n" +
            "   float flag = clamp(inRangeForTZHighlight + dayColorUpperQuantized.r+dayColorUpperQuantized.g+dayColorUpperQuantized.b+dayColorLowerQuantized.r+dayColorLowerQuantized.g+dayColorLowerQuantized.b, 0.0, 1.0);"+

            "   float continents = clamp((stepUpper.r+stepUpper.g+stepUpper.b)*(stepLower.r+stepLower.g+stepLower.b), 0.0, 1.0);"+
            "	vec4 tempColor = continents*flag*vec4(0.96, 0.96, 0.96, 1.0) + (vec4(0.965, 0.882, 0.557, 1.0)*timezoneVisibility + continents*vec4(0.96, 0.96, 0.96, 1.0)*(1.0-timezoneVisibility))*(1.0-flag);"+
                        "	vec4 dayColor = mix(vec4(0.498, 0.765, 0.894, 1.0), tempColor, continents);"+

            "   float nightColor = smoothstep(0.2, 0.5, dot(normal, lightDirec)+0.3);"+
            "   float darkOverlayFactor = 0.0;"+
            "   if(cameraDistance < overlayCrossOver){"+
            "       darkOverlayFactor = 1.0 - smoothstep(overlayCrossOver-0.5, overlayCrossOver, cameraDistance);"+
            "   }"+
            "   nightColor +=darkOverlayFactor;"+
            "   nightColor = clamp(nightColor, 0.0, 1.0);"+
            "   gl_FragColor = vec4(dayColor.rgb*(0.8 + 0.2*nightColor) + brightness*nightColor, 1.0);"+
            "}\n";
#endif

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

  Dali::ShaderEffect NewEffect()
  {
    std::string vertexShader = DALI_COMPOSE_SHADER(
      precision highp float;

      varying vec2  vTextureCoords1;
      varying vec2  vTextureCoords2;
      varying float vBrightness;

      void main()\n
      {
        vTextureCoords1 = vec2(aTexCoord.x, aTexCoord.y / 2.0);
        vTextureCoords2 = vec2(vTextureCoords1.x, vTextureCoords1.y + 0.5);

        mediump vec3 viewDirec = vec3(0.0, 0.0, 1.0);
        mediump vec3 normalPos = normalize(aPosition.xyz);
        vBrightness = pow(1.0 - dot(viewDirec, normalPos), 1.0);

        gl_Position = uMvpMatrix * vec4(aPosition.xyz, 1.0);
        //vNormal = aNormal;
        vNormal = uModelViewIT * aNormal;
      }
   );

    std::string fragmentShader = DALI_COMPOSE_SHADER(
      precision highp float;

      uniform vec3  timezoneColorUpper;
      uniform vec3  timezoneColorLower;

      varying vec2  vTextureCoords1;
      varying vec2  vTextureCoords2;
      varying float vBrightness;

      void main()
      {
        float inRangeForTZHighlight = 0.0;
        float timezoneVisibility = 0.0;

        mediump vec4 dayColorUpper = texture2D(sTexture, vTextureCoords1);
        mediump vec4 dayColorLower = texture2D(sTexture, vTextureCoords2);
        mediump vec3 stepUpper = step(0.5, dayColorUpper.rgb);
        mediump vec3 stepLower = step(0.5, dayColorLower.rgb);

        mediump vec3 dayColorUpperQuantized = abs(stepUpper - timezoneColorUpper);
        mediump vec3 dayColorLowerQuantized = abs(stepLower - timezoneColorLower);
        float flag = clamp(inRangeForTZHighlight + dayColorUpperQuantized.r+dayColorUpperQuantized.g+dayColorUpperQuantized.b+dayColorLowerQuantized.r+dayColorLowerQuantized.g+dayColorLowerQuantized.b, 0.0, 1.0);

        float continents = clamp((stepUpper.r+stepUpper.g+stepUpper.b)*(stepLower.r+stepLower.g+stepLower.b), 0.0, 1.0);
        mediump vec4 tempColor = continents*flag*vec4(0.96, 0.96, 0.96, 1.0) + (vec4(0.965, 0.882, 0.557, 1.0)*timezoneVisibility + continents*vec4(0.96, 0.96, 0.96, 1.0)*(1.0-timezoneVisibility))*(1.0-flag);
        mediump vec4 dayColor = mix(vec4(0.498, 0.765, 0.894, 1.0), tempColor, continents);

        mediump vec3 lightDirec = vec3(0.0, 0.0, 1.0);
        float nightColor = smoothstep(0.2, 0.5, dot(vNormal, lightDirec) + 0.3);
        nightColor = clamp(nightColor, 0.0, 1.0);
        nightColor = nightColor * 0.57;

        //gl_FragColor = vec4(dayColor.rgb + vBrightness, uColor.a*dayColor.r);
        //gl_FragColor = vec4(nightColor.rgb + vBrightness, uColor.a*nightColor.r);
        gl_FragColor = vec4(dayColor.rgb*(0.8 + 0.2*nightColor) + vBrightness*nightColor, 1.0);
      }
    );

    Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::New( vertexShader, fragmentShader,
                                                         Dali::GeometryType( Dali::GEOMETRY_TYPE_TEXTURED_MESH ),
                                                         Dali::ShaderEffect::GeometryHints( Dali::ShaderEffect::HINT_NONE ) );
    return shaderEffect;
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
        float y = (float) sinf(-fPI * 0.5f + fPI * r * R) * -1.f;
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

    MeshActor actor = MeshActor::New(CreateMesh());
    actor.SetName("mesh actor");
    actor.SetParentOrigin(ParentOrigin::CENTER);
    actor.SetAnchorPoint(AnchorPoint::CENTER);
    actor.SetScale(1.0f);
    //actor.SetSize(100.f, 100.f);
    stage.Add(actor);

    ShaderEffect effect = NewEffect();
    actor.SetShaderEffect(effect);

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
