#version 150 core
uniform sampler2D texture1;
uniform int useTexture = 1;

in Vertex {
	vec2 texCoord;
	vec4 colour;
}IN;

out vec4 fragColour;

void main(void){
	fragColour = IN.colour;
	if(useTexture > 0)
	{
		 fragColour *= texture(texture1, IN.texCoord);
	}

	fragColour = vec4(0.5, 0.5, 0.5, 1);
}