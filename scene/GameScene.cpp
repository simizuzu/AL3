#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg");

	// x.y,z軸周りの平行移動を指定
	worldTransform_[0].translation_ = { 0.0f, 5.0f, 0.0f };
	worldTransform_[1].translation_ = { -4.330127f, -2.5f, 0.0f };
	worldTransform_[2].translation_ = { 4.330127, -2.5f, 0.0f };

	//ワールドトランスフォーム初期化
	worldTransform_[0].Initialize();
	worldTransform_[1].Initialize();
	worldTransform_[2].Initialize();


	//カメラ視点座標を設定
	viewProjection_[0].eye = { 0.0f, 0.0f, -25.0f };
	viewProjection_[1].eye = { 0.0f, 0.0f, -25.0f };
	viewProjection_[2].eye = { 0.0f, 0.0f, -25.0f };

	//カメラ注視点座標を設定
	viewProjection_[0].target = { 0.0f, 5.0f, 0.0f };
	viewProjection_[1].target = { -4.330127, -2.5f, 0.0f };
	viewProjection_[2].target = { 4.330127, -2.5f, 0.0f };

	//上方向ベクトルを指定
	viewProjection_[0].up = { 0.0f, 1.0f, 0.0f };
	viewProjection_[1].up = { 0.0f, 1.0f, 0.0f };
	viewProjection_[2].up = { 0.0f, 1.0f, 0.0f };

	//ビュープロジェクション初期化
	viewProjection_[0].Initialize();
	viewProjection_[1].Initialize();
	viewProjection_[2].Initialize();
}

void GameScene::Update() {
	if (spacekey == false && input_->PushKey(DIK_SPACE)) {
		num++;
		spacekey = true;
	}
	else if (!input_->PushKey(DIK_SPACE)) {
		spacekey = false;
	}

	if (num > 2) {
		num = 0;
	}

	//デバッグ用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf("eye:(%f,%f,%f)", viewProjection_[0].eye.x, viewProjection_[0].eye.y, viewProjection_[0].eye.z);
	debugText_->SetPos(50, 70);
	debugText_->Printf("target:(%f,%f,%f)", viewProjection_[num].target.x, viewProjection_[num].target.y, viewProjection_[num].target.z);
	debugText_->SetPos(50, 90);
	debugText_->Printf("up:(%f,%f,%f)", viewProjection_[0].up.x, viewProjection_[0].up.y, viewProjection_[0].up.z);
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
	model_ = Model::Create();
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	model_->Draw(worldTransform_[0], viewProjection_[num], textureHandle_);
	model_->Draw(worldTransform_[1], viewProjection_[num], textureHandle_);
	model_->Draw(worldTransform_[2], viewProjection_[num], textureHandle_);

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
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}