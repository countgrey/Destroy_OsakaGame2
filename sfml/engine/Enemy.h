#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

class Enemy
{
	sf::Texture p_EnemyTexture;
	sf::Sprite p_Enemy;


	void InitEnemy()
	{
		if (!this->p_EnemyTexture.loadFromFile("assets/images/enemy/enemy.png"))
		{
			std::cout << "Can't load assets/images/enemy/enemy.png" << std::endl;
		}

		this->p_Enemy.setTexture(this->p_EnemyTexture);
		this->YSpeed = static_cast<float>(rand() % 6);
	}
public:
	float YSpeed;
	
	Enemy();
	~Enemy();

	void Move(sf::Vector2f Vec);
	void SetPosition(sf::Vector2f Vec);
	void Update();
	
	const sf::Vector2u GetSize() const;
	sf::Sprite GetEnemySprite() const;
};

