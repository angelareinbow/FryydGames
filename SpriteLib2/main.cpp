#include "spritelib/spritelib.h"
#include <fstream>
#include <streambuf>
#include <vector>
#include <iostream>
#include "Monster.h"
#include "Meat.h"
#include "Dairy.h"
#include <vector>
#include <time.h>
#include <Windows.h>

using namespace spritelib;
using namespace std;

Sprite background, background2, player, enemy, enemy2, platform, theFloor, theUI, dead, menu;

bool playerA = false;
int counter = 600;
int newCounter = 1300;
int health1 = 10, health2 = 30;
bool weapon1 = false, weapon2 = false;
float playerHealth = 100.0f;
float groundLvl = 50;
int cheese = 1, meat = 1;
int gamestates = 0;
vector<Monster*> enemies;

std::vector<Sprite*> spritesToDraw;
void IterativeSelectionSort(std::vector<Sprite*>& a_sprites);

//physics
float magnitude = 10;
float acc_y = 9.8;

int jumpCounter = 0;

enum states
{
	face_left,
	face_right
};

enum jstates
{
	jumped,
	Platform,
	ground
};

states curr_states = face_right;
jstates jump_state = ground;

void platformCollision(Sprite &player, Sprite &platform, float plw, float plh, float &gLvl)
{
	if ((player.get_position().x > platform.get_position().x - 20) && player.get_position().x < (platform.get_position().x + plw))
	{
		if (player.get_position().y >= platform.get_position().y + 95 && player.get_position().y <= platform.get_position().y + (100))
		{
			jump_state = Platform;
		}
	}
}

void scrollLeft(int sVal)
{

	float plx = platform.get_position().x;
	float ply = platform.get_position().y;

	float x = background.get_position().x;
	float y = background.get_position().y;

	background.set_position(x -= sVal, y);//This makes everything scroll as per the player
	platform.set_position(((plx) -= sVal), (ply));
	enemies[0]->shiftLeft(sVal);
	enemies[1]->shiftLeft(sVal);
}

void scrollRight(int sVal)
{

	float plx = platform.get_position().x;
	float ply = platform.get_position().y;

	float x = background.get_position().x;
	float y = background.get_position().y;

	background.set_position(x += sVal, y); //making sure the background is greater than zero in order for it to scroll back
	platform.set_position(((plx) += sVal), (ply));
	enemies[0]->shiftRight(sVal);
	enemies[1]->shiftRight(sVal);
}

void KeyboardFunc(Window::Key a_key, Window::EventType a_eventType)
{

	float x = background.get_position().x;
	float y = background.get_position().y;

	float px = player.get_position().x;
	float py = player.get_position().y;

	switch (a_eventType)
	{
	case Window::EventType::KeyPressed:
	{
		if (a_key == Window::Key::Space)
		{
			if (Window::EventType::KeyReleased && playerHealth > 0)
			{
				if (x + background.get_scale().x > Window::get_game_window().get_width(true) && 2 * x < 0)
				{
					jump_state = jumped;
					jumpCounter = 0;
				}
			}
		}

		else if (a_key == Window::Key::D && playerHealth > 0)
		{

			if (x + background.get_scale().x > Window::get_game_window().get_width(true))
			{
				scrollLeft(7);
			}

			if (px + 90 < Window::get_game_window().get_width(true))
			{
				player.set_flipped(false);
				player.set_position(px += 3, py);	 //if the player is less than the right boundary they can move forward
				player.set_animation("walk");

			}


			curr_states = face_right;
		}

		else if (a_key == Window::Key::A && playerHealth > 0)
		{
			if (2 * x < 0)
			{
				scrollRight(7);
			}

			if (px > 0)
			{
				player.set_position(px -= 3, py); // if the player is before the left boundary they can move back
				player.set_flipped(true);
				player.set_animation("walk");
			}
			curr_states = face_left;
		}
		else if (a_key == Window::Key::E)
		{
			if (Window::EventType::KeyReleased)
			{
				playerA = true;
			}
			break;
		}
		else if (a_key == Window::Key::Right)
		{
			weapon2 = true;
			weapon1 = false;
		}
		else if (a_key == Window::Key::Left)
		{
			weapon1 = true;
			weapon2 = false;
		}
	}
	break;
	case Window::EventType::KeyReleased:
	{
		if (curr_states == face_right)
			player.set_animation("idle");

		if (curr_states == face_left)
			player.set_animation("idle");
	}
	}
}

void UI()
{
	if (playerHealth > 0)
	{
		if (playerHealth>70) Shapes::set_color(0.0f, 1.0f, 0.0f);
		else if (playerHealth >40) Shapes::set_color(1.0f, 1.0f, 0.0f);
		else Shapes::set_color(1.0f, 0.0f, 0.0f);

		Shapes::draw_rectangle(true, 10, 490, 2 * playerHealth, 30, 0.0f); //x, y, width, height
	}

	if (weapon1 == true)
	{
		Shapes::set_color(0.0f, 0.0f, 0.9f);
		Shapes::draw_rectangle(true, 325, 480, 45, 45, 0);
	}
	else if (weapon2 == true)
	{
		Shapes::set_color(0.0f, 0.0f, 0.9f);
		Shapes::draw_rectangle(true, 375, 480, 45, 45, 0);
	}

	Shapes::set_color(1.0f, 1.0f, 0.0f);
	Shapes::draw_rectangle(true, 330, 485, 35, 35, 0);


	Shapes::set_color(1.0f, 0.0f, 0.0f);
	Shapes::draw_rectangle(true, 380, 485, 35, 35, 0);

	Text::load_font("assets/KBZipaDeeDooDah.ttf", "TEMP");

	Text::set_color(0.0f, 0.0f, 0.0f);
	Text::draw_string("Health", "TEMP", 10.0f, 530.0f);

	Text::draw_string("Weapons", "TEMP", 325.0f, 530.0f);


	if (cheese == 0 && meat == 0)
	{
		Text::set_color(0.0f, 0.0f, 0.0f);
		Text::draw_string("REACHED", "TEMP", 500.0f, 460.0f);
		Text::draw_string("GOAL", "TEMP", 520.0f, 500.0f);
	}
	else
	{
		if (cheese == 0)
		{
			Text::set_color(0.0f, 1.0f, 0.0f);
			Text::draw_string("Cheese", "TEMP", 500.0f, 500.0f);
		}
		else if (cheese == 1)
		{
			Text::set_color(1.0f, 0.0f, 0.0f);
			Text::draw_string("Cheese x1", "TEMP", 500.0f, 500.0f);
		}
		if (meat == 1)
		{
			Text::set_color(1.0f, 0.0f, 0.0f);
			Text::draw_string("Meat x1", "TEMP", 500.0f, 460.0f);
		}
		else if (meat == 0)
		{
			Text::set_color(0.0f, 1.0f, 0.0f);
			Text::draw_string("Meat", "TEMP", 500.0f, 460.0f);
		}
	}


}

void Jump()
{
	float x = background.get_position().x;
	float y = background.get_position().y;

	float posx = player.get_position().x;
	float posy = player.get_position().y;

	if (jump_state == jumped)
	{
		jumpCounter++;

		if (jumpCounter >= 10)
		{
			platformCollision(player, platform, 400, 20, groundLvl);
		}

		if (curr_states == face_right)
		{
			scrollLeft(7);

			if (jumpCounter <= 6)
			{
				posx = posx + (magnitude);
				posy = posy + (magnitude + acc_y);
			}

			else if (jumpCounter > 7 && jumpCounter <= 10)
			{
				posy = posy;
				posx = posx + (magnitude);

			}
			else if (jumpCounter > 10 && jumpCounter < 17)
			{
				posy = posy - (magnitude + acc_y);
				posx = posx + (magnitude);

			}
			else if (jumpCounter >= 17)
			{
				jump_state = ground;
				posy = groundLvl;
			}
		}
		if (curr_states == face_left)
		{
			scrollRight(7);
			if (jumpCounter <= 6)
			{
				posx = posx - (magnitude);
				posy = posy + (magnitude + acc_y);
			}

			else if (jumpCounter > 7 && jumpCounter <= 10)
			{
				posy = posy;
				posx = posx - (magnitude);
			}
			else if (jumpCounter > 10 && jumpCounter < 17)
			{
				posy = posy - (magnitude + acc_y);
				posx = posx - (magnitude);

			}

			else if (jumpCounter >= 17)
			{
				jump_state = ground;
				posy = groundLvl;
			}
		}
		player.set_position(posx, posy);
	}
}

void Update()
{
	background.draw();
	theUI.draw();
	theFloor.draw();
	platform.draw();

	if (playerHealth > 0)
		player.next_frame();
	((Dairy*)enemies[0])->nextFrame();
	((Meat*)enemies[1])->nextFrame();
}

void IterativeSelectionSort(std::vector<Sprite*>& a_sprites)
{
	for (unsigned int i = 0; i < a_sprites.size(); i++)
	{
		int minIndex = i;
		for (unsigned int j = i; j < a_sprites.size(); j++)
		{
			if (a_sprites[j]->get_depth() < a_sprites[minIndex]->get_depth())
			{
				minIndex = j;
			}
		}
		std::swap(a_sprites[minIndex], a_sprites[i]);
	}
}

void LoadSprites()
{
	//loading the sprites
	background.load_sprite_image("assets/images/Background_final.png")
		.set_scale(1717, 432);

	player.load_sprite_image("assets/images/main char SS.png")
		.set_scale(100, 120)
		.set_sprite_frame_size(141.2, 141)
		.push_frame_row("walk", 0, 0, 141.2, 0, 5)
		.push_frame("idle", 0, 0)
		.set_center(0, 0)
		.set_position(0, groundLvl)
		.set_animation("idle");

	enemy.load_sprite_image("assets/images/cheese walk ss.png")
		.set_sprite_frame_size(460, 460)
		.push_frame_row("walk", 0, 0, 460, 0, 8)
		.push_frame("idle", 0, 0)
		.set_scale(100, 100)
		.set_position(600, 50)
		.set_center(0, 0);

	enemy2.load_sprite_image("assets/images/meat walk ss.png")
		.set_scale(130, 130)
		.set_sprite_frame_size(160, 160)
		.push_frame_row("walk", 0, 0, 160, 0, 4)
		.push_frame("idle", 160, 0)
		.set_position(1200, 50)
		.set_animation("idle")
		.set_center(0, 0);

	platform.load_sprite_image("assets/images/Table.png")
		.set_scale(400, 120)
		.set_position(400, 50);

	theFloor.load_sprite_image("assets/images/floor.png")
		.set_scale(640, 50)
		.set_position(0, 0);

	theUI.load_sprite_image("assets/images/UI.png")
		.set_scale(660, 210)
		.set_center(0, 0)
		.set_position(-10, 390);

	dead.load_sprite_image("assets/images/Dead.png")
		.set_scale(140, 160);

	menu.load_sprite_image("assets/images/menu.png")
		.set_scale(640, 562);

	enemies.push_back(new Dairy(600, 130, 2, 10, enemy));
	enemies.push_back(new Meat(1200, 50, 5, 30, enemy2));

	spritesToDraw.push_back(&player);
	spritesToDraw.push_back(&enemy);
	spritesToDraw.push_back(&enemy2);
}

void FirstLevel()
{
	Update();

	UI();

	//Does the falling off the platform stuff
	if (jump_state == Platform)
	{
		if (player.get_position().x < platform.get_position().x - 60 || player.get_position().x >(platform.get_position().x + 380))
		{
			groundLvl = 50;
			player.set_position(player.get_position().x, groundLvl);
			jump_state = ground;
		}
	}

	Jump();

	//the player death and stuff
	if (playerHealth > 0)
		player.draw();
	if (playerHealth <= 0)
	{
		dead.set_position(player.get_position().x, player.get_position().y - 40);
		dead.draw();
	}

	//drawing the enemies + their death
	if (enemies[0]->getHealth() > 0)
	{
		((Dairy*)enemies[0])->move(400, 700, counter);
		((Dairy*)enemies[0])->draw();
		enemies[0]->Collisions(player.get_position().x, player.get_position().y, playerA, weapon1, playerHealth);
	}
	if (enemies[1]->getHealth() > 0)
	{
		((Meat*)enemies[1])->move(1200, 1600, newCounter);
		((Meat*)enemies[1])->draw();
		enemies[1]->Collisions(player.get_position().x, player.get_position().y, playerA, weapon2, playerHealth);
	}

	if (enemies[0]->getHealth() == 0)
	{
		cheese = 0;
	}
	if (enemies[1]->getHealth() == 0)
	{
		meat = 0;
	}
}

int main()
{
	Window& theGame = Window::get_game_window();//https://en.wikipedia.org/wiki/Singleton_pattern
	theGame.init("OPERATION CAJUN", 800, 562)
		.set_screen_size(640, 562)
		.set_keyboard_callback(KeyboardFunc)
		.set_clear_color(0, 255, 0);

	LoadSprites();

	//The main game loop
	while (theGame.update(30))
	{
		if (gamestates == 0)
		{
			//PUT THE CODE FOR THE MENU HERE
			//THIS IS A BASE
			menu.draw();

			if (GetAsyncKeyState('A')) gamestates = 1;
		}
		if (gamestates == 1)
		{
			FirstLevel();
		}

	}
	return 0;
}



