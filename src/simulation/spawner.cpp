#include "spawner.h"
#include "components.h"
#include "simulation.h"
#include "utils/math.h"

void Spawner::Update(Simulation& simulation, float dt)
{
	time += dt;

	if(time < settings.initialDelay)
	{
		return;
	}
	
	float condValue = 0.0f;
	switch(settings.spawnCondition)
	{
		case SpawnCondition::Duration:
			condValue = time;
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
	if(condValue >= settings.spawnConditionValue)
	{
		return;
	}

	spawnTimer -= dt;
	if(spawnTimer <= 0.0f)
	{
		float r = random.Range(settings.pSize.x, settings.pSize.y);
		float m = settings.pMass * (settings.scalePMass ? r : 1.0f);
		float b = settings.pBounciness;

		HSV hsv = settings.pColorSingle.ToHSV();
		float t = time / settings.pColorShiftDuration;
		t = settings.pColorShift == SpawnRepeatMode::Repeat ? std::fmodf(t, 1.0f) : Math::PingPong(t, 1.0f);
		switch(settings.pColorMode)
		{
			case SpawnColorMode::RandomHue:
				hsv.h = random.Value();
				break;
			case SpawnColorMode::ShiftHue:
				hsv.h = t;
				break;
			default:
				break;
		}
		Color c = Color::FromHSV(hsv.h, hsv.s, hsv.v);

		float f = random.Range(settings.spawnForce.x, settings.spawnForce.y) * (settings.scaleSpawnForce ? m : 1.0f);
		float dirOffset = random.Range(-settings.spawnDirectionVariation, settings.spawnDirectionVariation) * 0.5f;

		simulation.AddParticle(Particle(r, m, b, position, settings.SpawnDirVector(dirOffset) * f), position, c);
		spawned++;
		spawnTimer += settings.spawnRate * (settings.scaleSpawnRate ? r : 1.0f);
	}
}
