#include "Enemy.h"

void Enemy::Initailize(Model* model, const Vector3& position) {
	assert(model);

	model_ = model;
	//	テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemy.png");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
}

void Enemy::Move() {
	Vector3 move;
	const float enemySpeed = 0.5f;
	move = { 0,0,-enemySpeed };

	worldTransform_.translation_ += move;
}

void Enemy::Update(Affine* affine) {
	// 座標を移動させる（1フレーム分の移動量を足し込む）
	worldTransform_.translation_ += velocity_;

	// 移動処理
	Move();

	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = affine->CreateMatrix(worldTransform_);
	worldTransform_.TransferMatrix(); // 行列の転送
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}