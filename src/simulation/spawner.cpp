#include "spawner.h"
#include "components.h"
#include "simulation.h"
#include "utils/math.h"

static float AnimParam(float time, float duration, SpawnRepeatMode repeatMode)
{
	float t = time / duration;
	t = repeatMode == SpawnRepeatMode::Repeat ? std::fmodf(t, 1.0f) : Math::PingPong(t, 1.0f);
	return t;
}

void Spawner::Update(Simulation& simulation, float dt)
{
	time += dt;
	if(!CanSpawn(simulation))
	{
		return;
	}

	spawnTimer -= dt;
	if(spawnTimer <= 0.0f)
	{
		spawnTimer = SpawnParticle(simulation);
	}
}

bool Spawner::CanSpawn(Simulation& simulation) const
{
	if(time < settings.initialDelay)
	{
		return false;
	}

	float condValue = 0.0f;
	switch(settings.spawnCondition)
	{
		case SpawnCondition::Duration:
			condValue = time - settings.initialDelay;
			break;
		case SpawnCondition::LocalAmount:
			condValue = spawned;
			break;
		case SpawnCondition::GlobalAmount:
			condValue = simulation.ParticleAmount();
			break;
		default:
			break;
	}
	return condValue < settings.spawnConditionValue;
}

float Spawner::SpawnParticle(Simulation& simulation)
{
	float r = random.Range(settings.pSize.x, settings.pSize.y);
	float m = settings.pMass * (settings.scalePMass ? r : 1.0f);
	float b = settings.pBounciness;
	Color c = ParticleColor();

	float f = random.Range(settings.spawnForce.x, settings.spawnForce.y) * (settings.scaleSpawnForce ? m : 1.0f);
	float dirOffset = random.Range(-settings.spawnDirectionVariation, settings.spawnDirectionVariation) * 0.5f;
	float rotOffset = RotationOffset();

	simulation.AddParticle(Particle(r, m, b, false, position, settings.SpawnDirVector(dirOffset + rotOffset) * f), position, c);
	spawned++;
	return settings.spawnRate * (settings.scaleSpawnRate ? r : 1.0f);
}

Color Spawner::ParticleColor()
{
	HSV hsv = settings.pColorSingle.ToHSV();
	float t = AnimParam(time - settings.initialDelay, settings.pColorShiftDuration, settings.pColorShift);
	switch(settings.pColorMode)
	{
		case SpawnColorMode::RandomHue:
			hsv.h = random.Value();
			break;
		case SpawnColorMode::ShiftHue:
			hsv.h += t;
			break;
		case SpawnColorMode::RandomGradient:
			return settings.pColorGradient.Evaluate(random.Value());
		case SpawnColorMode::ShiftGradient:
			return settings.pColorGradient.Evaluate(t);
		default:
			break;
	}
	return Color::FromHSV(hsv.h, hsv.s, hsv.v);
}

float Spawner::RotationOffset() const
{
	float rotation = 0.0f;
	float start = settings.spawnDirectionRotationStart;
	float limit = settings.spawnDirectionRotationLimit;
	float baseOff = -start * limit;
	float t = AnimParam(time - settings.initialDelay + start * settings.spawnDirectionRotationDuration, settings.spawnDirectionRotationDuration, settings.spawnDirectionRotationRepeat);
	switch(settings.spawnDirectionRotation)
	{
		case SpawnDirectionRotation::RotateLinear:
			rotation = t * limit + baseOff;
			break;
		case SpawnDirectionRotation::RotateSmoothStep:
			rotation = Math::SmoothStep(t) * limit + baseOff;
			break;
		case SpawnDirectionRotation::RotateSmootherStep:
			rotation = Math::SmootherStep(t) * limit + baseOff;
			break;
		default:
			break;
	}
	return rotation;
}
