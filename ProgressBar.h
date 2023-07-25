#pragma once
#include <iostream>
#include <Windows.h>


class ProgressBar {
public:
	ProgressBar(const int max_length);
	ProgressBar(const ProgressBar&) = delete;
	ProgressBar(ProgressBar&&) noexcept = delete;
	ProgressBar& operator=(const ProgressBar&) = delete;
	ProgressBar& operator=(ProgressBar&&) noexcept = delete;
	~ProgressBar() {}

	int get_step() const;
	void getProgress(const bool error);
	ProgressBar& operator++(int);

private:
	const int m_length = 40;
	int m_max_length;
	double m_quotient;
	int m_step;
	int m_count = 0;
	bool m_error = false;
	int m_count_error = -1;
};

