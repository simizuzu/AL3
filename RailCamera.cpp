#include "RailCamera.h"

void RailCamera::Initialize(const Vector3& position, const Vector3& rotation) {
	// ワールドトランスフォームの初期設定
	worldTransform_.translation_ = position;
	// 回転角を設定
	worldTransform_.rotation_ = rotation;

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 2000.0f;
	viewProjection_.Initialize();

	// シングルインスタンスを取得する
	debugText_ = DebugText::GetInstance();
}

void RailCamera::Update() {
	using namespace MathUtility;
	
	// ワールドトランスフォームの座標の数値を加算
	worldTransform_.translation_ += Vector3(0, 0, 0.1f);
	// ワールドトランスフォームを更新
	worldTransform_.matWorld_ = math::UpdateMatrix(worldTransform_);
	// ワールド行列の平行移動成分を取得
	viewProjection_.eye.x = worldTransform_.matWorld_.m[3][0];
	viewProjection_.eye.y = worldTransform_.matWorld_.m[3][1];
	viewProjection_.eye.z = worldTransform_.matWorld_.m[3][2];

	// ワールド前方ベクトル
	Vector3 forward(0, 0, 1);
	// レールカメラの回転を反映
	forward = math::VecMatMul2(forward, worldTransform_.matWorld_);

	// 視点から前方に適当な距離進んだ位置が注意点
	viewProjection_.target = viewProjection_.eye + forward;
	// ワールド上方ベクトル
	Vector3 up(0, 1, 0);
	// レールカメラの回転を反映（レールカメラの上方ベクトル）
	viewProjection_.up = math::VecMatMul2(up, worldTransform_.matWorld_);

	// ビュープロジェクションを更新
	viewProjection_.UpdateMatrix();

	// デバック文字
	debugText_->SetPos(20, 120);
	debugText_->GetInstance()->Printf(
		"RailCamera Pos:(%f,%f,%f)",
		viewProjection_.eye.x,
		viewProjection_.eye.y,
		viewProjection_.eye.z);
}



