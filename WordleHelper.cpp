
#include <fstream>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

// #define ABSURDLE_MODE
#define VERBOSE
// #define WIN_OR_BUST

//**********************************************************************************************************************

struct Wordle
{
	Wordle()
	{
		memset(hidden, 0, 5);
		hidden[5] = '\0';
	}
	Wordle(char const *p)
	{
		memcpy(hidden, p, 5);
		hidden[5] = '\0';
	}

	int Score(char const *) const;

	inline operator char const *() const { return hidden; }

private:
	char hidden[6];
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
	int taken[5] = { 0, 0, 0, 0, 0 };

	for(int i = 0; i < 5; ++i)
	{
		if(guess[i] == hidden[i])
		{
			score[i] = 2; // Green!
			taken[i] = 1;
		}
	}

	for(int i = 0; i < 5; ++i)
	{
		if(score[i]) continue;

		for(int j = 0; j < 5; ++j)
		{
			if(taken[j]) continue;

			if(guess[i] == hidden[j])
			{
				score[i] = 1; // Yellow!
				taken[j] = 1;
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
#if defined(WIN_OR_BUST)
		return "LATEN";
#else
#if defined(ABSURDLE_MODE)
		return "RAISE";
#else
		return "TRACE";
#endif
#endif
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

	Wordle result;
	int min_maximum = INT_MAX;
	int max_classes = 0;
	int max_correct = 0;
	int max_uniques = 0;
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

#if defined(WIN_OR_BUST)

		// Impatient mode: Make as many options for the next guess as possible

		int uniques = 0;
		for(int i = 0; i < 243; ++i) if(count[i] == 1) ++uniques;

		if(uniques < max_uniques) continue;
		if(uniques > max_uniques)
		{
#if defined(VERBOSE)
			cout << guess << " <---- Number of unique candidates: " << classes << endl;
#endif
			result = guess;
			max_uniques = uniques;
			max_classes = classes;
			max_correct = count[242];
			keep = 1;
			continue;
		}

#endif

#if defined(ABSURDLE_MODE)

		// Absurdle-mode aims to reduce the candidate set as fast as possible
		// https://qntm.org/files/absurdle/absurdle.html

		// First, pick the guess whose largest candidate set is smallest among all guesses

		if(maximum > min_maximum) continue;
		if(maximum < min_maximum)
		{
#if defined(VERBOSE)
			cout << guess << " <---- Largest remaining candidate set: " << maximum << endl;
#endif
			result = guess;
			min_maximum = maximum;
			max_classes = classes;
			max_correct = count[242];
			keep = 1;
			continue;
		}

		// Next, pick the guess that produces greater multitude of candidate sets

		if(classes < max_classes) continue;
		if(classes > max_classes)
		{
#if defined(VERBOSE)
			cout << guess << " <---- Number of remaining candidate sets: " << classes << endl;
#endif
			result = guess;
			max_classes = classes;
			max_correct = count[242];
			keep = 1;
			continue;
		}

#else

		// Wordle mode aims to maximize the information content of the responses
		// https://www.nytimes.com/games/wordle/index.html

		// First, pick the guess that produces greatest multitude of candidate sets

		if(classes < max_classes) continue;
		if(classes > max_classes)
		{
#if defined(VERBOSE)
			cout << guess << " <---- Number of remaining candidate sets: " << classes << endl;
#endif
			result = guess;
			max_classes = classes;
			min_maximum = maximum;
			max_correct = count[242];
			keep = 1;
			continue;
		}

		// Next, pick the guess whose larger candidate set is smallest among all guesses

		if(maximum > min_maximum) continue;
		if(maximum < min_maximum)
		{
#if defined(VERBOSE)
			cout << guess << " <---- Largest remaining candidate set: " << maximum << endl;
#endif
			result = guess;
			min_maximum = maximum;
			max_correct = count[242];
			keep = 1;
			continue;
		}

#endif

		// Finally, pick the guess that is a member of the candidate set

		if(count[242] < max_correct) continue;
		if(count[242] > max_correct)
		{
#if defined(VERBOSE)
			cout << guess << " <---- Member of the current candidate set." << endl;
#endif
			result = guess;
			max_correct = count[242];
			keep = 1;
			continue;
		}

		// If still here, randomize...

		if(++keep * rnd() < 1)
		{
#if defined(VERBOSE)
			cout << guess << " <---- Randomized." << endl;
#endif
			result = guess;
		}
	}

	cout << "=====" << endl << result << endl;
	if(max_correct) cout << " * Member of the current candidate set." << endl;
	cout << " * Number of remaining candidate sets: " << max_classes << endl;
	cout << " * Largest remaining candidate set: " << min_maximum << endl;
	cout << " * Win-or-bust candidates: " << max_uniques << endl;
	cout << "=====" << endl;

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

int main()
{
	std::vector<Wordle> dictionary = Load("Dictionary.dat");
	std::vector<Wordle> candidates = Load("Candidates.dat");
	std::vector<Wordle> completion = dictionary;

	cout << endl << "How to play: Enter the word in Wordle and type the response back as follows: 'B'=black, 'G'=green, 'Y'=yellow." << endl;

	Wordle guess;
	int score;

	while(candidates.size() > 1)
	{
		guess = Contemplate(dictionary, candidates);
		score = Evaluate(guess);
		candidates = Eliminate(candidates, guess, score);
		completion = Eliminate(completion, guess, score);

		if(candidates.size() == 0 && completion.size() > 0)
		{
			cout << "Out of candidates, switching to completion." << endl;
			candidates = completion;
		}

		cout << "Candidates left: " << candidates.size() << endl;
#if defined(VERBOSE)
		for(auto const &item : candidates)
			cout << item << ", ";
		cout << endl;
#endif
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
