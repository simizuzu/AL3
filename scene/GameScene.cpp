#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg");

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());

	//乱数範囲(座標用)
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	//ワールドトランスフォーム初期化
	worldTransform_.Initialize();

	//カメラ視点座標を設定
	viewProjection_[0].eye = { posDist(engine), posDist(engine), posDist(engine) };
	viewProjection_[1].eye = { posDist(engine), posDist(engine), posDist(engine) };
	viewProjection_[2].eye = { posDist(engine), posDist(engine), posDist(engine) };

	//カメラ注視点座標を設定
	viewProjection_[0].target = { 0.0f, 0.0f, 0.0f };
	viewProjection_[1].target = { 0.0f, 0.0f, 0.0f };
	viewProjection_[2].target = { 0.0f, 0.0f, 0.0f };

	//カメラ上方向ベクトルを設定(右上45度指定)
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

	//行列の再計算
	viewProjection_[0].UpdateMatrix();
	viewProjection_[1].UpdateMatrix();
	viewProjection_[2].UpdateMatrix();

	//デバッグ用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf("Camera1");
	debugText_->SetPos(50, 70);
	debugText_->Printf(
		"eye:(%f,%f,%f)", viewProjection_[0].eye.x, viewProjection_[0].eye.y,
		viewProjection_[0].eye.z);
	debugText_->SetPos(50, 90);
	debugText_->Printf(
		"target:(%f,%f,%f)", viewProjection_[0].target.x, viewProjection_[0].target.y,
		viewProjection_[0].target.z);
	debugText_->SetPos(50, 110);
	debugText_->Printf(
		"up:(%f,%f,%f)", viewProjection_[0].up.x, viewProjection_[0].up.y, viewProjection_[0].up.z);

	debugText_->SetPos(50, 150);
	debugText_->Printf("Camera2");
	debugText_->SetPos(50, 170);
	debugText_->Printf(
		"eye:(%f,%f,%f)", viewProjection_[1].eye.x, viewProjection_[1].eye.y,
		viewProjection_[1].eye.z);
	debugText_->SetPos(50, 190);
	debugText_->Printf(
		"target:(%f,%f,%f)", viewProjection_[1].target.x, viewProjection_[1].target.y,
		viewProjection_[1].target.z);
	debugText_->SetPos(50, 210);
	debugText_->Printf(
		"up:(%f,%f,%f)", viewProjection_[1].up.x, viewProjection_[1].up.y, viewProjection_[1].up.z);

	debugText_->SetPos(50, 250);
	debugText_->Printf("Camera3");
	debugText_->SetPos(50, 270);
	debugText_->Printf(
		"eye:(%f,%f,%f)", viewProjection_[2].eye.x, viewProjection_[2].eye.y,
		viewProjection_[2].eye.z);
	debugText_->SetPos(50, 290);
	debugText_->Printf(
		"target:(%f,%f,%f)", viewProjection_[2].target.x, viewProjection_[2].target.y,
		viewProjection_[2].target.z);
	debugText_->SetPos(50, 310);
	debugText_->Printf(
		"up:(%f,%f,%f)", viewProjection_[2].up.x, viewProjection_[2].up.y, viewProjection_[2].up.z);
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
	model_->Draw(worldTransform_, viewProjection_[num], textureHandle_);

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