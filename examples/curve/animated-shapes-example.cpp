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

#define MAKE_STRING(A)#A

using namespace Dali;

namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-gradient.jpg" );


std::string vertexShader = MAKE_STRING(
  uniform highp vec3 uPoints[7];\n
  highp mat4 m3 = mat4( 1.0, -3.0, 3.0, -1.0, 0.0, 3.0, -6.0, 3.0, 0.0, 0.0, 3.0, -3.0, 0.0, 0.0, 0.0, 1.0 );\n
  highp mat4 m3Inverse = mat4( 1.0, 1.0, 1.0, 1.0, 0.0, 1.0/3.0, 2.0/3.0, 1.0, 0.0, 0.0, 1.0/3.0, 1.0, 0.0, 0.0, 0.0, 1.0 );\n
  highp mat4 OrientationReverse = mat4( -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 );\n

  struct SPoint\n
  {\n
    highp vec3 position;\n
    highp vec3 coefficients;\n
  };\n

  //Compute determinant of a 3x3 matrix
  float Determinant( in float c00, in float c01, in float c02,in float c10, in float c11, in float c12,in float c20, in float c21, in float c22)\n
  {\n
    return c00*c11*c22 + c10*c21*c02 + c20*c01*c12 - c02*c11-c20 - c12*c21*c00 - c22*c10*c01;\n
  }\n

  void ComputeCoefficients( inout SPoint points[4] )\n
  {\n
    highp mat4 controlPoints = mat4( points[0].position.x, points[1].position.x, points[2].position.x, points[3].position.x, \n
                                     points[0].position.y, points[1].position.y, points[2].position.y, points[3].position.y, \n
                                     points[0].position.z, points[1].position.z, points[2].position.z, points[3].position.z, \n
                                     1.0,          1.0,          1.0,          1.0      );\n


    highp mat4 C = m3 * controlPoints;\n
    highp float d1 = -Determinant( C[0][3],C[1][3],C[3][3],C[0][2],C[1][2],C[3][2],C[0][0],C[1][0],C[3][0] )*0.00001;\n
    highp float d2 =  Determinant( C[0][3],C[1][3],C[3][3],C[0][1],C[1][1],C[3][1],C[0][0],C[1][0],C[3][0] )*0.00001;\n
    highp float d3 = -Determinant( C[0][2],C[1][2],C[3][2],C[0][1],C[1][1],C[3][1],C[0][0],C[1][0],C[3][0] )*0.00001;\n

    highp mat4 result; \n
    highp float discriminant = 3.0*d2*d2 - 4.0*d1*d3;\n
    if( d1 != 0.0 && discriminant >= 0.0 )\n  //Serpentine or Cusp at infinity\n
    {\n
      highp float s0 = (1.0/sqrt(3.0))*sqrt(3.0*d2*d2-4.0*d1*d3);\n
      highp float lt = d2 + s0;\n
      highp float ls = 2.0*d1;\n
      highp float mt = d2 - s0;\n
      highp float ms = 2.0*d1;\n

      highp mat4 F = mat4(   lt*mt,   (-(ms*lt)-(ls*mt)),     ls*ms,          0.0,    \n
                     lt*lt*lt,  (-(3.0*ls*lt*lt)),  3.0*ls*ls*lt,  (-(ls*ls*ls)), \n
                     mt*mt*mt,  -(3.0*ms*mt*mt),   3.0*ms*ms*mt, -(ms*ms*ms), \n
                       1.0,          0.0,             0.0,            0.0 );  \n

      if( d1 < 0.0 )\n
        F = F * OrientationReverse;\n

      result = m3Inverse * F; //Change points to bernstein basis\n
    }
    else if( d1 != 0.0 && discriminant < 0.0 ) //Loop\n
    {\n

      highp float td = d2 + sqrt(4.0*d1*d3 - 3.0*d2*d2);\n
      highp float sd = 2.0*d1;\n
      highp float te = d2 - sqrt(4.0*d1*d3 - 3.0*d2*d2);\n
      highp float se = 2.0*d1;\n

      highp mat4 F = mat4(   td*te,          -(se*td)-(sd*te),              sd*se,            0.0,
                 td*td*te,  -(se*td*td)-(2.0*sd*te*td),  te*sd*sd+2.0*se*td*sd, -(sd*sd)*se,
                 td*te*te, -(sd*te*te)-(2.0*se*td*te), td*se*se + 2.0*sd*te*se, -sd*se*se,
                    1.0,             0.0,                      0.0,                0.0 );\n

      highp float h0 = 36.0 * ( d3*d1 - d2*d2);\n
      highp float h1 = 36.0 * ( ( d3*d1 - d2*d2) + d1*d2 - d1 );\n
      if( ( d1*h0 > 0.0 ) || (d1*h1 > 0.0 ) )\n
      {\n
        F = F * OrientationReverse;\n
      }\n

      result = m3Inverse * F;\n //Change points to bernstein basis\n
    }\n
    else if( d1 != 0.0 && d2 != 0.0 )  //Cusp with cusp at infinity\n
    {\n
      highp float tl = d3;\n
      highp float sl = 3.0*d2;\n
      highp mat4 F = mat4( tl,-sl,0.0,0.0,  tl*tl*tl,-(3.0*sl*tl*tl),3.0*sl*sl*tl,-(sl*sl*sl),  1.0,0.0,0.0,0.0,  1.0,0.0,0.0,0.0 );\n
      result = m3Inverse * F;\n //Change points to bernstein basis\n
    }\n
    else if( d1==0.0 && d2==0.0 && d3!=0.0 ) //Cuadratic
    {
      result = mat4(0.0, 1.0/3.0, 2.0/3.0, 1.0, 0.0, 0.0, 1.0/3.0, 1.0, 0.0, 1.0/3.0, 2.0/3.0, 1.0, 0.0, 0.0, 0.0, 0.0 );\n
      if( d3 < 0.0 )\n
        result = result * OrientationReverse;
    }

     points[0].coefficients = vec3( result[0][0],result[1][0],result[2][0]);\n
     points[1].coefficients = vec3( result[0][1],result[1][1],result[2][1]);\n
     points[2].coefficients = vec3( result[0][2],result[1][2],result[2][2]);\n
     points[3].coefficients = vec3( result[0][3],result[1][3],result[2][3]);\n
  }

  int ComputeIndex( in int index, in SPoint points[4] )\n
  {\n
    if( index == 0 )\n
      return 0;\n

    highp vec3 A = points[1].position - points[0].position;\n
    highp vec3 B = points[2].position - points[0].position;\n
    highp vec3 C = points[3].position - points[0].position;\n

    highp vec3 A0 = vec3(-A.y, A.x, A.z );\n  //Perpendicular to A
    highp vec3 B0 = vec3(-B.y, B.x, B.z );\n  //Perpendicular to B
    highp vec3 C0 = vec3(-C.y, C.x, C.z );\n  //Perpendicular to C

    ivec3 orderedIndex = ivec3(3,2,1);\n
    if( sign(dot(A0,B)) != sign(dot(A0,C) ) )\n
    {\n
      orderedIndex = ivec3(2,3,1);\n
    }\n
    else if( sign(dot(B0,A)) != sign(dot(B0,C) ) )\n
    {\n
      orderedIndex = ivec3(1,3,2);\n
    }\n
    else if( sign(dot(C0,A)) != sign(dot(C0,B) ) )\n
    {\n
      orderedIndex = ivec3(1,2,3);\n
    }\n

    return orderedIndex[index-1]\n;
  }\n

  void SplitCurve( in float t, in mat4 bezierControlPoints, out SPoint points[4] )\n
  {\n
    highp vec3 p0 = vec3(bezierControlPoints[0][0],bezierControlPoints[1][0],bezierControlPoints[2][0] );\n
    highp vec3 p1 = vec3(bezierControlPoints[0][1],bezierControlPoints[1][1],bezierControlPoints[2][1] );\n
    highp vec3 p2 = vec3(bezierControlPoints[0][2],bezierControlPoints[1][2],bezierControlPoints[2][2] );\n
    highp vec3 p3 = vec3(bezierControlPoints[0][3],bezierControlPoints[1][3],bezierControlPoints[2][3] );\n

    highp vec3 p4 = mix( p0,p1, t );\n
    highp vec3 p5 = mix( p1,p2, t );\n
    highp vec3 p6 = mix( p2,p3, t );\n
    highp vec3 p7 = mix( p4, p5, t );\n
    highp vec3 p8 = mix( p5, p6, t );\n
    highp vec3 p9 = mix( p7, p8, t );\n   //Point where the curve is splitted

    int vertexID = int(aNormal.x);
    int segment = vertexID / 8;
    int vertex = vertexID - 8*segment;
    if( vertex < 4 )\n
    {\n
      points[0].position = p0;\n
      points[1].position = p4;\n
      points[2].position = p7;\n
      points[3].position = p9;\n
    }\n
    else\n
    {\n
      points[0].position = p9;\n
      points[1].position = p8;\n
      points[2].position = p6;\n
      points[3].position = p3;\n
    }\n
  }\n

  void ComputeBezierPoints( in mat4 bezierControlPoints, out SPoint points[4] )\n
  {\n
    highp mat4 C = m3 * bezierControlPoints;\n
    highp float d1 = -Determinant( C[0][3],C[1][3],C[3][3],C[0][2],C[1][2],C[3][2],C[0][0],C[1][0],C[3][0] );\n
    highp float d2 =  Determinant( C[0][3],C[1][3],C[3][3],C[0][1],C[1][1],C[3][1],C[0][0],C[1][0],C[3][0] );\n
    highp float d3 = -Determinant( C[0][2],C[1][2],C[3][2],C[0][1],C[1][1],C[3][1],C[0][0],C[1][0],C[3][0] );\n

    highp float tSplit = 0.0;
    if( d1 != 0.0 &&  3.0*d2*d2 - 4.0*d1*d3 < 0.0 ) //Loop\n
    {
      highp float td = d2 + sqrt(4.0*d1*d3 - 3.0*d2*d2);\n
      highp float sd = 2.0*d1;\n
      highp float te = d2 - sqrt(4.0*d1*d3 - 3.0*d2*d2);\n
      highp float se = 2.0*d1;\n

      highp float d = td/sd;
      highp float e = te/se;

      tSplit = d*( step( 0.0, d) - step(1.0, d ) ) + e*( step( 0.0, e ) - step(1.0, e ) );
    }\n

    SplitCurve( tSplit, bezierControlPoints, points );\n
    ComputeCoefficients( points );\n
  }\n

  void main()\n
  {\n
    int segment = int(aNormal.x) / 8;

    int baseIndex = 3 * segment;
    highp mat4 controlPoints = mat4( uPoints[baseIndex].x, uPoints[baseIndex+1].x, uPoints[baseIndex+2].x, uPoints[baseIndex+3].x,
                                     uPoints[baseIndex].y, uPoints[baseIndex+1].y, uPoints[baseIndex+2].y, uPoints[baseIndex+3].y,
                                     uPoints[baseIndex].z, uPoints[baseIndex+1].z, uPoints[baseIndex+2].z, uPoints[baseIndex+3].z,
                                           1.0,                  1.0,                    1.0,                    1.0
                                   );\n

    SPoint points[4];\n
    ComputeBezierPoints( controlPoints, points  );\n

    int vertexInSegment = int(aNormal.x) - 8*segment;\n
    int vertexID = vertexInSegment - int( ( 4.0 * step( 3.0,float(vertexInSegment) ) ) );
    int index = ComputeIndex( vertexID, points );

    gl_Position = uMvpMatrix * vec4(points[index].position, 1.0);\n
    vNormal = points[index].coefficients;\n
  }\n
);

std::string fragmentShader = MAKE_STRING(
  uniform mediump float uLinewWidth;\n
  void main()\n
  {\n
    highp float C = (vNormal.x*vNormal.x*vNormal.x - vNormal.y*vNormal.z);\n
    highp float Cdx = dFdx(C);\n
    highp float Cdy = dFdy(C);\n

    highp float distance = abs(float(C / sqrt(Cdx*Cdx + Cdy*Cdy)));\n
    gl_FragColor = uColor*(uLinewWidth-distance);\n
  }\n
);

}

// This example shows resolution independent rendering and animation of curves using the gpu.
//
class AnimatedShapesExample : public ConnectionTracker
{
public:

  AnimatedShapesExample( Application& application )
: mApplication( application )
{
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &AnimatedShapesExample::Create );
}

  ~AnimatedShapesExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    //Create a view
    mView = Dali::Toolkit::View::New();
    stage.Add( mView );

    //Set background image for the view
    Dali::Image backgroundImage = Dali::Image::New( BACKGROUND_IMAGE );
    Dali::ImageActor backgroundImageActor = Dali::ImageActor::New( backgroundImage );
    mView.SetBackground( backgroundImageActor );

    CreateTriangleMorph(Vector3( stage.GetSize().x*0.5f,stage.GetSize().y*0.15f,0.0f), 100.0f );
    CreateCircleMorph( Vector3( stage.GetSize().x*0.5f,stage.GetSize().y*0.5f,0.0f), 60.0f );
    CreateBlobMorph( Vector3( stage.GetSize().x*0.5f,stage.GetSize().y*0.85f,0.0f), 50.0f );

    stage.KeyEventSignal().Connect(this, &AnimatedShapesExample::OnKeyEvent);
  }

  void CreateCircleMorph( Vector3 center, float radius )
  {
    mCircleShader = Toolkit::QuadraticBezier::New(16, true);

    mCircleShader.SetPoint(0, Vector3(-radius,-radius,0.0f));
    mCircleShader.SetPoint(1, Vector3( 0.0f,-radius,0.0f));
    mCircleShader.SetPoint(2, Vector3(radius,-radius,0.0f));

    mCircleShader.SetPoint(3, Vector3(radius,-radius,0.0f));
    mCircleShader.SetPoint(4, Vector3( radius,0.0f,0.0f));
    mCircleShader.SetPoint(5, Vector3(radius,radius,0.0f));

    mCircleShader.SetPoint(6, Vector3(radius,radius,0.0f));
    mCircleShader.SetPoint(7, Vector3( 0.0f,radius,0.0f));
    mCircleShader.SetPoint(8, Vector3( -radius,radius,0.0f));

    mCircleShader.SetPoint(9,  Vector3( -radius,radius,0.0f));
    mCircleShader.SetPoint(10, Vector3( -radius,0.0f,0.0f));
    mCircleShader.SetPoint(11, Vector3(-radius,-radius,0.0f));

    mCircleShader.SetPoint(12, Vector3(-radius,-radius,0.0f));
    mCircleShader.SetPoint(13, Vector3(radius,-radius,0.0f));
    mCircleShader.SetPoint(14, Vector3(radius,radius,0.0f));
    mCircleShader.SetPoint(15, Vector3( -radius,radius,0.0f));

    mCircleShader.SetColor(Vector4(1.0f,0.0f,0.0f,1.0f) );
    mCircleShader.SetLineWidth(2.0f);

    ////Generate the mesh
    Dali::MeshData::VertexContainer vertices;
    for( unsigned int i(0); i<12; i+=3 )
    {
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,0.0f,i)  ));
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.5f,0.0f,i+1)));
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(1.0f,1.0f,i+2)));
    }
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,12)  ));
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,13)));
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,14)));
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,15)));

    short unsigned int indexArray[] = { 0,2,1, 3,5,4,6,8,7, 9, 11, 10, 12,15,14,12,14,13};
    Dali::MeshData::FaceIndices index( indexArray, indexArray + sizeof(indexArray)/sizeof(short unsigned int) );

    //Material
    Dali::Material material = Material::New("Material");
    material.SetDiffuseColor( Vector4(1.0f,1.0f,1.0f,1.0f));

    //Create the Mesh object
    Dali::MeshData data;
    data.SetVertices(vertices);
    data.SetFaceIndices( index );
    data.SetMaterial( material );
    data.SetHasNormals( true );
    Mesh mesh = Mesh::New( data );

    //Create the mesh actor
    MeshActor meshActor = MeshActor::New(mesh);
    meshActor.SetAnchorPoint( AnchorPoint::CENTER );
    meshActor.SetAffectedByLighting(false);
    meshActor.SetShaderEffect(mCircleShader);
    meshActor.SetPosition( center );
    meshActor.SetBlendMode(BlendingMode::ON );
    mView.Add( meshActor );


    //Animation
    Animation animation = Animation::New(5.0f);
    KeyFrames k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( 0.0f,-radius, 0.0f) );
    k0.Add( 0.5f, Vector3(0.0f, -radius*4.0f, 0.0f));
    k0.Add( 1.0f, Vector3( 0.0f,-radius, 0.0f) );
    animation.AnimateBetween( Property(mCircleShader, mCircleShader.GetPointPropertyName(1)),k0,AlphaFunctions::EaseInOutSine );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( radius, 0.0f, 0.0f) );
    k0.Add( 0.5f, Vector3(radius*4.0f,0.0f, 0.0f));
    k0.Add( 1.0f, Vector3( radius,0.0f, 0.0f));
    animation.AnimateBetween( Property(mCircleShader, mCircleShader.GetPointPropertyName(4)),k0,AlphaFunctions::EaseInOutSine );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3(0.0f,radius, 0.0f) );
    k0.Add( 0.5f, Vector3(0.0f,radius*4.0f, 0.0f));
    k0.Add( 1.0f, Vector3(0.0f,radius, 0.0f) );
    animation.AnimateBetween( Property(mCircleShader, mCircleShader.GetPointPropertyName(7)),k0,AlphaFunctions::EaseInOutSine );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( -radius,  0.0f, 0.0f) );
    k0.Add( 0.5f, Vector3(-radius*4.0f,0.0f, 0.0f));
    k0.Add( 1.0f, Vector3( -radius,  0.0f, 0.0f) );
    animation.AnimateBetween( Property(mCircleShader, mCircleShader.GetPointPropertyName(10)),k0,AlphaFunctions::EaseInOutSine );

    animation.RotateBy(meshActor,Degree(90.0f), Vector3::ZAXIS );
    animation.SetLooping( true );
    animation.Play();
  }

  void CreateTriangleMorph( Vector3 center, float side )
  {
    float h = (side *0.5f)/0.866f;

    Vector3 v0 = Vector3(-h,h,0.0f);
    Vector3 v1 = Vector3(0.0f,-(side*0.366f),0.0f );
    Vector3 v2 = Vector3(h,h,0.0f);

    Vector3 v3 = v0 + ((v1-v0) * 0.5f);
    Vector3 v4 = v1 + ((v2-v1) * 0.5f);
    Vector3 v5 = v2 + ((v0-v2) * 0.5f);

    mTriangleShader = Toolkit::QuadraticBezier::New(12, true);

    mTriangleShader.SetPoint(0,v0);
    mTriangleShader.SetPoint(1,v3);
    mTriangleShader.SetPoint(2,v1);

    mTriangleShader.SetPoint(3,v1);
    mTriangleShader.SetPoint(4,v4);
    mTriangleShader.SetPoint(5,v2);

    mTriangleShader.SetPoint(6,v2);
    mTriangleShader.SetPoint(7,v5);
    mTriangleShader.SetPoint(8,v0);

    mTriangleShader.SetPoint(9, v0);
    mTriangleShader.SetPoint(10,v1);
    mTriangleShader.SetPoint(11,v2);

    mTriangleShader.SetColor(Vector4(0.0f,1.0f,0.0f,1.0f));
    mTriangleShader.SetLineWidth(2.0f);

    ////Generate the mesh
    Dali::MeshData::VertexContainer vertices;
    for( unsigned int i(0);i<9;i+=3 )
    {
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,0.0f,i)) );
      vertices.push_back(  MeshData::Vertex( Vector3::ZERO, Vector2::ZERO,Vector3(0.5f,0.0f,i+1) ) );
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(1.0f,1.0f,i+2)  ) );
    }

    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,9)) );
    vertices.push_back(  MeshData::Vertex( Vector3::ZERO, Vector2::ZERO,Vector3(0.0f,1.0f,10) ) );
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,11)  ) );

    short unsigned int indexArray[] = { 0,2,1,3,5,4,6,8,7,9,11,10 };
    Dali::MeshData::FaceIndices index( indexArray, indexArray + sizeof(indexArray)/sizeof(short unsigned int) );

    //Material
    Dali::Material material = Material::New("Material");
    material.SetDiffuseColor( Vector4(1.0f,1.0f,1.0f,1.0f));

    //Create the Mesh object
    Dali::MeshData data;
    data.SetVertices(vertices);
    data.SetFaceIndices( index );
    data.SetMaterial( material );
    data.SetHasNormals( true );
    Mesh mesh = Mesh::New( data );

//    //Create the mesh actor
    MeshActor meshActor = MeshActor::New(mesh);
    meshActor.SetAnchorPoint( AnchorPoint::CENTER );
    meshActor.SetAffectedByLighting(false);
    meshActor.SetShaderEffect(mTriangleShader);
    meshActor.SetPosition( center );
    meshActor.SetBlendMode(BlendingMode::ON );
    mView.Add( meshActor );

    //Animation
    Animation animation = Animation::New(5.0f);

    KeyFrames k0 = KeyFrames::New();
    k0.Add( 0.0f,v3  );
    k0.Add( 0.5f, v3 + Vector3(-200.0f,-200.0f,0.0f));
    k0.Add( 1.0f, v3 );
    animation.AnimateBetween( Property(mTriangleShader, mTriangleShader.GetPointPropertyName(1)),k0,AlphaFunctions::EaseInOutSine );

    k0 = KeyFrames::New();
    k0.Add( 0.0f,v4  );
    k0.Add( 0.5f, v4 + Vector3(200.0f,-200.0f,0.0f));
    k0.Add( 1.0f, v4 );
    animation.AnimateBetween( Property(mTriangleShader, mTriangleShader.GetPointPropertyName(4)),k0,AlphaFunctions::EaseInOutSine );

    k0 = KeyFrames::New();
    k0.Add( 0.0f,v5  );
    k0.Add( 0.5f, v5 + Vector3(0.0,200.0f,0.0f));
    k0.Add( 1.0f, v5 );
    animation.AnimateBetween( Property(mTriangleShader, mTriangleShader.GetPointPropertyName(7)),k0,AlphaFunctions::EaseInOutSine );
    animation.SetLooping( true );
    animation.Play();
  }

   void CreateBlobMorph(Vector3 center, float radius )
   {
     //Create the shader effect
     float offset = radius*4.0f/3.0f;
     mBlobShader = ShaderEffect::NewWithPrefix( "",vertexShader, "#extension GL_OES_standard_derivatives : enable\n", fragmentShader, GEOMETRY_TYPE_UNTEXTURED_MESH );
     mBlobShader.SetUniform( "uPoints[0]",Vector3(-radius,0.0f,0.0f ));
     mBlobShader.SetUniform( "uPoints[1]",Vector3(-radius,-offset,0.0f));
     mBlobShader.SetUniform( "uPoints[2]",Vector3(radius,-offset,0.0f));
     mBlobShader.SetUniform( "uPoints[3]",Vector3(radius,0.0,0.0f));
     mBlobShader.SetUniform( "uPoints[4]",Vector3(radius,offset,0.0f));
     mBlobShader.SetUniform( "uPoints[5]",Vector3(-radius,offset,0.0f));
     mBlobShader.SetUniform( "uPoints[6]",Vector3(-radius,0.0f,0.0f));
     mBlobShader.SetUniform( "uLinewWidth",2.0f);
     mBlobShader.SetUniform( "uColor",Vector4(1.0f,1.0f,0.0f,1.0f));

     ////Generate the mesh
     Dali::MeshData::VertexContainer vertices;
     for( unsigned int i(0); i!=16; ++i )
       vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(i,0.0f,0.0f) ) );

     short unsigned int indexArray[] = {0,2,1,2,3,1,4,6,5,6,7,5,8,10,9,10,11,9,12,14,13,14,15,13};
     Dali::MeshData::FaceIndices index( indexArray, indexArray + 24 );

     //Material
     Dali::Material material0 = Material::New("Material");
     material0.SetDiffuseColor( Vector4(0.0f,0.0f,0.0f,0.0f));

     //Create the Mesh Actor
     Dali::MeshData data;
     data.SetVertices(vertices);
     data.SetFaceIndices( index );
     data.SetMaterial( material0 );
     data.SetHasNormals( true );
     Mesh mesh0 = Mesh::New( data );

     MeshActor meshActor = MeshActor::New(mesh0);
     meshActor.SetAnchorPoint( AnchorPoint::CENTER );
     meshActor.SetParentOrigin( ParentOrigin::CENTER );
     meshActor.SetAffectedByLighting(false);
     Stage stage = Stage::GetCurrent();
     meshActor.SetPosition( Vector3( center.x - stage.GetSize().x * 0.5f,center.y - stage.GetSize().y * 0.5f, 0.0f) );
     meshActor.SetRotation(Degree(-90.0f), Vector3::ZAXIS);
     meshActor.SetShaderEffect(mBlobShader);
     mView.Add( meshActor );

     ///Animation
     Animation animation = Dali::Animation::New(5.0f);

     //Point 0 & 2
     KeyFrames k0 = KeyFrames::New();
     k0.Add( 0.0f, Vector3(-radius,0.0f, 0.0f ));
     k0.Add( 0.5f, Vector3(-radius-50.0f,0.0f, 0.0f ));
     k0.Add( 1.0f, Vector3(-radius,0.0f, 0.0f ));
     animation.AnimateBetween( Property(mBlobShader, "uPoints[0]"),k0,AlphaFunctions::EaseInOutSine);
     animation.AnimateBetween( Property(mBlobShader, "uPoints[6]"),k0,AlphaFunctions::EaseInOutSine);

     //Point 1
     k0 = KeyFrames::New();
     k0.Add( 0.0f, Vector3(radius,0.0f,0.0f));
     k0.Add( 0.5f, Vector3(radius+50.0f,0.0f,0.0f));
     k0.Add( 1.0f, Vector3(radius,0.0f,0.0f));
     animation.AnimateBetween( Property(mBlobShader, "uPoints[3]"),k0,AlphaFunctions::EaseInOutSine);

     //CP 1
     k0 = KeyFrames::New();
     k0.Add( 0.0f, Vector3(-radius,-offset,0.0f));
     k0.Add( 0.25f, Vector3(-radius-100.0f , -offset-100.0f, 0.0f) );
     k0.Add( 0.5f, Vector3(-radius,-offset, 0.0f) );
     k0.Add( 0.75f, Vector3(-radius+50, -offset-50.0f, 0.0f));
     k0.Add( 1.0f, Vector3(-radius,-offset,0.0f));
     animation.AnimateBetween( Property(mBlobShader, "uPoints[1]"),k0,AlphaFunctions::EaseInOutSine);

     //CP2
     k0 = KeyFrames::New();
     k0.Add( 0.0f, Vector3(radius,-offset,0.0f));
     k0.Add( 0.25f, Vector3(radius+100.0f,-offset-100.f,0.0f));
     k0.Add( 0.5f, Vector3(radius,-offset,0.0f));
     k0.Add( 0.75f, Vector3(radius-50,-offset-50.0f,0.0f));
     k0.Add( 1.0f, Vector3(radius,-offset,0.0f));
     animation.AnimateBetween( Property(mBlobShader, "uPoints[2]"),k0,AlphaFunctions::EaseInOutSine);

     //CP3
     k0 = KeyFrames::New();
     k0.Add( 0.0f, Vector3(radius,offset,0.0f));
     k0.Add( 0.25f, Vector3(radius+100.0f,offset+100.0f,0.0f));
     k0.Add( 0.5f, Vector3(radius,offset,0.0f));
     k0.Add( 0.75f, Vector3(radius-50.0f,offset+50.0f,0.0f));
     k0.Add( 1.0f, Vector3(radius,offset,0.0f));
     animation.AnimateBetween( Property(mBlobShader, "uPoints[4]"),k0,AlphaFunctions::EaseInOutSine);

     //CP4
     k0 = KeyFrames::New();
     k0.Add( 0.0f, Vector3(-radius,offset, 0.0f));
     k0.Add( 0.25f, Vector3(-radius-100.0f,offset+100.f, 0.0f));
     k0.Add( 0.5f, Vector3(-radius,offset, 0.0f));
     k0.Add( 0.75f, Vector3(-radius+50.0f,offset+50.0f, 0.0f));
     k0.Add( 1.0f, Vector3(-radius,offset, 0.0f));

     animation.AnimateBetween( Property(mBlobShader, "uPoints[5]"),k0,AlphaFunctions::EaseInOutSine);
     animation.SetLooping(true);
     animation.Play();
   }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down && (IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ))  )
    {
      //mApplication.Quit();
    }
  }

private:
  Application&                mApplication;
  Toolkit::QuadraticBezier    mCircleShader;
  Toolkit::QuadraticBezier    mTriangleShader;
  ShaderEffect                mBlobShader;
  Toolkit::View               mView;
};

void RunTest( Application& application )
{
  AnimatedShapesExample test( application );
  application.MainLoop();
}

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  RunTest( application );

  return 0;
}
