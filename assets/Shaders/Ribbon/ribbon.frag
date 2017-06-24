#version 400

out vec4 oColor;

in vec3 gPos;
in vec2 gTexcoord; 
in float gOffset; 

uniform float trailFade; //slider
uniform vec4 Color; //color

float Line( vec3 pos, vec3 a, vec3 b ) {
  vec3 pa = pos - a;
  vec3 ba = b - a;
  float t = clamp( dot( pa, ba ) / dot( ba, ba ), 0.0, 1.0);
  vec3 pt = a + t * ba;
  return length( pt - pos );
}

void main () {		
	oColor = Color;
	oColor.a *= mix( 1.0, 1.0 - gTexcoord.x, trailFade ) * Color.a;	
	// vec2 uv = gTexcoord * 2.0 - 1.0; 
	// float len = 2000.0; 
	// uv.x *= len;

	// float result = Line( 
	// 	vec3( uv, 0.0 ), 
	// 	vec3( -len + 1.0, 0.0, 0.0 ), 
	// 	vec3( len - 1.0, 0.0, 0.0 ) ) + 0.25; 

	// result = smoothstep( 0.7, 1.0, result ); 
	// oColor.rgb *= ( 1.0 - result ); 

}