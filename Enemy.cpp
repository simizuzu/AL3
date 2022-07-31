#include "Enemy.h"
#include "Player.h"

void Enemy::Initailize(Model* model, const Vector3& position) {
	assert(model);

	model_ = model;
	//	テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemy.png");

	debugText_ = DebugText::GetInstance();

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	ApproschInitislize();
}

void Enemy::OnCollision(){}

void Enemy::ApproschInitislize() {
	// 発射タイマーの初期化
	fireTimer = kFireInterbal;
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
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

	// 発射タイマーカウントダウン
	fireTimer--;
	// 指定時間に達した
	if (fireTimer < 0) {
		// 弾を発射
		Fire();
		// 発射タイマーを初期化
		fireTimer = kFireInterbal;
	}

	debugText_->SetPos(50, 100);
	debugText_->GetInstance()->Printf(
		"fierTimer:(%f)", fireTimer
	);
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
	assert(player_);

	// 自キャラの座標をコピー
	const float kBulletSpeed = 1.0f;

	// 自キャラのワールド座標を取得する
	Vector3 playerPos = player_->GetWorldPosition();
	// 敵キャラのワールド座標を取得する
	Vector3 enemyPos = GetWorldPosition();
	// 差分ベクトル
	Vector3 velocity =  playerPos - enemyPos;
	// 正規化
	velocity = MathUtility::Vector3Normalize(velocity);
	// ベクトルの長さを、速さに合わせる
	velocity *=  -kBulletSpeed;

	// 弾を生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique <EnemyBullet>();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);
}

void Enemy::Update() {

	// switch文でフェーズ分け
	switch (phase_) {
	case Phase::Approach:
	default:
		ApproechMove();
		//Fire();
		break;
	case Phase::Leave:
		LeaveMove();
		break;
	}


	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = math::UpdateMatrix(worldTransform_);
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