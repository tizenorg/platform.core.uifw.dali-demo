
attribute vec2 aPosition;
attribute vec2 aTexCoords;

varying vec2 vTexCoord;

void main()
{
  vTexCoord = aTexCoords; //aVertex.texCoords;
  gl_Position = vec4( aPosition, 0.0, 1.0);
}
