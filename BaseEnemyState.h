#pragma once

class Enemy;

class BaseEnemyState
{
public:
	virtual void Update(Enemy* pEnemy) = 0;
};