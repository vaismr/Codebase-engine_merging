#pragma once
#include "../../CSC8503Common/PhysicsSystem.h"

#include "Particle.h"

namespace NCL
{
	namespace CSC8503
	{
		class Camera;
		class ParticleEmitter;

		class ParticleEffect
		{
		public:
			struct Vertex
			{
				Vertex() : position(0, 0, 0), colour(0, 0, 0, 1), texCoords(0, 0)
				{

				}

				Vector3 position;
				Vector4 colour;
				Vector2 texCoords;
			};

			typedef std::vector<Vertex> VertexBuffer;

			ParticleEffect();
			virtual ~ParticleEffect();

		};
	}
}


