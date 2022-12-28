
#include <fstream>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

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

	bool Excludes(char c) { return hidden[0] != c && hidden[1] != c && hidden[2] != c && hidden[3] != c && hidden[4] != c; }
	bool Includes(char c) { return hidden[0] == c || hidden[1] == c || hidden[2] == c || hidden[3] == c || hidden[4] == c; }

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
	int used[5] = { 0, 0, 0, 0, 0 };

	for(int i = 0; i < 5; ++i)
	{
		if(guess[i] == hidden[i])
		{
			score[i] = 2; // Green!
			used[i] = 1;  // Do not re-score
		}
	}

	for(int i = 0; i < 5; ++i)
	{
		if(score[i]) continue;

		for(int j = 0; j < 5; ++j)
		{
			if(used[j]) continue;

			if(guess[i] == hidden[j])
			{
				score[i] = 1; // Yellow!
				used[j] = 1;  // Do not re-score
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

	Wordle result;
	int min_maximum = INT_MAX;
	int max_correct = 0;
	int max_classes = 0;
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

		// Pick the guess whose largest candidate set is smallest among all guesses

		if(maximum > min_maximum) continue;
		if(maximum < min_maximum)
		{
			cout << guess << " <---- Largest candidate set: " << maximum << endl;

			result = guess;
			min_maximum = maximum;
			max_correct = count[242];
			max_classes = classes;

			if(max_correct) cout << guess << " <---- Member of candidate set." << endl;

			keep = 1;
			continue;
		}

		// Next, pick the guess that is a member of the candidate set

		if(count[242] < max_correct) continue;
		if(count[242] > max_correct)
		{
			cout << guess << " <---- Member of candidate set." << endl;

			result = guess;
			max_correct = count[242];
			max_classes = classes;
			keep = 1;
			continue;
		}

		// Finally, pick the guess that produces greater multitude of candidate sets

		if(classes < max_classes) continue;
		if(classes > max_classes)
		{
			cout << guess << " <---- Number of candidate sets:" << classes << endl;
			if(max_correct) cout << guess << " <---- Member of candidate set." << endl;

			result = guess;
			max_classes = classes;
			keep = 1;
			continue;
		}

		// If still here, randomize...

		if(++keep * rnd() < 1) result = guess;
	}

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

	Wordle test("CLOOT");
	Wordle goal("CHORD");

	cout << goal.Score(test) << endl;
}


int main()
{
	// F(); return 0;

	std::vector<Wordle> dictionary = Load("Dictionary.dat");
	std::vector<Wordle> candidates = Load("Candidates.dat");

	cout << endl << "Playing: Enter the word in Wordle and type the response back as follows: 'B'=black, 'G'=green, 'Y'=yellow." << endl;

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
