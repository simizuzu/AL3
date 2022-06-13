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

void Player::Move() {

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
		move = { 0, kCharacterSpeed,0 };
	}
	else if (input_->PushKey(DIK_DOWN)) {
		move = { 0, -kCharacterSpeed,0 };
	}

	worldTransform_.translation_ += move;
}

void Player::ScreenOut() {

	// 移動限界座標
	const float kMoveLimitX = 35;
	const float kMoveLimitY = 18;

	// 範囲を超えない処理 worldTransform_.translation_値に制限を描ける
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);
}

void Player::Rotate() {

	const float roataionSpeed = 0.05f;

	// 自キャラの旋回
	if (input_->PushKey(DIK_J)) {
		worldTransform_.rotation_.y -= roataionSpeed;
	}
	else if (input_->PushKey(DIK_K)) {
		worldTransform_.rotation_.y += roataionSpeed;
	}
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {

		// 自キャラの座標をコピー
		Vector3 position = worldTransform_.translation_;

		// 弾を生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique <PlayerBullet>();
		newBullet->Initialize(model_, position);

		// 弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}

void Player::Update(Affine* affine) {
	// キャラクター移動処理
	Move();
	// キャラクター旋回処理
	Rotate();

	// 移動限界
	ScreenOut();

	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = affine->CreateMatrix(worldTransform_);
	worldTransform_.TransferMatrix(); // 行列の転送

	// キャラクター攻撃処理
	Attack();

	// 弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) { // if (bullet_ != nullptr)
		bullet->Update(affine);
	}

	// デバック文字
	debugText_->SetPos(50, 150);
	debugText_->GetInstance()->Printf(
		"Player(%f,%f,%f)",
		worldTransform_.translation_.x,
		worldTransform_.translation_.y,
		worldTransform_.translation_.z);
}

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) { // if (bullet_ != nullptr)
		bullet->Draw(viewProjection);
	}
}

