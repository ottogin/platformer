#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Stat.h"
#include <vector>

using namespace sf;

const int ground1 = 20 * 32;
const int weigth1 = 1000;
const int slots = 8;

struct point
{
	int x, y;
};


bool cursor_on_button(Sprite &a, Window &window, int smx = 0, int smy = 0)
{
	bool f = 0;

	point cursor;

	cursor.x = Mouse::getPosition().x;
	cursor.y = Mouse::getPosition().y;
		
	cursor.x -= window.getPosition().x + 8 - smx;
	cursor.y -= window.getPosition().y + 30 - smy;

	if (cursor.x <= (a.getPosition().x + a.getTextureRect().width) && cursor.x >= a.getPosition().x)
		if (cursor.y >= a.getPosition().y && cursor.y <= (a.getPosition().y + a.getTextureRect().height))
			f = 1;
	return f;
}

class item
{
public:
	int number;
	std::string name;
	std::string slot;
	Sprite sprite;
	bool show_har;
	int hp, mp, dmg;
	item(int num, String nam, String slot1 , int h, int m, int dm, Sprite sp)
	{
		number = num;
		name = nam;
		slot = slot1;
		hp = h;
		mp = m;
		show_har = 0;
		dmg = dm;	
		sprite = sp;
	}
};

void swap(item &a, item &b)
{
	Sprite s;
	item c(0, "","",  0, 0, 0, s);
	c = a;
	a = b;
	b = c;
}
class items
{
public:
	std::vector<item> list;
	int i;
	std::string loadFromFile(const char* filename)
	{
		int number, hp, mp, dmg, x, y, w, h;
		std::string name;
		std::string slot;
		std::string fname;
		Sprite sprite;

		TiXmlDocument doc( filename );
		if(!doc.LoadFile())
		{
			std::cout << "Failed to load file " <<  filename << std::endl;
  		}

		TiXmlElement *element = doc.FirstChildElement("sprites");

		fname = element -> Attribute("image");

		element = element -> FirstChildElement("item");
		int i = 0;
		while(element != NULL) 
		{
		    x = atoi(element->Attribute("x"));
			y = atoi(element->Attribute("y"));
			w = atoi(element->Attribute("w"));
			h = atoi(element->Attribute("h"));
			hp = atoi(element->Attribute("hp"));
			mp = atoi(element->Attribute("mp"));
			number = atoi(element->Attribute("number"));
			dmg = atoi(element->Attribute("dmg"));
			name = element->Attribute("title");
			slot = element->Attribute("slot");
			sprite.setTextureRect(IntRect(x, y, w, h));
			item itm(number, name,slot, hp, mp, dmg, sprite);
			list.push_back(itm);
			element = element -> NextSiblingElement("item"); 
		}
		return fname;

	}
};

items it;

class inventory
{
public:
	int bag_size;
	std::string fname;
	Sprite sprite, sp;
	int smvi;
	bool is_main;
	int str, stlb;
	Texture texture;
	bool rigth_pressed;
	std::vector <item> item_list;

	inventory()
	{
		bag_size = 64;
		smvi = -1;
		str = 8;
		rigth_pressed = 0;
		stlb = 8;
		fname = it.loadFromFile("Data/items.xml");
		for (int i = 0; i < bag_size; i++)
		{
			if (i % str == 0)
				item_list.push_back(it.list[i / str + 1]);
			else
				if (i < it.list.size() - slots)
					item_list.push_back(it.list[slots + i]);
				else 
					item_list.push_back(it.list[0]);
		}
		texture.loadFromFile(fname);
	}

	void set_not_main(int bs)
	{
		is_main = 0;
		bag_size = bs; 
		for (int i = 0; i < bag_size; i++)
		{
			item_list[i] = it.list[0];
		}
		str = 3;
	}

	bool add(int number)
	{
		bool f = 0;
		for (int i = 0; i < bag_size; i++)
		{
			if (item_list[i].number == 0)
			{
				item_list[i] = it.list[number];
				f = 1;
				break;
			}
		}
		return f;
	}

	void drop(int num)
	{
		item_list[num] = it.list[0];
	}

	void draw_main(RenderWindow &window, int sx, int sy, statis& stat, int x = 0, int y = 0)
	{
		for (int i = 0; i < bag_size; i++)
		{
			if (i % str == 0 && item_list[i].number == 0)
				item_list[i] = it.list[1];
			sprite = item_list[i].sprite;
			sprite.setTexture(texture);
			sprite.setPosition(weigth1 / 2 +((i % str) - 4) * sprite.getTextureRect().width + sx, ground1 / 2 +((i / str) - 4) * sprite.getTextureRect().height + sy);
			if (cursor_on_button(sprite, window, sx, sy))
			{
				if(Mouse::isButtonPressed(Mouse::Left) && item_list[i].number > slots  && smvi == -1)
				{	
					smvi = i;
				}
				if (smvi != -1 && !Mouse::isButtonPressed(Mouse::Left))
				{
					if ((item_list[i].number > slots || item_list[i].number == 0 || item_list[i].slot == item_list[smvi].slot) && smvi != i)
					{
						swap(item_list[smvi], item_list[i]);
						if (i % str == 0)
						{
							stat.hp += item_list[i].hp;
							stat.full_hp += item_list[i].hp;
							stat.mp += item_list[i].mp;
							stat.full_mp += item_list[i].mp;
						}
						if (smvi % str == 0)
						{
							stat.hp -= item_list[i].hp;
							stat.full_hp -= item_list[i].hp;
							stat.mp -= item_list[i].mp;
							stat.full_mp -= item_list[i].mp;
						}
						if (item_list[smvi].number <= slots)
							item_list[smvi] = it.list[0];
						if (smvi % str == 0)
							item_list[smvi] = it.list[smvi / str + 1];
					}
					smvi = -1;
				}
				if (Mouse::isButtonPressed(Mouse::Right))
				{
					if(item_list[i].number != 0 && smvi != i)
						rigth_pressed = 1;
				}
				else
					if(rigth_pressed)
					{
						rigth_pressed = 0;
						item_list[i].show_har = !item_list[i].show_har;
					}
				
			}
			if (smvi == i)
			{
				if (i % str == 0)
					sprite = it.list[smvi / str + 1].sprite;
				else
					sprite = it.list[0].sprite;
				sprite.setTexture(texture);
				sprite.setPosition(weigth1 / 2 +((i % str) - 4) * sprite.getTextureRect().width + sx, ground1 / 2 +((i / str) - 4) * sprite.getTextureRect().height + sy);
			
			}
			window.draw(sprite);
			///////////характеристики вещей/////////////////
			if (item_list[i].show_har && smvi != i)
			{
				if (item_list[i].number <= slots)
				{
					RectangleShape har(Vector2f(5 * 32, 32));
					har.setFillColor(Color::Color(0, 0, 0, 180));
					har.setPosition(sprite.getPosition().x + sprite.getTextureRect().width, sprite.getPosition().y - har.getSize().y);
					window.draw(har);

					Text Text;
					Text.setFont(stat.font);

					String str = item_list[i].slot;
					Text.setString(str);
					Text.setCharacterSize(14);
					Text.setColor(Color::White);
					Text.setPosition(har.getPosition().x + 10, har.getPosition().y + 10);
					window.draw(Text);
				}
				else
				{
					RectangleShape har(Vector2f(5 * 32, 5 * 32));
					har.setFillColor(Color::Color(0, 0, 0, 180));
					har.setPosition(sprite.getPosition().x + sprite.getTextureRect().width, sprite.getPosition().y - har.getSize().y);
					window.draw(har);

					Text Text;
					Text.setFont(stat.font);

					String str = item_list[i].name;
					Text.setString(str);
					Text.setCharacterSize(14);
					Text.setColor(Color::White);
					Text.setPosition(har.getPosition().x + 10, har.getPosition().y + 10);
					window.draw(Text);

					str = item_list[i].slot;
					Text.setString(str);
					Text.setCharacterSize(10);
					Text.setPosition(har.getPosition().x + 10, har.getPosition().y + 25);
					window.draw(Text);
				
					Text.setCharacterSize(14);
					str = "Increase Health: +" + raz(item_list[i].hp);
					Text.setString(str);
					Text.setColor(Color::Green);
					Text.setPosition(har.getPosition().x + 10, har.getPosition().y + 40);
					window.draw(Text);

					str = "Increase Mana: +" + raz(item_list[i].mp);
					Text.setString(str);
					Text.setColor(Color::Blue);
					Text.setPosition(har.getPosition().x + 10, har.getPosition().y + 55);
					window.draw(Text);
				}
			}
		}

		if (smvi != -1)
		{
			point cursor;
			cursor.x = Mouse::getPosition().x;
			cursor.y = Mouse::getPosition().y;
			cursor.x -= window.getPosition().x + 8 - sx;
			cursor.y -= window.getPosition().y + 30 - sy;

			sp = item_list[smvi].sprite;
			sp.setTexture(texture);
			sp.setPosition(cursor.x, cursor.y);
			window.draw(sp);
		}

		sprite.setPosition(weigth1 / 2 +((0 % str) - 4) * sprite.getTextureRect().width + sx, ground1 / 2 +((0 / str) - 4) * sprite.getTextureRect().height + sy);
		sprite.setTextureRect(IntRect(0, 0, 32 * str, 32 * str));
		if (smvi != -1 && !Mouse::isButtonPressed(Mouse::Left) && !cursor_on_button(sprite, window, sx, sy))
		{
			item_list[smvi] = it.list[0];
			smvi = -1;
		}			
	}
	void draw_else(RenderWindow &window, float x, float y, float sx, float sy, inventory &inve)
	{

		for (int i = 0; i < bag_size; i++)
		{
			sprite = item_list[i].sprite;
			sprite.setTexture(texture);
			sprite.setPosition(x + (i % str) * sprite.getTextureRect().width, y + (i / str) * sprite.getTextureRect().height);
			window.draw(sprite);
			if (cursor_on_button(sprite, window, sx, sy))
			{
				if(Mouse::isButtonPressed(Mouse::Left) && item_list[i].number != 0)
				{	
					inve.add(item_list[i].number);
					item_list[i] = it.list[0];
				}
			}
		}	
	}
};