#include "utils.h"
#include <vector>
#include <algorithm> // Required for std::min, std::max

// Function to encrypt a string using Caesar cipher
// It shifts characters within the printable ASCII range (32-126)
// The '|' character is ignored during encryption.
std::string cEncrypt(const std::string& str, int shift) {
    std::string result = str;
    for (char& c : result) {
        if (c != '|') { // Ignore pipe character
            if (c >= 32 && c <= 126) { // Check if character is in printable ASCII range
                // Apply Caesar cipher shift, wrapping around the 95 printable characters
                c = 32 + (c - 32 + shift) % 95;
            }
        }
    }
    return result;
}

// Function to decrypt a string using Caesar cipher
// It reverses the encryption process.
// The '|' character is ignored during decryption.
std::string cDecrypt(const std::string& str, int shift) {
    std::string result = str;
    for (char& c : result) {
        if (c != '|') { // Ignore pipe character
            if (c >= 32 && c <= 126) { // Check if character is in printable ASCII range
                // Apply Caesar cipher decryption, wrapping around the 95 printable characters
                c = 32 + (c - 32 - shift + 95) % 95;
            }
        }
    }
    return result;
}

// Function to calculate the Levenshtein distance between two strings
// This distance is the minimum number of single-character edits (insertions, deletions, or substitutions)
// required to change one word into the other.
int levenshteinDistance(const std::string& a, const std::string& b) {
    int m = a.size();
    int n = b.size();
    // Create a DP table to store distances between prefixes
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    // Base cases: distance from an empty string to a prefix
    for (int i = 0; i <= m; ++i) dp[i][0] = i; // Cost of deleting all characters of string a
    for (int j = 0; j <= n; ++j) dp[0][j] = j; // Cost of inserting all characters of string b

    // Fill the DP table
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (a[i - 1] == b[j - 1]) {
                // If characters are the same, no cost for this position
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                // If characters are different, cost is 1 + minimum of:
                // - Deletion (from a)
                // - Insertion (into a)
                // - Substitution
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }
    // The distance is in the bottom-right cell of the DP table
    return dp[m][n];
}

// Function to update the best matching string based on Levenshtein distance
// If the distance of the candidate string to the query is less than the current minDist,
// update bestMatch and minDist.
void updateBestMatch(const std::string& query, const std::string& candidate, std::string& bestMatch, int& minDist) {
    int dist = levenshteinDistance(query, candidate);
    if (dist < minDist) {
        minDist = dist;
        bestMatch = candidate;
    }
}

// Function to add a string to a vector if its similarity to a query string is above a threshold
// Similarity is calculated as 1 - (Levenshtein distance / max length of the two strings).
void addIfAccurateEnough(std::vector<std::string>& vec, const std::string& query, const std::string& current, double threshold) {
    int distance = levenshteinDistance(query, current);
    int maxLen = std::max(query.length(), current.length());

    if (maxLen == 0) return; // Avoid division by zero if both strings are empty

    double accuracy = 1.0 - static_cast<double>(distance) / maxLen;

    if (accuracy >= threshold) {
        vec.push_back(current);
    }
}
