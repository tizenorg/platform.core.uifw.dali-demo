attribute vec2 aPosition;
attribute vec2 aTextureCoords;

varying vec2 vTexCoord;
varying vec2 vPos;

void main()
{
  vPos = aPosition;
  vTexCoord = aTextureCoords;
  gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0);
}
