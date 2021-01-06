#pragma once
#include "game_object.h"
#include <maths/vector4.h>
#include <vector>

class Boid : public GameObject
{
public:
	Boid();
	~Boid();

	inline gef::Vector4 GetEnvironmentDimensions() { return gef::Vector4(environment_half_width_, environment_half_depth_, environment_half_height_); }

	void Update(std::vector<Boid*> boids, float delta_time);

	// public for debugging...
	float perception_;
	float max_force_;
	float max_speed_;
	float min_speed_;
	float separation_weight_;
	float alignment_weight_;
	float cohesion_weight_;
	float edges_weight_;
	float flee_weight_;
	float flee_radius_;

private:
	gef::Vector4 Separation(std::vector<Boid*> boids);
	gef::Vector4 Alignment(std::vector<Boid*> boids);
	gef::Vector4 Cohesion(std::vector<Boid*> boids);
	gef::Vector4 Flee();
	void Edges();

private:
	gef::Vector4 position_;
	gef::Vector4 velocity_;
	gef::Vector4 acceleration_;

	float environment_half_width_;
	float environment_half_depth_;
	float environment_half_height_;

	float vDistance(gef::Vector4 a, gef::Vector4 b); // these functions should be in their own library.
	float vMagnitudeSquared(gef::Vector4 vector);
	float vMagnitude(gef::Vector4 vector);
	gef::Vector4 vDivide(gef::Vector4 vector, float divisor);
	gef::Vector4 vMultiply(gef::Vector4 vector, float multiplier);
	gef::Vector4 vNormalize(gef::Vector4 vector);
	gef::Vector4 vSetMagnitude(gef::Vector4 vector, float magnitude);
	gef::Vector4 vLimit(gef::Vector4 vector, float max);
	gef::Vector4 vClamp(gef::Vector4 vector, float min, float max);
	float vMap(float value, float min_A, float max_A, float min_B, float max_B); // not vector math but should be in personal math library with vector functions.
};