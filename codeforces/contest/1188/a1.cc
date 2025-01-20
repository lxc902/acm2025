#include <iostream>
#include <vector>

using namespace std;

int main() {
  int n;
  cin >> n;

  vector<vector<int> > adj(n + 1);

  for (int i = 0; i < n - 1; ++i) {
    int u, v;
    cin >> u >> v;
    adj[u].push_back(v);
    adj[v].push_back(u);
  }

  bool ok = true;
  for (int i = 1; i <= n; ++i) {
    if (adj[i].size() == 2) {
      ok = false;
      break;
    }
  }

  if (ok) {
    cout << "YES" << endl;
  } else {
    cout << "NO" << endl;
  }

  return 0;
}
