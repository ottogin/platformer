#ifndef ANIM_H
#define ANIM_H

#include "tinyxml.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

class Animation
{
public:
	std::vector<IntRect> frames, frames_flip;
	float currentFrame, speed;
	bool flip, isPlaying;  
	Sprite sprite;

	Animation()
	{
		currentFrame = 0;
		isPlaying=true;
		flip=false;
	}

	bool tick(float time)
	{
		bool f = 0;
		if (!isPlaying) return 0;

		currentFrame += speed * time;

		if (currentFrame > frames.size())
		{ 
			currentFrame -= frames.size(); 
			f = 1;
		}

		int i = currentFrame;
		sprite.setTextureRect( frames[i] );
		if (sprite.getTextureRect().height > 100)
		    std::cout << frames[i].height << std::endl;
		if (flip) sprite.setTextureRect( frames_flip[i] );
		return f;
	}

};



class AnimationManager
{

public:
	String currentAnim;
	std::map<String, Animation> animList;

	AnimationManager()
	{}

	~AnimationManager()
	{ animList.clear();
	}

   //создание анимаций вручную
	void create(String name, Texture &texture, int x, int y, int w, int h, int count, float speed, int step=0)
	{
		Animation a;
		a.speed = speed;
		a.sprite.setTexture(texture);
		a.sprite.setOrigin(0,h);

		for (int i=0;i<count;i++)
		{			
			a.frames.push_back( IntRect(x+i*step, y, w, h)  );
			a.frames_flip.push_back( IntRect(x+i*step+w, y, -w, h)  );
		}
		animList[name] = a;
		currentAnim = name;
	}

	//загрузка из файла XML
	void loadFromXML(std::string fileName,Texture &t)
	{	  
		TiXmlDocument animFile(fileName.c_str());

		animFile.LoadFile();

		TiXmlElement *head;
		head = animFile.FirstChildElement("sprites");

		TiXmlElement *animElement;
		animElement = head->FirstChildElement("animation");
		while(animElement)
		{

			Animation anim;
			currentAnim = animElement->Attribute("title");
			int delay = atoi(animElement->Attribute("delay")); 
			anim.speed = 1.0/delay; 
			anim.sprite.setTexture(t); 

			TiXmlElement *cut;
			cut = animElement->FirstChildElement("cut");
			while (cut)
			{
				int x = atoi(cut->Attribute("x"));
				int y = atoi(cut->Attribute("y"));
				int w = atoi(cut->Attribute("w"));
				int h = atoi(cut->Attribute("h"));

				anim.frames.push_back( IntRect(x,y,w,h) );
				anim.frames_flip.push_back( IntRect(x+w,y,-w,h)  );
				cut = cut->NextSiblingElement("cut");
			}

			anim.sprite.setOrigin(0,anim.frames[0].height);

			animList[currentAnim] = anim;
			
			animElement = animElement->NextSiblingElement("animation");
		}
	}

	void set(String name)
	{   
		currentAnim = name; 
		animList[currentAnim].flip=0;
	}

	void draw(RenderWindow &window,int x=0, int y=0)
	{
		animList[currentAnim].sprite.setPosition(x,y);
		window.draw( animList[currentAnim].sprite );
	}

	void flip(bool b) {animList[currentAnim].flip = b;}

	void tick(float time)	 
	{
		bool f = animList[currentAnim].tick(time);
		/*if (f)
			if (currentAnim[0] == '1')
				currentAnim = "stay";
			else 
				if(currentAnim[0] == '2')
					currentAnim = "walk";*/
	}

	void pause() {animList[currentAnim].isPlaying=false;}

	void play()  {animList[currentAnim].isPlaying=true;}

	void play(String name)  {animList[name].isPlaying=true;}

	bool isPlaying()  {return animList[currentAnim].isPlaying;}

	float getH()  {return animList[currentAnim].frames[0].height;}

	float getW() {return animList[currentAnim].frames[0].width;}

	Sprite getSprite()
	{
		return animList[currentAnim].sprite;
	}

};

#endif ANIM_H