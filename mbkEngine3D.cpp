/* 
  This code was written by mbk following javidx9 tutorials
*/



#include "olcConsoleGameEngine.h"
using namespace std;


struct vec3d
{
	float x, y, z;

	vec3d operator+(const vec3d& v) const { return {x + v.x, y + v.y, z + v.z}; }
	vec3d operator-(const vec3d& v) const { return {x - v.x, y - v.y, z - v.z}; }
	vec3d operator*(float k) const { return {x * k, y * k, z * k}; }
	float DotProduct(const vec3d& v) const { return x * v.x + y * v.y + z * v.z; }
	float Length() const { return sqrtf(DotProduct(*this)); }
	vec3d Normalize() {
		float l = Length();
		return {x / l, y / l, z / l};
	}
};

struct triangle
{
	vec3d p[3];
	int color;  // For surface color
};

struct mesh
{
	vector<triangle> tris;
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

class mbkEngine3D : public olcConsoleGameEngine
{
public:
	mbkEngine3D()
	{
		m_sAppName = L"3D Demo";
	}


private:
	mesh meshCube;
	mat4x4 matProj;

	float fTheta;

	vec3d vCamera = {0, -2.0f, 0};        // Camera position
	vec3d vLookDir = {0, 0, 1};       // Look direction
	float fYaw = 0.0f;                // Camera horizontal rotation angle

	CHAR_INFO GetColour(float lum) {
		CHAR_INFO c;
		int pixel_bw = (int)(13.0f * lum);
		if (pixel_bw < 0) pixel_bw = 0;
		if (pixel_bw > 13) pixel_bw = 13;
		c.Attributes = pixel_bw;
		c.Char.UnicodeChar = PIXEL_SOLID;
		return c;
	}

	void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
	{
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
		float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

		if (w != 0.0f)
		{
			o.x /= w; o.y /= w; o.z /= w;
		}
	}

public:
	bool OnUserCreate() override
	{
		meshCube.tris = {

			// SOUTH
			{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			// EAST                                                      
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			// NORTH                                                     
			{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			// WEST                                                      
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			// TOP                                                       
			{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

			// BOTTOM                                                    
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		};

		// Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Camera Controls
		if (GetKey(VK_UP).bHeld)
			vCamera.y += 8.0f * fElapsedTime;    // Up
		if (GetKey(VK_DOWN).bHeld)
			vCamera.y -= 8.0f * fElapsedTime;    // Down
		if (GetKey(VK_LEFT).bHeld)
			vCamera.x -= 8.0f * fElapsedTime;    // Left
		if (GetKey(VK_RIGHT).bHeld)
			vCamera.x += 8.0f * fElapsedTime;    // Right
		
		if (GetKey(L'A').bHeld)
			fYaw -= 2.0f * fElapsedTime;
		if (GetKey(L'D').bHeld)
			fYaw += 2.0f * fElapsedTime;

		// Update camera direction vector
		vLookDir = {sinf(fYaw), 0, cosf(fYaw)};

		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		// Set up rotation matrices
		mat4x4 matRotZ, matRotX;
		fTheta += 1.0f * fElapsedTime;

		// Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;

		// Draw Triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			// Rotate in Z-Axis
			MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
			MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
			MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

			// Rotate in X-Axis
			MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

			// Offset into the screen
			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

			// Project triangles from 3D --> 2D
			MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
			MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
			MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

			// Scale into view
			triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
			triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

			// Calculate surface normal
			vec3d normal, line1, line2;
			line1 = triTranslated.p[1] - triTranslated.p[0];
			line2 = triTranslated.p[2] - triTranslated.p[0];
			
			// Calculate normal vector with cross product
			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;
			normal = normal.Normalize();

			// Light direction (simple directional light)
			vec3d lightDir = {0.0f, 0.0f, -1.0f};
			float dp = normal.DotProduct(lightDir);

			// Draw only surfaces facing the camera
			if (normal.DotProduct(triTranslated.p[0] - vCamera) < 0.0f)
			{
				// Select color based on lighting intensity
				CHAR_INFO c = GetColour(dp);
				DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y,
					PIXEL_SOLID, c.Attributes);
			}

		}


		return true;
	}

};




int main()
{

	mbkEngine3D demo;
	if (demo.ConstructConsole(120, 80, 4, 4))
		demo.Start();



	return 0;

}
