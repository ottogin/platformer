#pragma once

#include <SFML/Graphics.hpp>

struct lang
{
	IntRect rect;
	Sprite sprite;
};

class TextManager
{

public:
	std::map<String, lang> dict;
	String current_word;

	TextManager()
	{}

	~TextManager()
	{ 
		dict.clear();
	}

	//загрузка из файла XML
	void loadFromXML(std::string fileName, Texture &t)
	{	  
		TiXmlDocument animFile(fileName.c_str());

		animFile.LoadFile();

		TiXmlElement *head;
		head = animFile.FirstChildElement("sprites");

		TiXmlElement *animElement;
		animElement = head->FirstChildElement("animation");
		while(animElement)
		{

			IntRect word;

			current_word = animElement->Attribute("title");
			TiXmlElement *cut;
			cut = animElement->FirstChildElement("cut");
			while (cut)
			{
				int x = atoi(cut->Attribute("x"));
				int y = atoi(cut->Attribute("y"));
				int w = atoi(cut->Attribute("w"));
				int h = atoi(cut->Attribute("h"));

				word = IntRect(x,y,w,h);
				cut = cut->NextSiblingElement("cut");
			}
			Sprite s;
			s.setTexture(t);
			s.setTextureRect(word);
			lang a;
			a.rect= word;
			a.sprite = s;
			dict[current_word] = a;
			animElement = animElement->NextSiblingElement("animation");
		}
	}

	void draw(RenderWindow &window, String word, int x1 = 0, int y1 = 0)
	{
		int x = x1;
		int y = y1;
		for (int i = 0; i < word.getSize(); i++)
		{
			lang a = dict[word[i]];
			a.sprite.setPosition(x, y);
			window.draw(a.sprite);
			x += a.rect.width;	
		}
	}

};
