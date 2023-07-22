#include "ProgressBar.h"

ProgressBar::ProgressBar(const int max_length) : m_step(max_length / m_length), m_max_length(max_length) {}

int ProgressBar::get_step() const {
	return m_step;
}

void ProgressBar::getProgress(const bool error) {
	if (error) {
		m_error = true;
		m_count_error = m_count;
	}

	if (m_count >= m_max_length) {
		std::cout << " [";
		for (int i = 0; i < m_max_length; i += m_step) {
			if (i == m_count_error) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				std::cout << '+';
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
			else {
				std::cout << '+';
			}
		}
		std::cout << "] 100%";
		return;
	}

	std::cout << " [";
	for (int j = 0; j < m_count; j += m_step) {
		if (j == m_count_error) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
			std::cout << '+';
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		else {
			std::cout << '+';
		}
	}
	for (int k = 0; k < m_max_length - m_count; k += m_step) {
		std::cout << ' ';
	}
	std::cout << "] " << static_cast<double>(m_count) / m_max_length * 100 << '%';
}

ProgressBar& ProgressBar::operator++(int) {
	m_count += m_step;
	return *this;
}
