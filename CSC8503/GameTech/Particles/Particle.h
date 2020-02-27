#pragma once
#include "../../CSC8503Common/PhysicsSystem.h"


namespace NCL 
{
	namespace CSC8503 
	{
		struct Particle
		{
		public:
			Particle() : position(0,0,0), velocity(0,0,0), colour(0,0,0,1), rotate(0), size(0), age(0), lifeTime(0)
			{

			}
		private:
			Vector3 position;
			Vector3 velocity;
			Vector4 colour;

			float rotate;
			float size;
			float age;
			float lifeTime;
		};

		typedef std::vector<Particle> ParticleBuffer;
	}
}






