#version 330

#include "uniforms.glsl"
#include "pi.glsl"

#include "home.glsl"
#include "damping.glsl"
#include "curlnoise.glsl"
#include "ring.glsl"
#include "vortex.glsl"
#include "spherical.glsl"
#include "electro.glsl"
#include "boundary.glsl"


in vec4 position_id;
in vec4 velocity_mass;
in vec4 color;
in vec4 orientation;

uniform samplerBuffer uPositionId;
uniform samplerBuffer uVelocityMass;

uniform float electroAmp; //slider
uniform float sphericalAmp; //slider
uniform float sphereRadius; //slider:0.0,5.0,1.0
uniform float ringAmp; //slider
uniform float ringRadius; //slider:0.0,5.0,1.0
uniform float curlNoiseAmp; //slider
uniform float curlNoiseScale; //slider
uniform float timeSpeed; //slider
uniform float dt; //slider
uniform float damping; //slider
uniform float accLimit; //slider:0.0,0.1,0.05
uniform float velLimit; //slider:0.0,0.1,0.05

out vec4 tf_position_id;
out vec4 tf_velocity_mass;
out vec4 tf_color;
out vec4 tf_orientation;

void main()
{
	vec3 pos = position_id.xyz;
	vec3 vel = velocity_mass.xyz;
  vec4 outColor = color;
  float mass = velocity_mass.w;

  int id = int( position_id.w );
  int total = int( iNumParticles ) - 1;
  float idn = position_id.w / total;


  float time = iGlobalTime * timeSpeed;
  vec3 acc = vec3(0.0);
  acc += electroAmp * electroForce( position_id, velocity_mass, total, uPositionId, uVelocityMass );
  acc -= sphericalAmp * sphericalForce( pos, vec3(0.0), sphereRadius );
  acc -= ringAmp * ringForce( pos, vec3(0.0), normalize(vec3(sin(time), cos(time), sin(time)*cos(time))), ringRadius);
  acc += curlNoiseAmp * curlNoise(pos*curlNoiseScale + time);

  mass = 2.0;

  float accLen = length(acc);
  float velLen = length(vel);

  if(accLen > accLimit)
  {
    acc = normalize(acc) * accLimit;
  }

  vel -= dampingForce( vel, damping );
  vel += acc * dt;

  if(velLen > velLimit)
  {
    vel = normalize(vel) * velLimit;
  }

  pos += vel * dt;

  // pos = boundary( pos, 5.0 );

  outColor = vec4( 1.0, 1.0, 1.0, 1.0 );

	tf_position_id = vec4( pos, position_id.w );
	tf_velocity_mass = vec4( vel, mass );
  tf_color = outColor;
  tf_orientation = orientation;
}