#pragma once

#include "../../Common/Camera.h"
#include "Particle.h"
#include "../GameTechRenderer.h"
#include "../../CSC8503Common/PhysicsSystem.h"
#include "../../Common/TextureLoader.h"
#include "ParticleEmitter.h"
#include "../../Common/Maths.h"


namespace NCL
{
	namespace CSC8503
	{
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
			
			ParticleEffect(unsigned int numParticles = 0);
			virtual ~ParticleEffect();

			void SetCamera(Camera* camera); //needed to orient particles towards the camera
			void SetParticleEmitter(ParticleEmitter* emitter);
			void EmitParticles();
			void ResizePBuffer(unsigned int numParticles);
			void BuildVertexBuffer(); //build from the particle buffer

			virtual void Update(float dt);
			virtual void Render();

			void LoadTexture(const string& filename);

		protected:
			void EmitParticle(Particle& particle);
		private:
			Camera* camera;
			ParticleEmitter* pEmitter;

			ParticleBuffer particles;
			VertexBuffer vertices;
			Matrix4 localToWorld;
			OGLTexture* tex;
			Vector3 force; //force to be applied to every particle in this effect, could be upwards for smoke etc.
		};
	}
}


