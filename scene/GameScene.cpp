#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"AxisIndicator.h"
#include"PrimitiveDrawer.h"
#include <random>

using namespace MathUtility;


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

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seed_gen());
	//回転角用の乱数範囲を設定
	std::uniform_real_distribution<float>radian(0, 3.141592654);
	//乱数エンジンを渡し、指定範囲からランダムな数値を得る
	float matRotation = radian(engine);
	//座標用の乱数範囲を設定
	std::uniform_real_distribution<float>coordinate(-10, 10);
	//乱数エンジンを渡し、指定範囲からランダムな数値を得る
	float matTranslation = coordinate(engine);

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	//3Dモデルの生成
	model_ = Model::Create();

	//スケーリング行列を宣言
	Matrix4 matScale;
	//合成用回転行列を宣言
	Matrix4 matRot;
	//各軸用回転行列を宣言
	Matrix4 matRotX, matRotY, matRotZ;
	//平行移動行列宣言
	Matrix4 matTrans = MathUtility::Matrix4Identity();

	//キャラクター大元
	worldTransforms_[PartID::kRoot].Initialize();
	//脊椎
	worldTransforms_[PartID::kSpine].translation_ = { 0, 5.0f, 0 };
	worldTransforms_[PartID::kSpine].rotation_ = { 0, 0.5f, 0 };
	worldTransforms_[PartID::kSpine].parent_ = &worldTransforms_[PartID::kRoot];
	worldTransforms_[PartID::kSpine].Initialize();

	//上半身
	//Chest
	worldTransforms_[PartID::kChest].translation_ = { 0, 0, 0 };
	worldTransforms_[PartID::kChest].parent_ = &worldTransforms_[PartID::kSpine];
	worldTransforms_[PartID::kChest].Initialize();

	//Head
	worldTransforms_[PartID::kHead].translation_ = { 0, 3.0f, 0 };
	worldTransforms_[PartID::kHead].parent_ = &worldTransforms_[PartID::kChest];
	worldTransforms_[PartID::kHead].Initialize();

	//ArmL
	worldTransforms_[PartID::kArmL].translation_ = { -3.0f, 0, 0 };
	worldTransforms_[PartID::kArmL].parent_ = &worldTransforms_[PartID::kChest];
	worldTransforms_[PartID::kArmL].Initialize();
	//ArmR
	worldTransforms_[PartID::kArmR].translation_ = { 3.0f, 0, 0 };
	worldTransforms_[PartID::kArmR].parent_ = &worldTransforms_[PartID::kChest];
	worldTransforms_[PartID::kArmR].Initialize();

	//下半身
	//Hip
	worldTransforms_[PartID::kHip].translation_ = { 0, -3.0f, 0 };
	worldTransforms_[PartID::kHip].parent_ = &worldTransforms_[PartID::kSpine];
	worldTransforms_[PartID::kHip].Initialize();

	//LegL
	worldTransforms_[PartID::kLegL].translation_ = { -3.0f, -3.0f, 0 };
	worldTransforms_[PartID::kLegL].parent_ = &worldTransforms_[PartID::kHip];
	worldTransforms_[PartID::kLegL].Initialize();

	//LegR
	worldTransforms_[PartID::kLegR].translation_ = { 3.0f, -3.0f, 0 };
	worldTransforms_[PartID::kLegR].parent_ = &worldTransforms_[PartID::kHip];
	worldTransforms_[PartID::kLegR].Initialize();

	//ビュープロジェクション初期化
	viewProjection_.Initialize();

	//カメラ視点座標を設定
	viewProjection_.eye = { 0.0f, 3.0f, -30.0f };

	//カメラ注視点座標を設定
	viewProjection_.target = { 0.0f, 3.0f, 0.0f };

	//上方向ベクトルを指定
	viewProjection_.up = { 0.0f, 1.0f, 0.0f };

	viewProjection_.fovAngleY = 3.141592654f / 4;


	//デバッグカメラ生成
	debugCamera_ = new DebugCamera(1280, 720);

	////軸方向表示の表示を有効にする
	//AxisIndicator::GetInstance()->SetVisible(true);
	////軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	//AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	////ライン描画が参照するビュープロジェクションを指定する(アドレス渡し)
	//PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());
}

void GameScene::Update() {
	//デバッグ時のみ有効
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_B)) {
		if (isDebugCameraActive_) {
			isDebugCameraActive_ = false;
		}
		else {
			isDebugCameraActive_ = true;
		}
	}
#endif
	if (isDebugCameraActive_) {
		//デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}
	else
	{
		viewProjection_.UpdateMatrix();
		viewProjection_.TransferMatrix();
	}

	//キャラクター移動処理
		//キャラクターの移動ベクトル
	Vector3 move = { 0.0f ,0.0f ,0.0f };
	float moveSpeed = 0.2f;
	if (input_->PushKey(DIK_LEFT)) {
		move = { -moveSpeed,0.0f ,0.0f };
	}if (input_->PushKey(DIK_RIGHT)) {
		move = { moveSpeed,0.0f ,0.0f };
	}

	worldTransforms_[PartID::kRoot].translation_ += move;
	MatrixCalculation(worldTransforms_[PartID::kRoot]);
	worldTransforms_[PartID::kRoot].TransferMatrix();
	//上半身回転処理
	//回転の早さ
	const float rotSpeed = 0.05f;

	if (rotFlag == false) {
		worldTransforms_[PartID::kArmL].rotation_.x -= rotSpeed;
		worldTransforms_[PartID::kArmR].rotation_.x += rotSpeed;
		worldTransforms_[PartID::kLegL].rotation_.x += rotSpeed;
		worldTransforms_[PartID::kLegR].rotation_.x -= rotSpeed;
	}
	else {
		worldTransforms_[PartID::kArmL].rotation_.x += rotSpeed;
		worldTransforms_[PartID::kArmR].rotation_.x -= rotSpeed;
		worldTransforms_[PartID::kLegL].rotation_.x -= rotSpeed;
		worldTransforms_[PartID::kLegR].rotation_.x += rotSpeed;
	}

	if (worldTransforms_[PartID::kArmL].rotation_.x < -0.8f) {
		rotFlag = true;
	}if (worldTransforms_[PartID::kArmL].rotation_.x > 0.8f) {
		rotFlag = false;
	}

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransforms_[PartID::kRoot].rotation_.y -= rotSpeed;
	}	if (input_->PushKey(DIK_D)) {
		worldTransforms_[PartID::kRoot].rotation_.y += rotSpeed;
	}

	//子の更新大元から順に更新していく
	for (int i = 0; i < PartID::kNumPartID; i++) {
		if (i == 0) {
			continue;
		}
		worldTransforms_[i].ParentChilldrenUpdate();
	}

	//デバック表示
	debugText_->SetPos(50, 50);
	debugText_->Printf("eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	debugText_->SetPos(50, 70);
	debugText_->Printf("target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);
	debugText_->SetPos(50, 90);
	debugText_->Printf("up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
	debugText_->SetPos(50, 110);
	debugText_->Printf("fovAngleY(Degree):%f", ChangeDegree(viewProjection_.fovAngleY));
	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ:%f", viewProjection_.nearZ);
	debugText_->SetPos(50, 150);
	debugText_->Printf("Root:(%f,%f,%f)", worldTransforms_[PartID::kRoot].translation_.x, worldTransforms_[PartID::kRoot].translation_.y, worldTransforms_[PartID::kRoot].translation_.z);
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
	//3Dモデル描画
	for (int i = 0; i < PartID::kNumPartID; i++) {
		if (i < 2) {
			continue;
		}
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
	}

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