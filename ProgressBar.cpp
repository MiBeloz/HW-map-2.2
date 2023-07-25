#include "ProgressBar.h"

ProgressBar::ProgressBar(const int length) {
	m_quotient = static_cast<double>(m_length) / length;
	m_max_length = length;
	m_step = m_count * m_quotient;
}

int ProgressBar::get_step() const {
	return m_step;
}

void ProgressBar::getProgress(const bool error) {
	if (error) {
		m_error = true;
		m_count_error = m_count;
	}

	m_step = m_count * m_quotient;
	if (m_count >= m_max_length) {
		std::cout << '[';
		for (int i = 0; i < m_length; ++i) {
			if (i == m_count_error) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				std::cout << '+';
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
			else {
				std::cout << '+';
			}
		}
		std::cout << "] 100%     ";
		return;
	}

	std::cout << '[';

	for (int i = 0; i < m_step; ++i) {
		if (i == m_count_error) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
			std::cout << '+';
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		else {
			std::cout << '+';
		}
	}
	for (int i = 0; i < m_length - m_step; ++i) {
		std::cout << ' ';
	}
	std::cout << "] " << round(static_cast<double>(m_step) / m_length * 100) << "%     ";
}

ProgressBar& ProgressBar::operator++(int) {
	m_count++;
	return *this;
}
