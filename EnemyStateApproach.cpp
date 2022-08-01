#include"EnemyStateApproach.h"
#include"Enemy.h"
#include"EnemyStateLeave.h"

void EnemyStateApproach::Update(Enemy* pEnemy)
{
	//発射タイマーカウントダウン
	pEnemy->SetFileTimer(pEnemy->GetFileTimer() - 1);
	//指定時間に達した
	if (pEnemy->GetFileTimer() == 0)
	{
		//弾の発射
		pEnemy->Fire();
		//発射タイマーを初期化
		pEnemy->SetFileTimer(pEnemy->kFireInterval);
	}

	Vector3 move = pEnemy->GetApproachVelocity();
	//移動(ベクトルを加算)
	pEnemy->MoveTranslation(move);
	//既定の位置に到着したら離脱
	if (pEnemy->GetWorldTransform().translation_.z < 0.0f)
	{
		pEnemy->ChangeState(new EnemyStateLeave());
	}
}