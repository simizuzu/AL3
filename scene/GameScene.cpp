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
	delete sprite_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	textureHandleSprite_ = TextureManager::Load("reticle.png");
	textureHandleSprite2_ = TextureManager::Load("scope.png");

	//スプライトの生成
	sprite_ = Sprite::Create(textureHandleSprite_, { 576,296 });
	sprite2_ = Sprite::Create(textureHandleSprite2_, { 0,0 });


	//3Dモデルの生成
	model_ = Model::Create();

	//スケーリング行列を宣言
	Matrix4 matScale;
	//合成用回転行列を宣言
	Matrix4 matRot;
	//各軸用回転行列を宣言
	Matrix4 matRotX, matRotY, matRotZ;
	//平行移動行列宣言
	Matrix4 matTrans = Matrix4Identity();

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

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {

			//ワールドトランスフォーム初期化
			worldTransform_[i][j].Initialize();

			//X,Y,Z方向のスケーリングを設定
			SetMatScale(matScale, { 1.0f,1.0f,1.0f });
			//X,Y,Z軸周りの回転角を設定
			SetMatRotX(matRotX, radian(engine));
			SetMatRotY(matRotY, radian(engine));
			SetMatRotZ(matRotZ, radian(engine));
			//X,Y,Z軸周りの平行移動(座標)を設定
			SetMatTrans(matTrans, { coordinate(engine) ,coordinate(engine) ,coordinate(engine) });

			//worldTransform_.matWorld_に単位行列を代入する
			worldTransform_[i][j].matWorld_.m[0][0] = 1.0f;
			worldTransform_[i][j].matWorld_.m[1][1] = 1.0f;
			worldTransform_[i][j].matWorld_.m[2][2] = 1.0f;
			worldTransform_[i][j].matWorld_.m[3][3] = 1.0f;

			//行列の合成
			//各軸の回転行列を合成
			matRot = matRotZ * matRotX * matRotY;
			//worldTransform_.matWorld_にmatScaleを掛け算して代入
			worldTransform_[i][j].matWorld_ = matScale * matRot * matTrans;

			//行列の転送
			worldTransform_[i][j].TransferMatrix();
		}
	}

	//ビュープロジェクション初期化
	viewProjection_.Initialize();

	//カメラ視点座標を設定
	viewProjection_.eye = { 0.0f, 0.0f, -50.0f };

	//カメラ注視点座標を設定
	viewProjection_.target = { 0.0f, 0.0f, 0.0f };

	//上方向ベクトルを指定
	viewProjection_.up = { 0.0f, 1.0f, 0.0f };

	viewProjection_.fovAngleY = 3.141592654f / 2;
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

	//注視点移動
	if (input_->PushKey(DIK_LEFT)) {
		viewProjection_.target.x -= 0.2f;
	}if (input_->PushKey(DIK_RIGHT)) {
		viewProjection_.target.x += 0.2f;
	}if (input_->PushKey(DIK_UP)) {
		viewProjection_.target.y += 0.2f;
	}if (input_->PushKey(DIK_DOWN)) {
		viewProjection_.target.y -= 0.2f;
	}

	if (input_->TriggerKey(DIK_SPACE)) {
		if (scopeFlag == true) { scopeFlag = false; }
		else { scopeFlag = true; }
	}

	if (scopeFlag == true) {
		if (input_->TriggerKey(DIK_W)) { zoomFlag = true; }
		if (input_->TriggerKey(DIK_S)) { zoomFlag = false; }

		if (zoomFlag == true) { viewProjection_.fovAngleY -= 0.05f; }
		else { viewProjection_.fovAngleY += 0.05f; }
		if (viewProjection_.fovAngleY < 3.141592654f / 10.6f) { viewProjection_.fovAngleY = 3.141592654f / 10.6f; }
		if (viewProjection_.fovAngleY > 3.141592654f / 5.398f) { viewProjection_.fovAngleY = 3.141592654f / 5.398f; }
	}
	else {
		viewProjection_.fovAngleY = 3.141592654f / 2;//初期値
		zoomFlag = false;
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
	debugText_->SetPos(1000, 90);
	if (scopeFlag == true) {
		if (zoomFlag == false) { debugText_->Printf("x4"); }
		if (zoomFlag == true) { debugText_->Printf("x8"); }
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
	//3Dモデル描画
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			model_->Draw(worldTransform_[i][j], viewProjection_, textureHandle_);
		}
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
	if (scopeFlag == true) { sprite_->Draw(); sprite2_->Draw(); }


	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}