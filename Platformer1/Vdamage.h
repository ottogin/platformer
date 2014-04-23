#pragma once

struct VD
{
	float x, y;
	int damage;
	bool heal;
	float font;

	VD(float x1, float y1, int damage1, bool heal1, float font1)
	{
		x = x1;
		y = y1;
		damage = damage1;
		heal = heal1;
		font = font1;
	}
};