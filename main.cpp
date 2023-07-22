#include <iostream>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <mutex>
#include <Windows.h>

#include "ProgressBar.h"

using namespace std::chrono_literals;


std::mutex mtx;

std::atomic_int generate_error = 0;											//Опция генерации случайной ошибки
int random_error = 0;														//Случайное число ошибки

void set_cursor(const int x, const int y);
void some_work(ProgressBar& prb_all, const std::chrono::milliseconds time_interval, const int calculation_thread, const int thread_number, std::map<int, std::pair<std::thread::id, double>>& map_thread);

int main() {
	srand(unsigned(time(NULL)));

	setlocale(LC_ALL, "ru");
	std::cout << "\tПрогресс бар\n\n" << std::endl;
	std::cout << "Количество аппаратных ядер: " << std::thread::hardware_concurrency() << std::endl << std::endl;

	int threads_amount = 1;													//Количество потоков
	std::vector<std::thread> vector_thread;									//Вектор для созданных потоков
	std::map<int, std::pair<std::thread::id, double>> map_id_and_time;		//Контейнер для номера потока, id и времени
	int calculation_length = 400;											//Длина расчета
	int calculation_thread = 0;												//Длина расчета для каждого потока
	double all_time = 0;													//Общее затраченное машинное время

	std::cout << "Введите количество потоков(1-16): ";
	std::cin >> threads_amount;
	while (threads_amount < 1 || threads_amount > 16) {
		set_cursor(0, 5);
		std::cout << "Неккоректный ввод! Введите количество потоков от 1 до 16:                                                    ";
		set_cursor(58, 5);
		std::cin >> threads_amount;
	}

	std::cout << "\nУкажите длину расчета(1 - 100, 2 - 200, 3 - 400, 4 - 800, 5 - 1600): ";
	std::cin >> calculation_length;
	while (calculation_length < 1 || calculation_length > 5) {
		set_cursor(0, 7);
		std::cout << "Неккоректный ввод! Укажите длину расчета(1 - 100, 2 - 200, 3 - 400, 4 - 800, 5 - 1600):                                                    ";
		set_cursor(88, 7);
		std::cin >> calculation_length;
	}
	switch (calculation_length) {
	case 1: 
		calculation_length = 100;
		break;
	case 2:
		calculation_length = 200;
		break;
	case 3:
		calculation_length = 400;
		break;
	case 4:
		calculation_length = 800;
		break;
	case 5:
		calculation_length = 1600;
		break;
	default:
		calculation_length = 400;
		break;
	}
	ProgressBar prb_all(calculation_length);								//Класс имитации прогресс бар

	int g_e = 0;
	std::cout << "\nИмитировать случайную ошибку?(0 - нет, 1 - да): ";
	std::cin >> g_e;
	while (g_e < 0 || g_e > 1) {
		set_cursor(0, 9);
		std::cout << "Неккоректный ввод! Введите '0' для нормального запуска программы или '1' для генерации ошибки:                                                    ";
		set_cursor(95, 9);
		std::cin >> g_e;
	}
	generate_error.store(g_e);

	int k = 0;
	while ((calculation_length - k) % threads_amount) {
		k++;
	}
	calculation_thread = (calculation_length - k) / threads_amount;

	random_error = (rand() % (calculation_thread - 1)) + 1;

	std::cout << "\n\nКакая-то работа......." << std::endl;
	std::cout << "\nОбщий прогресс:";

	//Создание потоков
	auto all_time_start = std::chrono::steady_clock::now();

	for (int i = 0; i < threads_amount; ++i) {
		if (i == 0) {
			vector_thread.emplace_back(std::thread([&prb_all, calculation_length, calculation_thread, i, &map_id_and_time, k]() {some_work(prb_all, std::chrono::milliseconds(calculation_length), calculation_thread + k, i, map_id_and_time); }));
		}
		else {
			vector_thread.emplace_back(std::thread([&prb_all, calculation_length, calculation_thread, i, &map_id_and_time]() {some_work(prb_all, std::chrono::milliseconds(calculation_length), calculation_thread, i, map_id_and_time); }));
		}
	}

	//Ожидание потоков
	for (auto& it : vector_thread) {
		if (it.joinable()) {
			it.join();
		}
	}

	auto all_time_end = std::chrono::steady_clock::now();
	std::chrono::duration<double> all_dur = all_time_end - all_time_start;

	std::cout << std::endl << std::endl << std::endl;

	//Вывод данных о потоках
	for (const auto& it : map_id_and_time) {
		std::cout << "Поток №" << it.first << "\n\tИдентификатор: " << it.second.first << std::endl;
		std::cout << "\tВремя потока: " << it.second.second << " сек." << std::endl << std::endl;
		all_time += it.second.second;
	}

	std::cout << "---------------------------------------\n" << std::endl;
	std::cout << "Затрачено реального времени: " << all_dur.count() << " сек.\n" << std::endl;
	std::cout << "Общее машинное время: " << all_time << " сек." << std::endl;
	
	if (generate_error.load() == 2) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
		std::cout << "\nВ расчетах обнаружена ошибка!" << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}

	system("pause > nul");
	return 0;
}

void set_cursor(const int x, const int y) {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { static_cast<short>(x), static_cast<short>(y) });
}

void some_work(ProgressBar& prb_all, const std::chrono::milliseconds time_interval, const int calculation_thread, const int thread_number, std::map<int, std::pair<std::thread::id, double>>& map_thread) {
	auto time_start = std::chrono::steady_clock::now();

	int count = 0;
	while (count < calculation_thread) {
		std::unique_lock<std::mutex> ul(mtx);
		count += prb_all.get_step();
		prb_all++;
		set_cursor(15, 14);

		if (generate_error.load() == 1 && random_error <= count && random_error > count - prb_all.get_step()) {
			try {
				throw 1;
			}
			catch (int) {
				prb_all.getProgress(true);
				generate_error.store(2);
			}
		}
		else {
			prb_all.getProgress(false);
		}

		ul.unlock();

		if (count < calculation_thread) {
			std::this_thread::sleep_for(time_interval);
		}
	}

	auto time_end = std::chrono::steady_clock::now();

	std::chrono::duration<double> dur = time_end - time_start;
	
	map_thread.insert(std::make_pair(thread_number + 1, std::make_pair(std::this_thread::get_id(), dur.count())));
}
