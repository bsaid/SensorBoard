#ifndef VECTOR_3
#define VECTOR_3

template<typename T> class Vector3
{
public:
	T x;
	T y;
	T z;

	Vector3(T x, T y, T z)
		: x(x), y(y), z(z) {}

	Vector3()
	{
		Vector3(0, 0, 0);
	}
};

typedef Vector3<float> Vector3f;
typedef Vector3<int>   Vector3i;

# endif // VECTOR_3
