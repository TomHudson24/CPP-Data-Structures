#include <array>
#include <iostream>
#include <vector>
#include <utility>

template<typename T, size_t S>
class Array//making a basic templated Array class 
{
private:

	T _Data[S];//sets the internal array to the correct datatype and size as specified in the og call
public:
	constexpr int Size() const { return S; }//the constexpr states that this function can be evaluated at compile time

	T& operator[](size_t index) { return _Data[index]; }
	//using size_t to avoid negative numbers as size_t is unsigned
	//by returning a reference it lets us assign into that index without copying out the entire array, useful optimisation in string manipulation
	const T& operator[](size_t index) const { return _Data[index]; }//allows for indexing (access) if the array is marked as const 

	T* Data() { return _Data; }//values in the array will be unitiliased so using this could allow users to set that using memset
	const T* Data() const { return _Data; }
};

template<typename T>
class ResizableArray {
public:
	ResizableArray()
	{
		//allocate memory for initial data usage - 3 elements (arbitrary)
		ReAllocateMemory(3);
	}

	~ResizableArray()
	{
		Clear();
		::operator delete(_Data, _Capacity * sizeof(T));
		//using the delete operator assignment to correctly remove the data without causing an error if popback and clear are used 
		//(as everything is stack allocated if this isn't done and delete[] _Data; is used then an error will appear when the stack frame is popped because the memory would have already been deleted)
	}
	void PushBack(const T& value)
	{
		if (_Size >= _Capacity)
		{
			ReAllocateMemory(_Capacity * 2);//doubling the size every time we resize
		}
		std::cout << "Copying...\n";
		_Data[_Size] = value;
		_Size++;
	}

	void PushBack(T&& value)
	{
		if (_Size >= _Capacity)
		{
			ReAllocateMemory(_Capacity * 2);//doubling the size every time we resize
		}
		std::cout << "Moving...\n";
		_Data[_Size] = std::move(value);
		_Size++;
	}

	template<typename... Args>
	T& EmplaceBack(Args&&... args)//use this to construct the objects in place 
	{
		if (_Size >= _Capacity)
		{
			ReAllocateMemory(_Capacity * 2);
		}
		new(&_Data[_Size]) T(std::forward<Args>(args)...);//3 dots is saying to unpack the arguments
		//the new() is for constructing objects in place NOT for heap allocation
		return _Data[_Size++];
	}
	void PopBack()
	{
		if (_Size > 0)
		{
			_Size--;
			_Data[_Size].~T();//manually calling the destructor...
		}
	}

	void Clear()
	{
		for (size_t i = 0; i < _Size; i++)
		{
			_Data[i].~T();
		}
		_Size = 0;
	}

	const T& operator[](size_t index) const
	{
		if (index >= _Size)
		{
			//out of bounds
			std::cout << "Index " << index << " is out of bounds.\n";
			return _Data[0];
		}
		return _Data[index];
	}
	T& operator[](size_t index)
	{
		if (index >= _Size)
		{
			//out of bounds
			std::cout << "Index " << index << " is out of bounds.\nReturning first element of array.\n";
			return _Data[0];
		}
		return _Data[index];
	}
	size_t Size() const {
		return _Size;
	}
	size_t TotalCapacity() const
	{
		return _Capacity;
	}
private:
	void ReAllocateMemory(size_t newCapacity)
	{
		//alloc new block of mem
		T* newBlock = (T*)::operator new(newCapacity * sizeof(T));

		//am I downsizing or increasing in size?
		if (newCapacity < _Size)
		{
			_Size = newCapacity;
		}
		//move old elems
		for (size_t i = 0; i < _Size; i++)
		{
			newBlock[i] = std::move(_Data[i]);
		}
		for (size_t i = 0; i < _Size; i++)
		{
			_Data[i].~T();
		}
		//delete
		::operator delete(_Data, _Capacity * sizeof(T));
		_Data = newBlock;
		_Capacity = newCapacity;
	}

	
private:
	T* _Data = nullptr;
	size_t _Size = 0; //num of elements in vector
	size_t _Capacity = 0; //how much can the vector store without reallocation

};

struct Vec3
{
	float x = 0.0f, y = 0.0f, z = 0.0f;

	Vec3() { std::cout << "Default constructor\n"; }
	Vec3(float scalar)
		: x(scalar), y(scalar), z(scalar) {	std::cout << "Scalar constructor\n";	}
	Vec3(float x, float y, float z)
		:x(x), y(y), z(z) {	std::cout << "Float constructor\n"; }
	Vec3(const Vec3& other)
		: x(other.x), y(other.y), z(other.z)
	{
		std::cout << "Copy constructor\n";
	}
	Vec3(const Vec3&& other) noexcept
		: x(other.x), y(other.y), z(other.z)
	{
		std::cout << "Move constructor\n";
	}
	~Vec3()
	{
		std::cout << "Destructor\n";
	}

	Vec3& operator=(const Vec3& other)
	{
		std::cout << "Copied\n";
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	Vec3& operator=(const Vec3&& other) noexcept
	{
		std::cout << "Moved\n";
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
};
template<typename T>
void PrintVector(const ResizableArray<T>& vector)
{
	for (size_t i = 0; i < vector.Size(); i++)
	{
		std::cout << vector[i] << std::endl;
	}
}

template<>
void PrintVector(const ResizableArray<Vec3>& vector)
{
	for (size_t i = 0; i < vector.Size(); i++)
	{
		std::cout << vector[i].x << ", " << vector[i].y << ", " << vector[i].z << std::endl;
	}
}

#define _USE_ARRAY_ 0
#define _USE_VECTOR_ 0
#define _USE_VECTOR3_ 1
int main()
{

#if _USE_ARRAY_
	Array<int, 3> data;
	for (int i = 0; i < data.Size(); i++)
	{
		data[i] = i + 1;
		std::cout << data[i] << std::endl;
	}
	Array<std::string, 2> stringData;
	stringData[0] = "Tom";
	stringData[1] = "Lois";
	for (int i = 0; i < stringData.Size(); i++)
	{
		std::cout << stringData[i] << std::endl;
	}
#endif
#if _USE_VECTOR_
	ResizableArray<std::string> vector;
	vector.PushBack("Tom");
	vector.PushBack("Hudson");
	vector.PushBack("Lois");
	std::cout << "Current Vector Size: " << vector.TotalCapacity() << std::endl;
	vector.PushBack("Hudson");
	std::cout << "Current Vector Size: " << vector.TotalCapacity() << std::endl;
	PrintVector(vector);
	std::cout << vector[7];
#endif
#if _USE_VECTOR3_
	ResizableArray<Vec3> vec3;
	std::cout << "------------------------------\n";
	vec3.PushBack(Vec3(1.0f));//scalar constructor
	std::cout << "------------------------------\n";
	vec3.PushBack(Vec3(2, 3, 4));//float constructor
	std::cout << "------------------------------\n";
	vec3.PushBack(Vec3());//default constructor
	std::cout << "------------------------------\n";

	PrintVector(vec3);

	vec3.Clear();
	vec3.EmplaceBack(1);
	std::cout << "------------------------------\n";
	vec3.PopBack();
	vec3.EmplaceBack(1, 2, 4);
	std::cout << "------------------------------\n";
	PrintVector(vec3);
#endif
}