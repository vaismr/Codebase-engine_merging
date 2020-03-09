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
	/*float red = (texture(texture0, TexCoords).r + texture(texture1, TexCoords).r + texture(texture2, TexCoords).r + texture(texture3, TexCoords).r + texture(texture4, TexCoords).r) / 5.0f;
	float blue = (texture(texture0, TexCoords).g + texture(texture1, TexCoords).g + texture(texture2, TexCoords).g + texture(texture3, TexCoords).g + texture(texture4, TexCoords).g) / 5.0f;
	float green = (texture(texture0, TexCoords).b + texture(texture1, TexCoords).b + texture(texture2, TexCoords).b + texture(texture3, TexCoords).b + texture(texture4, TexCoords).b) / 5.0f;
	float alpha = (texture(texture0, TexCoords).a + texture(texture1, TexCoords).a + texture(texture2, TexCoords).a + texture(texture3, TexCoords).a + texture(texture4, TexCoords).a) / 5.0f;*/

	float weight0 = 0.5f;
	float weight1 = 0.25f;
	float weight2 = 0.125f;
	float weight3 = 0.0625f;
	float weight4 = 0.0625f;

	float red = texture(texture0, TexCoords).r * weight0 + texture(texture1, TexCoords).r * weight1 + texture(texture2, TexCoords).r * weight2 + texture(texture3, TexCoords).r * weight3 + texture(texture4, TexCoords).r * weight4;
	float blue = texture(texture0, TexCoords).g * weight0 + texture(texture1, TexCoords).g * weight1 + texture(texture2, TexCoords).g * weight2 + texture(texture3, TexCoords).g * weight3 + texture(texture4, TexCoords).g * weight4;
	float green = texture(texture0, TexCoords).b * weight0 + texture(texture1, TexCoords).b * weight1 + texture(texture2, TexCoords).b * weight2 + texture(texture3, TexCoords).b * weight3 + texture(texture4, TexCoords).b * weight4;
	float alpha = texture(texture0, TexCoords).a * weight0 + texture(texture1, TexCoords).a * weight1 + texture(texture2, TexCoords).a * weight2 + texture(texture3, TexCoords).a * weight3 + texture(texture4, TexCoords).a * weight4;
    

	//FragColor = texture(texture1, TexCoords);
	FragColor = vec4(red, blue, green, alpha);
}