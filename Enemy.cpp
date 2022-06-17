#include "Enemy.h"

void Enemy::Initailize(Model* model, const Vector3& position) {
	assert(model);

	model_ = model;
	//	テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemy.png");

	debugText_ = DebugText::GetInstance();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	Fire();
}

void Enemy::ApproechMove() {
	// 速度設定
	Vector3 approchMove;
	const float enemySpeed = 0.2f;
	approchMove = { 0,0,-enemySpeed };

	// 移動（ベクトルを加算）
	worldTransform_.translation_ += approchMove;
	// 規定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::LeaveMove() {
	// 速度設定
	Vector3 leaveMove;
	const float enemySpeed = 0.2f;
	leaveMove = { -0.1f,0.1f,-enemySpeed };

	// 移動（ベクトルを加算）
	worldTransform_.translation_ += leaveMove;
}

void Enemy::Fire() {
		// 自キャラの座標をコピー
	const float kBulletSpeed = 2.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	// 弾を生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique <EnemyBullet>();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);
	newBullet->IsDead();

	// 弾を登録する
	bullets_.push_back(std::move(newBullet));

	// デバック文字
	debugText_->SetPos(50, 80);
	debugText_->GetInstance()->Printf(
		"Enemybullet:%f",
		velocity);
}

void Enemy::Update(Affine* affine) {
	// switch文でフェーズ分け
	switch (phase_) {
	case Phase::Approach:
	default:
		ApproechMove();
		Fire();
		break;
	case Phase::Leave:
		LeaveMove();
		break;
	}

	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = affine->CreateMatrix(worldTransform_);
	worldTransform_.TransferMatrix(); // 行列の転送

	// デバック文字
	debugText_->SetPos(50, 40);
	debugText_->GetInstance()->Printf(
		"Enemy(%f,%f,%f)",
		worldTransform_.translation_.x,
		worldTransform_.translation_.y,
		worldTransform_.translation_.z);
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}