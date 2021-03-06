#version 330 core

uniform float particleSize = 0.5f;

uniform float	time;

layout(points) in;
layout(triangle_strip, max_vertices = 32) out;

in Vertex
{
	vec4 colour;
	//vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN[];

out Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} OUT;

void main()
{
	float riseSpeed = time * 5.0f;
	float shrinkSpeed = time * 2.0f;

	int numLevels = 8;

	for (int j = 0; j < numLevels; j++)
	{
		for (int i = 0; i < gl_in.length(); i++)
		{
			float shrinkFactor = 2.0f / j;

			OUT.colour = IN[i].colour;
			OUT.normal = IN[i].normal;
			OUT.shadowProj = IN[i].shadowProj;
			OUT.worldPos = IN[i].worldPos;

			//top right
			gl_Position = gl_in[i].gl_Position;
			gl_Position.x += (particleSize + shrinkFactor) / shrinkSpeed;
			gl_Position.y += (particleSize + shrinkFactor) / shrinkSpeed;
			
			if (j % 2 == 0)
			{
				gl_Position.x += j;
			}
			 
			gl_Position.y += riseSpeed + (j * numLevels);
			OUT.texCoord = vec2(1, 0);
			EmitVertex();

			//top left
			gl_Position = gl_in[i].gl_Position;
			gl_Position.x -= (particleSize + shrinkFactor) / shrinkSpeed;
			gl_Position.y += (particleSize + shrinkFactor) / shrinkSpeed;

			if (j % 2 == 0)
			{
				gl_Position.x += j;
			}

			gl_Position.y += riseSpeed + (j * numLevels);
			OUT.texCoord = vec2(0, 0);
			EmitVertex();

			//bottom right
			gl_Position = gl_in[i].gl_Position;
			gl_Position.x += (particleSize + shrinkFactor) / shrinkSpeed;
			gl_Position.y -= (particleSize + shrinkFactor) / shrinkSpeed;
			
			if (j % 2 == 0)
			{
				gl_Position.x += j;
			}

			gl_Position.y += riseSpeed + (j * numLevels);
			OUT.texCoord = vec2(1, 1);
			EmitVertex();

			//bottom left
			gl_Position = gl_in[i].gl_Position;
			gl_Position.x -= (particleSize + shrinkFactor) / shrinkSpeed;
			gl_Position.y -= (particleSize + shrinkFactor) / shrinkSpeed;

			if (j % 2 == 0)
			{
				gl_Position.x += j;
			}

			gl_Position.y += riseSpeed + (j * numLevels);
			OUT.texCoord = vec2(0, 1);
			EmitVertex();

			EndPrimitive();
		}
	}
}