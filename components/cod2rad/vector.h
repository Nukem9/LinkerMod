#pragma once

#ifndef __VECTOR_H_
#define __VECTOR_H_

template <typename T>
class tvec2;
typedef tvec2<float>			vec2;
typedef tvec2<double>			dvec2;
typedef tvec2<int>				ivec2;
typedef tvec2<unsigned int>		uvec2;
typedef tvec2<bool>				bvec2;

template <typename T>
class tvec3;
typedef tvec3<float>			vec3;
typedef tvec3<double>			dvec3;
typedef tvec3<int>				ivec3;
typedef tvec3<unsigned int>		uvec3;
typedef tvec3<bool>				bvec3;

template <typename T>
class tvec4;
typedef tvec4<float>			vec4;
typedef tvec4<double>			dvec4;
typedef tvec4<int>				ivec4;
typedef tvec4<unsigned int>		uvec4;
typedef tvec4<bool>				bvec4;

template <typename T>
class tvec2
{
public:
	union{ T x, r, s; };
	union{ T y, g, t; };

	template <typename T2>
	T dot(tvec2<T2> vec)
	{
		return(this->x * vec.x + this->y * vec.y);
	}

	T operator[](BYTE elem)
	{
		return ((T*)&x)[elem];
	}

	template <typename T2>
	operator tvec2<T2>()
	{
		return tvec2<T2>((T2)this->x, (T2)this->y);
	}

	template <typename T3>
	operator tvec3<T3>()
	{
		return tvec3<T3>((T3)this->x, (T3)this->y, (T3)0);
	}

	template <typename T4>
	operator tvec4<T4>()
	{
		return tvec4<T4>((T4)this->x, (T4)this->y, (T4)0, (T4)0);
	}

	template <typename T2>
	bool operator==(tvec2<T2>& arg)
	{
		return (this->x == arg.x || this->y == arg.y);
	}

	template <typename T2>
	bool operator!=(tvec2<T2>& arg)
	{
		return !(*this == arg);
	}

	//Addition
	tvec2<T> operator+(T arg)
	{
		return tvec2<T>(this->x + arg, this->y + arg);
	}

	template <typename T2>
	tvec2<T> operator+(tvec2<T2>& arg)
	{
		return tvec2<T>(this->x + arg.x, this->y + arg.y);
	}

	template <typename T3>
	tvec2<T> operator+(tvec3<T3>& arg)
	{
		return tvec2<T>(this->x + arg.x, this->y + arg.y);
	}

	template <typename T4>
	tvec2<T> operator+(tvec4<T4>& arg)
	{
		return tvec2<T>(this->x + arg.x, this->y + arg.y);
	}

	tvec2<T>& operator+=(T arg)
	{
		this->x += arg;
		this->y += arg;
		return *this;
	}

	template <typename T2>
	tvec2<T>& operator+=(tvec2<T2>& arg)
	{
		this->x += arg.x;
		this->y += arg.y;
		return *this;
	}

	template <typename T3>
	tvec2<T>& operator+=(tvec3<T3>& arg)
	{
		this->x += arg.x;
		this->y += arg.y;
		return *this;
	}

	template <typename T4>
	tvec2<T>& operator+=(tvec4<T4>& arg)
	{
		this->x += arg.x;
		this->y += arg.y;
		return *this;
	}

	//Subtraction
	tvec2<T> operator-(T arg)
	{
		return tvec2<T>(this->x - arg, this->y - arg);
	}

	template <typename T2>
	tvec2<T> operator-(tvec2<T2>& arg)
	{
		return tvec2<T>(this->x - arg.x, this->y - arg.y);
	}

	template <typename T3>
	tvec2<T> operator-(tvec3<T3>& arg)
	{
		return tvec2<T>(this->x - arg.x, this->y - arg.y);
	}

	template <typename T4>
	tvec2<T> operator-(tvec4<T4>& arg)
	{
		return tvec2<T>(this->x - arg.x, this->y - arg.y);
	}

	tvec2<T>& operator-=(T arg)
	{
		this->x -= arg;
		this->y -= arg;
		return *this;
	}

	template <typename T2>
	tvec2<T>& operator-=(tvec2<T2>& arg)
	{
		this->x -= arg.x;
		this->y -= arg.y;
		return *this;
	}

	template <typename T3>
	tvec2<T>& operator-=(tvec3<T3>& arg)
	{
		this->x -= arg.x;
		this->y -= arg.y;
		return *this;
	}

	template <typename T4>
	tvec2<T>& operator-=(tvec4<T4>& arg)
	{
		this->x -= arg.x;
		this->y -= arg.y;
		return *this;
	}

	//Multiplication
	tvec2<T> operator*(T arg)
	{
		return tvec2<T>(this->x * arg, this->y * arg);
	}

	template <typename T2>
	tvec2<T> operator*(tvec2<T2>& arg)
	{
		return tvec2<T>(this->x * arg.x, this->y * arg.y);
	}

	template <typename T3>
	tvec2<T> operator*(tvec3<T3>& arg)
	{
		return tvec2<T>(this->x * arg.x, this->y * arg.y);
	}

	template <typename T4>
	tvec2<T> operator*(tvec4<T4>& arg)
	{
		return tvec2<T>(this->x * arg.x, this->y * arg.y);
	}

	tvec2<T>& operator*=(T arg)
	{
		this->x *= arg;
		this->y *= arg;
		return *this;
	}

	template <typename T2>
	tvec2<T>& operator*=(tvec2<T2>& arg)
	{
		this->x *= arg.x;
		this->y *= arg.y;
		return *this;
	}

	template <typename T3>
	tvec2<T>& operator*=(tvec3<T3>& arg)
	{
		this->x *= arg.x;
		this->y *= arg.y;
		return *this;
	}

	template <typename T4>
	tvec2<T>& operator*=(tvec4<T4>& arg)
	{
		this->x *= arg.x;
		this->y *= arg.y;
		return *this;
	}

	//Division
	tvec2<T> operator/(T arg)
	{
		return tvec2<T>(this->x / arg, this->y / arg);
	}

	template <typename T2>
	tvec2<T> operator/(tvec2<T2>& arg)
	{
		return tvec2<T>(this->x / arg.x, this->y / arg.y);
	}

	template <typename T3>
	tvec2<T> operator/(tvec3<T3>& arg)
	{
		return tvec2<T>(this->x / arg.x, this->y / arg.y);
	}

	template <typename T4>
	tvec2<T> operator/(tvec4<T4>& arg)
	{
		return tvec2<T>(this->x / arg.x, this->y / arg.y);
	}

	tvec2<T>& operator/=(T arg)
	{
		this->x /= arg;
		this->y /= arg;
		return *this;
	}

	template <typename T2>
	tvec2<T>& operator/=(tvec2<T2>& arg)
	{
		this->x /= arg.x;
		this->y /= arg.y;
		return *this;
	}

	template <typename T3>
	tvec2<T>& operator/=(tvec3<T3>& arg)
	{
		this->x /= arg.x;
		this->y /= arg.y;
		return *this;
	}

	template <typename T4>
	tvec2<T>& operator/=(tvec4<T4>& arg)
	{
		this->x /= arg.x;
		this->y /= arg.y;
		return *this;
	}

	tvec2(){};
	tvec2(T initVal) : x(initVal), y(initVal){};
	tvec2(T X, T Y) : x(X), y(Y){};
	~tvec2(){};
};


template <typename T>
class tvec3
{
public:
	union{ T x, r, s; };
	union{ T y, g, t; };
	union{ T z, b, p; };

	template <typename T3>
	T dot(tvec3<T3> vec)
	{
		return(this->x * (T)vec.x + this->y * (T)vec.y + this->z * (T)vec.z);
	}

	T operator[](BYTE elem)
	{
		return ((T*)&x)[elem];
	}

	template <typename T3>
	tvec3<T> cross(tvec3<T3> vec)
	{
		return tvec3<T>(this->y * vec.z - this->z * vec.y, this->z*vec.x - this->x*vec.z, this->x * vec.y - this->y * vec.x);
	}

	template <typename T2>
	operator tvec2<T2>()
	{
		return tvec2<T2>((T2)this->x, (T2)this->y);
	}

	template <typename T3>
	operator tvec3<T3>()
	{
		return tvec3<T3>((T3)this->x, (T3)this->y, (T3)this->z);
	}

	template <typename T4>
	operator tvec4<T4>()
	{
		return tvec4<T4>((T4)this->x, (T4)this->y, (T4)this->z, (T4)0);
	}

	template <typename T3>
	bool operator==(tvec3<T3>& arg)
	{
		return (this->x == arg.x || this->y == arg.y || this->z == arg.z);
	}

	template <typename T3>
	bool operator!=(tvec3<T3>& arg)
	{
		return !(*this == arg);
	}

	//Addition
	tvec3<T> operator+(T arg)
	{
		return tvec3<T>(this->x + arg, this->y + arg, this->z + arg);
	}

	template <typename T2>
	tvec3<T> operator+(tvec2<T2>& arg)
	{
		return tvec3<T>(this->x + arg.x, this->y + arg.y, this->z);
	}

	template <typename T3>
	tvec3<T> operator+(tvec3<T3>& arg)
	{
		return tvec3<T>(this->x + arg.x, this->y + arg.y, this->z + arg.z);
	}

	template <typename T4>
	tvec3<T> operator+(tvec4<T4>& arg)
	{
		return tvec3<T>(this->x + arg.x, this->y + arg.y, this->z + arg.z);
	}

	tvec3<T>& operator+=(T arg)
	{
		this->x += arg;
		this->y += arg;
		this->z += arg;
		return *this;
	}

	template <typename T2>
	tvec3<T>& operator+=(tvec2<T2>& arg)
	{
		this->x += arg.x;
		this->y += arg.y;
		return *this;
	}

	template <typename T3>
	tvec3<T>& operator+=(tvec3<T3>& arg)
	{
		this->x += arg.x;
		this->y += arg.y;
		this->z += arg.z;
		return *this;
	}

	template <typename T4>
	tvec3<T>& operator+=(tvec4<T4>& arg)
	{
		this->x += arg.x;
		this->y += arg.y;
		this->z += arg.z;
		return *this;
	}

	//Subtraction
	tvec3<T> operator-(T arg)
	{
		return tvec3<T>(this->x - arg, this->y - arg, this->z - arg);
	}

	template <typename T2>
	tvec3<T> operator-(tvec2<T2>& arg)
	{
		return tvec3<T>(this->x - arg.x, this->y - arg.y, this->z);
	}

	template <typename T3>
	tvec3<T> operator-(tvec3<T3>& arg)
	{
		return tvec3<T>(this->x - arg.x, this->y - arg.y, this->z - arg.z);
	}

	template <typename T4>
	tvec3<T> operator-(tvec4<T4>& arg)
	{
		return tvec3<T>(this->x - arg.x, this->y - arg.y, this->z - arg.z);
	}

	tvec3<T>& operator-=(T arg)
	{
		this->x -= arg;
		this->y -= arg;
		this->z -= arg;
		return *this;
	}

	template <typename T2>
	tvec3<T>& operator-=(tvec2<T2>& arg)
	{
		this->x -= arg.x;
		this->y -= arg.y;
		return *this;
	}

	template <typename T3>
	tvec3<T>& operator-=(tvec3<T3>& arg)
	{
		this->x -= arg.x;
		this->y -= arg.y;
		this->z -= arg.z;
		return *this;
	}

	template <typename T4>
	tvec3<T>& operator-=(tvec4<T4>& arg)
	{
		this->x -= arg.x;
		this->y -= arg.y;
		this->z -= arg.z;
		return *this;
	}

	//Multiplication
	tvec3<T> operator*(T arg)
	{
		return tvec3<T>(this->x * arg, this->y * arg, this->z * arg);
	}

	template <typename T2>
	tvec3<T> operator*(tvec2<T2>& arg)
	{
		return tvec3<T>(this->x * arg.x, this->y * arg.y, this->z);
	}

	template <typename T3>
	tvec3<T> operator*(tvec3<T3>& arg)
	{
		return tvec3<T>(this->x * arg.x, this->y * arg.y, this->z * arg.z);
	}

	template <typename T4>
	tvec3<T> operator*(tvec4<T4>& arg)
	{
		return tvec3<T>(this->x * arg.x, this->y * arg.y, this->z * arg.z);
	}

	tvec3<T>& operator*=(T arg)
	{
		this->x *= arg;
		this->y *= arg;
		this->z *= arg;
		return *this;
	}

	template <typename T2>
	tvec3<T>& operator*=(tvec2<T2>& arg)
	{
		this->x *= arg.x;
		this->y *= arg.y;
		return *this;
	}

	template <typename T3>
	tvec3<T>& operator*=(tvec3<T3>& arg)
	{
		this->x *= arg.x;
		this->y *= arg.y;
		this->z *= arg.z;
		return *this;
	}

	template <typename T4>
	tvec3<T>& operator*=(tvec4<T4>& arg)
	{
		this->x *= arg.x;
		this->y *= arg.y;
		this->z *= arg.z;
		return *this;
	}

	//Division
	tvec3<T> operator/(T arg)
	{
		return tvec3<T>(this->x / arg, this->y / arg, this->z / arg);
	}

	template <typename T2>
	tvec3<T> operator/(tvec2<T2>& arg)
	{
		return tvec3<T>(this->x / arg.x, this->y / arg.y, this->z);
	}

	template <typename T3>
	tvec3<T> operator/(tvec3<T3>& arg)
	{
		return tvec3<T>(this->x / arg.x, this->y / arg.y, this->z / arg.z);
	}

	template <typename T4>
	tvec3<T> operator/(tvec4<T4>& arg)
	{
		return tvec3<T>(this->x / arg.x, this->y / arg.y, this->z / arg.z);
	}

	tvec3<T>& operator/=(T arg)
	{
		this->x /= arg;
		this->y /= arg;
		this->z /= arg;
		return *this;
	}

	template <typename T2>
	tvec3<T>& operator/=(tvec2<T2>& arg)
	{
		this->x /= arg.x;
		this->y /= arg.y;
		return *this;
	}

	template <typename T3>
	tvec3<T>& operator/=(tvec3<T3>& arg)
	{
		this->x /= arg.x;
		this->y /= arg.y;
		this->z /= arg.z;
		return *this;
	}

	template <typename T4>
	tvec3<T>& operator/=(tvec4<T4>& arg)
	{
		this->x /= arg.x;
		this->y /= arg.y;
		this->z /= arg.z;
		return *this;
	}

	tvec3(){};
	tvec3(T initVal) : x(initVal), y(initVal), z(initVal){};
	tvec3(T X, tvec2<T> Vec2) : x(X), y(Vec2.x), z(Vec2.y){};
	tvec3(tvec2<T> Vec2, T Z) : x(Vec2.x), y(Vec2.y), z(Z){};
	tvec3(T X, T Y, T Z) : x(X), y(Y), z(Z){};
	~tvec3(){};
};


template <typename T>
class tvec4
{
public:
	union{ T x, r, s; };
	union{ T y, g, t; };
	union{ T z, b, p; };
	union{ T w, a, q; };

	template <typename T4>
	T dot(tvec4<T4> vec)
	{
		return(this->x * vec.x + this->y * vec.y + this->z * vec.z + this->w * vec.w);
	}

	T operator[](BYTE elem)
	{
		return ((T*)&x)[elem];
	}

	template <typename T2>
	operator tvec2<T2>()
	{
		return tvec2<T2>((T2)this->x, (T2)this->y);
	}

	template <typename T3>
	operator tvec3<T3>()
	{
		return tvec3<T3>((T3)this->x, (T3)this->y, (T3)this->z);
	}

	template <typename T4>
	operator tvec4<T4>()
	{
		return tvec4<T4>((T4)this->x, (T4)this->y, (T4)this->z, (T4)this->w);
	}

	template <typename T4>
	bool operator==(tvec4<T4>& arg)
	{
		return (this->x == arg.x || this->y == arg.y || this->z == arg.z || this->w == arg.w);
	}

	template <typename T4>
	bool operator!=(tvec4<T4>& arg)
	{
		return !(*this == arg);
	}

	//Addition
	tvec4<T> operator+(T arg)
	{
		return tvec4<T>(this->x + arg, this->y + arg, this->z + arg, this->w + arg);
	}

	template <typename T2>
	tvec4<T> operator+(tvec2<T2>& arg)
	{
		return tvec4<T>(this->x + arg.x, this->y + arg.y, this->z, this->w);
	}

	template <typename T3>
	tvec4<T> operator+(tvec3<T3>& arg)
	{
		return tvec4<T>(this->x + arg.x, this->y + arg.y, this->z + arg.z, this->w);
	}

	template <typename T4>
	tvec4<T> operator+(tvec4<T4>& arg)
	{
		return tvec4<T>(this->x + arg.x, this->y + arg.y, this->z + arg.z, this->w + arg.w);
	}

	tvec4<T>& operator+=(T arg)
	{
		this->x += arg;
		this->y += arg;
		this->z += arg;
		this->w += arg;
		return *this;
	}

	template <typename T2>
	tvec4<T>& operator+=(tvec2<T2>& arg)
	{
		this->x += arg.x;
		this->y += arg.y;
		return *this;
	}

	template <typename T3>
	tvec4<T>& operator+=(tvec3<T3>& arg)
	{
		this->x += arg.x;
		this->y += arg.y;
		this->z += arg.z;
		return *this;
	}

	template <typename T4>
	tvec4<T>& operator+=(tvec4<T4>& arg)
	{
		this->x += arg.x;
		this->y += arg.y;
		this->z += arg.z;
		this->w += arg.w;
		return *this;
	}

	//Subtraction
	tvec4<T> operator-(T arg)
	{
		return tvec4<T>(this->x - arg, this->y - arg, this->z - arg, this->w - arg);
	}

	template <typename T2>
	tvec4<T> operator-(tvec2<T2>& arg)
	{
		return tvec4<T>(this->x - arg.x, this->y - arg.y, this->z, this->w);
	}

	template <typename T3>
	tvec4<T> operator-(tvec3<T3>& arg)
	{
		return tvec4<T>(this->x - arg.x, this->y - arg.y, this->z - arg.z, this->w);
	}

	template <typename T4>
	tvec4<T> operator-(tvec4<T4>& arg)
	{
		return tvec4<T>(this->x - arg.x, this->y - arg.y, this->z - arg.z, this->w - arg.w);
	}

	tvec4<T>& operator-=(T arg)
	{
		this->x -= arg;
		this->y -= arg;
		this->z -= arg;
		this->w -= arg;
		return *this;
	}

	template <typename T2>
	tvec4<T>& operator-=(tvec2<T2>& arg)
	{
		this->x -= arg.x;
		this->y -= arg.y;
		return *this;
	}

	template <typename T3>
	tvec4<T>& operator-=(tvec3<T3>& arg)
	{
		this->x -= arg.x;
		this->y -= arg.y;
		this->z -= arg.z;
		return *this;
	}

	template <typename T4>
	tvec4<T>& operator-=(tvec4<T4>& arg)
	{
		this->x -= arg.x;
		this->y -= arg.y;
		this->z -= arg.z;
		this->w -= arg.w;
		return *this;
	}

	//Multiplication
	tvec4<T> operator*(T arg)
	{
		return tvec4<T>(this->x * arg, this->y * arg, this->z * arg, this->w * arg);
	}

	template <typename T2>
	tvec4<T> operator*(tvec2<T2>& arg)
	{
		return tvec4<T>(this->x * arg.x, this->y * arg.y, this->z, this->w);
	}

	template <typename T3>
	tvec4<T> operator*(tvec3<T3>& arg)
	{
		return tvec4<T>(this->x * arg.x, this->y * arg.y, this->z * arg.z, this->w);
	}

	template <typename T4>
	tvec4<T> operator*(tvec4<T4>& arg)
	{
		return tvec4<T>(this->x * arg.x, this->y * arg.y, this->z * arg.z, this->w * arg.w);
	}

	tvec4<T>& operator*=(T arg)
	{
		this->x *= arg;
		this->y *= arg;
		this->z *= arg;
		this->w *= arg;
		return *this;
	}

	template <typename T2>
	tvec4<T>& operator*=(tvec2<T2>& arg)
	{
		this->x *= arg.x;
		this->y *= arg.y;
		return *this;
	}

	template <typename T3>
	tvec4<T>& operator*=(tvec3<T3>& arg)
	{
		this->x *= arg.x;
		this->y *= arg.y;
		this->z *= arg.z;
		return *this;
	}

	template <typename T4>
	tvec4<T>& operator*=(tvec4<T4>& arg)
	{
		this->x *= arg.x;
		this->y *= arg.y;
		this->z *= arg.z;
		this->w *= arg.w;
		return *this;
	}

	//Division
	tvec4<T> operator/(T arg)
	{
		return tvec4<T>(this->x / arg, this->y / arg, this->z / arg, this->w / arg);
	}

	template <typename T2>
	tvec4<T> operator/(tvec2<T2>& arg)
	{
		return tvec4<T>(this->x / arg.x, this->y / arg.y, this->z, this->w);
	}

	template <typename T3>
	tvec4<T> operator/(tvec3<T3>& arg)
	{
		return tvec4<T>(this->x / arg.x, this->y / arg.y, this->z / arg.z, this->w);
	}

	template <typename T4>
	tvec4<T> operator/(tvec4<T4>& arg)
	{
		return tvec4<T>(this->x / arg.x, this->y / arg.y, this->z / arg.z, this->w / arg.w);
	}

	tvec4<T>& operator/=(T arg)
	{
		this->x /= arg;
		this->y /= arg;
		this->z /= arg;
		this->w /= arg;
		return *this;
	}

	template <typename T2>
	tvec4<T>& operator/=(tvec2<T2>& arg)
	{
		this->x /= arg.x;
		this->y /= arg.y;
		return *this;
	}

	template <typename T3>
	tvec4<T>& operator/=(tvec3<T3>& arg)
	{
		this->x /= arg.x;
		this->y /= arg.y;
		this->z /= arg.z;
		return *this;
	}

	template <typename T4>
	tvec4<T>& operator/=(tvec4<T4>& arg)
	{
		this->x /= arg.x;
		this->y /= arg.y;
		this->z /= arg.z;
		this->w /= arg.w;
		return *this;
	}

	tvec4(){};
	tvec4(T initVal) : x(initVal), y(initVal), z(initVal), w(initVal){};
	tvec4(T X, T Y, tvec2<T> Vec2) : x(X), y(Y), z(Vec2.x), w(Vec2.y){};
	tvec4(T X, tvec2<T> Vec2, T W) : x(X), y(Vec2.x), z(Vec2.y), w(W){};
	tvec4(tvec2<T> Vec2, T Z, T W) : x(Vec2.x), y(Vec2.y), z(Z), w(W){};
	tvec4(T X, tvec3<T> Vec3) : x(X), y(Vec3.x), z(Vec3.y), w(Vec3.z){};
	tvec4(tvec3<T> Vec3, T W) : x(Vec3.x), y(Vec3.y), z(Vec3.z), w(W){};
	tvec4(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W){};
	~tvec4(){};
};

void Vec3Normalize(vec3* v);
void Vec3Normalize(float* v);

float Vec3Dot(vec3* a, vec3* b);
float Vec3Dot(float* a, float* b);

#endif


