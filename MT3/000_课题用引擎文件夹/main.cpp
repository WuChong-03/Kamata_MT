#include <Novice.h>
#include <Windows.h>
#include <cmath>
#include <cstring>

const char kWindowTitle[] = "GC2C_02_ゴ_チュウ";

// 4x4行列
struct Matrix4x4 {
	float m[4][4];
};

// 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}

	return result;
}

// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}

	return result;
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			for (int i = 0; i < 4; i++) {
				result.m[row][column] += m1.m[row][i] * m2.m[i][column];
			}
		}
	}

	return result;
}

// 単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result{};

	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1.0f;
	}

	return result;
}

// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m.m[column][row];
		}
	}

	return result;
}

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result = MakeIdentity4x4();

	float temp[4][8]{};

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			temp[row][column] = m.m[row][column];
		}

		for (int column = 0; column < 4; column++) {
			temp[row][column + 4] = result.m[row][column];
		}
	}

	for (int i = 0; i < 4; i++) {

		int pivotRow = i;

		for (int row = i + 1; row < 4; row++) {
			if (fabsf(temp[row][i]) > fabsf(temp[pivotRow][i])) {
				pivotRow = row;
			}
		}

		if (pivotRow != i) {
			for (int column = 0; column < 8; column++) {
				float swap = temp[i][column];
				temp[i][column] = temp[pivotRow][column];
				temp[pivotRow][column] = swap;
			}
		}

		float pivot = temp[i][i];

		if (pivot == 0.0f) {
			return Matrix4x4{};
		}

		for (int column = 0; column < 8; column++) {
			temp[i][column] /= pivot;
		}

		for (int row = 0; row < 4; row++) {
			if (row != i) {
				float factor = temp[row][i];

				for (int column = 0; column < 8; column++) {
					temp[row][column] -= factor * temp[i][column];
				}
			}
		}
	}

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = temp[row][column + 4];
		}
	}

	return result;
}

// 4x4行列の値を画面に表示
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	static const int kRowHeight = 20;
	static const int kColumnWidth = 60;

	Novice::ScreenPrintf(x, y, "%s", label);

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			Novice::ScreenPrintf(
				x + column * kColumnWidth,
				y + (row + 1) * kRowHeight,
				"%6.02f",
				matrix.m[row][column]
			);
		}
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	const int kRowHeight = 20;
	const int kColumnWidth = 60;

	Matrix4x4 m1 = {
		3.2f, 0.7f, 9.6f, 4.4f,
		5.5f, 1.3f, 7.8f, 2.1f,
		6.9f, 8.0f, 2.6f, 1.0f,
		0.5f, 7.2f, 5.1f, 3.3f
	};

	Matrix4x4 m2 = {
		4.1f, 6.5f, 3.3f, 2.2f,
		8.8f, 0.6f, 9.9f, 7.7f,
		1.1f, 5.5f, 6.6f, 0.0f,
		3.3f, 9.9f, 8.8f, 2.2f
	};

	Matrix4x4 resultAdd = Add(m1, m2);
	Matrix4x4 resultSubtract = Subtract(m1, m2);
	Matrix4x4 resultMultiply = Multiply(m1, m2);
	Matrix4x4 inverseM1 = Inverse(m1);
	Matrix4x4 inverseM2 = Inverse(m2);
	Matrix4x4 transposeM1 = Transpose(m1);
	Matrix4x4 transposeM2 = Transpose(m2);
	Matrix4x4 identity = MakeIdentity4x4();

	while (Novice::ProcessMessage() == 0) {

		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		MatrixScreenPrintf(0, 0, resultAdd, "Add");
		MatrixScreenPrintf(0, kRowHeight * 5, resultSubtract, "Subtract");
		MatrixScreenPrintf(0, kRowHeight * 5 * 2, resultMultiply, "Multiply");
		MatrixScreenPrintf(0, kRowHeight * 5 * 3, inverseM1, "inverseM1");
		MatrixScreenPrintf(0, kRowHeight * 5 * 4, inverseM2, "inverseM2");

		MatrixScreenPrintf(kColumnWidth * 7, 0, transposeM1, "transposeM1");
		MatrixScreenPrintf(kColumnWidth * 7, kRowHeight * 5, transposeM2, "transposeM2");
		MatrixScreenPrintf(kColumnWidth * 7, kRowHeight * 5 * 2, identity, "identity");

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}

		Novice::EndFrame();
	}

	Novice::Finalize();

	return 0;
}