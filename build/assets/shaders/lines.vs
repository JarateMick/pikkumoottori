precision highp float;

attribute vec2 vPosition;
attribute vec4 inColor;
// color, colour hahaaa!!!
varying vec4 outColour;
uniform mat4 position;

void main()
{
	gl_Position.xy = (position * vec4(vPosition, 0.0, 1.0)).xy;
	gl_Position.zw = vec2(0.0, 1.0);
	outColour = inColor;
}
