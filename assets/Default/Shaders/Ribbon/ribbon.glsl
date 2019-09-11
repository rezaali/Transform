#version 400

#include "uniforms.glsl"

#define TWO_PI 6.2831853072
#define PI 3.14159265359
#define HALF_PI 1.57079632679

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 ciModelViewProjection;
uniform mat4 ciViewMatrix;
uniform samplerBuffer tex_position;

uniform float linEffect; //slider:0,1,0  
uniform float cosEffect; //slider:0,1,0
uniform float sinEffect; //slider:0,1,0
uniform float thickness; //slider:0,0.1,0.0025

uniform int trailLength; 

in vec2 vLookUp[]; 		//vec2( float( gl_InstanceID ), ciTexCoord0.x ); 
out vec3 gPos; 
out vec2 gTexcoord; 
out float gOffset; 

void main()
{	
	vec4 upp = inverse( ciViewMatrix ) * vec4( 0.0, 0.0, 1.0, 1.0 );
	vec3 up = upp.xyz;  
	int offset = int( vLookUp[0].x*trailLength ); 
	int index = int( vLookUp[0].y ); 
	vec4 info = texelFetch( tex_position, int( offset + index ) );  						
	if( ( index + 1 ) % trailLength > 0 && index % trailLength > 0 )
	{			
		vec3 ppos = texelFetch( tex_position, int( offset + ( index - 1 ) ) ).xyz; 
		vec3 pos = info.xyz; 
		vec3 npos = texelFetch( tex_position, int( offset + ( index + 1 ) ) ).xyz; 

		vec3 pvel = ppos - pos; 		
		float pvelLen = length(pvel); 
		pvel /= pvelLen; 

		vec3 vel = pos - npos; 
		float velLen = length(vel); 
		vel /= velLen; 		

		vec3 ppush = cross(-up, pvel); 
		vec3 push = cross(-up, vel); 

		float tc = float(index) / float(trailLength-2); 
		float ptc = float(index-1) / float(trailLength-2);
		
		float amp = mix( 1.0, 1.0 - tc, linEffect ); 
		float pamp = mix( 1.0, 1.0 - ptc, linEffect ); 		

		amp *= mix( 1.0, sin(PI*tc),  sinEffect ); 
		pamp *= mix( 1.0, sin(PI*ptc), sinEffect ); 

		amp *= mix( 1.0, cos(HALF_PI*tc), cosEffect );
		pamp *= mix( 1.0, cos(HALF_PI*ptc), cosEffect ); 

		push *= amp; 
		ppush *= pamp; 

		float th = thickness; 
		ppush *= th; 
		push *= th; 

		gTexcoord = vec2( ptc, 1.0 ); 	
		gPos = pos + ppush; 
		gl_Position = ciModelViewProjection * vec4( gPos, 1.0 ); 	
		EmitVertex();
		
		gTexcoord = vec2( ptc, 0.0 ); 				
		gPos = pos - ppush; 
		gl_Position = ciModelViewProjection * vec4( gPos, 1.0 );	
		EmitVertex();
		
		gTexcoord = vec2( tc, 1.0 );	
		gPos = npos + push; 
		gl_Position = ciModelViewProjection * vec4( gPos, 1.0 );
		EmitVertex();
		
		gTexcoord = vec2( tc, 0.0 ); 	
		gPos = npos - push; 
		gl_Position = ciModelViewProjection * vec4( gPos, 1.0 );
		EmitVertex();

		EndPrimitive();
	}		
}