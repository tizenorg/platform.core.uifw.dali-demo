#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <iostream>
#include <fstream>

using namespace Dali;
using namespace std;
using Dali::Toolkit::TextLabel;

namespace
{
const char * const MODEL_FILE[] = {
    DEMO_MODEL_DIR "Dino.obj",						//0, normal
    DEMO_MODEL_DIR "ToyRobot-Metal.obj",			//1, normal
    DEMO_MODEL_DIR "Toyrobot-Plastic.obj",			//2, normal
    DEMO_MODEL_DIR "Cube.obj",						//3, textureless
    DEMO_MODEL_DIR "ToyRobot-Metal-Copy.obj",		//4, incomplete
    DEMO_MODEL_DIR "Dino-Copy.obj",					//5, textureless
    DEMO_MODEL_DIR "ToyRobot-Plastic-Copy.obj",		//6, normal-less
    DEMO_MODEL_DIR "Sphere.obj"						//7, texture- and normal-less
};

const char * const MATERIAL_FILE[] = {
    DEMO_MODEL_DIR "Dino.mtl",						//0
    DEMO_MODEL_DIR "ToyRobot-Metal.mtl",			//1
    DEMO_MODEL_DIR "Toyrobot-Plastic.mtl",			//2
    DEMO_MODEL_DIR "Dino-No-Bump.mtl",				//3
    DEMO_MODEL_DIR "Dino-No-Texture.mtl"			//4
};

const char * const IMAGE_PATH( DEMO_IMAGE_DIR "" );

const char * const RENDERER_TYPE_NAME( "rendererType" );

//#define MAKE_SHADER(A)#A
//
//#if 1
//const char* VERTEX_SHADER = MAKE_SHADER(
//attribute mediump vec3    aPosition;
//attribute mediump vec3    aColor;
//uniform   mediump mat4    uMvpMatrix;
//uniform   mediump vec3    uSize;
//uniform   mediump float   uDelta;
//uniform   lowp    vec4    uColor;
//varying   lowp    vec4    vColor;
////vertexPosition.xyz *= uSize;
//void main()
//{
//  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
//  vertexPosition.xyz *= 500.0f;
//  vertexPosition = uMvpMatrix * vertexPosition;
//  gl_Position = vertexPosition;
//  vColor = vec4(vertexPosition.x / 500.0f, vertexPosition.y / 500.0f, vertexPosition.z / 500.0f, 0.0) * uColor;
//}
//);
//#endif
//
//const char* FRAGMENT_SHADER = MAKE_SHADER(
//varying   lowp    vec4    vColor;
//
//void main()
//{
//  gl_FragColor = vColor;
//}
//);

} // End namespace

// Creates and displays a cube.
class Model3dLoader : public ConnectionTracker
{
public:

	Model3dLoader( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &Model3dLoader::Create );
  }

  ~Model3dLoader()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    //mObjLoader = new ObjLoader();

    LoadModel();

    //TextLabel textLabel = TextLabel::New( "Hello World" );
    //textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    //textLabel.SetName( "helloWorldLabel" );
    //stage.Add( textLabel );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &Model3dLoader::OnTouch );
  }

//  void LoadGeometry(const char * url)
//  {
//    streampos fileSize;
//    char * fileContent;
//
//    ifstream file( url, ios::in|ios::ate);
//
//    if ( file.is_open() )
//    {
//      //Read data from file
//      fileSize = file.tellg();
//      fileContent = new char[fileSize];
//      file.seekg( 0, ios::beg );
//      file.read( fileContent, fileSize );
//      file.close();
//
//      mObjLoader.ClearArrays();
//
//      //Load data into obj (usable) form
//      string materialUrl;
//      mObjLoader.Load(fileContent, fileSize, materialUrl);
//
//      //Get size information from the obj loaded
//      mSceneCenter = mObjLoader.GetCenter();
//      mSceneSize = mObjLoader.GetSize();
//    }
//    else
//    {
//      //Error
//    }
//  }

  //Performs the steps necessary to load a 3D model and place it on the stage.
  void LoadModel()
  {
    Stage stage = Stage::GetCurrent();

    //Set up 3D layer to place object on.
    Layer layer = Layer::New();
    layer.SetParentOrigin( ParentOrigin::CENTER );
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    layer.SetBehavior( Layer::LAYER_3D );
    stage.Add( layer );

    //Load the geometry object from file.
//    LoadGeometry(MODEL_FILE);
//    Geometry mesh = mObjLoader.CreateGeometry(Toolkit::Model3dView::DIFFUSE_WITH_NORMAL_MAP);

    //Set up a control renderer to use.
    Toolkit::RendererFactory rendererFactory = Toolkit::RendererFactory::Get();
    Toolkit::ControlRenderer meshRenderer = rendererFactory.GetControlRenderer(MODEL_FILE[3], MATERIAL_FILE[1], IMAGE_PATH, ImageDimensions(200, 200));

    //Create actor to house renderer.
    Actor meshActor = Actor::New();
//    meshActor.AddRenderer( renderer );
    meshRenderer.SetOnStage(meshActor);
    meshActor.SetSize(300, 300);
    meshActor.SetParentOrigin( ParentOrigin::CENTER );
    meshActor.SetAnchorPoint( AnchorPoint::CENTER );
    layer.Add( meshActor );

    //Make actor spin to demonstrate 3D.
    Animation rotationAnimation = Animation::New(15.f);
    rotationAnimation.AnimateBy(Property(meshActor, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    rotationAnimation.SetLooping(true);
    rotationAnimation.Play();
  }

  //Creates a cube.
  Geometry CreateGeometry()
  {
    // Create vertices
    struct Vertex { Vector3 position; };

#if 0
    Vertex v1 = { Vector3( 0.0f,  0.0f, 0.0f ) };
    Vertex v2 = { Vector3( 0.5f,  0.0f, 0.0f ) };
    Vertex v3 = { Vector3( 0.5f,  0.5f, 0.0f ) };
    Vertex v4 = { Vector3( 0.0f,  0.5f, 0.0f ) };
    Vertex v5 = { Vector3( 0.3f, -0.2f, 0.5f ) };
    Vertex v6 = { Vector3( 0.8f, -0.2f, 0.5f ) };
    Vertex v7 = { Vector3( 0.8f,  0.3f, 0.5f ) };
    Vertex v8 = { Vector3( 0.3f,  0.3f, 0.5f ) };
#else
    Vertex v1 = { Vector3( 0.0f,  0.0f,  0.0f ) };
    Vertex v2 = { Vector3( 0.5f,  0.0f,  0.0f ) };
    Vertex v3 = { Vector3( 0.5f,  0.5f,  0.0f ) };
    Vertex v4 = { Vector3( 0.0f,  0.5f,  0.0f ) };
    Vertex v5 = { Vector3( 0.0f,  0.0f, -0.5f ) };
    Vertex v6 = { Vector3( 0.5f,  0.0f, -0.5f ) };
    Vertex v7 = { Vector3( 0.5f,  0.5f, -0.5f ) };
    Vertex v8 = { Vector3( 0.0f,  0.5f, -0.5f ) };
#endif
    Vertex red =   { Vector3( 1.0, 0.0, 0.0 ) };
    Vertex green = { Vector3( 0.0, 1.0, 0.0 ) };
    Vertex blue =  { Vector3( 0.0, 0.0, 1.0 ) };

    Vertex cubeVertexData[] = {

      //Rear face
      v6, v5, v8, v6, v8, v7,

      //Bottom face
      v4, v3, v7, v4, v7, v8,

      //Left face
      v5, v1, v4, v5, v4, v8,

      //Right face
      v2, v6, v7, v2, v7, v3,

      //Top face
      v1, v5, v6, v6, v2, v1,

      //Front face
      v1, v2, v3, v1, v3, v4
    };

    Vertex colours[] = {
      //Rear face
      red, red, red, red, red, red,

      //Bottom face
      blue, blue, blue, blue, blue, blue,

      //Left face
      green, green, green, green, green, green,

      //Right face
      green, green, green, green, green, green,

      //Top face
      blue, blue, blue, blue, blue, blue,

      //Front face
      red, red, red, red, red, red
    };

    Property::Map cubeVertexFormat;
    cubeVertexFormat["aPosition"] = Property::VECTOR3;
    PropertyBuffer cubeVertices = PropertyBuffer::New( cubeVertexFormat );
    cubeVertices.SetData( cubeVertexData, 36 );

    Property::Map colorVertexFormat;
    colorVertexFormat["aColor"] = Property::VECTOR3;
    PropertyBuffer colorVertices = PropertyBuffer::New( colorVertexFormat );
    colorVertices.SetData( colours, 36 );

    // Create the geometry object
    Geometry cubeGeometry = Geometry::New();
    cubeGeometry.AddVertexBuffer( cubeVertices );
    cubeGeometry.AddVertexBuffer( colorVertices );
    //cubeGeometry.SetGeometryType( Geometry::LINES );

    cubeGeometry.SetRequiresDepthTesting(true);

    return cubeGeometry;
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  //ObjLoader mObjLoader;
  Vector3 mSceneCenter;
  Vector3 mSceneSize;
};

void RunTest( Application& application )
{
	Model3dLoader test( application );

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
