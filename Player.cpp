#include "Player.h"

void Player::Initailize(Model* model, uint32_t textureHandle) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void Player::Move(Affine* createMatrix) {

	// キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// キーボード入力による移動処理
	// 横移動(押した方向で移動ベクトルを変更)
	if (input_->PushKey(DIK_LEFT)) {
		move = { -kCharacterSpeed,0,0 };
	}
	else if (input_->PushKey(DIK_RIGHT)) {
		move = { kCharacterSpeed,0,0 };
	}

	// 上下移動(押した方向で移動ベクトルを変更)
	if (input_->PushKey(DIK_UP)) {
		move = {0, kCharacterSpeed,0 };
	}
	else if (input_->PushKey(DIK_DOWN)) {
		move = { 0, -kCharacterSpeed,0 };
	}

	// 移動限界座標
	const float kMoveLimitX = WinApp::kWindowWidth;
	const float kMoveLimitX = WinApp::kWindowHeight;

	worldTransform_.translation_ += move;
	worldTransform_.matWorld_ = createMatrix->CreateMatrix(worldTransform_);
	worldTransform_.TransferMatrix(); // 行列の転送
}

void Player::Update(Affine* createMatrix) {
	Move(createMatrix);

	debugText_->SetPos(50, 150);
	debugText_->GetInstance()->Printf(
		"Player(%f,%f,%f)",
		worldTransform_.translation_.x,
		worldTransform_.translation_.y,
		worldTransform_.translation_.z);
}

void Player::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);
}

