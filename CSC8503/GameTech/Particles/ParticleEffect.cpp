#include "ParticleEffect.h"
#include "ParticleEmitter.h"
#include "../../Common/Camera.h"

ParticleEffect::ParticleEffect(unsigned int numParticles)
{
	camera = NULL;
	emitter = NULL;

	localToWorld = Matrix4();
	texID = 0;
	force = Vector3(0, -9.81f, 0); //initialised to gravity

	ResizePBuffer(numParticles);
}
