#include <gtest/gtest.h>
#include <HodEngine/Core/Event.hpp>

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <memory>

namespace hod::event_tests // named: the other test files may have helpers of the same names (unity builds)
{
	namespace
	{
		struct Counter
		{
			void OnEvent(int value)
			{
				_sum += value;
				++_calls;
			}

			int _sum = 0;
			int _calls = 0;
		};

		/// @brief Its destructor tells when its function is destroyed (the function holds it)
		struct DestructionWitness
		{
			explicit DestructionWitness(bool& destroyed)
			: _destroyed(destroyed)
			{
			}
			~DestructionWitness() { _destroyed = true; }

			bool& _destroyed;
		};
	}

	// ============================================================================
	// Connection
	// ============================================================================

	TEST(Event, ConnectLambdaAndMethod)
	{
		Event<int> event;
		int        lambdaSum = 0;
		Counter    counter;

		EventConnection lambdaConnection = event.Connect([&lambdaSum](int value) { lambdaSum += value; });
		EventConnection methodConnection = event.Connect(&counter, &Counter::OnEvent);
		EXPECT_EQ(event.GetConnectionCount(), 2u);

		event.Emit(3);
		event.Emit(4);
		EXPECT_EQ(lambdaSum, 7);
		EXPECT_EQ(counter._sum, 7);
		EXPECT_EQ(counter._calls, 2);
	}

	TEST(Event, CallOrderIsConnectionOrder)
	{
		Event<>          event;
		Vector<int>      order;
		EventConnection  first = event.Connect([&order]() { order.PushBack(1); });
		EventConnection  second = event.Connect([&order]() { order.PushBack(2); });
		EventConnection  third = event.Connect([&order]() { order.PushBack(3); });

		second.Disconnect(); // Not swapped with the last
		EventConnection fourth = event.Connect([&order]() { order.PushBack(4); });

		event.Emit();
		ASSERT_EQ(order.Size(), 3u);
		EXPECT_EQ(order[0], 1);
		EXPECT_EQ(order[1], 3);
		EXPECT_EQ(order[2], 4);
	}

	TEST(Event, DisconnectedByItsDestruction)
	{
		Event<int> event;
		Counter    counter;
		{
			EventConnection connection = event.Connect(&counter, &Counter::OnEvent);
			event.Emit(1);
		}
		event.Emit(1);
		EXPECT_EQ(counter._calls, 1);
		EXPECT_EQ(event.GetConnectionCount(), 0u);
	}

	TEST(Event, DisconnectDestroysTheFunction)
	{
		Event<>         event;
		bool            destroyed = false;
		EventConnection connection = event.Connect([witness = std::make_shared<DestructionWitness>(destroyed)]() {});
		EXPECT_FALSE(destroyed);
		connection.Disconnect();
		EXPECT_TRUE(destroyed);
		EXPECT_FALSE(connection.IsConnected());
	}

	TEST(Event, ConnectionMoved)
	{
		Event<int> event;
		Counter    counter;

		EventConnection moved;
		{
			EventConnection connection = event.Connect(&counter, &Counter::OnEvent);
			moved = std::move(connection);
			EXPECT_FALSE(connection.IsConnected());
		}
		EXPECT_TRUE(moved.IsConnected());
		EXPECT_TRUE(moved.IsConnectedTo(event));
		event.Emit(1);
		EXPECT_EQ(counter._calls, 1);

		// Moved into a Vector growing (each element moved)
		Vector<EventConnection> connections;
		for (int index = 0; index < 20; ++index)
		{
			connections.PushBack(event.Connect(&counter, &Counter::OnEvent));
		}
		event.Emit(1);
		EXPECT_EQ(counter._calls, 22);

		connections.Erase(0u);
		event.Emit(1);
		EXPECT_EQ(counter._calls, 42);

		connections.Clear();
		event.Emit(1);
		EXPECT_EQ(counter._calls, 43);
	}

	TEST(Event, AssignedConnectionDisconnectsThePrevious)
	{
		Event<int>      event;
		Counter         first;
		Counter         second;
		EventConnection connection = event.Connect(&first, &Counter::OnEvent);
		connection = event.Connect(&second, &Counter::OnEvent);
		event.Emit(1);
		EXPECT_EQ(first._calls, 0);
		EXPECT_EQ(second._calls, 1);
		EXPECT_EQ(event.GetConnectionCount(), 1u);
	}

	TEST(Event, DestroyedBeforeItsConnections)
	{
		Counter         counter;
		EventConnection connection;
		{
			Event<int> event;
			connection = event.Connect(&counter, &Counter::OnEvent);
			EXPECT_TRUE(connection.IsConnected());
		}
		EXPECT_FALSE(connection.IsConnected());
		connection.Disconnect(); // Nothing to touch
	}

	TEST(Event, DisconnectAll)
	{
		Event<int>      event;
		Counter         counter;
		EventConnection first = event.Connect(&counter, &Counter::OnEvent);
		EventConnection second = event.Connect(&counter, &Counter::OnEvent);
		event.DisconnectAll();
		EXPECT_FALSE(first.IsConnected());
		EXPECT_FALSE(second.IsConnected());
		event.Emit(1);
		EXPECT_EQ(counter._calls, 0);
	}

	// ============================================================================
	// From a callback
	// ============================================================================

	TEST(Event, CallbackDisconnectsItself)
	{
		Event<>         event;
		int             calls = 0;
		bool            destroyed = false;
		EventConnection connection;
		connection = event.Connect(
			[&calls, &connection, &destroyed, witness = std::make_shared<DestructionWitness>(destroyed)]()
			{
				++calls;
				connection.Disconnect();
				EXPECT_FALSE(destroyed); // Its function isn't destroyed while it runs
				EXPECT_EQ(witness.use_count(), 1);
			});

		event.Emit();
		EXPECT_TRUE(destroyed); // Once the Emit ended
		event.Emit();
		EXPECT_EQ(calls, 1);
		EXPECT_EQ(event.GetConnectionCount(), 0u);
	}

	TEST(Event, CallbackDisconnectsTheNext)
	{
		Event<>         event;
		int             secondCalls = 0;
		EventConnection second;
		EventConnection first = event.Connect([&second]() { second.Disconnect(); });
		second = event.Connect([&secondCalls]() { ++secondCalls; });

		event.Emit();
		EXPECT_EQ(secondCalls, 0); // Not called from the Emit running
	}

	TEST(Event, CallbackDisconnectsThePrevious)
	{
		Event<>         event;
		int             firstCalls = 0;
		int             thirdCalls = 0;
		EventConnection first = event.Connect([&firstCalls]() { ++firstCalls; });
		EventConnection second = event.Connect([&first]() { first.Disconnect(); });
		EventConnection third = event.Connect([&thirdCalls]() { ++thirdCalls; });

		event.Emit();
		event.Emit();
		EXPECT_EQ(firstCalls, 1);
		EXPECT_EQ(thirdCalls, 2); // The ones after still called
	}

	TEST(Event, CallbackConnects)
	{
		Event<>                 event;
		int                     addedCalls = 0;
		Vector<EventConnection> added;
		// Enough to grow the event's list: the function running must not move
		EventConnection connection = event.Connect(
			[&event, &added, &addedCalls]()
			{
				for (int index = 0; index < 64; ++index)
				{
					added.PushBack(event.Connect([&addedCalls]() { ++addedCalls; }));
				}
			});

		event.Emit();
		EXPECT_EQ(addedCalls, 0); // Called from the next Emit
		connection.Disconnect();
		event.Emit();
		EXPECT_EQ(addedCalls, 64);
	}

	TEST(Event, CallbackEmitsAgain)
	{
		Event<int>      event;
		Vector<int>     calls;
		EventConnection connection;
		EventConnection first = event.Connect(
			[&event, &calls](int depth)
			{
				calls.PushBack(depth);
				if (depth == 0)
				{
					event.Emit(1);
				}
			});
		EventConnection second = event.Connect([&first](int depth) {
			if (depth == 1)
			{
				first.Disconnect(); // Inside the inner Emit: kept until the outer one ends
			}
		});

		event.Emit(0);
		ASSERT_EQ(calls.Size(), 2u);
		EXPECT_EQ(calls[0], 0);
		EXPECT_EQ(calls[1], 1);
		EXPECT_EQ(event.GetConnectionCount(), 1u);

		event.Emit(0);
		EXPECT_EQ(calls.Size(), 2u);
	}

	TEST(Event, CallbackDestroysTheEvent)
	{
		auto            event = std::make_unique<Event<>>();
		int             secondCalls = 0;
		bool            destroyed = false;
		EventConnection second;
		EventConnection first = event->Connect(
			[&event, &destroyed, witness = std::make_shared<DestructionWitness>(destroyed)]()
			{
				event.reset();
				EXPECT_FALSE(destroyed); // Its function isn't destroyed while it runs
				EXPECT_EQ(witness.use_count(), 1);
			});
		second = event->Connect([&secondCalls]() { ++secondCalls; });

		event->Emit();
		EXPECT_TRUE(destroyed);
		EXPECT_EQ(secondCalls, 0); // The Emit stopped
		EXPECT_FALSE(first.IsConnected());
		EXPECT_FALSE(second.IsConnected());
	}

	TEST(Event, CallbackDestroysTheEventWhileEmittingAgain)
	{
		auto            event = std::make_unique<Event<int>>();
		int             calls = 0;
		EventConnection first = event->Connect(
			[&event, &calls](int depth)
			{
				++calls;
				if (depth == 0)
				{
					event->Emit(1);
				}
				else
				{
					event.reset();
				}
			});
		EventConnection second = event->Connect([&calls](int) { ++calls; });

		event->Emit(0);
		EXPECT_EQ(calls, 2); // Both Emits stopped
		EXPECT_FALSE(first.IsConnected());
	}

	TEST(Event, CallbackDestroysItsConnectionOwner)
	{
		Event<int> event;
		struct Owner
		{
			Counter         _counter;
			EventConnection _connection;
		};
		auto owner = std::make_unique<Owner>();
		int  otherCalls = 0;
		owner->_connection = event.Connect([&owner](int) { owner.reset(); });
		EventConnection other = event.Connect([&otherCalls](int) { ++otherCalls; });

		event.Emit(1);
		EXPECT_EQ(owner, nullptr);
		EXPECT_EQ(otherCalls, 1);
		EXPECT_EQ(event.GetConnectionCount(), 1u);
	}

	// ============================================================================
	// EventListener
	// ============================================================================

	TEST(EventListener, ListenAndStop)
	{
		Event<int> first;
		Event<int> second;
		Counter    counter;
		int        lambdaSum = 0;
		{
			EventListener listener;
			listener.Listen(first, &counter, &Counter::OnEvent);
			listener.Listen(first, [&lambdaSum](int value) { lambdaSum += value; });
			listener.Listen(second, &counter, &Counter::OnEvent);
			EXPECT_TRUE(listener.IsListening(first));

			first.Emit(1);
			second.Emit(10);
			EXPECT_EQ(counter._sum, 11);
			EXPECT_EQ(lambdaSum, 1);

			listener.StopListening(first);
			EXPECT_FALSE(listener.IsListening(first));
			EXPECT_TRUE(listener.IsListening(second));
			first.Emit(1);
			second.Emit(10);
			EXPECT_EQ(counter._sum, 21);
			EXPECT_EQ(lambdaSum, 1);
		}
		second.Emit(10); // Cut by its destruction
		EXPECT_EQ(counter._sum, 21);
		EXPECT_EQ(second.GetConnectionCount(), 0u);
	}

	TEST(EventListener, StopListeningAll)
	{
		Event<int>    first;
		Event<int>    second;
		Counter       counter;
		EventListener listener;
		listener.Listen(first, &counter, &Counter::OnEvent);
		listener.Listen(second, &counter, &Counter::OnEvent);
		listener.StopListeningAll();
		first.Emit(1);
		second.Emit(1);
		EXPECT_EQ(counter._calls, 0);
	}

	TEST(EventListener, StopListeningFromACallback)
	{
		Event<int>    event;
		EventListener listener;
		int           calls = 0;
		listener.Listen(event, [&listener, &event, &calls](int) {
			++calls;
			listener.StopListening(event);
		});
		listener.Listen(event, [&calls](int) { ++calls; }); // Disconnected by the first: not called

		event.Emit(1);
		event.Emit(1);
		EXPECT_EQ(calls, 1);
	}

	TEST(EventListener, EventDestroyedFirst)
	{
		EventListener listener;
		Counter       counter;
		{
			Event<int> event;
			listener.Listen(event, &counter, &Counter::OnEvent);
		}
		Event<int> other;
		listener.Listen(other, &counter, &Counter::OnEvent); // Forgets the dead one
		other.Emit(1);
		EXPECT_EQ(counter._calls, 1);
	}
}
