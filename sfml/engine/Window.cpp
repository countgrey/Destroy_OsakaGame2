#include "Window.h"
#include <iostream>

void Game::InitVariables()
{
	this->p_Window	= nullptr;

	this->Points				= 0;
	this->Multiplier			= 1;
	this->EnemySpawnTimerMax	= 3.f;
	this->EnemySpawnTimer		= this->EnemySpawnTimerMax;
	this->MaxEnemys				= 15;
	this->MouseHeld				= false;
	this->IsHeat				= false;

	if (!this->BG_Texture.loadFromFile("assets/images/bg.png"))
	{
		std::cout << "Cant load BG" << std::endl;
	}

	this->BG.setTexture(this->BG_Texture);
}

void Game::InitWindow()
{
	this->p_VMode.width		= 1280;
	this->p_VMode.height	= 960;
	
	this->p_Title			= "Destroy the Osaka:The Game";

	this->p_Window			= new sf::RenderWindow(p_VMode, p_Title, sf::Style::Titlebar | sf::Style::Close);
	this->p_Window->setFramerateLimit(60);

	this->p_Running			= true;
}

void Game::InitFonts()
{
	if (!this->Font.loadFromFile("assets/fonts/comic.ttf"))
	{
		std::cout << "Failed to load Font" << std::endl;
	}
}

void Game::InitText()
{
	this->Text.setFont(this->Font);
	this->Text.setCharacterSize(24);
	this->Text.setFillColor(sf::Color::White);
	this->Text.setString("NULL");
}

void Game::InitSound()
{
	if (!p_SB.loadFromFile("assets/sound/boom.wav"))
	{
		std::cout << "Cant load BOOM sound" << std::endl;
	}

	snd.setBuffer(p_SB);
}

Game::Game()
{
	this->InitVariables();
	this->InitWindow();
	this->InitFonts();
	this->InitText();
	this->InitSound();
}

Game::~Game()
{
	delete this->p_Window;
}

void Game::UpdateWindow()
{
	this->HandleEvents();
	this->UpdateMousePos();
	this->UpdateEnemy();
	this->UpdateText();

	for (auto& a : this->p_ExplodeVector)
		a.Update();
	

	for (size_t i = 0; i < this->p_ExplodeVector.size(); i++)
	{
		if (this->p_ExplodeVector[i].finish)
		{
			this->p_ExplodeVector.erase(this->p_ExplodeVector.begin() + i);
			break;
		}

	}
}

void Game::HandleEvents()
{
	while (this->p_Window->pollEvent(this->e))
	{
		if (this->e.type == sf::Event::Closed)
			this->p_Running = false;
	}
}

void Game::DrawWindow()
{
	this->p_Window->clear();

	this->p_Window->draw(this->BG);
	for (auto& a : this->p_EnemyVector)
		this->p_Window->draw(a.GetEnemySprite());
	
	for (auto& a : this->p_ExplodeVector)
		a.Render(*this->p_Window);
	//this->p_Boom.Render(*this->p_Window);

	this->RenderText(*this->p_Window);

	this->p_Window->display();
}

void Game::UpdateMousePos()
{
	this->MousePosWindow = sf::Mouse::getPosition(*this->p_Window);
	this->MousePosView = this->p_Window->mapPixelToCoords(this->MousePosWindow);
}

void Game::UpdateEnemy()
{
	for (auto& a : this->p_EnemyVector)
		a.Update();

	if (this->p_EnemyVector.size() < this->MaxEnemys)
	{
		if (this->EnemySpawnTimer >= this->EnemySpawnTimerMax)
		{
			this->EnemySpawnTimer = 0.f;
			this->SpawnEnemy();
		}
		this->EnemySpawnTimer += 1.f;
	}

	for (size_t i = 0; i < this->p_EnemyVector.size(); i++)
	{
		if (this->p_EnemyVector[i].GetEnemySprite().getPosition().y > this->p_Window->getSize().y)
			this->p_EnemyVector.erase(this->p_EnemyVector.begin() + i);
	}
	
	IsHeat = false;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (this->MouseHeld == false)
		{
			this->MouseHeld = true;
			for (size_t i = 0; i < this->p_EnemyVector.size(); i++)
			{
				if (this->p_EnemyVector[i].GetEnemySprite().getGlobalBounds().contains(this->MousePosView))
				{
					this->p_EnemyVector.erase(this->p_EnemyVector.begin() + i);
					this->IsHeat = true;
					this->Points += 1 * this->Multiplier;
					this->Multiplier += 0.5;
					//this->p_Window->setFramerateLimit(this->Points+60);
					this->SpawnExplode(this->MousePosView);
					snd.play();
					break;
				}
			}
			if (!this->IsHeat) {
				this->Multiplier = 1;
			}
		}
	}
	else
	{
		this->MouseHeld = false;
	}
	
}

void Game::RenderText(sf::RenderTarget& target)
{
	target.draw(this->Text);
}

void Game::UpdateText()
{
	std::stringstream ss;
	sf::Color textColor = sf::Color::White;

	if (this->Multiplier > 1) {
		textColor = sf::Color::Green;
	}
	else {
		textColor = sf::Color::White;
	}

	ss << "Points: " << this->Points;
	ss << "    Multiplier: " << this->Multiplier << "x";
	this->Text.setString(ss.str());
	if (static_cast<int>(this->Multiplier) % 2 == 0) {
		sf::Color randomColor = sf::Color(rand() % 256, rand() % 256, rand() % 256);
		this->Text.setFillColor(randomColor);
	}
}

void Game::SpawnEnemy()
{
	this->p_Enemy.SetPosition(sf::Vector2f(
		static_cast<float>(rand() % static_cast<int>(this->p_Window->getSize().x - this->p_Enemy.GetSize().x)),
		static_cast<float>(rand() % static_cast<int>((this->p_Window->getSize().y - 200.f) - this->p_Enemy.GetSize().y)))
	);

	this->p_Enemy.YSpeed = 0.5f + static_cast<float>(this->Points / 50);
	this->MaxEnemys += this->Points / 20;


	this->p_EnemyVector.push_back(this->p_Enemy);
}

void Game::SpawnExplode(sf::Vector2f Vec)
{
	this->p_Boom.ExplodeSprite.setPosition(Vec.x - 25.f, Vec.y - 45.f);
	this->p_Boom.clock = 0;
	this->p_Boom.finish = false;
	this->p_Boom.UVShit.left = 0;
	this->p_ExplodeVector.push_back(this->p_Boom);
}

const bool Game::GetGameRunningState() const
{
	return this->p_Running;
}
