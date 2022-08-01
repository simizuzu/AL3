#include "EnemyStateLeave.h"
#include"Enemy.h"

void EnemyStateLeave::Update(Enemy* pEnemy)
{
	//移動(ベクトルを加算)
	Vector3 move = pEnemy->GetLeaveVelocity();
	//移動(ベクトルを加算)
	pEnemy->MoveTranslation(move);
}