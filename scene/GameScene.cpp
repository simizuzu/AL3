#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>

float PI = 3.141592654f;
float DegreeMethod(const float& degree) {
	float radian = degree * PI / 180.0f;
	return radian;
}

/// <summary>
/// 単位行列を設定
/// </summary>
/// <returns>単位行列</returns>
Matrix4 Identity() {
	// 単位行列を設定
	Matrix4 matIdentity;
	matIdentity.m[0][0] = 1.0f;
	matIdentity.m[1][1] = 1.0f;
	matIdentity.m[2][2] = 1.0f;
	matIdentity.m[3][3] = 1.0f;

	return matIdentity;
}

/// <summary>
/// スケーリング行列を設定
/// </summary>
/// <param name="scaleX"></param>
/// <param name="scaleY"></param>
/// <param name="scaleZ"></param>
/// <returns>スケーリング行列</returns>
Matrix4 Scaling(Vector3 scale) {
	Matrix4 matScale;
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;
	matScale.m[3][3] = 1.0f;

	return matScale;
}

/// <summary>
/// Z軸の回転行列
/// </summary>
/// <param name="angle"></param>
/// <returns>Z軸の回転行列</returns>
Matrix4 RotationZ(float angle) {
	Matrix4 matRotZ;
	matRotZ.m[0][0] = cosf(angle);
	matRotZ.m[0][1] = sinf(angle);

	matRotZ.m[1][1] = cosf(angle);
	matRotZ.m[1][0] = -sinf(angle);

	matRotZ.m[2][2] = 1.0f;
	matRotZ.m[3][3] = 1.0f;

	return matRotZ;
}

/// <summary>
/// X軸の回転行列
/// </summary>
/// <param name="angle"></param>
/// <returns>X軸の回転行列</returns>
Matrix4 RotationX(float angle) {
	Matrix4 matRotX;
	matRotX.m[1][1] = cosf(angle);
	matRotX.m[1][2] = sinf(angle);

	matRotX.m[2][1] = -sinf(angle);
	matRotX.m[2][2] = cosf(angle);

	matRotX.m[0][0] = 1.0f;

	matRotX.m[3][3] = 1.0f;

	return matRotX;
}

/// <summary>
/// Y軸の回転行列
/// </summary>
/// <param name="angle"></param>
/// <returns>Y軸の回転行列</returns>
Matrix4 RotationY(float angle) {
	Matrix4 matRotY;
	matRotY.m[0][0] = cosf(angle);
	matRotY.m[0][2] = -sinf(angle);

	matRotY.m[2][0] = sinf(angle);
	matRotY.m[2][2] = cosf(angle);

	matRotY.m[1][1] = 1.0f;
	matRotY.m[3][3] = 1.0f;

	return matRotY;
}

/// <summary>
/// 平行移動行列の宣言
/// </summary>
/// <param name="tranlationX"></param>
/// <param name="tranlationY"></param>
/// <param name="tranlationZ"></param>
/// <returns>平行移動行列</returns>
Matrix4 Translation(Vector3 translation) {
	Matrix4 matTrans;
	//単位行列を代入
	matTrans = Identity();

	matTrans.m[3][0] = translation.x;
	matTrans.m[3][1] = translation.y;
	matTrans.m[3][2] = translation.z;

	return matTrans;
}

/// <summary>
/// ワールド行列の計算(Vector3)
/// </summary>
/// <param name="scale">スケール</param>
/// <param name="rotation">回転</param>
/// <param name="transform">平行移動</param>
/// <returns>ワールド行列</returns>
Matrix4 UpdateMatrix(Vector3 scale, Vector3 rotation, Vector3 translation) { // 行列の合成関数
	// 合成用の変数を宣言
	Matrix4 matWorld;

	matWorld = Identity();
	matWorld *= Scaling(scale);
	matWorld *= RotationZ(rotation.z);
	matWorld *= RotationX(rotation.x);
	matWorld *= RotationY(rotation.y);
	matWorld *= Translation(translation);

	return matWorld;
}

/// <summary>
/// ワールド行列の計算(参照渡し)
/// </summary>
/// <param name="worldTransform">スケール, 回転, 平行移動</param>
/// <returns>ワールド行列</returns>
Matrix4 CreateMatrix(const WorldTransform& worldTransform) {
	Matrix4 matWorld;

	matWorld = Identity();
	matWorld *= Scaling(worldTransform.scale_);
	matWorld *= RotationZ(worldTransform.rotation_.z);
	matWorld *= RotationX(worldTransform.rotation_.x);
	matWorld *= RotationY(worldTransform.rotation_.y);
	matWorld *= Translation(worldTransform.translation_);

	return matWorld;
}

/// <summary>
/// 1つ分のワールドトランスフォーム更新関数
/// </summary>
/// <param name="parentWorldTrans">親のワールドトランスフォーム</param>
/// <param name="childWorldtrans">子のワールドトランスフォーム</param>
void WorldTransUpdate(WorldTransform& parentWorldTrans, WorldTransform& childWorldtrans) {

	childWorldtrans.matWorld_ = CreateMatrix(childWorldtrans); // 合成した行列の計算
	childWorldtrans.matWorld_ *= parentWorldTrans.parent_->matWorld_; // parent_のワールド行列の掛け算代入
	childWorldtrans.TransferMatrix(); // 行列の転送
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

	// キャラクターの大元
	worldTransforms_[PartId::kRoot].Initialize();
	// 脊髄
	worldTransforms_[PartId::kSpine].Initialize();
	worldTransforms_[PartId::kSpine].parent_ = &worldTransforms_[PartId::kRoot];
	worldTransforms_[PartId::kSpine].translation_ = { 0.0f,4.5f,0.0f };

	// 上半身
	//Chest
	worldTransforms_[PartId::kChest].Initialize();
	worldTransforms_[PartId::kChest].parent_ = &worldTransforms_[PartId::kSpine];
	worldTransforms_[PartId::kChest].translation_ = { 0.0f, 1.3f, 0.0f };
	//Head
	worldTransforms_[PartId::kHead].Initialize();
	worldTransforms_[PartId::kHead].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kHead].translation_ = { 0.0f,3.0f,0.0f };
	//ArmL
	worldTransforms_[PartId::kArmL].Initialize();
	worldTransforms_[PartId::kArmL].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmL].translation_ = { 3.0f,0.0f,0.0f };
	//ArmR
	worldTransforms_[PartId::kArmR].Initialize();
	worldTransforms_[PartId::kArmR].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmR].translation_ = { -3.0f,0.0f,0.0f };

	// 下半身
	//Hip
	worldTransforms_[PartId::kHip].Initialize();
	worldTransforms_[PartId::kHip].parent_ = &worldTransforms_[PartId::kSpine];
	worldTransforms_[PartId::kHip].translation_ = { 0.0f,-2.0f,0.0f };
	//LegL
	worldTransforms_[PartId::kLegL].Initialize();
	worldTransforms_[PartId::kLegL].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegL].translation_ = { 3.0f,-3.0f,0.0f };
	//LegR
	worldTransforms_[PartId::kLegR].Initialize();
	worldTransforms_[PartId::kLegR].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegR].translation_ = { -3.0f,-3.0f,0.0f };

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する (アドレス渡し)
	AxisIndicator::SetTargetViewProjection(&viewProjection_);

	// ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());
}

void GameScene::Update() {
	debugCamera_->Update();

	// キャラクター移動処理
	// 親の更新
	{
		// キャラクターの移動ベクトル
		Vector3 move = { 0,0,0 };

		// キャラクターの移動速さ
		const float kCharacterSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move = { -kCharacterSpeed,0,0 };
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			move = { kCharacterSpeed,0,0 };
		}

		worldTransforms_[PartId::kSpine].translation_ += move;

		//worldTransforms_[0].matWorld_ = CreateMatrix(worldTransforms_[0]);

		//worldTransforms_[0].TransferMatrix(); // 行列の転送

		debugText_->SetPos(50, 150);
		debugText_->Printf(
			"Root:(%f,%f,%f)",
			worldTransforms_[PartId::kSpine].translation_.x,
			worldTransforms_[PartId::kSpine].translation_.y,
			worldTransforms_[PartId::kSpine].translation_.z);
	}

	//	大元から順に更新していく
	for (int i = 0; i < 9; i++) {
		WorldTransUpdate(worldTransforms_[i], worldTransforms_[i]);
	}
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

	// 大元から順に描画していく
	for (int i = 0; i < 9; i++) {
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
	}

	// ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	//PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3{ 0,0,0 }, Vector3{ 100,100,100 }, Vector4{ 0xff,0x00,0x00,0xff });

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

	//#pragma region 前景スプライト描画
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
