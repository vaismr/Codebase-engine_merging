#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;
uniform sampler2D lut;

#define MAXCOLOUR 15.0
#define COLOURS 16.0 //colours per channel
#define WIDTH 256.0 //lut width
#define HEIGHT 16.0 //lut height

void main()
{    
	vec4 colour = texture2D(texture0, TexCoords); //get colour of pixel from texture

	float cell = colour.b * MAXCOLOUR;
	float cellUp = ceil(cell);
	float cellDown = floor(cell);

	float halfPxX= 0.5 / WIDTH;
	float halfPxY = 0.5 / HEIGHT;
	float rOffset = halfPxX + colour.r / COLOURS * (MAXCOLOUR / COLOURS);
	float gOffset = halfPxY + colour.g * (MAXCOLOUR / COLOURS);

	vec2 lutPosDown = vec2(cellDown / COLOURS + rOffset, gOffset);
	vec2 lutPosUp = vec2(cellUp / COLOURS + rOffset, gOffset);

	vec4 gradedColourDown = texture2D(lut, lutPosDown);
	vec4 gradedColourUp = texture2D(lut, lutPosUp);

	vec4 gradedColour = mix(gradedColourDown, gradedColourUp, fract(cell));

    FragColor = gradedColour;
    FragColor = texture2D(texture0, TexCoords);
}