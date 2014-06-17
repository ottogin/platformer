#pragma once

#include <SFML/Graphics.hpp>
#include "anim.h"
#include "level.h"
#include "Vdamage.h"
#include <iostream>
#include <vector>



String raz(int a)
{
	std::string s = "";
	if (a == 0)
		s = "0";
	while(a > 0)
		{
			s = s + char(a % 10 + int('0'));
			a = a / 10; 
		}
	String b = "";
	for (int i = s.size() - 1; i >= 0; i--)
		{
			b += s[i];
		}
	return b;
}

String fraz(float a)
{
	std::string s = "";
	float a2 = a - int(a);
	int a1 = int(a);
	if (a1 == 0)
		s = "0";
	while(a1 > 0)
		{
			s = s + char(a1 % 10+ int('0'));
			a1 = a1 / 10; 
		}
	String b = "";
	for (int i = s.size() - 1; i >= 0; i--)
		{
			b += s[i];
		}
	b += '.';
	b += char(int(10 * a2) + int ('0'));
	return b;
}


class statis
{
public:
	AnimationManager anim;
	int w, h;
	Font font;
	int coins_counter;
	AnimationManager *target_anim;
	float *target_mp;
	float target_max_hp;
	float *target_hp;
	float target_max_mp;
	std::vector<VD> vdam;
	int cast;
	float cast_time;
	float last_cast_time;
	bool target;
	float full_hp;
	float hp;
	float mp;
	float full_mp;
	statis(Texture &text, Texture &t, String name = "Data/statistic.xml")
	{
		anim.loadFromXML(name, text);
		w = 48;
		h = 48;
		target = 0;
		full_hp = 100;
		full_mp = 100;
		cast = 0;
		last_cast_time = 0;
		mp = full_mp;
		hp = full_hp;
		coins_counter = 0; 
		if (!font.loadFromFile("Data/arial.ttf"))
		{
			printf("Cant download the font!! \n");
		}
	}

	void update(float time)
	{
		anim.set("normal");
		anim.tick(time);
		if (cast != 0)
		{
			last_cast_time += time * 0.001;
		}
		if (last_cast_time >= cast_time + 0.5)
		{
			cast = 0;
			last_cast_time = 0;
		}
		if (mp < full_mp)
		{
			mp += time * 0.0007;
		}
		else 
			mp = full_mp;
		int to_del = -1;
		for (int i = 0; i < vdam.size(); i++)
		{
			if (int(vdam[i].font) == 10)
			{
				to_del = i;
			}
			vdam[i].y -= 0.08;
			vdam[i].font -= 0.01;
		}
		if (to_del != -1)
			vdam.erase(vdam.begin() + to_del);
	}

	void add_VD(VD a)
	{
		vdam.push_back(a);
	}

	void draw(RenderWindow &window, int x, int y, Texture &word_texture)
	{
		/////состояние персонажа////////

		anim.draw(window, x, y + h);

		/////     хп    ///////

		RectangleShape a(Vector2f(hp / full_hp * 100, 10));
		RectangleShape b(Vector2f(100, 10));
		RectangleShape a1(Vector2f(mp / full_mp * 100, 10));
		RectangleShape b1(Vector2f(100, 10));
		a.setFillColor(Color::Green);
		b.setFillColor(Color::Red);
		a1.setFillColor(Color::Blue);
		b1.setFillColor(Color::Red);
		a.setPosition(x + w, y + h - 26);
		b.setPosition(x + w , y + h - 26);
		a1.setPosition(x + w, y + h - 13);
		b1.setPosition(x + w , y + h - 13);
		window.draw(b);
		window.draw(a);
		window.draw(b1);
		window.draw(a1);

		Text Text;
		Text.setFont(font);
		String str = raz(hp) + '/' + raz(full_hp);
		String str1 = raz(hp);
		Text.setString(str);
		Text.setCharacterSize(14);
		Text.setColor(Color::Black);
		Text.setPosition( x + w + 100 / 2 - str1.getSize() * 10, y + h - 30);
		window.draw(Text);

		str = raz(mp) + '/' + raz(full_mp);
		str1 = raz(mp);
		Text.setString(str);
		Text.setPosition( x + w + 100 / 2 - str1.getSize() * 10, y + h - 17);
		window.draw(Text);

		/////////////  цель /////////////////

		if (target)
		{
			RectangleShape target_fon(Vector2f(w + 10, h));
			target_fon.setPosition(x + 200 - 5, y);
			target_fon.setFillColor(Color(100, 100, 100));
			window.draw(target_fon);
			(*target_anim).draw(window, x + 200, y + h - 5);
			a.setFillColor(Color::Green);
			b.setFillColor(Color::Red);
			if (target_max_mp == 0)
				a1.setFillColor(Color(155, 155, 155));
			else 
				a1.setFillColor(Color::Blue);
			b1.setFillColor(Color::Red);
			a.setPosition(x + w + 200 + 5, y + h - 26);
			b.setPosition(x + w + 200 + 5, y + h - 26);
			a1.setPosition(x + w + 200 + 5, y + h - 13);
			b1.setPosition(x + w + 200 + 5, y + h - 13);
			a.setSize(Vector2f(100 * (*target_hp / target_max_hp), 10));
			b.setSize(Vector2f(100, 10));
			if (target_max_mp != 0)
				a1.setSize(Vector2f(100 * (*target_mp / target_max_mp), 10));
			else
				a1.setSize(Vector2f(100, 10));
			b1.setSize(Vector2f(100, 10));
			window.draw(b);
			window.draw(a);
			window.draw(b1);
			window.draw(a1);

			str = raz(*target_hp) + '/' + raz(target_max_hp);
			str1 = raz(*target_hp);
			Text.setFont(font);
			Text.setString(str);
			Text.setCharacterSize(14);
			Text.setColor(Color::Black);
			Text.setPosition( x + 200 + 5 + w + full_hp / 2 - str1.getSize() * 10, y + h - 30);
			window.draw(Text);

			/*str = raz(mp) + '/' + raz(full_mp);
			str1 = raz(mp);
			Text.setString(str);
			Text.setPosition( x + w + full_mp / 2 - str1.getSize() * 10, y + h - 17);
			window.draw(Text);*/
		}

		//////////////полученный дамаг///////////////

		for (int i = 0; i < vdam.size(); i++)
		{
			String ps;
			if (vdam[i].heal)
			{
				ps = "+";
				Text.setColor(Color::Green);
			}
			else 
			{
					ps = "-";
					Text.setColor(Color::Red);
			}
			ps += raz(vdam[i].damage);
			Text.setString(ps);
			Text.setCharacterSize(vdam[i].font);
			Text.setPosition(int(vdam[i].x), int(vdam[i].y));
			window.draw(Text);
		}


		//////   монеты    ///////

		String printstr = raz(coins_counter);
		Text.setString(printstr);
		Text.setCharacterSize(24);
		Text.setColor(Color::White);
		Text.setPosition(x + 1000 - printstr.getSize() * 24 , y);
		window.draw(Text);

		/////////////// касты ////////////////////

		if(cast == 1)
		{
			RectangleShape cast_shape(Vector2f(200, 20));
			RectangleShape cast_shape2(Vector2f(200 * (last_cast_time / cast_time), 20));
			cast_shape2.setFillColor(Color::Yellow);
			cast_shape.setFillColor(Color::Black);
			cast_shape.setPosition(x + 500 - 100, y + 640 - 200);
		    cast_shape2.setPosition(x + 500 - 100, y + 640 - 200);
			window.draw(cast_shape);
			window.draw(cast_shape2);
			printstr = fraz(last_cast_time);

			Text.setString(printstr);
			Text.setCharacterSize(20);
			Text.setColor(Color::White);
			Text.setPosition(x + 500 - printstr.getSize() * 5, y + 640 - 200 - 2);
			window.draw(Text);
		}
		if(cast == 2)
		{
			RectangleShape cast_shape(Vector2f(200, 20));
			cast_shape.setFillColor(Color(0, 255, 0 , 255 * (1 - (last_cast_time - cast_time))));
			cast_shape.setPosition(x + 500 - 100, y + 640 - 200);
			window.draw(cast_shape);	
		}
	}


	void casting(float cast_time1)
	{
		cast = 1;
		cast_time = cast_time1;
		last_cast_time = 0;
	}

	void end_of_casting()
	{
		cast = 2;
	}

	void break_casting()
	{
		cast = 0;
	}

	void setTarget(float *target_hp1, float target_max_hp1,float *target_mp1, float target_max_mp1, AnimationManager *target_anim1)
	{
		target_mp = target_mp1;
		target_hp = target_hp1;
		target_anim = target_anim1;
		target = 1;
		target_max_hp = target_max_hp1;
		target_max_mp = target_max_mp1;
	}
};