#include "ParticleEffect.h"
#include <algorithm>


ParticleEffect::ParticleEffect(unsigned int numParticles)
{
	camera = NULL;
	pEmitter = NULL;
	tex = NULL;

	localToWorld = Matrix4();
	force = Vector3(0, -9.81f, 0); //initialised to gravity

	ResizePBuffer(numParticles);
}

ParticleEffect::~ParticleEffect()
{
	delete tex;
}

void ParticleEffect::SetCamera(Camera* cam)
{
	camera = cam;
}

void ParticleEffect::SetParticleEmitter(ParticleEmitter* emitter)
{
	pEmitter = emitter;
}

void ParticleEffect::LoadTexture(const string& filename)
{
	tex = (OGLTexture*)TextureLoader::LoadAPITexture(filename);
}

void ParticleEffect::EmitParticle(Particle& particle)
{
	if (pEmitter != NULL)
	{
		pEmitter->EmitParticle(particle);
	}
}

void ParticleEffect::EmitParticles()
{
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		EmitParticle(particles[i]);
	}
}

void ParticleEffect::Update(float dt)
{
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		Particle& particle = particles[i];

		particle.age += dt;
		if (particle.age > particle.lifeTime) //if particle is dead, re-emit as a new particle
		{
			if (pEmitter != NULL)
				EmitParticle(particle);
		}

		particle.velocity += (force * dt);
		particle.position += particle.velocity * dt;
		
		float lifeRatio = std::clamp(particle.age / particle.lifeTime, 0.0f, 1.0f);
		particle.size = Lerp(5.0f, 0.0f, lifeRatio);
		/*More things can be interpolated using lifeRatio, such as rotation or colour*/
	}

	BuildVertexBuffer();
}

void ParticleEffect::BuildVertexBuffer()
{
	const Vector3 X(0.5, 0, 0);
	const Vector3 Y(0, 0.5, 0);
	const Vector3 Z(0, 0, 1.0);

	Quaternion cameraRotation;

	if (camera != NULL)
		cameraRotation = cameraRotation.EulerAnglesToQuaternion(camera->GetPitch(), camera->GetYaw(), 0.0f);

	vertices.resize(particles.size() * 4, Vertex());
	//vertices.resize(particles.size() * 4);

	for (unsigned int i = 0; i < particles.size(); i++)
	{
		Particle& particle = particles[i];
		Quaternion rotation = rotation.AxisAngleToQuaterion(Z, particle.rotate);

		unsigned int vertexIndex = i * 4;
		Vertex& v0 = vertices[vertexIndex]; //bottom left
		Vertex& v1 = vertices[vertexIndex + 1]; //bottom right
		Vertex& v2 = vertices[vertexIndex + 2]; //top right
		Vertex& v3 = vertices[vertexIndex + 3]; //top left

		//bottom left
		v0.position = particle.position + (rotation * cameraRotation * (-X - Y) * particle.size);
		v0.texCoords = Vector2(0, 1);
		v0.colour = particle.colour;
		
		//bottom right
		v1.position = particle.position + (rotation * cameraRotation * (X - Y) * particle.size);
		v1.texCoords = Vector2(1, 1);
		v1.colour = particle.colour;
		
		//top right
		v2.position = particle.position + (rotation * cameraRotation * (X + Y) * particle.size);
		v2.texCoords = Vector2(1, 0);
		v2.colour = particle.colour;
		
		//top left
		v3.position = particle.position + (rotation * cameraRotation * (-X + Y) * particle.size);
		v3.texCoords = Vector2(0, 0);
		v3.colour = particle.colour;
	}
}

void ParticleEffect::Render()
{
	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDepthMask(GL_FALSE); //disable depth writes
	glEnable(GL_BLEND); //enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	//glMultMatrixf();
}

void ParticleEffect::ResizePBuffer(unsigned int numParticles)
{
	particles.resize(numParticles, Particle());
	vertices.resize(numParticles * 4, Vertex());
}