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

	// x.y,z方向のスケーリングを設定
	worldTransform_.scale_ = { 1.0f, 1.0f, 1.0f };

	// x.y,z軸周りの回転角を設定
	worldTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };

	// x.y,z軸周りの平行移動を指定
	worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

	//ワールドトランスフォーム初期化
	worldTransform_.Initialize();

	//カメラ視点座標を設定
	viewProjection_.eye = { 0.0f, 1.0f, 0.0f };

	//カメラ注視点座標を設定
	viewProjection_.target = { 0.0f, 0.0f, 0.0f };

	//カメラ上方向ベクトルを設定
	viewProjection_.up = { 0.0f, 1.0f, 0.0f };

	//ビュープロジェクション初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
	//視点の移動の速さ
	const float kEyeSpeed = -0.02f;

	viewAngle += kEyeSpeed;
	// 2πを超えたら0に戻す
	viewAngle = fmodf(viewAngle, XM_2PI);

	//視点ベクトルを計算(半径1の円周上の座標)
	viewProjection_.eye = { worldTransform_.translation_.x + sinf(viewAngle) * length, 0.0f,worldTransform_.translation_.x + cosf(viewAngle) * length };

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
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

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