#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"AxisIndicator.h"
#include"PrimitiveDrawer.h"

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
	Matrix4 matTrans = Matrix4Identity();

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {

			//ワールドトランスフォーム初期化
			worldTransform_[i][j].Initialize();

			//X,Y,Z方向のスケーリングを設定
			SetMatScale(matScale, { 1.0f,1.0f,1.0f });
			//X,Y,Z軸周りの回転角を乱数で設定
			SetMatRotX(matRotX, 0.0f);
			SetMatRotY(matRotY, 0.0f);
			SetMatRotZ(matRotZ, 0.0f);
			//X,Y,Z軸周りの平行移動(座標)を設定
			SetMatTrans(matTrans, { -16 + i * 4.0f,-16 + j * 4.0f,0 });

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

	//デバッグカメラ生成
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
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
	if (input_->PushKey(DIK_A)) {
		viewProjection_.target.x -= 0.2f;
	}if (input_->PushKey(DIK_D)) {
		viewProjection_.target.x += 0.2f;
	}if (input_->PushKey(DIK_W)) {
		viewProjection_.target.y += 0.2f;
	}if (input_->PushKey(DIK_S)) {
		viewProjection_.target.y -= 0.2f;
	}

	//ズームイン,アウト
	if (input_->PushKey(DIK_UP)) {
		viewProjection_.fovAngleY -= 0.01f;
	}if (input_->PushKey(DIK_DOWN)) {
		viewProjection_.fovAngleY += 0.01f;
	}

	if (viewProjection_.fovAngleY < 0.01f) {
		viewProjection_.fovAngleY = 0.01f;
	}if (viewProjection_.fovAngleY > 3.141592654f) {
		viewProjection_.fovAngleY = 3.141592654f;
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

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}