#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <algorithm> // Required for std::min, std::max

// Define the shift for Caesar cipher
#define SHIFT 32

// Function to encrypt a string using Caesar cipher
// Input: const string& str - the string to encrypt
//        int shift - the amount to shift characters by
// Output: string - the encrypted string
std::string cEncrypt(const std::string& str, int shift);

// Function to decrypt a string using Caesar cipher
// Input: const string& str - the string to decrypt
//        int shift - the amount characters were shifted by
// Output: string - the decrypted string
std::string cDecrypt(const std::string& str, int shift);

// Function to calculate the Levenshtein distance between two strings
// Input: const string& a - the first string
//        const string& b - the second string
// Output: int - the Levenshtein distance
int levenshteinDistance(const std::string& a, const std::string& b);

// Function to update the best matching string based on Levenshtein distance
// Input: const string& query - the string to match against
//        const string& candidate - the candidate string
//        string& bestMatch - reference to the current best match (will be updated)
//        int& minDist - reference to the current minimum distance (will be updated)
void updateBestMatch(const std::string& query, const std::string& candidate, std::string& bestMatch, int& minDist);

// Function to add a string to a vector if its Levenshtein distance accuracy to a query string is above a threshold
// Input: vector<string>& vec - the vector to add the string to
//        const string& query - the query string
//        const string& current - the string to check and potentially add
//        double threshold - the minimum accuracy threshold (default 0.75)
void addIfAccurateEnough(std::vector<std::string>& vec, const std::string& query, const std::string& current, double threshold = 0.75);

#endif // UTILS_H
