
#include <fstream>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

#define VERBOSE true

struct Wordle
{
	Wordle() { memset(letter, 0, 5); }
	Wordle(char const* p) { memcpy(letter, p, 5); }

	int Score(char const*) const;

	friend std::ostream& operator<<(std::ostream& out, Wordle const& w) { return out << w.letter[0] << w.letter[1] << w.letter[2] << w.letter[3] << w.letter[4]; }
	inline operator char const* () const { return letter; }

private:
	char letter[5];
};

int Wordle::Score(char const* guess) const
{
	int score[5] = { 0,0,0,0,0 };

	for (int i = 0; i < 5; ++i)
	{
		if (letter[i] == guess[i])
		{
			score[i] = 2;  // Green!
			continue;
		}

		for (int j = 0; j < 5; ++j)
		{
			if (letter[i] == guess[j] && score[j] == 0)
			{
				score[j] = 1;  // Yellow!
				break;
			}
		}
	}

	return score[0] + 3 * (score[1] + 3 * (score[2] + 3 * (score[3] + 3 * score[4])));
}

double rnd(double prop = 0)
{
	prop = (prop + rand()) / (RAND_MAX + 1);
	prop = (prop + rand()) / (RAND_MAX + 1);
	prop = (prop + rand()) / (RAND_MAX + 1);
	prop = (prop + rand()) / (RAND_MAX + 1);
	return (prop + rand()) / (RAND_MAX + 1);
}

Wordle Contemplate(std::vector<Wordle> const& dictionary, std::vector<Wordle> const& candidates)
{
	Wordle result;
	int best = INT_MAX;
	int keep = 0;

	for (auto const& guess : dictionary)
	{
		int count[243]; for (int i = 0; i < 243; ++i) count[i] = 0;
		int entropy = 0;

		for (auto const& candidate : candidates)
		{
			auto score = candidate.Score(guess);
			++count[score];
			if (entropy < count[score]) entropy = count[score];
		}

		entropy = entropy * 2 - count[242];

		if (best < entropy) continue;
		if (best == entropy && ++keep * rnd() > 1) continue;
		if (best > entropy) keep = 1;

		best = entropy;
		result = guess;

		// if (VERBOSE) cout << guess << " ----> " << best << endl;
	}

	return result;
}

std::vector<Wordle> Eliminate(std::vector<Wordle> const& candidates, char const* guess, int score)
{
	std::vector<Wordle> result;
	for (auto const& candidate : candidates) if (candidate.Score(guess) == score) result.push_back(candidate);
	return result;
}

int Response(Wordle const& guess)
{
	std::string input;
	int score[5] = { 0,0,0,0,0 };
	int result;

redo:
	cout << guess << " ----> ";
	std::cin >> input;

	for (int i = 0; i < 5; ++i)
	{
		switch (input[i])
		{
		case '0': case 'b': case 'B':
			score[i] = 0;
			break;

		case '1': case 'y': case 'Y':
			score[i] = 1;
			break;

		case '2': case 'g': case 'G':
			score[i] = 2;
			break;

		default:
			goto redo;
		}
	}

	return score[0] + 3 * (score[1] + 3 * (score[2] + 3 * (score[3] + 3 * score[4])));
}

//**********************************************************************************************************************

int main()
{
	std::vector<Wordle> dictionary;

	cout << "Building the dictionary..." << endl;

	std::ifstream input("WordleHelper.dat");

	while (!input.eof())
	{
		char c[5] = { 0,0,0,0,0 };

		input >> c[0];
		c[0] = toupper(c[0]);

		if (c[0] >= 'A' && c[0] <= 'Z') for (int i = 1; i < 5; ++i) input >> c[i]; else continue;

		for (int i = 0; i < 5; ++i) c[i] = toupper(c[i]);

		dictionary.emplace_back(c);
	}

	input.close();

	std::vector<Wordle> candidates = dictionary;
	Wordle guess;
	int score;

	cout << "Optimizing initial guess..." << endl;

	while (candidates.size() > 1)
	{
		guess = Contemplate(dictionary, candidates);
		score = Response(guess);
		candidates = Eliminate(candidates, guess, score);

		if (VERBOSE && candidates.size() < 1000)
		{
			for (auto const& candidate : candidates) cout << candidate << ", ";
			cout << endl;
		}
	}

	switch (candidates.size())
	{
	case 0:
		cout << "No idea!" << endl;
		break;

	case 1:
		cout << "Answer: " << candidates[0] << endl;
		break;

	default:
		cout << "Candidate words: " << candidates.size() << endl;
		for (auto const& item : candidates) cout << item << ", "; cout << endl;
	}

	return EXIT_SUCCESS;
}

//**********************************************************************************************************************
