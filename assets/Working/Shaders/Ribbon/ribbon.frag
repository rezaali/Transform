#version 400

#include "bezier.glsl"
#include "uniforms.glsl"

out vec4 oColor;

in float pid;
in vec3 gPos;
in vec2 gTexcoord;
in vec3 vVel;
in vec4 vColor;
in float gOffset;

uniform bool fog; //toggle:1
uniform float fogDistance; //slider:0.0,10.0,1.5
uniform float fogPower; //slider:0.0,8.0,2.0
uniform float trailFade; //slider
uniform vec4 Color; //color

float Line( vec3 pos, vec3 a, vec3 b ) {
  vec3 pa = pos - a;
  vec3 ba = b - a;
  float t = clamp( dot( pa, ba ) / dot( ba, ba ), 0.0, 1.0);
  vec3 pt = a + t * ba;
  return length( pt - pos );
}


const vec3 palette[9]= vec3[9]
(
  vec3(0.05490,0.09804,1.00000),
  vec3(0.20000,0.38824,1.00000),
  vec3(0.40000,0.77647,1.00000),
  vec3(0.84706,0.00000,0.28627),
  vec3(0.90980,0.00000,0.58824),
  vec3(1.00000,0.00000,1.00000),
  vec3(1.00000,0.64314,0.00000),
  vec3(1.00000,0.79608,0.00000),
  vec3(1.00000,1.00000,0.00000)
);


void main () {
	oColor = vColor * Color;

    int colorIndex = int(pid) % 3;
    colorIndex *= 3;
    oColor.rgb *= bezier(gTexcoord.x, palette[colorIndex+0], palette[colorIndex+1], palette[colorIndex+2]);
	oColor.a *= mix( 1.0, 1.0 - gTexcoord.x, trailFade ) * Color.a;

    if(fog) {
        float d = length( gPos - iCamEyePos );
        float f = pow( fogDistance/d, fogPower );
        f = 1.0 - clamp( f, 0.0, 1.0 );
        oColor = mix( oColor, iBackgroundColor, f );
    }
}
