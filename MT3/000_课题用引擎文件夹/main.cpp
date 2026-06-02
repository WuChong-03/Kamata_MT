#include <Novice.h>
#include <Windows.h>
#include <cmath>
#include <cstring>

const char kWindowTitle[] = "GC2C_02_ゴ_チュウ";

// 3次元ベクトル
struct Vector3 {
	float x;
	float y;
	float z;
};

// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;
}

// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;
}

// スカラー倍
Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 result{};

	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;

	return result;
}

// 内積
float Dot(const Vector3& v1, const Vector3& v2) {
	float result = 0.0f;

	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

	return result;
}

// 長さ
float Length(const Vector3& v) {
	float result = 0.0f;

	result = sqrtf(Dot(v, v));

	return result;
}

// 正規化
Vector3 Normalize(const Vector3& v) {
	Vector3 result{};

	float length = Length(v);

	if (length != 0.0f) {
		result.x = v.x / length;
		result.y = v.y / length;
		result.z = v.z / length;
	}

	return result;
}

// Vector3の値を画面に表示
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	static const int kColumnWidth = 60;

	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// Novice初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	const int kRowHeight = 20;

	Vector3 v1{ 1.0f, 3.0f, -5.0f };
	Vector3 v2{ 4.0f, -1.0f, 2.0f };

	float k = 4.0f;

	// 各演算結果
	Vector3 resultAdd = Add(v1, v2);
	Vector3 resultSubtract = Subtract(v1, v2);
	Vector3 resultMultiply = Multiply(k, v1);
	float resultDot = Dot(v1, v2);
	float resultLength = Length(v1);
	Vector3 resultNormalize = Normalize(v2);

	// メインループ
	while (Novice::ProcessMessage() == 0) {

		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// 演算結果を表示
		VectorScreenPrintf(0, 0, resultAdd, " : Add");
		VectorScreenPrintf(0, kRowHeight, resultSubtract, " : Subtract");
		VectorScreenPrintf(0, kRowHeight * 2, resultMultiply, " : Multiply");
		Novice::ScreenPrintf(0, kRowHeight * 3, "%.02f : Dot", resultDot);
		Novice::ScreenPrintf(0, kRowHeight * 4, "%.02f : Length", resultLength);
		VectorScreenPrintf(0, kRowHeight * 5, resultNormalize, " : Normalize");

		// ESCで終了
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}

		Novice::EndFrame();
	}

	Novice::Finalize();

	return 0;
}