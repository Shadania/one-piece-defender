#pragma once

struct BasicStats {
	int level;
	int points;
	int lives;
};

struct Point2f {
	float x;
	float y;
};

struct Color4f {
	float r;
	float g;
	float b;
	float a;
};

struct Rectf
{
	float left;
	float bottom;
	float width;
	float height;
};

struct Circlef
{
	Point2f center;
	float radius;
};

struct Vector2f
{
	float x;
	float y;
};