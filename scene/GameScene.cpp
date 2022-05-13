#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"

Matrix4 Identity() {
	// 単位行列を設定
	Matrix4 matIdentity;
	matIdentity.m[0][0] = 1.0f;
	matIdentity.m[1][1] = 1.0f;
	matIdentity.m[2][2] = 1.0f;
	matIdentity.m[3][3] = 1.0f;

	return matIdentity;
}

// スケーリング行列を設定
Matrix4 Scaling(Vector3 scale) {
	Matrix4 matScale;
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;
	matScale.m[3][3] = 1.0f;

	return matScale;
}

//回転行列を設定
Matrix4 RotationX(float angle) {
	//X軸周りの回転
	Matrix4 matRotX;
	matRotX.m[1][1] = cos(angle);
	matRotX.m[1][2] = -sin(angle);

	matRotX.m[2][1] = sin(angle);
	matRotX.m[2][2] = cos(angle);

	matRotX.m[0][0] = 1.0f;
	matRotX.m[3][3] = 1.0f;

	return matRotX;
}

Matrix4 RotationY(float angle) {
	//Y軸周りの回転
	Matrix4 matRotY;
	matRotY.m[0][0] = cos(angle);
	matRotY.m[0][2] = sin(angle);

	matRotY.m[2][0] = -sin(angle);
	matRotY.m[2][2] = cos(angle);
	
	matRotY.m[1][1] = 1.0f;
	matRotY.m[3][3] = 1.0f;

	return matRotY;
}

Matrix4 RotationZ(float angle) {
	//Z軸回りの回転
	Matrix4 matRotZ;
	matRotZ.m[0][0] = cos(angle);
	matRotZ.m[0][1] = sin(angle);

	matRotZ.m[1][1] = -sin(angle);
	matRotZ.m[1][0] = cos(angle);

	matRotZ.m[2][2] = 1.0f;
	matRotZ.m[3][3] = 1.0f;

	return matRotZ;
}

float PI = 3.14f;
float DegreeMethod(float degree) {
	return degree * PI / 180.0;
}

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する (アドレス渡し)
	AxisIndicator::SetTargetViewProjection(&debugCamera_->GetViewProjection());

	// ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

	//// X, Y, Z 方向のスケーリングを設定
	//worldTransform_.scale_ = { 5,1,1 };
	//// スケーリング行列を宣言
	//Matrix4 matScale = Scaling(worldTransform_.scale_);
	//// 単位行列の代入
	//worldTransform_.matWorld_ = Identity();
	//// matScaleの代入
	//worldTransform_.matWorld_ *= matScale;

	// X, Y, Z 軸回りの回転角を設定
	worldTransform_.rotation_ = { 0.0f ,DegreeMethod(45.0f) ,0.0f};
	// 合成用回転行列を宣言
	Matrix4 matRot = Identity(); // 単位行列を代入
	// 各軸回転行列を宣言
	Matrix4 matRotX = RotationX(worldTransform_.rotation_.x);
	Matrix4 matRotY = RotationY(worldTransform_.rotation_.y);
	Matrix4 matRotZ = RotationZ(worldTransform_.rotation_.z);

	
	// 角回転行列の各要素を設定
	matRot *= matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;
	
	// 単位行列の代入
	worldTransform_.matWorld_ = Identity();
	// matScaleの代入
	worldTransform_.matWorld_ *= matRot;

	// 行列の転送
	worldTransform_.TransferMatrix();
}

void GameScene::Update() {
	debugCamera_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデル描画
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);
	// ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	//PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3{ 0,0,0 }, Vector3{ 100,100,100 }, Vector4{ 0xff,0x00,0x00,0xff });

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
