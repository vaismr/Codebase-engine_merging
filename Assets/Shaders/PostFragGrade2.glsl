#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;
uniform sampler2D lut;

vec4 sampleAs3DTexture(sampler2D tex, vec3 uv, float width)
{
	float sliceSize = 1.0 / width;
	float slicePixelSize = sliceSize / width;
	float sliceInnerSize = slicePixelSize * (width - 1.0);

	float zSlice0 = min(floor(uv.z * width), width - 1.0);
	float zSlice1 = min(zSlice0 + 1.0, width - 1.0);

	float xOffset = slicePixelSize * 0.5 + uv.x * sliceInnerSize;

	float s0 = xOffset + (zSlice0 * sliceSize);
	float s1 = xOffset + (zSlice1 * sliceSize);
	vec4 slice0Colour = texture2D(tex, vec2(s0, uv.y));
	vec4 slice1Colour = texture2D(tex, vec2(s1, uv.y));
	
	float zOffset = mod(uv.z * width, 1.0);
	vec4 result = mix(slice0Colour, slice1Colour, zOffset);
	return result;
}

void main()
{    
	vec4 colour = texture2D(texture0, TexCoords);

	vec4 gradedColour = sampleAs3DTexture(lut, colour.rgb, 16);
	gradedColour.a = colour.a;
	FragColor = gradedColour;
}