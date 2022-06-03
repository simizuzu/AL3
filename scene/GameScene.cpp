#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>
#include "Affine.h"

// クラス呼び出し
Affine* worldTransUpdate = nullptr;
Affine* createMatrix = nullptr;

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

		//横移動(押した方向で移動ベクトルを変更)
		if (input_->PushKey(DIK_LEFT)) {
			move = { -kCharacterSpeed,0,0 };
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			move = { kCharacterSpeed,0,0 };
		}

		// 上半身回転処理
		{
			// 押した方向で移動ベクトルを変更
			if (input_->PushKey(DIK_U)) {
				worldTransforms_[PartId::kChest].rotation_.y -= 0.05f;
			}else if (input_->PushKey(DIK_I)) {
				worldTransforms_[PartId::kChest].rotation_.y += 0.05f;
			}
		}

		// 下半身回転処理
		{
			// 押した方向で移動ベクトルを変更
			if (input_->PushKey(DIK_J)) {
				worldTransforms_[PartId::kHip].rotation_.y -= 0.05f;
			}
			else if (input_->PushKey(DIK_K)) {
				worldTransforms_[PartId::kHip].rotation_.y += 0.05f;
			}
		}

		worldTransforms_[PartId::kRoot].translation_ += move;
		worldTransforms_[PartId::kRoot].matWorld_ = createMatrix->CreateMatrix(worldTransforms_[PartId::kRoot]);
		worldTransforms_[PartId::kRoot].TransferMatrix(); // 行列の転送

		debugText_->SetPos(50, 150);
		debugText_->Printf(
			"Root:(%f,%f,%f)",
			worldTransforms_[PartId::kRoot].translation_.x,
			worldTransforms_[PartId::kRoot].translation_.y,
			worldTransforms_[PartId::kRoot].translation_.z);
	}

	//	大元から順に更新していく
	for (int i = 1; i < 9; i++) {
		worldTransUpdate->WorldTransUpdate(worldTransforms_[i]);
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
		if (i < 2) {
			continue;
		}
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
