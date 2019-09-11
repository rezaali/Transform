#version 400

in vec4 ciPosition;
in vec2 ciTexCoord0;

out int id;
out vec2 vLookUp;

void main ()
{
	vLookUp = vec2( float( gl_InstanceID ), ciTexCoord0.x );
    id = gl_InstanceID;
	gl_Position = ciPosition;
}