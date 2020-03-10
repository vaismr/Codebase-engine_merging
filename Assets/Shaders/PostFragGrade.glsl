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

	float cell = floor(colour.b * MAXCOLOUR); //calculate which slice of cube to use from the blue

	float halfPxOffsetX = 0.5 / WIDTH;
	float halfPxOffsetY = 0.5 / HEIGHT;

	halfPxOffsetX = halfPxOffsetX + colour.r / COLOURS * (MAXCOLOUR / COLOURS);
	halfPxOffsetY = halfPxOffsetY + colour.g * (MAXCOLOUR / COLOURS);

	vec2 lutPos = vec2(cell / COLOURS + halfPXOffsetX, halfPxOffsetY);

	vec4 gradedColour = texture2D(lut, lutPos);

    FragColor = gradedColour;
}