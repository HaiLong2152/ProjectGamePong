#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int W, H;
vector<string> grid;
vector<vector<bool>> visited;

int dx[] = {-1, 1, 0};
int dy[] = {0, 0, 1};

bool in_bounds(int x, int y) {
    return x >= 0 && x < W && y >= 0 && y < H;
}

string bfs() {
    queue<pair<int, int>> q;

    for (int x = 0; x < W; ++x) {
        if (grid[H - 1][x] == 'Y') {
            q.push({x, H - 1});
            visited[H - 1][x] = true;
        }
    }

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();

        if (y == 0) return "YES";

        for (int dir = 0; dir < 3; ++dir) {
            int nx = x + dx[dir];
            int ny = y - dy[dir];

            if (in_bounds(nx, ny) && !visited[ny][nx] && grid[ny][nx] == 'E') {
                visited[ny][nx] = true;
                q.push({nx, ny});
            }
        }
    }

    return "NO";
}

int main() {
    cin >> W >> H;
    grid.resize(H);
    visited.assign(H, vector<bool>(W, false));

    for (int i = 0; i < H; ++i) {
        cin >> grid[i];
    }

    cout << bfs() << "\n";
}
