#ifndef USER_CODE_H
#define USER_CODE_H
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include "fileIterator.h"
#include "fileWriter.h"
#include <unordered_map>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
using namespace std::chrono;

//////////////////////////////////////////////////////////////////////////////////
// MODIFY THIS SECTION
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Modify this code to solve the assignment. 
 * 
 * #PROMPT# Process hashtags, purchases, and prices to group customers based on their top-k hashtags.
 * 
 * @param hashtags 
 * @param purchases 
 * @param prices 
 */

unordered_map<int, set<string>> productHashtags;
bool exceuted = false;
unordered_map<int, unordered_map<string, int>> customerHashtagCount;
bool exceuted_price = false;
unordered_map<int, vector<int>> pur;

// function to convert string to lower case
std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return lowerStr;
}

void groupCustomersByHashtags(fileIterator& hashtags, fileIterator& purchases, fileIterator& prices, int k, const string& outputFilePath) {
    auto start = high_resolution_clock::now();

    // Map to store product_id to its hashtags
    unordered_map<int, set<string>> productHashtagsdummy;
    
    // Read hashtags file
    if (!exceuted) {
        while (hashtags.hasNext()) {
            string line = hashtags.next();
            if (line.empty()) {
                continue;
            }
            stringstream ss(line);
            string product_id_str, hashtag;
            getline(ss, product_id_str, ',');
            int product_ID = stoi(product_id_str);
            set<string> hashtagsList;
            
            if (productHashtags[product_ID].empty()) {
                while (getline(ss, hashtag, ',')) {
                    hashtagsList.insert(toLower(hashtag));
                }
                productHashtags[product_ID] = hashtagsList;
            }
        }
        exceuted = true;
    }
    productHashtagsdummy = productHashtags;

    // Read purchases file and update customerHashtagCount
    if (!exceuted_price) {
        while (purchases.hasNext()) {
            string line = purchases.next();
            if (line.empty()) {
                continue;
            }
            stringstream ss(line);
            string customer_id_str, product_id_str;
            getline(ss, customer_id_str, ',');
            getline(ss, product_id_str);

            int customer_ID = stoi(customer_id_str);
            int product_ID = stoi(product_id_str);
            pur[product_ID].push_back(customer_ID);

            auto it = productHashtagsdummy.find(product_ID);
            if (it != productHashtagsdummy.end()) {
                const set<string>& hashtagsList = it->second;
                for (const string& hashtag : hashtagsList) {
                    customerHashtagCount[customer_ID][hashtag]++;
                }
            }
        }
        exceuted_price = true;
    }

    // Group customers by top-k hashtags
    unordered_map<int, vector<string>> customerTopKHashtags;
    for (auto& entry : customerHashtagCount) {
        int customerID = entry.first;
        auto& hashtagFreqMap = entry.second;

        vector<pair<string, int>> freqList(hashtagFreqMap.begin(), hashtagFreqMap.end());
        sort(freqList.begin(), freqList.end(), [](const auto& a, const auto& b) {
            if (a.second == b.second) {
                return a.first < b.first;
            }
            return a.second > b.second;
        });

        vector<string> topKHashtags;
        for (int i = 0; i < k && i < freqList.size(); i++) {
            topKHashtags.push_back(freqList[i].first);
        }
        sort(topKHashtags.begin(), topKHashtags.end());

        customerTopKHashtags[customerID] = topKHashtags;
    }

    // Group customers by their top-k hashtags
    unordered_map<string, vector<int>> groups;
    for (auto& entry : customerTopKHashtags) {
        int customerID = entry.first;
        vector<string> topKHashtags = entry.second;

        string key = "";
        for (const string& hashtag : topKHashtags) {
            key += hashtag + ",";
        }
        if (!key.empty()) key.pop_back();

        groups[key].push_back(customerID);
    }

    // Write grouped customers to the output file
    for (const auto& groupEntry : groups) {
        const vector<int>& group = groupEntry.second;
        writeOutputToFile(group, outputFilePath);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Time logging can be added here
}

//////////////////////////////////////////////////////////////////////////////////
// MODIFY THIS SECTION
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Modify this code to calculate the average expense for a group of customers.
 * 
 * #PROMPT# Calculate the group average expense based on their purchases and the price list.
 * 
 * @param customerList 
 * @param purchases 
 * @param prices 
 */

float calculateGroupAverageExpense(vector<int> customerList, fileIterator& purchases, fileIterator& prices) {
    auto start = high_resolution_clock::now();

    // Map product_id to price
    unordered_map<int, float> priceMap;
    while (prices.hasNext()) {
        string line = prices.next();
        stringstream ss(line);
        string product_id_str, price_str;
        getline(ss, product_id_str, ',');
        getline(ss, price_str);

        int product_id = stoi(product_id_str);
        float price = stof(price_str);
        priceMap[product_id] = price;
    }

    // Map customer_id to total expense
    unordered_map<int, float> customerTotalExpenses;
    while (purchases.hasNext()) {
        string line = purchases.next();
        stringstream ss(line);
        string customer_id_str, product_id_str;
        getline(ss, customer_id_str, ',');
        getline(ss, product_id_str);

        int customer_ID = stoi(customer_id_str);
        int product_ID = stoi(product_id_str);

        if (priceMap.find(product_ID) != priceMap.end()) {
            customerTotalExpenses[customer_ID] += priceMap[product_ID];
        }
    }

    // Calculate average expense for customer list
    float totalExpense = 0.0;
    int customerCount = 0;

    for (int customerID : customerList) {
        if (customerTotalExpenses.find(customerID) != customerTotalExpenses.end()) {
            totalExpense += customerTotalExpenses[customerID];
            customerCount++;
        }
    }

    float averageExpense = (customerCount > 0) ? (totalExpense / customerCount) : 0.0;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Time logging can be added here

    return averageExpense;
}

//////////////////////////////////////////////////////////////////////////////////
// MODIFY THIS SECTION
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Modify this code to dynamically update and group customers by hashtags.
 * 
 * #PROMPT# Dynamically update hashtags for products and re-group customers by their top-k hashtags.
 * 
 * @param hashtags 
 * @param purchases 
 * @param prices 
 * @param newHashtags 
 * @param k 
 * @param outputFilePath 
 */

void groupCustomersByHashtagsForDynamicInserts(fileIterator& hashtags, fileIterator& purchases, fileIterator& prices, vector<string> newHashtags, int k, string outputFilePath) {
    auto start = high_resolution_clock::now();

    unordered_map<int, set<string>> productHashtagsdummy = productHashtags;
    unordered_map<int, unordered_map<string, int>> customerHashtagCount_dummy = customerHashtagCount;

    // Process dynamic hashtag updates
    for (const string& newHashtagEntry : newHashtags) {
        stringstream ss(newHashtagEntry);
        string product_id_str;
        getline(ss, product_id_str, ',');
        int product_id = stoi(product_id_str);
        string newHashtag;

        while (getline(ss, newHashtag, ',')) {
            if (productHashtagsdummy[product_id].find(newHashtag) == productHashtagsdummy[product_id].end()) {
                vector<int> customers = pur[product_id];
                for (int customer_id : customers) {
                    customerHashtagCount_dummy[customer_id][newHashtag]++;
                }
                productHashtagsdummy[product_id].insert(newHashtag);
            }
        }
    }

    // Rebuild customer top-k hashtags
    unordered_map<int, vector<string>> customerTopKHashtags;
    for (auto& entry : customerHashtagCount_dummy) {
        int customerID = entry.first;
        auto& hashtagFreqMap = entry.second;

        vector<pair<string, int>> freqList(hashtagFreqMap.begin(), hashtagFreqMap.end());
        sort(freqList.begin(), freqList.end(), [](const auto& a, const auto& b) {
            if (a.second == b.second) {
                return toLower(a.first) < toLower(b.first);
            }
            return a.second > b.second;
        });

        vector<string> topKHashtags;
        for (int i = 0; i < k && i < freqList.size(); i++) {
            topKHashtags.push_back(freqList[i].first);
        }
        sort(topKHashtags.begin(), topKHashtags.end());

        customerTopKHashtags[customerID] = topKHashtags;
    }

    // Re-group customers by top-k hashtags
    unordered_map<string, vector<int>> groups;
    for (auto& entry : customerTopKHashtags) {
        int customerID = entry.first;
        vector<string> topKHashtags = entry.second;

        string key = "";
        for (const string& hashtag : topKHashtags) {
            key += hashtag + ",";
        }
        if (!key.empty()) key.pop_back();

        groups[key].push_back(customerID);
    }

    // Write grouped customers to the output file
    for (const auto& groupEntry : groups) {
        const vector<int>& group = groupEntry.second;
        writeOutputToFile(group, outputFilePath);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Time logging can be added here
}
#endif