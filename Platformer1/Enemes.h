#pragma once

#include <SFML/Graphics.hpp>
#include "anim.h"
#include <iostream>
#include "level.h"


class enemy
{
public:
	float dx;
	float x, y;
	float hp;
	int max_hp;
	int mp;
	int max_mp;
	int w;
	int h;
	int damage;
	AnimationManager anim;
	std::vector<Object> objects;

	enemy(int rx, int ry, Level &levl, Texture &enemys_texture, String name = "Data/enemysliz.xml")
	{	
		dx = -0.1;
		x = rx;
		y = ry;
		max_hp = 100;
		hp = max_hp;
		h = 23;
		w = 50;
		damage = 10;
		anim.loadFromXML(name, enemys_texture);
		objects = levl.GetAllObjects();	
	}

	void update(float time)
	{
		if (hp > 0)
		{
			x += dx * time;
			collision();
			if (hp > 0)
				anim.set("walk");
			if (dx > 0)
				anim.flip(1);
			else 
				anim.flip(0);
			anim.tick(time);
		}
		else
		{
			anim.set("die");
			anim.tick(time);
		}
	}

	FloatRect GetRect()
	{
		return FloatRect(x, y - h, w, h);
	}

     bool collision()
	{
		for (int i = 0; i < objects.size(); i++)
			if (FloatRect(x,y - h, w, h).intersects(objects[i].rect))
			{
				 if (objects[i].name == "solid" || objects[i].name == "enemy_barrier")
				{           
					if (dx > 0)	
					{
						x =  objects[i].rect.left -  w;
						dx = - dx;
						return 1;
					}
					if (dx < 0)	
					{ 
						x =  objects[i].rect.left + objects[i].rect.width;
						dx = - dx;
						return 1;
					}
				}
			}
		return 0;
	}

	void draw(RenderWindow &window,int x=0, int y=0)
	{
		anim.draw(window, x, y);
	}

	void set(String name)
	{
		anim.set(name);
	}

};