#include "Math.h"

#include <cassert>

void debug(const Matrix &m)
{
	printf(
"\
_11 = %6.2f, _12 = %6.2f _13 = %6.2f _14 = %6.2f\n\
_21 = %6.2f, _22 = %6.2f _23 = %6.2f _24 = %6.2f\n\
_31 = %6.2f, _32 = %6.2f _33 = %6.2f _34 = %6.2f\n\
_41 = %6.2f, _42 = %6.2f _43 = %6.2f _44 = %6.2f\n\
\n",
		m._11, m._12, m._13, m._14, 
		m._21, m._22, m._23, m._24, 
		m._31, m._32, m._33, m._34, 
		m._41, m._42, m._43, m._44);
		
}


bool affine(const Matrix& m)
{
	if (fabs(m._14) > ZERO) return false;
	if (fabs(m._24) > ZERO) return false;
	if (fabs(m._34) > ZERO) return false;
	if (fabs(m._44-1)>ZERO) return false;
	return true;
}

float abs(const Vector3 &A)
{
	return sqrt(A.x*A.x + A.y*A.y + A.z*A.z);
}

Vector3 cross(const Vector3 &A, const Vector3 &B)
{
	Vector3 C;
	D3DXVec3Cross(&C, &A, &B);
	return C;
}

float dot(const Vector3 &A, const Vector3 &B)
{
	return D3DXVec3Dot(&A, &B);
}

Vector3& normalize(Vector3& A){
	D3DXVec3Normalize(&A, &A);
	return A;
}


Vector3 normalized(const Vector3& A){
	Vector3 result;
	D3DXVec3Normalize(&result, &A);
	return result;
}


Vector3 mul0(const Matrix &m, const Vector3 &v)
{
	//Make sure the matrix is affine
	assert(affine(m));
	Vector3 r;
	D3DXVec3TransformNormal(&r, &v, &m);
	return r;
}

Point3	mul1(const Matrix &m, const Point3  &v)
{
	//Make sure the matrix is affine
	assert(affine(m));

	Vector4 t;
	D3DXVec3Transform(&t, &v, &m);

	Vector3 r;
	memcpy(&r, &t, sizeof(Vector3));
	return r;
}

Vector3 transformedAffine(const Matrix& T, const Vector3& u){
	//Make sure the matrix is affine
	assert(affine(T));


	Vector4 temp;
	D3DXVec3Transform(&temp, &u, &T);

	Vector3 result;
	memcpy(&result, &temp, sizeof(Vector3));
	return result;

}

const Vector3& transformAffine(const Matrix& T, Vector3& u){
	//Make sure the matrix is affine
	assert(affine(T));

	Vector4 temp;
	D3DXVec3Transform(&temp, &u, &T);

	memcpy(&u, &temp, sizeof(Vector3));
	return u;
}

Matrix CreateMatrix(const Point3& position, const Matrix& orientation)
{
	Matrix r = orientation;
	r._41 = position.x;
	r._42 = position.y;
	r._43 = position.z;
	return r;
}

Matrix CreateMatrix(const Point3& position, float angle, const Vector3& axis, float scaleX, float scaleY, float scaleZ){
	static Matrix m1;
	static Matrix m2;
	Matrix result;
	D3DXMatrixScaling(&result,scaleX,scaleY,scaleZ);
	D3DXMatrixRotationAxis(&m2, &axis,angle);
	D3DXMatrixMultiply(&m1, &result, &m2);
	D3DXMatrixTranslation(&m2, position.x,position.y,position.z); 
	return *D3DXMatrixMultiply(&result, &m1, &m2);
}

Matrix CreateMatrix(const Point3& position, float yaw, float pitch, float roll, float scaleX, float scaleY, float scaleZ){
	static Matrix m1;
	static Matrix m2;
	Matrix result;
	D3DXMatrixScaling(&result,scaleX,scaleY,scaleZ);
	D3DXMatrixRotationYawPitchRoll(&m2, yaw, pitch, roll);
	D3DXMatrixMultiply(&m1, &result, &m2);
	D3DXMatrixTranslation(&m2, position.x,position.y,position.z); 
	return *D3DXMatrixMultiply(&result, &m1, &m2);
}

Matrix CreateMatrix(float scaleX, float scaleY, float scaleZ){
	Matrix result;
	D3DXMatrixScaling(&result,scaleX,scaleY,scaleZ);
	return result;
}


void ExtractPosition(const Matrix& matrix, Point3& position){
	position.x = matrix._41;
	position.y = matrix._42;
	position.z = matrix._43;
}

void ExtractScaling(const Matrix& matrix, float& scaleX, float& scaleY, float& scaleZ){
	static Vector3 scale;
	static Vector3 translation;
	static D3DXQUATERNION rotation;
	assert(D3DXMatrixDecompose(&scale,&rotation,&translation, &matrix));
	scaleX = scale.x;
	scaleY = scale.y;
	scaleZ = scale.z;
}
void ExtractRotation(const Matrix& matrix, Matrix& rotationMatrix){
	static Vector3 scaleOrAxis;
	static Vector3 translation;
	static D3DXQUATERNION rotation;
	float angle;
	assert(D3DXMatrixDecompose(&scaleOrAxis,&rotation,&translation, &matrix));
	D3DXQuaternionToAxisAngle(&rotation,&scaleOrAxis, &angle);
	D3DXMatrixRotationAxis(&rotationMatrix, &scaleOrAxis,angle);
}


void ExtractAngleAxis(const Matrix& matrix, float& angle, Vector3& axis){
	static Vector3 scale;
	static Vector3 translation;
	static D3DXQUATERNION rotation;
	assert(D3DXMatrixDecompose(&scale,&rotation,&translation, &matrix));
	D3DXQuaternionToAxisAngle(&rotation,&axis, &angle);
}



void Decompose(const Matrix& matrix, Point3& position, Matrix& rotation, float& scaleX, float& scaleY, float& scaleZ){
	static Vector3 scaleOrAxis;
	static D3DXQUATERNION quat;
	
	float angle;
	assert(D3DXMatrixDecompose(&scaleOrAxis,&quat,&position, &matrix));
	scaleX = scaleOrAxis.x;
	scaleY = scaleOrAxis.y;
	scaleZ = scaleOrAxis.z;

	D3DXQuaternionToAxisAngle(&quat,&scaleOrAxis, &angle);
	D3DXMatrixRotationAxis(&rotation, &scaleOrAxis,angle);
}

Matrix& SetTranslation(Matrix& matrix, const Vector3& translation){
	matrix._41 = translation.x;
	matrix._42 = translation.y;
	matrix._43 = translation.z;
	return matrix;
}

Matrix& Translate(Matrix& matrix, const Vector3& translation){
	matrix._41 += translation.x;
	matrix._42 += translation.y;
	matrix._43 += translation.z;
	return matrix;
}

Matrix Translated(const Matrix& matrix, const Vector3& translation){
	Matrix result(matrix);
	return Translate(result,translation);
}

Vector3 GetTranslation(const Matrix& matrix){
	return Vector3(matrix._41, matrix._42, matrix._43);
}


