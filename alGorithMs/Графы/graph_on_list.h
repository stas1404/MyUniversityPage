#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
template <typename Ver, typename Weight = int>
struct MyEdge {
  Ver from;
  Ver to;
  Weight weight;
  MyEdge(Ver verfrom, Ver verto, Weight edgweight) {
    from = verfrom;
    to = verto;
    weight = edgweight;
  }
  bool operator<(const MyEdge<Ver>& next) const {
    return next.weight > this->weight;
  }
};
template <typename Ver = int, typename Edge = MyEdge<Ver>>
class Graph {
 public:
  using EdgeType = Edge;
  using Iterator = typename std::vector<Ver>::iterator;
  using VertexType = Ver;
  virtual size_t GetAmountOfVertex() { return 0; }
  virtual size_t GetAmountOfedges() { return 0; }
  virtual std::vector<Ver> GetNeighbours(Ver ver) = 0;
  virtual size_t GetWeightOfEdge(Ver fir, Ver sec) = 0;
  virtual std::set<Ver> GetVertex() = 0;
  virtual Iterator GetIterOfNeighbours(Ver ver) = 0;
};
template <typename Ver = int, typename Edge = MyEdge<Ver>>
class GraphOnList : public Graph<Ver, Edge> {
 public:
  using EdgeType = Edge;
  using Iterator = typename std::vector<Ver>::iterator;
  using VertexType = Ver;
  size_t GetAmountOfVertex() { return am_ver_; }
  size_t GetAmountOfedges() { return am_edg_; }
  size_t GetWeightOfEdge(Ver fir, Ver sec) { return edges_[{fir, sec}]; }
  std::set<Ver> GetVertex() { return multipleofvers_; }
  std::vector<Ver> GetNeighbours(Ver ver) { return vers_[GetNumb(ver)]; }
  Iterator GetIterOfNeighbours(Ver ver) { return vers_[GetNumb(ver)].begin(); }
  GraphOnList() {}
  GraphOnList(size_t number)
      : am_ver_(number), vers_(new std::vector<Ver>[number]) {}
  void Append(Edge edge) {
    size_t fir = GetNumb(edge.from);
    size_t sec = GetNumb(edge.to);
    multipleofvers_.insert(edge.from);
    multipleofvers_.insert(edge.to);
    if (fir == sec) {
      return;
    }
    vers_[fir].push_back(edge.to);
    vers_[sec].push_back(edge.from);
    ++am_edg_;
    MarkEdge(edge);
  }
  ~GraphOnList() { delete[] vers_; }

 private:
  std::unordered_map<Ver, int> vernumber_;
  std::set<Ver> multipleofvers_;
  size_t am_ver_ = 0;
  size_t am_edg_ = 0;
  int last_ = 0;
  std::map<std::pair<Ver, Ver>, int> edges_;
  std::vector<Ver>* vers_;
  int GetNumb(Ver ver) {
    auto iter = vernumber_.find(ver);
    if (iter == vernumber_.end()) {
      vernumber_[ver] = last_;
      ++last_;
    }
    return vernumber_[ver];
  }
  void MarkEdge(Edge edge) {
    auto atpos = edges_.find({edge.from, edge.to});
    if (atpos == edges_.end()) {
      edges_[{edge.from, edge.to}] = edge.weight;
      edges_[{edge.to, edge.from}] = edge.weight;
      return;
    }
    edges_[{edge.from, edge.to}] =
        std::min(edges_[{edge.from, edge.to}], edge.weight);
    edges_[{edge.to, edge.from}] =
        std::min(edges_[{edge.from, edge.to}], edge.weight);
  }
};
