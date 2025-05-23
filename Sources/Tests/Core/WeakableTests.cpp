#include <gtest/gtest.h>

#include <HodEngine/Core/Weakable/Weakable.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>

namespace hod
{
	class MyWeakable : public Weakable
	{
	public:
		MyWeakable() = default;
	};

	TEST(Weakable, Ctor)
	{
		WeakPtr<MyWeakable> weakPtrDefault;
		EXPECT_EQ(weakPtrDefault.Get(), nullptr);

		MyWeakable weakable;
		WeakPtr<MyWeakable> weakPtrA(weakable);
		EXPECT_EQ(weakPtrA.Get(), &weakable);

		WeakPtr<MyWeakable> weakPtrB(&weakable);
		EXPECT_EQ(weakPtrB.Get(), &weakable);

		WeakPtr<MyWeakable> weakPtrCopy(weakPtrB);
		EXPECT_EQ(weakPtrCopy.Get(), weakPtrB.Get());
	}

	TEST(Weakable, AfterDelete)
	{
		MyWeakable* weakable = new MyWeakable();

		WeakPtr<MyWeakable> weakPtrA(weakable);
		EXPECT_EQ(weakPtrA.Get(), weakable);

		WeakPtr<MyWeakable> weakPtrB(weakable);
		EXPECT_EQ(weakPtrB.Get(), weakable);

		delete weakable;

		EXPECT_EQ(weakPtrA.Get(), nullptr);
		EXPECT_EQ(weakPtrB.Get(), nullptr);
	}

	TEST(Weakable, Reassign)
	{
		MyWeakable weakableA;
		WeakPtr<MyWeakable> weakPtrA(&weakableA);
		EXPECT_EQ(weakPtrA.Get(), &weakableA);

		MyWeakable weakableB;
		weakPtrA = &weakableB;
		EXPECT_EQ(weakPtrA.Get(), &weakableB);
	}
}
