#include <iostream>

using namespace std;

int main() {
	int size;
	string in;

	// get size, then discard that line of input
	cin >> size;
	getline(cin, in);

	// initialize the board
	char board [size][size];

	// get board
	for (int row = 0; row < size; row++)
	{
		getline(cin, in);
		for (int col = 0; col < size; col++)
		{
			board[row][col] = in[col];
		}
	}

	char temp;
	bool moved = false;

	// simulate board
	do
	{
		moved = false;
		for (int row = size - 1; row >= 0; row--)
		{
			for (int col = 0; col < size; col++)
			{
				if (board[row][col] == '.' && board[row+1][col] == ' ')
				{
					board[row][col] = ' ';
					board[row+1][col] = '.';
					moved = true;
				}
			}
		}
	} while (moved);

	// print 
	for (int row = 0; row < size; row++)
	{
		for (int col = 0; col < size; col++)
		{
			cout << board[row][col];
		}
		cout << endl;
	}
}
