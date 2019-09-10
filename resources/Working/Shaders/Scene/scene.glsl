#include "uniforms.glsl"
#include "shapes.glsl"
#include "noise3D.glsl"
#include "rotate.glsl"
#include "csg.glsl"
#include "pi.glsl"

float five( in vec3 p, float size ) {
  p -= vec3( -size*2, size*2, 0 );
  float result = Box( p, vec3( size, size * 3.0, size ) );
  result = Union( result, Box( p - vec3( size*3, size*2, 0 ), vec3( size * 2.0, size, size ) ) );
  result = Union( result, Box( p - vec3( size*3, -size*2, 0 ), vec3( size * 2.0, size, size ) ) );
  result = Union( result, Box( p - vec3( size*4, -size*4, 0 ), vec3( size, size * 3.0, size ) ) );
  result = Union( result, Box( p - vec3( size*2, -size*6, 0 ), vec3( size * 3.0, size, size ) ) );
  return result;
}

float kay( in vec3 p, float size ) {
  p -= vec3( -size*2, 0, 0 );
  float result = Box( p, vec3( size, size * 5.0, size ) );
  float angle = HALF_PI * 0.42;
  vec3 pr0 = ( p - vec3( size, 0, 0 ) ) * rotationMatrix( vec3( 0, 0, 1 ), -angle );
  pr0.y += size*4;
  result = Union( result, Box( pr0, vec3( size, size * 5.0, size ) ) );
  vec3 pr1 = ( p - vec3( size, 0, 0 ) ) * rotationMatrix( vec3( 0, 0, 1 ), angle );
  pr1.y -= size*4;
  result = Union( result, Box( pr1, vec3( size, size * 5.0, size ) ) );
  result = Intersection( result, Box( p - vec3( size*2, 0, 0 ), vec3( size*3, size * 5, size ) ) );
  return result;
}


float scene(in vec3 pos) {
  vec3 p = pos;
  p *= 4.0;
  float size = 0.5;
  float f = five( p - vec3( -size*4, 0, 0 ), size );
  float k = kay( p - vec3( size*4, 0, 0 ), size );
  float result = Union( f, k );
  return result;
}
