uniform vec4 Color; //color

in vec4 vColor;

out vec4 oColor;

void main(void)
{
	oColor = vColor*Color;
}