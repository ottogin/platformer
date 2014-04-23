#include <SFML/Graphics.hpp>
#include "anim.h"
#include "tinyxml.h"
#include "level.h"
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream> 
#include <time.h>
#include "Enemes.h"
#include "Stat.h"
#include "Draw_text.h"
#include "Vdamage.h"
#include <vector>  
#include "bullet.h"

using namespace sf;

const double a = 0.0005;
const int W = 100;
const int H = 40;
const int ground = 20 * 32;
const int weigth = 1000;
int camera_x, camera_y;
bool up_pressed;
bool down_pressed;
bool hold_e;
bool f_L_is_last = 0;


int min(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

int max(int a, int b)
{
	if (a < b)
		return b;
	return a;
}

struct point
{
	int x, y;
};

class player
{
public:
	float kx;
	float dx, dy;
	float x, y;
	bool on_ground; 
	int w;
	float can_be_hit;
	float can_shot;
	int h;
	AnimationManager anim;
	std::vector<Object> objects;
	bool on_ladder;
	bool on_ladder_last;

	player(Texture &player_texture,  Level &levl, int x1 = 64, int y1 = (H - 1) * 32, String name = "Data/player.xml")
	{	
		dx = 0;
		dy = 0;
		kx = 0;
		x = x1;
		y = y1;
		w = 32;
		h = 47;
		can_be_hit = 1;
		can_shot = 1;
		on_ground = 0;
		on_ladder = 0;
		on_ladder_last = 0;
		anim.loadFromXML(name ,player_texture);	
		objects = levl.GetAllObjects();
	}

	void update(float timer, int we)
	{
		if (can_be_hit < 1)
			can_be_hit += 2 * a * timer;
		else 
			can_be_hit = 1;
		if (can_shot < 1)
			can_shot += 2 * a * timer;
		else 
			can_shot = 1;
		if (kx == 0)
			x += 2 * dx * timer;
		x += kx * timer;
		if (kx > 0)
			kx -= a * timer;
		else 
			if (kx != 0)
				kx += a * timer;
		if (abs(kx) < a)
			kx = 0;
		collision(0);
		if (!on_ground && !on_ladder)
			dy = dy + a * timer;
		y += dy * timer;
		on_ground = 0;
		collision(1);
		if (on_ladder)
		{
			anim.set("ladder");
			if (dy == 0)
				anim.pause();
			else 
				anim.play();
		}
		if (on_ladder)
		{
			if (up_pressed)
				dy = - 0.1;
		    else if (down_pressed)
				dy = 0.1;
			else dy = 0;
		}
		on_ladder_last = on_ladder;
		on_ladder = 0;
		dx = 0;
		anim.tick(timer);
	}

	FloatRect GetRect()
	{
		return FloatRect(x, y - h, w, h);
	}

    void collision(int d)
	{
		for (int i = 0; i < objects.size(); i++)
			if (FloatRect(x,y - h, w, h).intersects(objects[i].rect))
			{
				if (objects[i].name == "solid" && !on_ladder)
				{           
					if (dy > 0 && d == 1)	
					{ 
						y = objects[i].rect.top - 1;  
						dy = 0;   
						on_ground = 1;
					}
					if (dy < 0 && d == 1)
					{
						y = objects[i].rect.top + objects[i].rect.height + h;  
						dy = 0; 
					}
					if ((dx > 0 || kx > 0 ) && d == 0 && x < objects[i].rect.left)	
					{
						x =  objects[i].rect.left -  w;
						kx = 0;
					} 
					else
						if ((dx < 0 || kx < 0) && d == 0)	
						{ 
							x =  objects[i].rect.left + objects[i].rect.width;
							kx = 0;
						}
				} 
				if (objects[i].name == "ladder")
				{
					if (up_pressed || down_pressed || on_ladder_last)
					{
						on_ladder = 1;
						x = objects[i].rect.left + objects[i].rect.width / 2 - w / 2;
					}
				}
				if (objects[i].name == "rigth_up")
				{
					 FloatRect r = objects[i].rect;
					 int y0 = r.top + r.height - (x + w / 2 - r.left) * r.height / r.width;
					 if (y > y0 && x + w / 2 < r.left + r.width)
					 {
						y = y0;
						dy = 0;
						on_ground = 1;
					 }
				}
			}
	}
	void draw(RenderWindow &window,int x=0, int y=0)
	{
		anim.draw(window, x, y);
	}

	void set(String name)
	{
		anim.set(name);
	}

	void flip(bool b)
	{
		anim.flip(b);
	}
};


int main()	
{
	RenderWindow window(VideoMode(weigth, ground), "adventure");
	View view( FloatRect(0, 0, weigth, ground) );

	Level lvl;
	lvl.LoadFromFile("Data/level1.tmx");


	//////загрузка текстур//////
	Texture player_texture;
	Texture bullet_texture;
	Texture coin_texture;
	Texture enemys_texture;
	Texture alpha;
	player_texture.loadFromFile("Data/player.png");
	bullet_texture.loadFromFile("Data/firebal.png");
	alpha.loadFromFile("Data/alpha.png");
	player_texture.setSmooth(true);
	coin_texture.loadFromFile("Data/coinGold.png");
	coin_texture.setSmooth(true);
    enemys_texture.loadFromFile("Data/enemies_spritesheet.png");
	enemys_texture.setSmooth(true);
	Sprite coin_sprite;
	coin_sprite.setTexture(coin_texture);

	Clock clock;

	////////создание персонажей///////
	Object pl = lvl.GetObject("player_resp");
	player katja(player_texture, lvl, pl.rect.left, pl.rect.top + pl.rect.height);
	statis stat(player_texture, alpha);
    std::vector<enemy> enemys;
	std::vector <bullet> bullets;
	std::vector<Object> enemys_objects = lvl.GetObjects("enemy_resp");
	std::vector<Object> coins = lvl.GetObjects("coin");
	hold_e = 0;

	for (int i = 0; i < enemys_objects.size(); i++)
	{
		enemy a(enemys_objects[i].rect.left, enemys_objects[i].rect.top + enemys_objects[i].rect.height, lvl, enemys_texture);
		enemys.push_back(a);
	}

	while (window.isOpen())
	{
		float timer = clock.getElapsedTime().asMicroseconds();
		clock.restart();

		point cursor;
		cursor.x = Mouse::getPosition().x;
		cursor.y = Mouse::getPosition().y;

		timer = timer/1000;

		if (timer > 40) timer = 40; 
		Event event;
		while (window.pollEvent(event))
		{
			if(event.type == Event::Closed)
				window.close();	
		}

	    katja.set("stay");
		int we = 33;
		bool f_run = 0;
		bool if_attack = 0;
		up_pressed = 0;
		down_pressed = 0;

		///////////////обработка нажатий//////////////////////

		if (Keyboard::isKeyPressed(Keyboard::Up))
		{
			up_pressed = 1;
		}
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			down_pressed = 1;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right) && !katja.on_ladder_last)
		{
			katja.set("walk");
			katja.dx = 0.1;
			f_L_is_last = 0;
			f_run = 1;
			stat.break_casting();
		}
		if (Keyboard::isKeyPressed(Keyboard::Left) && !katja.on_ladder_last)
		{
			katja.set("walk");
			katja.dx =  -0.1;
			katja.flip(1);
			f_L_is_last = 1;
			f_run = 1;
			stat.break_casting();
		}
		if (Keyboard::isKeyPressed(Keyboard::Q))
		{
			/*if (katja.can_shot == 1)
			{*/
				if_attack = 1;
				if (f_run)
					katja.set("2run_attack");
				else 
					katja.set("1attack");
				if (katja.dx < 0 || f_L_is_last)
					katja.flip(1);
				we = 42;
				stat.break_casting();
				katja.can_shot = 0;
			/*}*/
		}

		if (Keyboard::isKeyPressed(Keyboard::E))
		{
			if (!hold_e && katja.can_shot == 1 && stat.mp >= 10)
			{
				hold_e = 1;
				stat.mp -= 10;
				stat.casting(2);
				katja.can_shot = 0;
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::F7))
		{
			std::ifstream in("Data/saves.txt");
			in >> stat.hp >> stat.mp >> katja.x >> katja.y;
			in.close();
		}
		if (Keyboard::isKeyPressed(Keyboard::F6))
		{
			std::ofstream out ("Data/saves.txt");
			out.clear();
			out << stat.hp << " " << stat.mp << " " << katja.x << " " << katja.y;
			out.close();
		}
		else 
			hold_e = 0;

	    bool f_spase = 0;
		if (Keyboard::isKeyPressed(Keyboard::Space)) 
		{
			f_spase = 1;
			if (katja.on_ground)
			{
				katja.set("jump");
				katja.dy = -0.41;
				katja.on_ground = 0;
		    }
			if (katja.dx < 0)
				katja.flip(1);
			stat.break_casting();
		}
		/*if (katja.dy < 0 && !f_spase && !katja.on_ladder_last)
			katja.dy = 0;*/
		camera_x = 0;
		if (katja.x > weigth / 2 && katja.x < W * 32 - weigth / 2)
			camera_x = katja.x - weigth / 2;
		if (katja.x >= W * 32 - weigth / 2)
			camera_x = W * 32 - weigth;
		camera_y = 0;
		if (katja.y > ground / 2 && katja.y < H * 32 - ground / 2)
			camera_y = katja.y - ground / 2;
		if (katja.y >= H* 32 - ground / 2)
			camera_y = H * 32 - ground;
		view.setCenter( camera_x + weigth / 2,camera_y + ground / 2);
		window.setView(view);
		window.clear(Color(127, 127, 127));
		for (int i = 0; i < coins.size(); i++)
		{
			FloatRect coin_rect(coins[i].rect.left, coins[i].rect.top, 32, 32);
			if (katja.GetRect().intersects(coin_rect))
			{
				stat.coins_counter ++;
				coins.erase(coins.begin() + i, coins.begin() + i + 1);
				VD fpb(katja.x, katja.y - katja.h, 1, 1, 14);
							stat.add_VD(fpb);
				i--;
			}
		}
		////////////// вылет пули //////////////////////////////
		if (katja.kx != 0)
			stat.break_casting();
		if (stat.cast == 1 && stat.cast_time <= stat.last_cast_time)
		{
			int sp = 1;
				if (f_L_is_last)
				{
					sp = -1;
				}
			bullet bull(sp, 10, katja.x, katja.y - katja.h / 2 + 10, bullet_texture, "Data/fireball.xml", lvl);
			bullets.push_back(bull);
			katja.can_shot = 0;
			stat.end_of_casting();
		}

		
		cursor.x -= window.getPosition().x + 8 - camera_x;
		cursor.y -= window.getPosition().y + 30 - camera_y;
		

		///////////// cтолконовения с врагами///////////////////
		for (int i = 0; i < enemys.size(); i++)
		{
			enemys[i].update(timer);
			if (cursor.x <= (enemys[i].GetRect().left + enemys[i].GetRect().width) && cursor.x >= enemys[i].GetRect().left)
				if (cursor.y >= enemys[i].GetRect().top && cursor.y <= (enemys[i].GetRect().top + enemys[i].GetRect().height))
					if (Mouse::isButtonPressed(Mouse::Left))
					{
						stat.setTarget(&enemys[i].hp, 100, 0, 0, &enemys[i].anim);
					}
			for (int j = 0; j < bullets.size(); j++)
			{
				if (bullets[j].GetRect().intersects(enemys[i].GetRect()) && enemys[i].hp > 0)
				{
					int damage = rand() % int(0.5 * 50) + 0.75 * 50;
					enemys[i].hp = max(0, enemys[i].hp - damage);
					VD fpb(enemys[i].x, enemys[i].y - 40, damage, 0, 14);
					stat.add_VD(fpb);
					bullets[j].del = 1;
					stat.setTarget(&enemys[i].hp, 100, 0, 0, &enemys[i].anim);
				}
			}
			if (katja.GetRect().intersects(enemys[i].GetRect()))
			{
				if (enemys[i].hp > 0)
				{
					if (if_attack && ((katja.dx > 0 && katja.x < enemys[i].x) || (katja.dx < 0 && katja.x > enemys[i].x)
						|| (!f_L_is_last && katja.x < enemys[i].x) || (f_L_is_last && katja.x > enemys[i].x) ) )
						{
							if(katja.can_shot == 1)
							{
								int damage = rand() % int(0.5 * 10) + 0.75 * 10;
								enemys[i].hp = max(0, enemys[i].hp - damage);
								VD fpb(enemys[i].x, enemys[i].y - 40, damage, 0, 14);
								stat.add_VD(fpb);
								katja.can_shot = 0;
								stat.setTarget(&enemys[i].hp, 100, 0, 0, &enemys[i].anim);
							}
						}
					else 
						{
							if (katja.can_be_hit == 1)
							{
								if (katja.dy > 0)
									katja.dy = -0.2;
								if (katja.x < enemys[i].x)
									katja.kx = -0.4;
								else katja.kx = 0.4;
								srand(time(	NULL));
								int damage = rand() % int(0.5 * enemys[i].damage) + 0.75 * enemys[i].damage;
								VD fpb(katja.x, katja.y - katja.h, damage, 0, 14);
								stat.add_VD(fpb);
								stat.hp = max(stat.hp - damage, 0);
								katja.can_be_hit = 0;
							}
						}


				}
			}
		}

		/////////////////////////////updates////////////////////////////////
		katja.update(timer, we);
		stat.update(timer);
		lvl.Draw(window);
		int ii = 0;
		while (ii != bullets.size())
		{
			bullets[ii].update(timer);
			//printf("%d \n", bullets[ii].x);
			if (bullets[ii].del)
			{
				bullets.erase(bullets.begin() + ii, bullets.begin() + ii + 1);
				ii--;
			}
			ii++;
		}
		for (int i = 0; i < coins.size(); i++)
		{
			coin_sprite.setPosition(coins[i].rect.left, coins[i].rect.top);
			window.draw(coin_sprite);
		}
		for (int i = 0; i < bullets.size(); i++)
		{
			bullets[i].draw(window);
		}
		for (int i = 0; i < enemys.size(); i++)
			enemys[i].draw(window, int(enemys[i].x) , int(enemys[i].y));
		katja.draw(window, int(katja.x), int(katja.y));
		stat.draw(window, camera_x, camera_y, alpha);
		window.display();
	}
	return 0;
}