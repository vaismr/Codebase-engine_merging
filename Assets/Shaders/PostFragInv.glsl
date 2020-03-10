#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;

void main()
{    
	vec4 textureColour = texture(texture0, TexCoords);

    FragColor = vec4(1.0 - textureColour.r, 1.0 - textureColour.g, 1.0 - textureColour.b, 1.0);
}