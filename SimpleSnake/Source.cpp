#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>
#include <vector>


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
	friend bool operator==(const Point& p1, const Point& p2);

	static Point generateRandomPoint(int fieldWidth, const int fieldHeight, const Point* snakeArr, int snakeLength) {
		std::vector<int> arr;
		arr.resize(fieldHeight - 2);
		for (int i = 0; i < fieldHeight - 2; ++i) { //(1-28) = 28 strings so (0-27) [if field is 30*30]
			arr[i] = fieldWidth - 2;
		}
		Point p;
		for (int count = 0; count < snakeLength; ++count) { //permissible cords for fruit
			p = snakeArr[count];
			--arr[p.getY() - 1]; //because permissible cords starts with 0, not with 1.
		}

		int string; //find string that has positive amount of permissible cords. (there are fieldHeight - 2 strings)
		do {
			string = rand() % (fieldHeight - 2);
		} while (arr[string] == 0);
		int column = rand() % (arr[string]);
		
		//find the cord on the field
		arr.clear();
		std::vector<bool> arr2;
		arr2.resize(fieldWidth - 2);
		for (int i = 0; i < fieldWidth - 2; ++i) {
			arr2[i] = 1;
		}
		for (int count = 0; count < snakeLength; ++count) {
			p = snakeArr[count];
			arr2[p.getX() - 1] = 0; //because permissible cords starts with 0.
		}
		int column2;
		for (column2 = 0; column2 < fieldWidth - 2; ++column2) {
			if (arr2[column2] == 1) {
				if (column == 0) {
					break;
				}
				--column; //because permissible cord can be zero.
			}
		}
		return Point(string, column2);
	}
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
	const Point* getCoords() const {
		return m_arr;
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
//-------------------------methods()------------------------------//

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
	Point p = Point::generateRandomPoint(field.getWidth(), field.getHeight(), snake.getCoords(), snake.getCurrentLength());
	fruit.setCoordinates(p);
	fruit.print(stdOut);

	while (true) {
		//as more easy solution for handling user's keypresses.
		Sleep(100 / 3);
		snake.updateDirection();
		Sleep(100 / 3);
		snake.updateDirection();
		Sleep(100 / 3);
		snake.updateDirection();

		snake.updatePosition();
		snake.print(stdOut);
		snake.setIsAlive(field);
		if (snake.getHeadCoord() == fruit.getCoordinates()) {
			snake.increaseLength();
			p = Point::generateRandomPoint(field.getWidth(), field.getHeight(), snake.getCoords(), snake.getCurrentLength());
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
	}
	Sleep(1000);
	std::cout << "\nPress enter to exit.";
}