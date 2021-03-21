#pragma once

#include <cstdint>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Buffer
		{
		public:

			enum Usage
			{
				Vertex = 0,
				Index,
				Uniform,
				Count
			};

		public:

							Buffer(Usage usage);
							Buffer(const Buffer&) = delete;
							Buffer(Buffer&&) = delete;
			virtual			~Buffer() = default;

			void			operator=(const Buffer&) = delete;
			void			operator=(Buffer&&) = delete;

		public:

			virtual bool	Resize(uint32_t size) = 0;
			virtual void*	Lock() = 0;
			virtual void	Unlock() = 0;

		protected:

			Usage			_usage;
			uint32_t		_size = 0;
		};
	}
}
