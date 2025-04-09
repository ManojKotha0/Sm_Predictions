#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <chrono>
#include <cassert>

class SocialNetwork {
private:
    // Adjacency list representation of the social graph
    std::unordered_map<int, std::unordered_set<int>> graph;

public:
    
    void addUser(int userId) {
        if (graph.find(userId) == graph.end()) {
            graph[userId] = std::unordered_set<int>();
        }
    }

    // Add a connection between two users
    void addConnection(int userId1, int userId2) {
        // Ensure both users exist
        addUser(userId1);
        addUser(userId2);

        // Add bidirectional connection
        graph[userId1].insert(userId2);
        graph[userId2].insert(userId1);
    }

    // Remove connection
    void removeConnection(int userId1, int userId2) {
        if (graph.find(userId1) != graph.end() && 
            graph.find(userId2) != graph.end()) {
            graph[userId1].erase(userId2);
            graph[userId2].erase(userId1);
        }
    }

    // Get direct friends of a user
    std::unordered_set<int> getFriends(int userId) const {
        auto it = graph.find(userId);
        if (it != graph.end()) {
            return it->second;
        }
        return {};
    }

    // Method 1: Recommend friends based on common friends
    std::vector<std::pair<int, int>> recommendByCommonFriends(int userId) const {
        // Map to store potential friends and their common friend count
        std::unordered_map<int, int> potentialFriends;

        // Get user's existing friends
        auto userFriends = getFriends(userId);

        // Find friends of friends
        for (int currentFriend : userFriends) {
            for (int friendOfFriend : getFriends(currentFriend)) {
                // Skip if already a friend or the user itself
                if (friendOfFriend == userId || userFriends.count(friendOfFriend)) {
                    continue;
                }

                // Increment common friends count
                potentialFriends[friendOfFriend]++;
            }
        }

        // Convert to vector for sorting
        std::vector<std::pair<int, int>> recommendations;
        for (const auto& pair : potentialFriends) {
            recommendations.push_back(pair);
        }

        // Sort by number of common friends in descending order
        std::sort(recommendations.begin(), recommendations.end(), 
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            });

        return recommendations;
    }

    // Method 2: Recommend friends based on network distance
    std::vector<std::pair<int, int>> recommendByNetworkDistance(
        int userId, int maxDistance) const {
        
        std::unordered_map<int, int> distances;
        std::unordered_set<int> visited;
        std::queue<std::pair<int, int>> queue;

        // Start BFS from the user
        queue.push({userId, 0});
        visited.insert(userId);

        while (!queue.empty()) {
            int currentUser = queue.front().first;
            int currentDistance = queue.front().second;
            queue.pop();

            // Stop if we've exceeded max distance
            if (currentDistance > maxDistance) {
                break;
            }

            // Check friends of current user
            for (int neighbor : getFriends(currentUser)) {
                if (visited.count(neighbor) == 0) {
                    visited.insert(neighbor);
                    queue.push({neighbor, currentDistance + 1});

                    // If not direct friend, consider for recommendation
                    if (neighbor != userId && graph.at(userId).count(neighbor) == 0) {
                        distances[neighbor] = currentDistance + 1;
                    }
                }
            }
        }

        // Convert to vector for sorting
        std::vector<std::pair<int, int>> recommendations;
        for (const auto& pair : distances) {
            recommendations.push_back(pair);
        }

        // Sort by network distance
        std::sort(recommendations.begin(), recommendations.end(), 
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            });

        return recommendations;
    }

    // Advanced recommendation with weighted scoring
    std::vector<std::pair<int, int>> advancedRecommendation(int userId,int maxDistance) const {
        std::unordered_map<int, double> recommendationScores;

        // Get user's friends
        auto userFriends = getFriends(userId);

        // Compute recommendations
        for (int currentFriend : userFriends) {
            for (int friendOfFriend : getFriends(currentFriend)) {
                // Skip if already a friend or the user itself
                if (friendOfFriend == userId || userFriends.count(friendOfFriend)) {
                    continue;
                }

                // Compute weighted score
                // 1. Common friends factor
                int commonFriends = 0;
                for (int commonFriend : userFriends) {
                    if (getFriends(friendOfFriend).count(commonFriend)) {
                        commonFriends++;
                    }
                }

                // 2. Network proximity factor
                int networkDistance = getNetworkDistance(userId, friendOfFriend);

                // Combine factors
                double score = (commonFriends * 2) + (1.0 / (networkDistance + 1));
                recommendationScores[friendOfFriend] += score;
            }
        }

        // Convert to vector for sorting
        std::vector<std::pair<int, int>> recommendations;
        for (const auto& pair : recommendationScores) {
            recommendations.push_back({pair.first, static_cast<int>(pair.second)});
        }

        // Sort by score in descending order
        std::sort(recommendations.begin(), recommendations.end(), 
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            });

        return recommendations;
    }

    // Helper method to get network distance between two users
    int getNetworkDistance(int userId1, int userId2) const {
        std::unordered_set<int> visited;
        std::queue<std::pair<int, int>> queue;

        queue.push({userId1, 0});
        visited.insert(userId1);

        while (!queue.empty()) {
            int currentUser = queue.front().first;
            int distance = queue.front().second;
            queue.pop();

            if (currentUser == userId2) {
                return distance;
            }

            for (int neighbor : getFriends(currentUser)) {
                if (visited.count(neighbor) == 0) {
                    visited.insert(neighbor);
                    queue.push({neighbor, distance + 1});
                }
            }
        }

        // No path found
        return std::numeric_limits<int>::max();
    }

    // Get total number of users in the network
    size_t getTotalUsers() const {
        return graph.size();
    }

    // Print entire network structure (for debugging)
    void printNetwork() const {
        for (const auto& entry : graph) {
            int userId = entry.first;
            const std::unordered_set<int>& friendSet = entry.second;
            
            std::cout << "User " << userId << " is connected to: ";
            for (int friendId : friendSet) {
                std::cout << friendId << " ";
            }
            std::cout << std::endl;
        }
    }
};

// Demonstration function
void demonstrateSocialNetwork() {
    SocialNetwork socialNetwork;

    // Add users
    // socialNetwork.addUser(1);
    // socialNetwork.addUser(2);
    // socialNetwork.addUser(3);
    // socialNetwork.addUser(4);
    // socialNetwork.addUser(5);
    // socialNetwork.addUser(6);
    int users;
    int maxDistance;
    std::cin >> users;
    std::cin >> maxDistance; 
    for(int i=0;i<users;i++){
      socialNetwork.addUser(i);
    }

    // Add connections
    // socialNetwork.addConnection(1, 2);
    // socialNetwork.addConnection(1, 3);
    // socialNetwork.addConnection(2, 4);
    // socialNetwork.addConnection(3, 4);
    // socialNetwork.addConnection(3, 5);
    // socialNetwork.addConnection(4, 5);
    // socialNetwork.addConnection(4, 6);
    int connections;
    std::cin >> connections;
    for(int i=0;i<connections;i++){
      int a,b;
      std::cin >> a >> b;
      socialNetwork.addConnection(a, b);
    }

    // Print entire network
    std::cout << "Social Network Structure:" << std::endl;
    socialNetwork.printNetwork();

    for(int i=1;i<=users;i++){
      // Demonstrate friend recommendations
    std::cout << "\nFriend Recommendations for " << i << std::endl;
    
    std::cout << "By Common Friends:" << std::endl;
    auto commonFriendRecommendations = socialNetwork.recommendByCommonFriends(i);
    for (const auto& recommendation : commonFriendRecommendations) {
        std::cout << "User " << recommendation.first 
                  << " (Common Friends: " << recommendation.second << ")" << std::endl;
    }

    std::cout << "\nBy Network Distance:" << std::endl;
    auto networkDistanceRecommendations = socialNetwork.recommendByNetworkDistance(i,maxDistance);
    for (const auto& recommendation : networkDistanceRecommendations) {
        std::cout << "User " << recommendation.first 
                  << " (Distance: " << recommendation.second << ")" << std::endl;
    }

    std::cout << "\nAdvanced Recommendation:" << std::endl;
    auto advancedRecommendations = socialNetwork.advancedRecommendation(i,maxDistance);
    for (const auto& recommendation : advancedRecommendations) {
        std::cout << "User " << recommendation.first 
                  << " (Score: " << recommendation.second << ")" << std::endl;
    }
    }

    
}

int main() {
    try {
        demonstrateSocialNetwork();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}