#include "uniforms.glsl"
#include "spherical.glsl"
#include "pi.glsl"
#include "shapes.glsl"
#include "csg.glsl"

in vec4 position_mass;		
in vec4 velocity_mass;		
in ivec4 info;				

uniform samplerBuffer uPositionMass;
uniform samplerBuffer uVelocity;

out vec4 tf_position_mass;
out vec4 tf_velocity_mass;
out ivec4 tf_info;

uniform float dt; //slider
uniform float radius; //slider:1,10,5
uniform float damping; //slider
uniform float scale; //slider
uniform float amp; //slider:0,10,5
uniform float pointsize; //slider:0,15,5
uniform float freq; //slider:0,10,2

void main()
{
	vec3 pos = position_mass.xyz; 
	float mass = position_mass.w; 
	vec3 vel = velocity_mass.xyz; 

	float limit = sqrt( iNumParticles ); 
	float hl = limit * 0.5f; 
	float id = float(info.x);
	// vel -= spherical( pos, vec3(0.0), radius ) * dt; 
	// pos += vel * dt; 
	// vel *= (1.0 - damping);


	float x = mod(id, limit); 
	float y = floor( id/limit ); 

	pos.x = x - hl; 
	pos.y = y - hl; 
	pos.z = 0.0; 

	float dist = length( vec2( pos.x, pos.y ) );
	float distc = clamp( hl - dist, 0.0, hl ) / hl;
	
		

	float time = iAnimationTime * TWO_PI;
	float value0 = sin( freq * length( pos.xy + vec2( 0, 0 ) ) + time );
	float value1 = sin( freq * length( pos.xy + vec2( hl, 0 ) ) + time );
	float value2 = sin( freq * length( pos.xy + vec2( 0, hl ) ) + time );
	float value3 = sin( freq * length( pos.xy + vec2( -hl, 0) ) + time );
	float value4 = sin( freq * length( pos.xy + vec2( 0, -hl ) ) + time );


	float total = value0 + value1 + value2 + value3 + value4; 

	
	// pos.z = amp * sin( lineDistance * 0.25 + iAnimationTime * TWO_PI ); 	
	
	pos.z = amp * total / 5.0; 
	 

	mass = pointsize * pos.z * abs( sin( total + iAnimationTime * TWO_PI ) ); 
	pos *= scale;
	//pos.z = 0.0; 
	tf_position_mass = vec4( pos, mass );
	tf_velocity_mass = vec4( vel, velocity_mass.w );
	tf_info = info; 
}

