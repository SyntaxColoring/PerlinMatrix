#include "framerate.hpp"

const static unsigned MICROSECONDS_PER_SECOND = 1000000;

framerate::Limiter::Limiter(const unsigned max_fps)
: target_microseconds_per_frame_(MICROSECONDS_PER_SECOND/max_fps),
  frame_started_at_microseconds_(0)
{ }

void framerate::Limiter::begin_frame(const unsigned long current_microseconds)
{
	frame_started_at_microseconds_ = current_microseconds;
}

unsigned framerate::Limiter::end_frame(const unsigned long current_microseconds)
{
	const unsigned long elapsed_microseconds = current_microseconds - frame_started_at_microseconds_;
	if (elapsed_microseconds < target_microseconds_per_frame_)
		return target_microseconds_per_frame_ - elapsed_microseconds;
	else
		return 0;
}


framerate::Counter::Counter()
: frame_started_at_microseconds_(0),
  total_duration_since_reset_(0),
  num_frames_since_reset_(0)
{ }

void framerate::Counter::begin_frame(const unsigned long current_microseconds)
{
	frame_started_at_microseconds_ = current_microseconds;
}

void framerate::Counter::end_frame(const unsigned long current_microseconds)
{
	total_duration_since_reset_ += current_microseconds - frame_started_at_microseconds_;
	num_frames_since_reset_++;
}

void framerate::Counter::reset()
{
	total_duration_since_reset_ = 0;
	num_frames_since_reset_ = 0;
}

float framerate::Counter::get_average_fps()
{
	const unsigned average_micros_per_frame = total_duration_since_reset_ / num_frames_since_reset_;
	return static_cast<float>(MICROSECONDS_PER_SECOND) / average_micros_per_frame;
}
