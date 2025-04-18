/*
Implementation: Spam Detection System in C++
This implementation includes:
1.	Rule-Based Filtering: Checks for specific keywords or patterns in the email content.
2.	Bayesian Filtering: Uses probabilities to classify emails as spam or not spam based on word frequencies.

Explanation
1.	Rule-Based Filtering:
•	Predefined keywords (e.g., "win", "free", "money") are checked in the email content.
•	If any keyword is found, the email is flagged as spam.
2.	Bayesian Filtering:
•	Tokenizes the email content into words.
•	Uses a predefined probability map (spamWordProbabilities) to calculate the likelihood of the email being spam.
•	If the spam score is greater than 0.5, the email is classified as spam.
3.	Tokenization:
•	Splits the email content into words, removes punctuation, and converts them to lowercase for consistent matching.
4.	Output:
•	The system outputs whether the email is classified as spam or not by each filter.
---
Example Input/Output
Input: Email Content: "Congratulations! You have won a free lottery. Claim your money now!"
Output: 
Email detected as spam by rule-based filter.
Email detected as spam by Bayesian filter.


Key Features
1.	Rule-Based Filtering:
•	Simple and fast.
•	Effective for detecting common spam patterns.
2.	Bayesian Filtering:
•	Probabilistic approach that adapts to new spam patterns.
•	Can be trained with real-world data to improve accuracy.
3.	Extensibility:
•	Additional filters (e.g., blacklists, whitelists, or machine learning models) can be integrated.
---
Complexity
•	Rule-Based Filtering:
•	Time Complexity: O(n * m), where n is the number of keywords and m is the length of the email content.
•	Space Complexity: O(n) for storing keywords.
•	Bayesian Filtering:
•	Time Complexity: O(w), where w is the number of words in the email content.
•	Space Complexity: O(k), where k is the number of unique words in the probability map.

*/

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

// Predefined spam keywords for rule-based filtering
const vector<string> spamKeywords = {"win", "free", "offer", "money", "urgent", "lottery"};

// Function to check if an email contains spam keywords
bool ruleBasedFilter(const string& emailContent) {
    for (const string& keyword : spamKeywords) {
        if (emailContent.find(keyword) != string::npos) {
            return true; // Spam keyword found
        }
    }
    return false;
}

// Function to tokenize a string into words
vector<string> tokenize(const string& text) {
    vector<string> tokens;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        // Remove punctuation and convert to lowercase
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        tokens.push_back(word);
    }
    return tokens;
}

// Bayesian filtering: Calculate spam probability
bool bayesianFilter(const string& emailContent, const unordered_map<string, double>& spamWordProbabilities) {
    vector<string> words = tokenize(emailContent);
    double spamScore = 1.0, nonSpamScore = 1.0;

    for (const string& word : words) {
        if (spamWordProbabilities.find(word) != spamWordProbabilities.end()) {
            double prob = spamWordProbabilities.at(word);
            spamScore *= prob;
            nonSpamScore *= (1.0 - prob);
        }
    }

    // Normalize scores
    double totalScore = spamScore + nonSpamScore;
    spamScore /= totalScore;
    nonSpamScore /= totalScore;

    return spamScore > 0.5; // Return true if spam score is greater than 0.5
}

int main() {
    // Example spam word probabilities for Bayesian filtering
    unordered_map<string, double> spamWordProbabilities = {
        {"win", 0.9}, {"free", 0.8}, {"offer", 0.7}, {"money", 0.85},
        {"urgent", 0.75}, {"lottery", 0.95}, {"hello", 0.1}, {"meeting", 0.05}
    };

    // Example email content
    string emailContent = "Congratulations! You have won a free lottery. Claim your money now!";

    // Rule-based filtering
    if (ruleBasedFilter(emailContent)) {
        cout << "Email detected as spam by rule-based filter." << endl;
    } else {
        cout << "Email passed rule-based filter." << endl;
    }

    // Bayesian filtering
    if (bayesianFilter(emailContent, spamWordProbabilities)) {
        cout << "Email detected as spam by Bayesian filter." << endl;
    } else {
        cout << "Email passed Bayesian filter." << endl;
    }

    return 0;
}
