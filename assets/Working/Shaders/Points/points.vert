#include "uniforms.glsl"

uniform mat4 ciModelViewProjection;

in vec4 position_id;
in vec4 velocity_mass;
in vec4 color;
in vec4 orientation;

uniform float size; //slider:1,10,3

out highp vec4 vColor;

void main( void )
{
	vColor = color;
    gl_Position = ciModelViewProjection * vec4( position_id.xyz, 1.0 );
	gl_PointSize = velocity_mass.w * iRenderScale * size;
}
