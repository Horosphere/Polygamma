#ifndef _POLYGAMMA_MATH_VECTOR_HPP__
#define _POLYGAMMA_MATH_VECTOR_HPP__

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>

//#define VECTOR_CPP_ALLOC

namespace pg
{

/**
 * Define the macro VECTOR_CPP_ALLOC to use new[] and delete[] instead of
 *  malloc and free.
 * @brief Class for storing any kind of vector
 * @tparam T The underlying algebra system. Must support +, -, *, /
 */
template <typename T>
class Vector
{
public:
	/**
	 * @brief Constructs a vector with size 0 and no data. Equivalent to
	 * Vector(0).
	 */
	Vector();
	/**
	 * @brief Creates a vector of size fed as the argument
	 * @param size The number of elements to be allocated.
	 */
	Vector(std::size_t size);
	Vector(std::size_t size, T const& value);
	/**
	 * @brief Creates a vector of pre-allocated data. The ownership of the
	 *  pointer is transferred to the Vector object.
	 * @data A pre-allocated array.
	 * @size The number of objects in the array.
	 */
	Vector(std::size_t size, T* const data);

	~Vector();

	Vector(Vector<T> const&);
	Vector(Vector<T>&&);
	Vector& operator=(Vector<T> const&);
	Vector& operator=(Vector<T>&&);
	Vector& operator+=(Vector<T> const&);
	Vector& operator-=(Vector<T> const&);
	Vector& operator*=(Vector<T> const&);
	Vector& operator/=(Vector<T> const&);

	/**
	 * @warning Users is responsible for releasing the memory pointed by T* data.
	 * @brief Releases ownership of the data pointer and set the vector's size to
	 *  0.
	 */
	void clear() noexcept;


	std::size_t getSize() const noexcept;
	bool isEmpty() const noexcept;
	T const* getData() const noexcept;
	T* getData() noexcept;

	T operator[](std::size_t index) const;
	T& operator[](std::size_t index);

private:
	std::size_t size;
	T* data;

	//friend bool operator==(Vector<T> const&, Vector<T> const&);
	friend bool operator!=(Vector<T> const&, Vector<T> const&);
	friend Vector<T> operator+(Vector<T> const&, Vector<T> const&);
	friend Vector<T> operator-(Vector<T> const&, Vector<T> const&);
	friend Vector<T> operator*(Vector<T> const&, Vector<T> const&);
	friend Vector<T> operator/(Vector<T> const&, Vector<T> const&);
};

template <typename T> inline bool
operator==(Vector<T> const&, Vector<T> const&);
template <typename T> inline bool
operator!=(Vector<T> const&, Vector<T> const&);
template <typename T> inline Vector<T>
operator+(Vector<T> const&, Vector<T> const&);
template <typename T> inline Vector<T>
operator-(Vector<T> const&, Vector<T> const&);
template <typename T> inline Vector<T>
operator*(Vector<T> const&, Vector<T> const&);
template <typename T> inline Vector<T>
operator/(Vector<T> const&, Vector<T> const&);


// Implementations


template <typename T> inline
Vector<T>::Vector(): size(0), data(nullptr)
{
}
template <typename T> inline
Vector<T>::Vector(std::size_t size):
	size(size),
#ifdef VECTOR_CPP_ALLOC
	data(size ? new T[size] : nullptr)
#else
	data(size ? (T*) std::malloc(size * sizeof(T)) : nullptr)
#endif
{
}
template <typename T> inline
Vector<T>::Vector(std::size_t size, T const& value): Vector(size)
{
	for (std::size_t i = 0; i < size ; ++i)
		data[i] = value;
}
template <typename T> inline
Vector<T>::Vector(std::size_t size, T* const data):
	size(size), data(data)
{
}

template <typename T> inline
Vector<T>::~Vector()
{
#ifdef VECTOR_CPP_ALLOC
	delete[] data;
#else
	free(data);
#endif
}

// Duplications
template <typename T> inline
Vector<T>::Vector(Vector<T> const& vector):
	size(vector.size),
#ifdef VECTOR_CPP_ALLOC
	data(new T[size])
#else
	data((T*) std::malloc(size * sizeof(T)))
#endif
{
}
template <typename T> inline
Vector<T>::Vector(Vector<T>&& vector):
	size(vector.size), data(vector.data)
{
	vector.data = nullptr;
}
template <typename T> inline Vector<T>&
Vector<T>::operator=(Vector<T> const& vector)
{
	delete[] data;
	size = vector.size;
#ifdef VECTOR_CPP_ALLOC
	data = new T[size];
#else
	data = (T*) std::malloc(size * sizeof(T));
#endif
	std::memcpy(data, vector.data, size * sizeof(T));
	return *this;
}
template <typename T> inline Vector<T>&
Vector<T>::operator=(Vector<T>&& vector)
{
	size = vector.size;
	data = vector.data;
	vector.data = nullptr;
	return *this;
}
template <typename T> inline Vector<T>&
Vector<T>::operator+=(Vector<T> const& vector)
{
	for (std::size_t i = 0; i < std::min(size, vector.size); ++i)
		data[i] += vector.data[i];
	return *this;
}
template <typename T> inline Vector<T>&
Vector<T>::operator-=(Vector<T> const& vector)
{
	for (std::size_t i = 0; i < std::min(size, vector.size); ++i)
		data[i] += vector.data[i];
	return *this;
}
template <typename T> inline Vector<T>&
Vector<T>::operator*=(Vector<T> const& vector)
{
	for (std::size_t i = 0; i < std::min(size, vector.size); ++i)
		data[i] += vector.data[i];
	return *this;
}
template <typename T> inline Vector<T>&
Vector<T>::operator/=(Vector<T> const& vector)
{
	for (std::size_t i = 0; i < std::min(size, vector.size); ++i)
		data[i] += vector.data[i];
	return *this;
}

template <typename T> inline void
Vector<T>::clear() noexcept
{
	data = nullptr;
	size = 0;
}

template <typename T> inline std::size_t
Vector<T>::getSize() const noexcept
{
	return size;
}
template <typename T> inline bool
Vector<T>::isEmpty() const noexcept
{
	return size == 0;
}

template <typename T> inline T const*
Vector<T>::getData() const noexcept
{
	return data;
}
template <typename T> inline T*
Vector<T>::getData() noexcept
{
	return data;
}

template <typename T> inline T
Vector<T>::operator[](std::size_t index) const
{
	return data[index];
}
template <typename T> inline T&
Vector<T>::operator[](std::size_t index)
{
	return data[index];
}

// Comparisons and arithmetics

template <typename T> inline bool
operator==(Vector<T> const& v0, Vector<T> const& v1)
{
	if (v0.getSize() != v1.getSize()) return false;
	for (std::size_t i = 0; i < v0.getSize(); ++i)
		if (v0[i] != v1[i]) return false;
	return true;
}
template <typename T> inline bool
operator!=(Vector<T> const& v0, Vector<T> const& v1)
{
	if (v0.size != v1.size) return true;
	for (std::size_t i = 0; i < v0.size; ++i)
		if (v0.data[i] != v1.data[i]) return true;
	return false;
}
template <typename T> inline Vector<T>
operator+(Vector<T> const& v0, Vector<T> const& v1)
{
	Vector<T> result(std::min(v0, v1));
	for (std::size_t i = 0; i < result.size; ++i)
		result.data[i] = v0.data[i] + v1.data[i];
	return result;
}
template <typename T> inline Vector<T>
operator-(Vector<T> const& v0, Vector<T> const& v1)
{
	Vector<T> result(std::min(v0, v1));
	for (std::size_t i = 0; i < result.size; ++i)
		result.data[i] = v0.data[i] - v1.data[i];
	return result;
}
template <typename T> inline Vector<T>
operator*(Vector<T> const& v0, Vector<T> const& v1)
{
	Vector<T> result(std::min(v0, v1));
	for (std::size_t i = 0; i < result.size; ++i)
		result.data[i] = v0.data[i] * v1.data[i];
	return result;
}
template <typename T> inline Vector<T>
operator/(Vector<T> const& v0, Vector<T> const& v1)
{
	Vector<T> result(std::min(v0, v1));
	for (std::size_t i = 0; i < result.size; ++i)
		result.data[i] = v0.data[i] / v1.data[i];
	return result;
}

} // namespace pg

#endif // !_POLYGAMMA_MATH_VECTOR_HPP__
