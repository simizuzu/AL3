#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<random>

using namespace DirectX;

/// <summary>
/// 
/// </summary>
/// <param name="現在のX座標"></param>
/// <param name="現在のY座標"></param>
/// <param name="終点のX座標"></param>
/// <param name="終点のY座標"></param>
/// <param name="スピード(小さいほど早い)"></param>
void Move(float& x1, float& y1, float x2, float y2, float flame) {

	float distanceX = x2 - x1;
	float distanceY = y2 - y1;
	float dividedDistanceX = distanceX / flame;
	float dividedDistanceY = distanceY / flame;

	x1 += dividedDistanceX;
	y1 += dividedDistanceY;
}

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
	viewProjection_.eye = { 0.0f, 0.0f, -25.0f };

	//カメラ注視点座標を設定
	viewProjection_.target = { 0.0f, 5.0f, 0.0f };

	//上方向ベクトルを指定
	viewProjection_.up = { 0.0f, 1.0f, 0.0f };

	//ビュープロジェクション初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
	if (input_->TriggerKey(DIK_SPACE)) {
		flag = true;
		num++;
	}if (num > 2) {
		num = 0;
	}

	switch (num) {
	case 0:
		if (flag == true) {
			Move(viewProjection_.target.x, viewProjection_.target.y, worldTransform_[0].translation_.x, worldTransform_[0].translation_.y, 20);
		}

		break;
	case 1:
		if (flag == true) {
			Move(viewProjection_.target.x, viewProjection_.target.y, worldTransform_[1].translation_.x, worldTransform_[1].translation_.y, 20);
		}
		break;
	case 2:
		if (flag == true) {
			Move(viewProjection_.target.x, viewProjection_.target.y, worldTransform_[2].translation_.x, worldTransform_[2].translation_.y, 20);
		}
		break;
	}

	//行列の再計算
	viewProjection_.UpdateMatrix();

	//デバッグ用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf("eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	debugText_->SetPos(50, 70);
	debugText_->Printf("target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);
	debugText_->SetPos(50, 90);
	debugText_->Printf("up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
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
	model_->Draw(worldTransform_[0], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[1], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[2], viewProjection_, textureHandle_);

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