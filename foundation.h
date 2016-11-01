#ifndef FOUNDATION_H
#define FOUNDATION_H
#define SAFE_DELETE(p){if(p){delete p;}p=NULL;}

#include <string>
#include <vector>

#include <Eigen\geometry>
#include <Eigen\dense>
#include <eigen\Core>
#include <eigen\Svd>
#include <eigen\StdVector>
#include <eigen\Sparse>

using namespace Eigen;
using namespace Eigen::internal;
using namespace Eigen::Architecture;
typedef Vector2f Vec2;
typedef Vector3f Vec3;
typedef Vector4f Vec4;
typedef Matrix3f Mat3;
typedef Matrix4f Mat4;

#endif