#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>
#include <ctime>


class Field;
class Fruit;

//--------------------------class Point----------------------------//
class Point {
private:
	int m_x = 0;
	int m_y = 0;
public:
	Point() {};
	Point(int x, int y) : m_x(x), m_y(y) {};

	int getX() {
		return m_x;
	}
	int getY() {
		return m_y;
	}
	static Point generateRandomPoint(int fieldWidth, int fieldHeight) {
		int x, y;
		do {
			x = rand() % (fieldWidth - 1);
			y = rand() % (fieldHeight - 1);
		} while (x == 0 || y == 0);
		return Point(x, y);
	}
	friend bool operator==(const Point& p1, const Point& p2);
};

bool operator==(const Point& p1, const Point& p2) {
	return (p1.m_x == p2.m_x && p1.m_y == p2.m_y);
}

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
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
};

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
	const int max_length;
	int m_length = 1;
	Direction m_dir{ Direction::RIGHT };
	bool m_isAlive{ true };
	bool m_isEaten{ false };

public:
	//constructor, destructor                                 
	Snake(int length) : max_length(length) {
		m_arr = new Point[max_length+1]; //for >>
		*m_arr = Point(1, 1); //the start position of the snake
	};
	~Snake() {
		delete[] m_arr;
	}

	//length of the snake
	void increaseLength() {
		++m_length;
	}
	int getCurrentLength() const {
		return m_length;
	}

	//the life status of the snake
	void setIsAlive(const Field& f) {
		Point p = this->getHeadCoord(); //border check
		if (p.getX() >= (f.getWidth() - 1) || p.getX() == 0) m_isAlive = false;
		if (p.getY() >= (f.getHeight() - 1) || p.getY() == 0) m_isAlive = false;

		for (int count = 1; count < m_length; ++count) { //snake stuck check
			if (m_arr[count] == p) {
				m_isAlive = false;
			}
		}
	}
	bool getIsAlive() {
		return m_isAlive;
	}
	bool hasWon() {
		return max_length <= m_length;
	}

	//reprint the snake
	void updatePosition() {
		for (int count = m_length - 1; count >= 0; --count) {
			m_arr[count + 1] = m_arr[count]; //сдвиг вправо на единицу
		}
		int head_x = m_arr[1].getX();
		int	head_y = m_arr[1].getY();

		switch (m_dir) {
		case Direction::DOWN:
			head_y -= 1;
			break;
		case Direction::LEFT:
			head_x -= 1;
			break;
		case Direction::RIGHT:
			head_x += 1;
			break;
		case Direction::UP:
			head_y += 1;
			break;
		}
		Point newHead(head_x, head_y);
		m_arr[0] = newHead;
	}

	void updateDirection() {
		if ((GetKeyState('W') & 0x8000) && (m_dir != Direction::UP)) m_dir = Direction::DOWN;
		if ((GetKeyState('A') & 0x8000) && (m_dir != Direction::RIGHT)) m_dir = Direction::LEFT;
		if ((GetKeyState('S') & 0x8000) && (m_dir != Direction::DOWN)) m_dir = Direction::UP;
		if ((GetKeyState('D') & 0x8000) && (m_dir != Direction::LEFT)) m_dir = Direction::RIGHT;
	}

	void print(HANDLE h) {
		DWORD written;
		COORD coord;
		char snake_chr = 'O';
		const char* chr = &snake_chr;

		//painting the head.
		Point head = m_arr[0];
		coord.X = head.getX();
		coord.Y = head.getY();
		WriteConsoleOutputCharacterA(h, chr, 1, coord, &written);

		//removing the tail.
		Point oldTail = m_arr[m_length];
		coord.X = oldTail.getX();
		coord.Y = oldTail.getY();
		snake_chr = ' ';
		WriteConsoleOutputCharacterA(h, chr, 1, coord, &written);
	}
	//to check whether the snake is alive
	const Point& getHeadCoord() const {
		return *m_arr; //the head of the snake always is the first element of the arr.
	}

};

//-------------------------class Fruit---------------------------------//
class Fruit {
private:
	Point m_pos;
	const char m_chr;
public:
	Fruit(char chr = '&') : m_chr(chr) {
	}

	void setCoordinates(Point& p) {
		m_pos = p;
	}
	const Point& getCoordinates() {
		return m_pos;
	}

	void print(HANDLE h) {
		DWORD written;
		COORD coord;

		const char* chr = &m_chr;
		coord.X = m_pos.getX();
		coord.Y = m_pos.getY();
		WriteConsoleOutputCharacterA(h, chr, 1, coord, &written);
	}
};

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

	//установка системных часов для правильной генерации случайных чисел
	srand(static_cast<unsigned int>(time(0)));
	Fruit fruit;
	Point p = Point::generateRandomPoint(field.getWidth(), field.getHeight());
	fruit.setCoordinates(p);
	fruit.print(stdOut);

	while (true) {
		snake.updateDirection();
		snake.updatePosition();
		snake.print(stdOut);
		snake.setIsAlive(field);
		if (snake.getHeadCoord() == fruit.getCoordinates()) {
			snake.increaseLength();
			p = Point::generateRandomPoint(field.getWidth(), field.getHeight());
			fruit.setCoordinates(p);	
			fruit.print(stdOut);
		}
		if (!snake.getIsAlive()) {
			std::system("CLS");
			std::cout << "You lost with the length " << snake.getCurrentLength() << std::endl;
			break;
		}
		if (snake.hasWon()) {
			std::system("CLS");
			std::cout << "You has won this game." << std::endl;
			break;
		}
		Sleep(100);
	}
	Sleep(1000);
	std::cout << "\nPress enter to exit.";
}