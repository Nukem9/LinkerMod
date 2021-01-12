#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <string.h>

#define UNUSED(x) (void)(x)

template <typename _SRC_T, typename _SLOT_T>
class Handler
{
  private:
	enum SlotState
	{
		EMPTY = 0,
		INUSE = -1
	};

	std::mutex mtx_slots;
	std::mutex mtx_fs;

	std::mutex mtx_busy;
	std::condition_variable cv_data;

	_SLOT_T **slots;
	unsigned int numSlots;

	std::vector<_SRC_T> input_data;
	unsigned int input_index;

	// Always lock mtx_slots before calling this
	void FreeSlot(_SLOT_T **slot)
	{
		if (*slot != (_SLOT_T *)SlotState::INUSE)
			delete[] * slot;
		*slot = (_SLOT_T *)SlotState::EMPTY;
	}

	typedef std::function<void(_SRC_T &_src, _SLOT_T &_outData)> _SlotInit_f; // Initializes an empty slot with data

	typedef std::function<void(_SLOT_T &_slotData)> _SlotPostInit_f; // Executes any post slot init code
																	 // returns a pointer to the (new) _SLOT_T data
																	 // Useful for decompression, etc.

	typedef std::function<void(_SLOT_T &_slot)> _SlotHandleData_f; // Handle loaded slot data

	_SlotInit_f _SlotInit_Callback;
	_SlotPostInit_f _SlotPostInit_Callback;
	_SlotHandleData_f _SlotHandleData_Callback;

	static void _SlotInit_DefaultCallback(_SRC_T &_src, _SLOT_T &_outData)
	{
		UNUSED(_src);
		UNUSED(_outData);
	};
	static void _SlotPostInit_DefaultCallback(_SLOT_T &_slotData) { UNUSED(_slotData); };
	static void _SlotHandleData_DefaultCallback(_SLOT_T &_slotData) { UNUSED(_slotData); };

	// Run a single handler iteration
	// Either loads data, handles loaded data, or waits for data to be loaded (if all slots are busy)
	int ExecuteLoopIteration()
	{
		_SLOT_T **slot = (_SLOT_T **)SlotState::INUSE;

		// Data handler lambda - lock mtx_slots before calling this
		auto Handle_Data = [&slot, this](void) -> int {
			_SLOT_T *data = *slot;
			*slot = (_SLOT_T *)SlotState::INUSE;
			mtx_slots.unlock();

			// Run the handler callback on the slot data
			_SlotHandleData_Callback(*data);

			//printf("FREEING DATA 0x%" PRIx64 "\n", (long unsigned int)data);
			delete data;

			// Free the slot & mark it as empty
			mtx_slots.lock();
			FreeSlot(slot);
			mtx_slots.unlock();

			return 0;
		};

		mtx_slots.lock();

		//
		// Attempt to get an EMPTY or Unhandled slot
		// Otherwise we just get an INUSE handle
		//
		for (unsigned int i = 0; i < numSlots; i++)
		{
			if (slots[i] == (_SLOT_T *)SlotState::INUSE)
				continue;

			slot = &slots[i];
			break;
		}

		if (slot == (_SLOT_T **)SlotState::INUSE || *slot == (_SLOT_T *)SlotState::INUSE)
		{
			// There are no usable slots
			mtx_slots.unlock();

			//printf("All slots are in-use!\n");

			// Wait for cv_data to be notified before checking for data again
			std::unique_lock<std::mutex> lock(mtx_busy);
			cv_data.wait(lock);

			return 0;
		}
		else if (*slot == (_SLOT_T *)SlotState::EMPTY)
		{
			// Slot is empty - needs to be populated with data
			//printf("Got an empty slot\n");

			// If we've already loaded all source data
			// We just exit current handle loop
			// (might be better for the thread to try handling any data thats left?)
			if (input_index >= input_data.size())
			{
				mtx_slots.unlock();

				// Notify any threads that are waiting for data
				// They'll either find an empty slot - realize theres no data and exit
				//  or they'll handle any unhandled data first
				cv_data.notify_all();

				return -1;
			}

			// If another thread is already using the filesytem
			// we try to find some loaded data that hasn't been handled
			// and handle it
			if (!mtx_fs.try_lock())
			{
				// printf("Filesystem busy... looking for unhandled data\n");
				for (unsigned int i = 0; i < numSlots; i++)
				{
					if (slots[i] == (_SLOT_T *)SlotState::INUSE || slots[i] == (_SLOT_T *)SlotState::EMPTY)
						continue;

					slot = &slots[i];
					break;
				}

				if (*slot != (_SLOT_T *)SlotState::INUSE && *slot != (_SLOT_T *)SlotState::EMPTY)
					return Handle_Data();

				mtx_slots.unlock();
				return 0;
			}

			// Prevent any other threads from taking the slot while we populate the data
			*slot = (_SLOT_T *)SlotState::INUSE;

			// Update the active input data index, but grab the current one first
			unsigned int src_index = input_index++;

			mtx_slots.unlock();

			// Allocate the data for the slot
			// and call the _SlotInit callback to run the init code for the new data
			_SLOT_T *data = new _SLOT_T;
			_SlotInit_Callback(input_data[src_index], *data);

			mtx_fs.unlock();

			// Run any PostInit code after the filesystem mutex has been freed
			_SlotPostInit_Callback(*data);

			// Set slot to point to the data that we initialized
			mtx_slots.lock();
			*slot = data; // Assign the data pointer to the slot
			// printf("ALLOC DATA: 0x%" PRIx64 "\n", (long unsigned int)data);
			mtx_slots.unlock();

			// Notify any threads that were waiting for data
			cv_data.notify_one();

			return 0;
		}
		else
		{
			// We got a normal slot with data in it
			// printf("Found data to handle!\n");
			return Handle_Data();
		}

		return 1;
	}

	static int WorkerFunc(Handler* handler)
	{
		int result;
		do
		{
			result = handler->ExecuteLoopIteration();
		} while (result == 0);

		return result;
	}

  public:
	Handler(std::vector<_SRC_T> &source_data, unsigned int _slotCount,
			_SlotHandleData_f HandlerCallback=Handler::_SlotHandleData_DefaultCallback,
			_SlotInit_f InitCallback = Handler::_SlotInit_DefaultCallback,
			_SlotPostInit_f PostInitCallback = Handler::_SlotPostInit_DefaultCallback)
		: input_data(source_data), input_index(0),
		_SlotInit_Callback(InitCallback),
		_SlotPostInit_Callback(PostInitCallback),
		_SlotHandleData_Callback(HandlerCallback)
	{
		numSlots = _slotCount;
		slots = new _SLOT_T *[numSlots];
		memset(slots, 0, sizeof(_SLOT_T *) * numSlots);
	}

	~Handler()
	{
		mtx_slots.lock();

		for (unsigned int i = 0; i < numSlots; i++)
		{
			FreeSlot(&slots[i]);
		}

		_SLOT_T **p = slots;
		slots = NULL;
		delete[] p;

		mtx_slots.unlock();
	}

	void RunHandler(unsigned int workerCount = 1)
	{
		workerCount -= 1;

		std::thread* threads = new std::thread[workerCount];
		for(unsigned int i = 0; i < workerCount; i++)
		{
			threads[i] = std::thread(Handler::WorkerFunc, this);
		}

		Handler::WorkerFunc(this);

		for(unsigned int i = 0; i < workerCount; i++)
		{
			threads[i].join();
		}

		delete[] threads;
	}
};