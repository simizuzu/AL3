#include"EnemyStateApproach.h"
#include"Enemy.h"
#include"EnemyStateLeave.h"

void EnemyStateApproach::Update(Enemy* pEnemy)
{
	//���˃^�C�}�[�J�E���g�_�E��
	pEnemy->SetFileTimer(pEnemy->GetFileTimer() - 1);
	//�w�莞�ԂɒB����
	if (pEnemy->GetFileTimer() == 0)
	{
		//�e�̔���
		pEnemy->Fire();
		//���˃^�C�}�[��������
		pEnemy->SetFileTimer(pEnemy->kFireInterval);
	}

	Vector3 move = pEnemy->GetApproachVelocity();
	//�ړ�(�x�N�g�������Z)
	pEnemy->MoveTranslation(move);
	//����̈ʒu�ɓ��������痣�E
	if (pEnemy->GetWorldTransform().translation_.z < 0.0f)
	{
		pEnemy->ChangeState(new EnemyStateLeave());
	}
}