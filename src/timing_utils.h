#pragma once

class Timing
{
public:
	static Timing& Get()
	{
		static Timing s_timing;
		return s_timing;
	}

	float GetTimef()
	{
		return 0.0f;
	}

private:
	Timing() {}
};