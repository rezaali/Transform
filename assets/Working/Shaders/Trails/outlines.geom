#version 400

#define TWO_PI 6.2831853072
#define PI 3.14159265359
#define HALF_PI 1.57079632679

layout(points) in;
layout(line_strip, max_vertices = 5) out;

uniform mat4 ciModelViewProjection;
uniform samplerBuffer tex_position;
uniform float thickness = 0.125; 
uniform float linEffect = 0.0; 
uniform float cosEffect = 0.0;  
uniform float sinEffect = 0.0; 
uniform int trailLength = 14; 
uniform vec3 up = vec3( 0.0, 0.0, 1.0 ); 

in vec2 vLookUp[]; 		//vec2( float( gl_InstanceID ), ciTexCoord0.x ); 
out vec3 gPos; 
out vec2 gTexcoord; 
out float gOffset; 

void main()
{	
	int offset = int( vLookUp[0].x*trailLength ); 
	int index = int( vLookUp[0].y ); 
	vec4 info = texelFetch( tex_position, int( offset + index ) );  					
	gOffset = info.w-1.0; 
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

		// vec3 u = -up;  
		// u = normalize(pos);
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

		ppush *= thickness*(info.w-1.0); 
		push *= thickness*(info.w-1.0); 

		// top left
		gTexcoord = vec2(ptc, 1.0); 		
		gPos = pos + ppush; 
		gl_Position = ciModelViewProjection * vec4(gPos, 1.0); 
		EmitVertex();
		
		// top right 
		gTexcoord = vec2(ptc, 0.0); 				
		gPos = pos - ppush; 
		gl_Position = ciModelViewProjection * vec4(gPos, 1.0);
		EmitVertex();
		
		// bot right 
		gTexcoord = vec2(tc, 0.0); 	
		gPos = npos - push; 
		gl_Position = ciModelViewProjection * vec4(gPos, 1.0);
		EmitVertex();

		// bot left
		gTexcoord = vec2(tc, 1.0);
		gPos = npos + push; 
		gl_Position = ciModelViewProjection * vec4(gPos, 1.0);
		EmitVertex();

		// top left
		gTexcoord = vec2(ptc, 1.0); 		
		gPos = pos + ppush; 
		gl_Position = ciModelViewProjection * vec4(gPos, 1.0); 
		EmitVertex();
		

		EndPrimitive();
	}		
}