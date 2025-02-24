#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

// Define the log tag
static const char *TAG = "BFS_APP";

// Function to perform BFS
void BFS(int start, unordered_map<int, vector<int>>& adj) {
    // Create a queue for BFS
    queue<int> q;
    unordered_map<int, bool> visited;

    // Start BFS from the 'start' node
    visited[start] = true;
    q.push(start);

    // Perform BFS traversal
    while (!q.empty()) {
        int node = q.front();
        q.pop();

        // Print the node (using ESP_LOG for logging)
        printf("Visited node: %d\n", node);

        // Visit all neighbors of the current node
        for (int neighbor : adj[node]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
}

extern "C" void app_main() {
    // Initialize logging
    esp_log_level_set("*", ESP_LOG_INFO);

    // Create an adjacency list for the graph
    unordered_map<int, vector<int>> adj;

    // Example graph (undirected)
    adj[1] = {2, 3, 7};
    adj[2] = {1, 3, 4, 5};
    adj[3] = {1, 2, 6, 7};
    adj[4] = {2, 5};
    adj[5] = {2, 4, 6};
    adj[6] = {3, 5, 7};
    adj[7] = {1, 3, 6};
    adj[8] = {4, 5};
    adj[9] = {5, 6};
    adj[10] = {3, 7};

    // Run BFS starting from node 1
    ESP_LOGI(TAG, "BFS traversal starting from node 1:");
    BFS(1, adj);
}
