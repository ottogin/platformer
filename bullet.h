#pragma once
#include "anim.h"
#include "level.h"

class bullet
{
public:
	AnimationManager anim;
	std::vector<Object> objects;
	int damage;
	float dx;
	bool del;
	int dy;
	float x, y;
	bullet ( float dx1, int damage1, int x1, int y1, Texture &t, String xml, Level &lvl)
	{
		anim.loadFromXML(xml, t);
		dx = dx1;
		del = 0; 
		damage = damage1;
		objects = lvl.GetAllObjects();
		x = x1;
		y = y1;
		if (dx < 0)
			anim.flip(1);
	}

	FloatRect GetRect()
	{
		return FloatRect(x, y - anim.getH(), anim.getW(), anim.getH());
	}

	void update(float timer)
	{
		colission();
		int dex = x;
		x += 0.5 * dx * timer;
		dex = x - dex;
		anim.tick(timer);
	}

	void colission()
	{
		for (int i = 0; i < objects.size(); i++)
			if (GetRect().intersects(objects[i].rect))
			{
				if (objects[i].name == "rigth_up" || objects[i].name == "solid")
					del = 1;
			}
	}

	void draw(RenderWindow &window)
	{
		anim.draw(window, x, y);
	}
};	