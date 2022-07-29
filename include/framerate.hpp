#ifndef FRAME_LIMITER_HPP_INCLUDED
#define FRAME_LIMITER_HPP_INCLUDED

namespace framerate
{
	class Limiter
	{
		unsigned long target_microseconds_per_frame_;
		unsigned long frame_started_at_microseconds_;

		public:

		Limiter(unsigned max_fps);

		void begin_frame(unsigned long current_microseconds);
		unsigned end_frame(unsigned long current_microseconds);
	};

	class Counter
	{
		unsigned long frame_started_at_microseconds_;
		unsigned long total_duration_since_reset_;
		unsigned num_frames_since_reset_;

		public:

		Counter();

		void begin_frame(unsigned long current_microseconds);
		void end_frame(unsigned long current_microseconds);

		void reset();

		float get_average_fps();
	};
}

#endif
