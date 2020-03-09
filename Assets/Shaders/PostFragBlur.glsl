#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

void main()
{    
	float red = (texture(texture0, TexCoords).r + texture(texture1, TexCoords).r + texture(texture2, TexCoords).r + texture(texture3, TexCoords).r + texture(texture4, TexCoords).r) / 5.0f;
	float blue = (texture(texture0, TexCoords).g + texture(texture1, TexCoords).g + texture(texture2, TexCoords).g + texture(texture3, TexCoords).g + texture(texture4, TexCoords).g) / 5.0f;
	float green = (texture(texture0, TexCoords).b + texture(texture1, TexCoords).b + texture(texture2, TexCoords).b + texture(texture3, TexCoords).b + texture(texture4, TexCoords).b) / 5.0f;
	float alpha = (texture(texture0, TexCoords).a + texture(texture1, TexCoords).a + texture(texture2, TexCoords).a + texture(texture3, TexCoords).a + texture(texture4, TexCoords).a) / 5.0f;
    

	//FragColor = texture(texture1, TexCoords);
	FragColor = vec4(red, blue, green, alpha);
}