#include <Novice.h>
#include <Windows.h>
#include <imgui.h>
#include <cmath>
#include <cstring>
#include <cstdint>

const char kWindowTitle[] = "GC2C_02_ゴ_チュウ";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;
const float kPi = 3.14159265358979323846f;

// 3次元ベクトル
struct Vector3 {
	float x;
	float y;
	float z;
};

// 4x4行列
struct Matrix4x4 {
	float m[4][4];
};

// AABB
struct AABB {
	Vector3 min;
	Vector3 max;
};

// 球
struct Sphere {
	Vector3 center;
	float radius;
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

// 内積
float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// 長さ
float Length(const Vector3& v) {
	return std::sqrt(Dot(v, v));
}

// スカラー倍
Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 result{};

	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;

	return result;
}

// 正規化
Vector3 Normalize(const Vector3& v) {
	float length = Length(v);

	if (length == 0.0f) {
		return { 0.0f, 0.0f, 0.0f };
	}

	return { v.x / length, v.y / length, v.z / length };
}

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}

// minとmaxの入れ替わりを直す
void FixAABB(AABB& aabb) {
	float minX = aabb.min.x < aabb.max.x ? aabb.min.x : aabb.max.x;
	float minY = aabb.min.y < aabb.max.y ? aabb.min.y : aabb.max.y;
	float minZ = aabb.min.z < aabb.max.z ? aabb.min.z : aabb.max.z;
	float maxX = aabb.min.x > aabb.max.x ? aabb.min.x : aabb.max.x;
	float maxY = aabb.min.y > aabb.max.y ? aabb.min.y : aabb.max.y;
	float maxZ = aabb.min.z > aabb.max.z ? aabb.min.z : aabb.max.z;

	aabb.min = { minX, minY, minZ };
	aabb.max = { maxX, maxY, maxZ };
}

// 範囲内に値を収める
float Clamp(float value, float min, float max) {
	if (value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}

	return value;
}

// AABBと球の衝突判定
bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	Vector3 closestPoint{
		Clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		Clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		Clamp(sphere.center.z, aabb.min.z, aabb.max.z),
	};

	float distance = Length(Subtract(closestPoint, sphere.center));

	if (distance <= sphere.radius) {
		return true;
	}

	return false;
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			for (int i = 0; i < 4; i++) {
				result.m[row][column] += matrix1.m[row][i] * matrix2.m[i][column];
			}
		}
	}

	return result;
}

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};

	float w = 0.0f;

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

	if (w != 0.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}

	return result;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result{};

	result.m[0][0] = scale.x;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{};

	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
}

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result{};

	float sin = std::sin(radian);
	float cos = std::cos(radian);

	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = cos;
	result.m[1][2] = sin;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = -sin;
	result.m[2][2] = cos;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result{};

	float sin = std::sin(radian);
	float cos = std::cos(radian);

	result.m[0][0] = cos;
	result.m[0][1] = 0.0f;
	result.m[0][2] = -sin;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = sin;
	result.m[2][1] = 0.0f;
	result.m[2][2] = cos;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result{};

	float sin = std::sin(radian);
	float cos = std::cos(radian);

	result.m[0][0] = cos;
	result.m[0][1] = sin;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = -sin;
	result.m[1][1] = cos;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	return Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
}

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& matrix) {
	Matrix4x4 result{};

	result.m[0][0] = matrix.m[0][0];
	result.m[0][1] = matrix.m[1][0];
	result.m[0][2] = matrix.m[2][0];
	result.m[0][3] = 0.0f;

	result.m[1][0] = matrix.m[0][1];
	result.m[1][1] = matrix.m[1][1];
	result.m[1][2] = matrix.m[2][1];
	result.m[1][3] = 0.0f;

	result.m[2][0] = matrix.m[0][2];
	result.m[2][1] = matrix.m[1][2];
	result.m[2][2] = matrix.m[2][2];
	result.m[2][3] = 0.0f;

	result.m[3][0] = -(matrix.m[3][0] * result.m[0][0] + matrix.m[3][1] * result.m[1][0] + matrix.m[3][2] * result.m[2][0]);
	result.m[3][1] = -(matrix.m[3][0] * result.m[0][1] + matrix.m[3][1] * result.m[1][1] + matrix.m[3][2] * result.m[2][1]);
	result.m[3][2] = -(matrix.m[3][0] * result.m[0][2] + matrix.m[3][1] * result.m[1][2] + matrix.m[3][2] * result.m[2][2]);
	result.m[3][3] = 1.0f;

	return result;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result{};

	float cot = 1.0f / std::tan(fovY / 2.0f);

	result.m[0][0] = cot / aspectRatio;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = cot;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	result.m[3][3] = 0.0f;

	return result;
}

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result{};

	result.m[0][0] = width / 2.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[2][3] = 0.0f;

	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;

	return result;
}

// Gridの描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + kGridEvery * float(xIndex);

		Vector3 start{ x, 0.0f, -kGridHalfWidth };
		Vector3 end{ x, 0.0f, kGridHalfWidth };
		start = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		end = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		uint32_t color = 0xAAAAAAFF;
		if (xIndex == kSubdivision / 2) {
			color = 0x000000FF;
		}

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), color);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + kGridEvery * float(zIndex);

		Vector3 start{ -kGridHalfWidth, 0.0f, z };
		Vector3 end{ kGridHalfWidth, 0.0f, z };
		start = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		end = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		uint32_t color = 0xAAAAAAFF;
		if (zIndex == kSubdivision / 2) {
			color = 0x000000FF;
		}

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), color);
	}
}

// AABBの描画
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 vertices[8]{
		{ aabb.min.x, aabb.min.y, aabb.min.z },
		{ aabb.max.x, aabb.min.y, aabb.min.z },
		{ aabb.min.x, aabb.max.y, aabb.min.z },
		{ aabb.max.x, aabb.max.y, aabb.min.z },
		{ aabb.min.x, aabb.min.y, aabb.max.z },
		{ aabb.max.x, aabb.min.y, aabb.max.z },
		{ aabb.min.x, aabb.max.y, aabb.max.z },
		{ aabb.max.x, aabb.max.y, aabb.max.z },
	};

	for (int32_t index = 0; index < 8; ++index) {
		vertices[index] = Transform(Transform(vertices[index], viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(int(vertices[0].x), int(vertices[0].y), int(vertices[1].x), int(vertices[1].y), color);
	Novice::DrawLine(int(vertices[0].x), int(vertices[0].y), int(vertices[2].x), int(vertices[2].y), color);
	Novice::DrawLine(int(vertices[1].x), int(vertices[1].y), int(vertices[3].x), int(vertices[3].y), color);
	Novice::DrawLine(int(vertices[2].x), int(vertices[2].y), int(vertices[3].x), int(vertices[3].y), color);

	Novice::DrawLine(int(vertices[4].x), int(vertices[4].y), int(vertices[5].x), int(vertices[5].y), color);
	Novice::DrawLine(int(vertices[4].x), int(vertices[4].y), int(vertices[6].x), int(vertices[6].y), color);
	Novice::DrawLine(int(vertices[5].x), int(vertices[5].y), int(vertices[7].x), int(vertices[7].y), color);
	Novice::DrawLine(int(vertices[6].x), int(vertices[6].y), int(vertices[7].x), int(vertices[7].y), color);

	Novice::DrawLine(int(vertices[0].x), int(vertices[0].y), int(vertices[4].x), int(vertices[4].y), color);
	Novice::DrawLine(int(vertices[1].x), int(vertices[1].y), int(vertices[5].x), int(vertices[5].y), color);
	Novice::DrawLine(int(vertices[2].x), int(vertices[2].y), int(vertices[6].x), int(vertices[6].y), color);
	Novice::DrawLine(int(vertices[3].x), int(vertices[3].y), int(vertices[7].x), int(vertices[7].y), color);
}

// Sphereの描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = 2.0f * kPi / float(kSubdivision);
	const float kLatEvery = kPi / float(kSubdivision);

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -kPi / 2.0f + kLatEvery * float(latIndex);

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = kLonEvery * float(lonIndex);

			Vector3 a{};
			Vector3 b{};
			Vector3 c{};

			a.x = sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon);
			a.y = sphere.center.y + sphere.radius * std::sin(lat);
			a.z = sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon);

			b.x = sphere.center.x + sphere.radius * std::cos(lat + kLatEvery) * std::cos(lon);
			b.y = sphere.center.y + sphere.radius * std::sin(lat + kLatEvery);
			b.z = sphere.center.z + sphere.radius * std::cos(lat + kLatEvery) * std::sin(lon);

			c.x = sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon + kLonEvery);
			c.y = sphere.center.y + sphere.radius * std::sin(lat);
			c.z = sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon + kLonEvery);

			a = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			b = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			c = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);

			Novice::DrawLine(int(a.x), int(a.y), int(b.x), int(b.y), color);
			Novice::DrawLine(int(a.x), int(a.y), int(c.x), int(c.y), color);
		}
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

	AABB aabb1{
		{ -0.5f, -0.5f, -0.5f },
		{ 0.0f, 0.0f, 0.0f },
	};
	Sphere sphere{ { 1.0f, 0.74f, 1.0f }, 1.0f };

	while (Novice::ProcessMessage() == 0) {

		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		const float kCameraMoveSpeed = 0.05f;
		const float kCameraRotateSpeed = 0.02f;

		if (keys[DIK_W]) {
			cameraTranslate.z += kCameraMoveSpeed;
		}
		if (keys[DIK_S]) {
			cameraTranslate.z -= kCameraMoveSpeed;
		}
		if (keys[DIK_A]) {
			cameraTranslate.x -= kCameraMoveSpeed;
		}
		if (keys[DIK_D]) {
			cameraTranslate.x += kCameraMoveSpeed;
		}
		if (keys[DIK_Q]) {
			cameraTranslate.y -= kCameraMoveSpeed;
		}
		if (keys[DIK_E]) {
			cameraTranslate.y += kCameraMoveSpeed;
		}

		if (keys[DIK_LEFT]) {
			cameraRotate.y -= kCameraRotateSpeed;
		}
		if (keys[DIK_RIGHT]) {
			cameraRotate.y += kCameraRotateSpeed;
		}
		if (keys[DIK_UP]) {
			cameraRotate.x -= kCameraRotateSpeed;
		}
		if (keys[DIK_DOWN]) {
			cameraRotate.x += kCameraRotateSpeed;
		}

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		ImGui::Begin("Window");
		ImGui::DragFloat3("aabb1.min", &aabb1.min.x, 0.01f);
		ImGui::DragFloat3("aabb1.max", &aabb1.max.x, 0.01f);
		ImGui::DragFloat3("sphere.center", &sphere.center.x, 0.01f);
		ImGui::DragFloat("sphere.radius", &sphere.radius, 0.01f);
		ImGui::InputFloat3("Camera.Translate", &cameraTranslate.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Camera.Rotate", &cameraRotate.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::End();

		FixAABB(aabb1);
		if (sphere.radius < 0.0f) {
			sphere.radius = 0.0f;
		}

		bool isCollision = IsCollision(aabb1, sphere);

		DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, isCollision ? RED : WHITE);
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, isCollision ? RED : WHITE);

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}

		Novice::EndFrame();
	}

	Novice::Finalize();

	return 0;
}
