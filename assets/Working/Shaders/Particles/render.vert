uniform mat4 uModelViewProjection;

in vec4 position_mass;

void main( void ) 
{
	gl_Position = uModelViewProjection * vec4( position_mass.xyz, 1.0 ); 
	gl_PointSize = position_mass.w; 
}
