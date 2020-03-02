#pragma once

#include "Particle.h"

class ParticleEmitter
{
public: 
	ParticleEmitter() {};
	virtual ~ParticleEmitter() {};
	virtual void EmitParticle(Particle& particle) {};
};
