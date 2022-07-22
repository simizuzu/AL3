#include "Skydome.h"

void Skydome::Initialize() {
	// ワールド変換の初期化
	worldTransform_.Initialize();
	// スケールを設定
	worldTransform_.scale_ = { 250.0f,250.0f,250.0f };
	// スケーリング行列
	worldTransform_.matWorld_ = math::UpdateMatrix(worldTransform_);
	// 行列の転送
	worldTransform_.TransferMatrix();

	model_ =  Model::CreateFromOBJ("skydome", true);
}

void Skydome::Update() {}

void Skydome::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}
