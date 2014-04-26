#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace sf;

const int ground1 = 20 * 32;
const int weigth1 = 1000;

class item
{
public:
	int number;
	std::string name;
	Sprite sprite;
	int hp, mp, dmg;
	item(int num, String nam, int h, int m, int dm, Sprite sp)
	{
		number = num;
		name = nam;
		hp = h;
		mp = mp;
		dmg = dm;	
		sprite = sp;
	}
};

class items
{
public:
	std::vector<item> list;
	int i;
	std::string loadFromFile(const char* filename)
	{
		int number, hp, mp, dmg, x, y, w, h;
		std::string name;
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
			sprite.setTextureRect(IntRect(x, y, w, h));
			item itm(number, name, hp, mp, dmg, sprite);
			list.push_back(itm);
			element = element -> NextSiblingElement("item"); 
		}
		return fname;

	}
};

class inventory
{
public:
	int bag_size;
	items it;
	std::string fname;
	Sprite sprite;
	Texture texture;
	std::vector <item> item_list;

	inventory()
	{
		bag_size = 64;
		fname = it.loadFromFile("Data/items.xml");
		for (int i = 0; i < bag_size; i++)
		{
			if (i < it.list.size() - 1)
				item_list.push_back(it.list[i + 1]);
			else 
				item_list.push_back(it.list[0]);
		}
		texture.loadFromFile(fname);
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

	void drap(int num)
	{
		item_list[num] = it.list[0];
	}

	void draw(RenderWindow &window, int sx, int sy, int x = 0, int y = 0)
	{
		for (int i = 0; i < bag_size; i++)
		{
			sprite = item_list[i].sprite;
			sprite.setTexture(texture);
			sprite.setPosition(weigth1 / 2 +((i % 8) - 4) * sprite.getTextureRect().width + sx, ground1 / 2 +((i / 8) - 4) * sprite.getTextureRect().height + sy);
			window.draw(sprite);
		}
	}
};