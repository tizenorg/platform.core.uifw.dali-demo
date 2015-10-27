#ifndef __COMMON_H__
#define __COMMON_H__

using namespace Dali::Toolkit;

std::string stringf(const char* format, ...)
{
    va_list arg_list;                                                           
    va_start(arg_list, format);                                                 

    // SUSv2 version doesn't work for buf NULL/size 0, so try printing
    // into a small buffer that avoids the double-rendering and alloca path too...
    char short_buf[256];                                                        
    const size_t needed = vsnprintf(short_buf, sizeof short_buf,
                                    format, arg_list) + 1;
    if (needed <= sizeof short_buf)
        return short_buf;

    // need more space...
    char* p = static_cast<char*>(alloca(needed));
    vsnprintf(p, needed, format, arg_list);
    return p;
}

Actor CreateCross( const Vector3& pos, const Vector4& color, float size)
{
  Actor cross = Actor::New();
  int num = 4;
  for(int i=0; i<num+1; ++i)
  {
    TextLabel h,v;

    h = TextLabel::New(".");
    h.SetColor(color);
    h.SetPosition(Vector3(size*(float)i/num-size*.5, 0, 0));
    cross.Add(h);

    v = TextLabel::New(".");
    v.SetColor(color);
    v.SetPosition(Vector3(0, size*(float)i/num-size*.5, 0));
    cross.Add(v);
  }
  cross.SetPosition(pos);
  return cross;
}

Actor CreateCrossCube(int num_x, int num_y, int num_z, float size,
                      const Vector3& fromRange, const Vector3& toRange)
{
  Actor cube = Actor::New();
  for(int x=0; x<num_x; ++x)
    for(int y=0; y<num_y; ++y)
      for(int z=0; z<num_z; ++z)
      {
        float tx = float(x)/(num_x<2 ? 1:num_x-1);
        float ty = float(y)/(num_y<2 ? 1:num_y-1);
        float tz = float(z)/(num_z<2 ? 1:num_z-1);

        Vector3 pos( (1-tx)*fromRange.x + tx*toRange.x, 
                    (1-ty)*fromRange.y + ty*toRange.y,
                    (1-tz)*fromRange.z + tz*toRange.z );

        Actor cross = CreateCross(pos, Vector4(0,0,1,1), size);

        cube.Add(cross);
      }
  return cube;
} 

Actor CreateCoordAxesText(float size)
{
  Actor coord = Actor::New();
  int num = 10;
  for(int i=0; i<num+1; ++i)
  {
    TextLabel x,y,z;
    if(i==num-1) x = TextLabel::New("x");
    else      x = TextLabel::New(".");
    x.SetColor(Vector4(1,0,0,1));
    x.SetPosition(Vector3(size*(float)i/num,0,0));
    coord.Add(x);
    if(i==num-1) y = TextLabel::New("y");
    else      y = TextLabel::New(".");
    y.SetColor(Vector4(0,1,0,1));
    y.SetPosition(Vector3(0,size*(float)i/num,0));
    coord.Add(y);
    if(i==num-1) z = TextLabel::New("z");
    else      z = TextLabel::New(".");
    z.SetColor(Vector4(0,0,1,1));
    z.SetPosition(Vector3(0,0,size*(float)i/num));
    coord.Add(z);
  }
  coord.SetParentOrigin(ParentOrigin::CENTER);
  return coord;
}

Actor CreateTextLine( const Vector3& p1, const Vector3& p2, const Vector4& color, int numPoints )
{
  Actor line = Actor::New();
  for(int i=0; i<numPoints; ++i)
  {
    float t = float(i)/(numPoints<2 ? 1:numPoints-1);
    TextLabel c;
    c = TextLabel::New( "." );
    c.SetColor(color);
    c.SetPosition(p1*t + p2*(1.-t));
    line.Add(c);
  }
  line.SetParentOrigin(ParentOrigin::CENTER);
  return line;
}

Actor CreateVerticalLine( const Vector3& pos, const Vector4& color, float height, int numPoints )
{
  return CreateTextLine(pos-Vector3(0,height*.5,0), pos+Vector3(0,height*.5,0), color, numPoints);
}

Vector3 GetRotVecBtwnTwoVecs(const Vector3& fromV, const Vector3& toV)
{
  double angle = acos(fromV.Dot(toV));
  Vector3 axis = fromV.Cross(toV);
  axis.Normalize();
  return axis*angle;
}

Matrix3 GetProjMatOnVector(const Vector3& onVector_unit)
{
  const Vector3& v = onVector_unit;
  return Matrix3(v.x*v.x, v.x*v.y, v.x*v.z,
                v.x*v.y, v.y*v.y, v.y*v.z,
                v.x*v.z, v.y*v.z, v.z*v.z);
}

Matrix3 mat3_sub_mat3(const Matrix3& m1, const Matrix3& m2)
{
  const float* f1 = m1.AsFloat();
  const float* f2 = m2.AsFloat();
  return Matrix3(f1[0]-f2[0],f1[1]-f2[1],f1[2]-f2[2],
                f1[3]-f2[3],f1[4]-f2[4],f1[5]-f2[5],
                f1[6]-f2[6],f1[7]-f2[7],f1[8]-f2[8]);
}
Matrix3 GetProjMatOnPlane(const Vector3& planeNormal_unit)
{
  return mat3_sub_mat3(Matrix3::IDENTITY, GetProjMatOnVector(planeNormal_unit));
}
Vector3 Mat3DotVec3(const Matrix3& m, const Vector3& v)
{
  const float* f = m.AsFloat();
  return Vector3(f[0],f[3],f[6])*v.x 
        + Vector3(f[1],f[4],f[7])*v.y 
        + Vector3(f[2],f[5],f[8])*v.z;
}

Vector3 DeviceCoord2DaliCoord(const Vector3& v)
{
  return Vector3(v.x,-v.y,v.z);
}

Vector3 DaliCoord2StreoCoord(const Vector3& v)
{
  return Vector3(v.y,-v.x,v.z);
}

Matrix Mat3ToMat4(const Matrix3& m3)
{
  const float* f = m3.AsFloat();
  Matrix m4;
  m4.SetIdentity();
  m4.SetXAxis(Vector3(f[0],f[3],f[6]));
  m4.SetYAxis(Vector3(f[1],f[4],f[7]));
  m4.SetZAxis(Vector3(f[2],f[5],f[8]));
  return m4;
}

Actor CreateImageCube(int num_x, int num_y, int num_z,
                      const Vector3& imageSize,
                      const Vector3& fromRange, const Vector3& toRange,
                      const string& imagePath)
{
  Actor cube = Actor::New();
  for(int x=0; x<num_x; ++x)
    for(int y=0; y<num_y; ++y)
      for(int z=0; z<num_z; ++z)
      {
        float tx = float(x)/(num_x<2 ? 1:num_x-1);
        float ty = float(y)/(num_y<2 ? 1:num_y-1);
        float tz = float(z)/(num_z<2 ? 1:num_z-1);

        Vector3 pos( (1-tx)*fromRange.x + tx*toRange.x, 
                    (1-ty)*fromRange.y + ty*toRange.y,
                    (1-tz)*fromRange.z + tz*toRange.z );

        string name = stringf("%d%d%d",x,y,z);

        ImageActor imageActor = ImageActor::New( ResourceImage::New( imagePath ) );
        imageActor.SetName(name);
        imageActor.SetSize(imageSize);
        imageActor.SetPosition(pos); 
        cube.Add(imageActor);

        TextLabel textLabel = TextLabel::New( stringf("%.0f %.0f %.0f", pos.x, pos.y, pos.z) );
        textLabel.SetColor(Vector4(1,1,1,1));
        //textLabel.SetOutline(true, Vector4(0,0,0,1));
        //textLabel.SetWeight(TextStyle::BOLD);
        textLabel.SetScale(1.1);
        textLabel.SetParentOrigin(ParentOrigin::CENTER);
        textLabel.SetPosition(Vector3(0, imageSize.y/2., .01));
        imageActor.Add(textLabel);

        TextLabel nameActor = TextLabel::New( name );
        nameActor.SetColor(Vector4(0,1,0,1));
        //nameActor.SetOutline(true, Vector4(0,0,0,1));
        //nameActor.SetWeight(TextStyle::BOLD);
        nameActor.SetScale(1.2);
        nameActor.SetParentOrigin(ParentOrigin::CENTER);
        nameActor.SetPosition(Vector3(0, -imageSize.y/2., .01));
        imageActor.Add(nameActor);
      }
  return cube;
} 

#endif
