#include <iostream>
#include <string>
#include <vector>
#include <memory>

using string = std::string:

class RadixTree {
	using RTPtr = std::unique_ptr<RadixTree>;
public:
	RadixTree() {}
	RadixTree(const string& val) : m_value(val) {}

	void Insert(const string &val) {
		if (val.empty())
			return;
		if (m_value.empty() && m_nodes.empty()) {
			m_value = val;
			return;
		}
		const string eq_part = GetEq(val);
		if (eq_part.size() == m_value.size()) {
			const string tail = val.substr(eq_part.size());
			for (auto &ptr : m_nodes) {
				if (ptr->GetRootVal()[0] == tail[0]) {
					ptr->Insert(tail);
					return;
				}
			}
			m_nodes.push_back(std::make_unique<RadixTree>(tail));
			return;
		}
		const string tail = m_value.substr(eq_part.size());
		m_value = eq_part;
		auto node = std::make_unique<RadixTree>(tail);
		std::swap(m_nodes, node->m_nodes);
		m_nodes.push_back(node);
		if (eq_part.size() < val.size())
			m_nodes.push_back(std::make_unique<RadixTree>(val.substr(eq_part.size())));
	}

	string GetRootVal() { return m_value; }

	std::vector<RTPtr> m_nodes;
private:
	string m_value;

	string GetEq(const string &val) {
		const int size = std::min(m_value.size(), val.size());
		string result;
		for (int i = 0; i < size; ++i)
			if (m_value[i] == val[i])
				result += m_value[i];
		return result;
	}
};

int main() {
	try {
		RadixTree rt;
		for (string line; std::getline(std::cin, line);) {
			rt.Insert(line);
		}
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
