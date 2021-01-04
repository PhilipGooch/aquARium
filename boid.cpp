#include "boid.h"

#include <math.h>
#include <random>
#include<graphics\mesh_instance.h>

Boid::Boid() :
	perception_(0.5f),
	max_force_(0.06f),
	max_speed_(0.1f),
	min_speed_(0.0f),
	separation_weight_(1.3f),
	alignment_weight_(1.0f),
	cohesion_weight_(1.1),
	edges_weight_(1.5f),
	environment_half_width_(0.08f),
	environment_half_depth_(0.08f),
	environment_half_height_(0.08f)
{
	float x = ((float)(rand() % (int)(environment_half_width_ * 2000) - environment_half_width_ * 1000)) / 1000;
	float y = ((float)(rand() % (int)(environment_half_width_ * 2000) - environment_half_width_ * 1000)) / 1000;
	float z = ((float)(rand() % (int)(environment_half_width_ * 2000))) / 1000;
	float a = (float)(rand() % 360);
	float vx = cosf(a * 3.1415f / 180) * max_speed_;
	float vy = sinf(a * 3.1415f / 180) * max_speed_;
	float vz = 0.0f;
	float ax = 0.0f;
	float ay = 0.0f;
	float az = 0.0f;
	position_ = gef::Vector4(x, y, z);
	velocity_ = gef::Vector4(vx, vy, vz);
	acceleration_ = gef::Vector4(ax, ay, az);
}

Boid::~Boid()
{
}

void Boid::Update(std::vector<Boid*> boids, float delta_time)
{
	acceleration_ = gef::Vector4(0.f, 0.f, 0.f);

	Edges();

	acceleration_ += Separation(boids) * separation_weight_;
	acceleration_ += Alignment(boids) * alignment_weight_;
	acceleration_ += Cohesion(boids) * cohesion_weight_;

	velocity_ += acceleration_ * delta_time;
	velocity_ = vClamp(velocity_, min_speed_, max_speed_);

	position_ += velocity_ * delta_time;

	gef::Matrix44 rotationY;
	rotationY.SetIdentity();
	rotationY.RotationY(-atan2(velocity_.z(), sqrt(pow(velocity_.x(), 2) + pow(velocity_.y(), 2))));

	gef::Matrix44 rotationZ;
	rotationZ.SetIdentity();
	rotationZ.RotationZ(atan2(velocity_.y(), velocity_.x()));

	gef::Matrix44 rotation;
	rotation = rotationY * rotationZ;

	gef::Matrix44 translation;
	translation.SetIdentity();
	translation.SetTranslation(position_);

	local_transform_ = rotation * translation;

	world_transform_ = local_transform_ * offset_transform_ * parent_transform_;
}

gef::Vector4 Boid::Separation(std::vector<Boid*> boids)
{
	gef::Vector4 steering = gef::Vector4(0.f, 0.f, 0.f);
	int count = 0;
	for (Boid* boid : boids)
	{
		if (boid == this) continue;

		float distance = vDistance(position_, boid->position_); 
		if (distance < perception_)
		{
			gef::Vector4 difference = position_ - boid->position_;
			difference = vDivide(difference, pow(distance, 2));
			steering += difference;
			count++;
		}
	}
	if (count > 0)
	{
		steering = vDivide(steering, count);
		steering = vSetMagnitude(steering, max_speed_);
		steering -= velocity_;
		steering = vLimit(steering, max_force_);
	}

	return steering;
}

gef::Vector4 Boid::Alignment(std::vector<Boid*> boids)
{
	gef::Vector4 steering = gef::Vector4(0.f, 0.f, 0.f);
	int count = 0;
	for (Boid* boid : boids)
	{
		if (boid == this) continue;

		float distance = vDistance(position_, boid->position_);
		if (distance < perception_)
		{
			steering += boid->velocity_;
			count++;
		}
	}
	if (count > 0)
	{
		steering = vDivide(steering, count);
		steering = vSetMagnitude(steering, max_speed_);
		steering -= velocity_;
		steering = vLimit(steering, max_force_);
	}

	return steering;
}

gef::Vector4 Boid::Cohesion(std::vector<Boid*> boids)
{
	gef::Vector4 steering = gef::Vector4(0.f, 0.f, 0.f);
	int count = 0;
	for (Boid* boid : boids)
	{
		if (boid == this) continue;

		float distance = vDistance(position_, boid->position_); 
		if (distance < perception_)
		{
			steering += boid->position_;
			count++;
		}
	}
	if (count > 0)
	{
		steering = vDivide(steering, count);
		steering -= position_;
		steering = vSetMagnitude(steering, max_speed_);
		steering -= velocity_;
		steering = vLimit(steering, max_force_);
	}

	return steering;
}

void Boid::Edges()
{
	if (position_.x() < -environment_half_width_ * 2)
	{
		gef::Vector4 steering = gef::Vector4(max_speed_, 0.f, 0.f);
		steering -= velocity_;
		steering = vLimit(steering, max_force_ * edges_weight_);
		acceleration_ += steering;
	}
	if (position_.x() > 0)
	{
		gef::Vector4 steering = gef::Vector4(-max_speed_, 0.f, 0.f);
		steering -= velocity_;
		steering = vLimit(steering, max_force_ * edges_weight_);
		acceleration_ += steering;
	}
	if (position_.y() < -environment_half_depth_ * 2)
	{
		gef::Vector4 steering = gef::Vector4(0.f, max_speed_, 0.f);
		steering -= velocity_;
		steering = vLimit(steering, max_force_ * edges_weight_);
		acceleration_ += steering;
	}
	if (position_.y() > 0)
	{
		gef::Vector4 steering = gef::Vector4(0.f, -max_speed_, 0.f);
		steering -= velocity_;
		steering = vLimit(steering, max_force_ * edges_weight_);
		acceleration_ += steering;
	}
	if (position_.z() < 0)
	{
		gef::Vector4 steering = gef::Vector4(0.f, 0.f, max_speed_);
		steering -= velocity_;
		steering = vLimit(steering, max_force_ * edges_weight_);
		acceleration_ += steering;
	}
	if (position_.z() > environment_half_height_ * 2)
	{
		gef::Vector4 steering = gef::Vector4(0.f, 0.f, -max_speed_);
		steering -= velocity_;
		steering = vLimit(steering, max_force_ * edges_weight_);
		acceleration_ += steering;
	}
}



// vector functions
///////////////////////////////////////

float Boid::vDistance(gef::Vector4 a, gef::Vector4 b)
{
	return sqrt(pow(b.x() - a.x(), 2) + pow(b.y() - a.y(), 2) + pow(b.z() - a.z(), 2));
}

float Boid::vMagnitudeSquared(gef::Vector4 vector)
{
	return pow(vector.x(), 2) + pow(vector.y(), 2) + pow(vector.z(), 2);
}

float Boid::vMagnitude(gef::Vector4 vector)
{
	return sqrt(pow(vector.x(), 2) + pow(vector.y(), 2) + pow(vector.z(), 2));
}

gef::Vector4 Boid::vDivide(gef::Vector4 vector, float divisor)
{
	vector /= divisor;
	return vector;
}

gef::Vector4 Boid::vMultiply(gef::Vector4 vector, float multiplier)
{
	vector *= multiplier;
	return vector;
}

gef::Vector4 Boid::vNormalize(gef::Vector4 vector)
{
	float length = vMagnitude(vector);
	if (length != 0)
	{
		vector = vMultiply(vector, 1.f / length);
	}
	return vector;
}

gef::Vector4 Boid::vSetMagnitude(gef::Vector4 vector, float magnitude)
{
	vector = vNormalize(vector);
	vector = vMultiply(vector, magnitude);
	return vector;
}

gef::Vector4 Boid::vLimit(gef::Vector4 vector, float max)
{
	float magnitude_squared = vMagnitudeSquared(vector);
	if (magnitude_squared > pow(max, 2))
	{
		vector = vDivide(vector, sqrt(magnitude_squared));
		vector = vMultiply(vector, max);
	}
	return vector;
}

gef::Vector4 Boid::vClamp(gef::Vector4 vector, float min, float max)
{
	float magnitude_squared = vMagnitudeSquared(vector);
	if (magnitude_squared < pow(min, 2))
	{
		vector = vDivide(vector, sqrt(magnitude_squared));
		vector = vMultiply(vector, min);
	}
	else if (magnitude_squared > pow(max, 2))
	{
		vector = vDivide(vector, sqrt(magnitude_squared));
		vector = vMultiply(vector, max);
	}
	return vector;
}

float Boid::vMap(float value, float min_A, float max_A, float min_B, float max_B) {
	return min_B + (max_B - min_B) * ((value - min_A) / (max_A - min_A));
}