#version 150 core

uniform sampler2D mainTex;
uniform vec2 pixelSize;
uniform int isVertical;

in Vertex
{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

const float weights[5] = float[](0.12, 0.22, 0.32, 0.22, 0.12);

void main(void)
{
	vec2 values[5];

	fragColour = vec4(0.0, 0.0, 0.0, 1.0);

	if(isVertical == 1)
	{
		values = vec2[](vec2(0.0, -pixelSize.y*2), 
						vec2(0.0, -pixelSize.y), 
						vec2(0.0, 0.0), 
						vec2(0.0, pixelSize.y), 
						vec2(0.0, pixelSize.y*2));
	}
	else
	{
		values = vec2[](vec2(0.0, -pixelSize.x*2), 
						vec2(0.0, -pixelSize.x), 
						vec2(0.0, 0.0), 
						vec2(0.0, pixelSize.x), 
						vec2(0.0, pixelSize.x*2));
	}

	for(int i = 0; i < 5; i++)
	{
		vec4 tmp = texture2D(mainTex, IN.texCoord.xy + values[i]);
		fragColour += tmp * weights[i];
	}
}