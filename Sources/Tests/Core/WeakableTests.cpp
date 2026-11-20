#include <gtest/gtest.h>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
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

		MyWeakable          weakable;
		WeakPtr<MyWeakable> weakPtrA(weakable);
		EXPECT_EQ(weakPtrA.Get(), &weakable);

		WeakPtr<MyWeakable> weakPtrB(&weakable);
		EXPECT_EQ(weakPtrB.Get(), &weakable);

		WeakPtr<MyWeakable> weakPtrCopy(weakPtrB);
		EXPECT_EQ(weakPtrCopy.Get(), weakPtrB.Get());
	}

	TEST(Weakable, AfterDelete)
	{
		MyWeakable* weakable = DefaultAllocator::GetInstance().New<MyWeakable>();

		WeakPtr<MyWeakable> weakPtrA(weakable);
		EXPECT_EQ(weakPtrA.Get(), weakable);

		WeakPtr<MyWeakable> weakPtrB(weakable);
		EXPECT_EQ(weakPtrB.Get(), weakable);

		DefaultAllocator::GetInstance().Delete(weakable);

		EXPECT_EQ(weakPtrA.Get(), nullptr);
		EXPECT_EQ(weakPtrB.Get(), nullptr);
	}

	TEST(Weakable, Reassign)
	{
		MyWeakable          weakableA;
		WeakPtr<MyWeakable> weakPtrA(&weakableA);
		EXPECT_EQ(weakPtrA.Get(), &weakableA);

		MyWeakable weakableB;
		weakPtrA = &weakableB;
		EXPECT_EQ(weakPtrA.Get(), &weakableB);
	}

	TEST(Weakable, CopyHasItsOwnAliveController)
	{
		MyWeakable*         original = DefaultAllocator::GetInstance().New<MyWeakable>();
		WeakPtr<MyWeakable> toOriginal(original);
		{
			MyWeakable          copy(*original);
			WeakPtr<MyWeakable> toCopy(copy);
			EXPECT_EQ(toCopy.Get(), &copy);
		}
		EXPECT_EQ(toOriginal.Get(), original); // The copy destroyed, not the original

		MyWeakable assigned;
		*original = assigned;
		EXPECT_EQ(toOriginal.Get(), original); // Assigned: it keeps its own

		DefaultAllocator::GetInstance().Delete(original);
		EXPECT_EQ(toOriginal.Get(), nullptr);
	}
}
