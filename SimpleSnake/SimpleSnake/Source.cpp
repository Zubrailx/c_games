#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>

struct Point {
	int x;
	int y;
};

class Field;

//-------------------------class Snake---------------------------------//
class Snake {
public:
	enum class Direction {
		LEFT,
		UP,
		RIGHT,
		DOWN
	};
private:
	Point* m_arr;
	int m_length = 1;
	Direction m_dir{ Direction::RIGHT };
	bool m_isAlive{ true };

	const int max_length;
public:

	Snake(int length) : max_length(length) {
		m_arr = new Point[max_length];
	};
	~Snake() {
		delete[] m_arr;
	}

	const Point* getSnakeCoord() const {
		return  m_arr;
	}
	int getCurrentLength() const {
		return m_length;
	}

	bool IsAlive() {
		return m_isAlive;
	}
	void update(HANDLE h, const Field &field);

};

//-------------------------class Field---------------------------------//
class Field {
private:
	const int m_width;
	const int m_height;
	const char m_border = '%';
public:
	Field(int width, int height) :m_width(width), m_height(height) {};

	void printBorder(HANDLE h) {
		const char* chr = &m_border;
		COORD coord;
		DWORD written;
		std::cout << "\n\n\n";
		for (int x = 0; x < m_width; ++x) {
			coord.Y = 0;
			coord.X = x;
			WriteConsoleOutputCharacterA(h, chr, 1, coord, &written);
			coord.Y = m_height - 1;
			WriteConsoleOutputCharacterA(h, chr, 1, coord, &written);
		}
		for (int y = 0; y < m_height; ++y) {
			coord.X = 0;
			coord.Y = y;
			WriteConsoleOutputCharacterA(h, chr, 1, coord, &written);
			coord.X = m_width - 1;
			WriteConsoleOutputCharacterA(h, chr, 1, coord, &written);
		}
	}
	friend void Snake::update(HANDLE h, const Field& field);
};


//-------------------------class Fruit---------------------------------//
class Fruit {
private:
	Point m_pos;
public:
	const Point& getCoordinates() {
		return m_pos;
	}
};

//-------------------------friend-----------------------------------//
void Snake::update(HANDLE h, const Field& field) {

}


//-------------------------main()---------------------------------//
int main() {
	std::cout << "Enter the field width, height:";
	int width, height;
	std::cin >> width >> height;
	Field field(width, height);

	std::cout << "Enter the maximus size of snake:";
	int size;
	std::cin >> size;
	Snake snake(size);

	std::system("CLS");
	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetCursorPos(40, 40);
	field.printBorder(stdOut);
	while (snake.IsAlive()) {

	}
	snake.update(stdOut, field);
}