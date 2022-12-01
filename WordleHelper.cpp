
#include <fstream>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

struct Wordle
{
	Wordle()
	{
		memset(letter, 0, 5);
		letter[5] = '\0';
	}
	Wordle(char const *p)
	{
		memcpy(letter, p, 5);
		letter[5] = '\0';
	}

	int Score(char const *) const;

	inline operator char const *() const { return letter; }

private:
	char letter[6];
};

static size_t rand(size_t n)
{
	size_t accu = std::rand();
	accu = accu << 15 | std::rand();
	accu = accu << 15 | std::rand();
	accu = accu << 15 | std::rand();
	accu = accu << 15 | std::rand();
	return accu % n;
}

double rnd() noexcept
{
	return double(rand(0x20000000000000ULL)) / 0x20000000000000ULL;
}

int Wordle::Score(char const *guess) const
{
	int score[5] = { 0, 0, 0, 0, 0 };

	for(int i = 0; i < 5; ++i)
	{
		if(letter[i] == guess[i])
		{
			score[i] = 2; // Green!
			continue;
		}

		for(int j = 0; j < 5; ++j)
		{
			if(letter[j] == guess[i])
			{
				score[i] = 1; // Yellow!
				break;
			}
		}
	}

	return score[0] + 3 * (score[1] + 3 * (score[2] + 3 * (score[3] + 3 * score[4])));
}

Wordle Contemplate(std::vector<Wordle> const &dictionary, std::vector<Wordle> const &candidates)
{
	static int round = 0;

	switch(++round)
	{
	case 1:
		return "RAISE";
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	}

#if 1
	Wordle result;
	double best = 0;
	int keep = 0;

	for(auto const &guess : dictionary)
	{
		int count[243] = { 0 };
		int classes = 0;
		int maximum = 0;

		for(auto const &candidate : candidates)
		{
			auto score = candidate.Score(guess);
			if(++count[score] == 1)
				++classes;
			if(maximum < count[score])
				maximum = count[score];
		}

		double current = double(classes) / maximum;
		// int current = 488 * maximum - 2 * classes - count[242];

		if(best > current)
			continue;
		if(best == current && ++keep * rnd() > 1)
			continue; // Randomize between equally good scores
		if(best < current)
			keep = 1;

		best = current;
		result = guess;
	}
#else
	Wordle result;
	int best = INT_MAX;
	int keep = 0;

	for(auto const &guess : dictionary)
	{
		int count[243];
		int classes = 0;
		int current = 0;

		for(int i = 0; i < 243; ++i)
			count[i] = 0;
		for(auto const &candidate : candidates)
			if(++count[candidate.Score(guess)] == 1)
				++classes;
		for(int i = 0; i < 243; ++i)
			current = count[i] * count[i];

		if(best < current)
			continue;
		if(best == current && ++keep * rnd() > 1)
			continue; // Randomize between equally good scores
		if(best > current)
			keep = 1;

		best = current;
		result = guess;
	}
#endif
	return result;
}

int Evaluate(Wordle const &guess)
{
	std::string input;
	int score[5] = { 0, 0, 0, 0, 0 };

redo:
	cout << guess << " ----> ";
	std::cin >> input;

	for(int i = 0; i < 5; ++i)
	{
		switch(input[i])
		{
		case '0':
		case 'b':
		case 'B':
			score[i] = 0;
			break;

		case '1':
		case 'y':
		case 'Y':
			score[i] = 1;
			break;

		case '2':
		case 'g':
		case 'G':
			score[i] = 2;
			break;

		default:
			goto redo;
		}
	}

	return score[0] + 3 * (score[1] + 3 * (score[2] + 3 * (score[3] + 3 * score[4])));
}

std::vector<Wordle> Eliminate(std::vector<Wordle> const &candidates, char const *guess, int score)
{
	std::vector<Wordle> result;
	for(auto const &candidate : candidates)
		if(candidate.Score(guess) == score)
			result.push_back(candidate);
	return result;
}

std::vector<Wordle> Load(char const *filename)
{
	std::vector<Wordle> result;
	std::ifstream input(filename);

	while(!input.eof())
	{
		char c[6] = { 0, 0, 0, 0, 0, 0 };

		input >> c[0];
		c[0] = toupper(c[0]);

		if(c[0] < 'A' || c[0] > 'Z')
			continue;

		for(int i = 1; i < 5; ++i)
		{
			input >> c[i];
			c[i] = toupper(c[i]);
		}

		result.emplace_back(c);
	}

	input.close();
	return result;
}

//**********************************************************************************************************************

void F()
{
	std::vector<Wordle> dictionary = Load("Dictionary.dat");

	Wordle test("LXOYZ");
	Wordle goal("GLORY");

	cout << goal.Score(test) << endl;

	for(auto const &word : dictionary)
	{
		if(word[0] != 'L' && word[0] == 'L' && word[0] == 'L') continue;
		if(word[1] != 'L' && word[1] == 'L' && word[1] == 'L') continue;
		if(word[2] != 'O') continue;
		if(word[3] != 'L' && word[3] == 'L' && word[3] == 'L') continue;
		if(word[4] != 'L' && word[4] == 'L' && word[4] == 'L') continue;
	}
}


int main()
{
	// F(); return 0;

	std::vector<Wordle> dictionary = Load("Dictionary.dat");
	std::vector<Wordle> candidates = Load("Candidates.dat");

	cout << endl
		 << "Playing: Enter the word in Wordle and type the response back as follows: 'B'=black, 'G'=green, 'Y'=yellow." << endl;

	Wordle guess;
	int score;

	while(candidates.size() > 1)
	{
		guess = Contemplate(dictionary, candidates);
		score = Evaluate(guess);
		candidates = Eliminate(candidates, guess, score);

		cout << "Candidates left: " << candidates.size() << endl;
		for(auto const &item : candidates)
			cout << item << ", ";
		cout << endl;
	}

	if(candidates.size())
	{
		cout << "Answer: " << candidates[0] << endl;
	}
	else
	{
		cout << "No idea!" << endl;
	}
}

//**********************************************************************************************************************
