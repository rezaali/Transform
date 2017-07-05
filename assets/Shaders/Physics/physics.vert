#version 330 
#include "uniforms.glsl"
#include "pi.glsl"
#include "noise3D.glsl"

in vec4 position_mass;		
in vec4 velocity_mass;		
in ivec4 info;				

uniform samplerBuffer uPositionMass;
uniform samplerBuffer uVelocity;

out vec4 tf_position_mass;
out vec4 tf_velocity_mass;
out ivec4 tf_info;

void main()
{
	vec3 pos = position_mass.xyz; 
	float mass = position_mass.w; 
	vec3 vel = velocity_mass.xyz; 

    int id = info.x; 
    float idf = float( id ); 

    int total = int( sqrt( iNumParticles ) );
    float totalf = float( total ); 
    
    int row = id % total;  
    int col = int( floor( id / total ) ); 

    float norm = idf / iNumParticles; 

    float x = float( row ) / ( totalf - 1 ); 
    float z = float( col ) / ( totalf - 1 ); 
    float y = 0.25 * snoise( vec3( x, z, 2.0 * iGlobalTime ) ); 
	
	float amp = texture( iAmplitude, vec2( norm/4, 0.0 ) ).r; 
	y = max( y, 0.0 ); 
    pos = vec3( x - 0.5, y, z - 0.5 );

    mass = y * amp; 

	tf_position_mass = vec4( pos, mass );
	tf_velocity_mass = vec4( vel, mass );
	tf_info = info; 
}


/*

Rect 
RoundedRect
Cube
Icosahedron
Icosphere
Teapot
Circle
Ring
Sphere
Capsule
Torus
TorusKnot
Helix
Cylinder
Cone
Plane

*/