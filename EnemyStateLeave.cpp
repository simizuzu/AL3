#include "EnemyStateLeave.h"
#include"Enemy.h"

void EnemyStateLeave::Update(Enemy* pEnemy)
{
	//�ړ�(�x�N�g�������Z)
	Vector3 move = pEnemy->GetLeaveVelocity();
	//�ړ�(�x�N�g�������Z)
	pEnemy->MoveTranslation(move);
}