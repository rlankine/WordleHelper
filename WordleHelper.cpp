
#include <assert.h>
#include <fstream>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

// Normal mode plays smart: Generates guesses where the number of candidate set partitions is as large as possible
// https://www.nytimes.com/games/wordle/index.html
// Absurdle-mode plays greedy: Generates guesses where the largest candidate set partition is as small as possible
// https://qntm.org/files/absurdle/absurdle.html

// #define ABSURDLE_MODE
#define VERBOSE

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

	operator char const *() const { return hidden; }

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
Wordle Contemplate(std::vector<Wordle> const &dictionary, std::vector<Wordle> const &candidates, int round)
{
	assert(candidates.size() > 0);

	// For one or two candidates either there is no choice or the order of choices does not matter
	// For three candidates a choice is possible that is either correct or eliminates one of the other two
	if(candidates.size() < 3) return candidates[0];

	switch(round)
	{
	case 1:
		break;  // Add forced guesses here
	case 2:
		break;  // Add forced guesses here
	case 3:
		break;  // etc.
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

#if defined(ABSURDLE_MODE)

		// First, pick the guess whose largest candidate set is smallest among all guesses

		if(maximum > min_maximum) continue;
		if(maximum < min_maximum)
		{
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
			result = guess;
			max_classes = classes;
			max_correct = count[242];
			keep = 1;
			continue;
		}

#else

		// First, pick the guess that produces greatest multitude of candidate sets

		if(classes < max_classes) continue;
		if(classes > max_classes)
		{
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
			result = guess;
			min_maximum = maximum;
			max_correct = count[242];
			keep = 1;
			continue;
		}

		// Finally, pick the guess that is a member of the candidate set

		if(count[242] < max_correct) continue;
		if(count[242] > max_correct)
		{
			result = guess;
			max_correct = count[242];
			keep = 1;
			continue;
		}

#endif

		// If still here, randomize...

		if(++keep * rnd() < 1)
		{
			result = guess;
		}
	}

#if defined(VERBOSE)
	cout << "=====" << endl << result << endl;
	if(max_correct) cout << " * Member of the current candidate set." << endl;
	cout << " * Number of remaining candidate sets: " << max_classes << endl;
	cout << " * Largest remaining candidate set: " << min_maximum << endl;
	cout << "=====" << endl;
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

std::vector<Wordle> Trim(std::vector<Wordle> candidates, std::vector<Wordle>const &completion)
{
    std::vector<Wordle> result;

    for (auto const &candidate : candidates)
    {
        for (auto const &word : completion) if (strcmp(candidate, word) == 0) goto next;
        result.push_back(candidate);
next:   ;
    }
    return result;
}

//**********************************************************************************************************************

int CountMoves(std::vector<Wordle> const &dictionary, std::vector<Wordle> candidates, Wordle const &hidden)
{
	Wordle guess;
	int round = 0;
	int score;

	while(candidates.size())
	{
		++round;
		guess = Contemplate(dictionary, candidates, round);
		cout << guess << ",";
		score = hidden.Score(guess);
		if(score == 242) break;
		candidates = Eliminate(candidates, guess, score);
	}

	return round;
}

//**********************************************************************************************************************

void Analyze(std::vector<Wordle> const &dictionary, std::vector<Wordle> const &candidates)
{
	int count = 0;
	int moves;
	int total = 0;

	for(auto const &candidate : candidates)
	{
		cout << candidate << ":";
		++count;
		moves = CountMoves(dictionary, candidates, candidate);
		total += moves;
		cout << ":" << moves << endl;
	}

	cout << endl;
	cout << "Count: " << count << endl;
	cout << "Total: " << total << endl;
}

//**********************************************************************************************************************

int main()
{
	std::vector<Wordle> candidates = Load("Candidates.dat");
	std::vector<Wordle> completion = Load("Completion.dat");
    std::vector<Wordle> dictionary = Load("Dictionary.dat");

	// See https://wordfinder.yourdictionary.com/wordle/answers/ for more completion

#if !defined(ABSURDLE_MODE)
    candidates = Trim(candidates, completion);
	completion = dictionary;
#endif

	cout << endl << "How to play: Enter the word in Wordle and type the response back as follows: 'B'=black, 'G'=green, 'Y'=yellow." << endl;

	// Analyze(dictionary, candidates); return 0;
	// Analyze(candidates, candidates); return 0;
	// Analyze(dictionary, dictionary); return 0;
	// CountMoves(dictionary, candidates, "INBOX"); return 0;

	Wordle guess;
	int round = 0;
	int score;

	while(candidates.size())
	{
		++round;
		guess = Contemplate(dictionary, candidates, round);
		score = Evaluate(guess);
		if(score == 242) break;
		candidates = Eliminate(candidates, guess, score);
		completion = Eliminate(completion, guess, score);

		if(candidates.size() == 0 && completion.size() > 0)
		{
#if defined(VERBOSE)
			cout << "Out of candidates, switching to completion." << endl;
#endif
			candidates = completion;
		}

#if defined(VERBOSE)
		cout << "Candidates left: " << candidates.size() << endl;
		for(auto const &item : candidates)
			cout << item << ", ";
		cout << endl;
#endif
	}

	if(candidates.size())
	{
		cout << "Answer: " << guess << ", found in " << round << " rounds." << endl;
	}
	else
	{
		cout << "No idea!" << endl;
	}
}

//**********************************************************************************************************************
