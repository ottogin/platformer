	#pragma once

#include <SFML/Graphics.hpp>
#include "anim.h"
#include <iostream>
#include "level.h"
#include "inventory.h"


class enemy
{
public:
	float dx;
	float x, y;
	float hp;
	int max_hp;
	int mp;
	int max_mp;
	inventory inv;
	bool should_draw_inv;
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
		should_draw_inv = 0;
		anim.loadFromXML(name, enemys_texture);
		objects = levl.GetAllObjects();	
		inv.set_not_main(9);
		inv.item_list[0] = it.list[15];
		inv.item_list[1] = it.list[20];
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

	IntRect GetIntRect()
	{
		return IntRect(x, y - h, w, h);
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

	 void draw(RenderWindow &window, int sx, int sy, inventory &inve)
	{
		anim.draw(window, x, y);
		if (should_draw_inv)
		{
			inv.draw_else(window, x, y, sx, sy, inve);
		}
	}

	void set(String name)
	{
		anim.set(name);
	}

};