#include <iostream>
#include <string>
#include <vector>
#include <memory>

using string = std::string;

class RadixTree {
	using RTPtr = std::unique_ptr<RadixTree>;
public:
	RadixTree() {}
	RadixTree(const string& val, const bool is_val = true) : m_value(val), m_is_elem(is_val) {}

	void Insert(const string &val) {
		if (val.empty())
			return;
		if (m_value.empty() && m_nodes.empty()) {
			m_value = val;
			m_is_elem = true;
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

		std::vector<RTPtr> nodes;
		std::swap(m_nodes, nodes);
		const string tail = m_value.substr(eq_part.size());
		m_nodes.push_back(std::make_unique<RadixTree>(tail, m_is_elem));
		std::swap(m_nodes.front()->m_nodes, nodes);
		m_value = eq_part;
		m_is_elem = eq_part.size() == val.size();
		if (!m_is_elem)
			m_nodes.push_back(std::make_unique<RadixTree>(val.substr(eq_part.size())));
	}

	void PrintNickname(const string &prefix = "") {
		if (m_is_elem)
			std::cout << prefix + m_value
				<< " " + prefix + m_value[0]
				<< std::endl;
		for (auto &node : m_nodes)
			node->PrintNickname(prefix + m_value);
	}

	void Print(const string &prefix = "", const bool is_last = true) {
		if (!m_value.empty()) {
			std::cout << prefix << (is_last ? "└" : "├")
				<< m_value << (m_is_elem ? "$" : "") << std::endl;
			for (auto &node : m_nodes)
				node->Print(prefix + (is_last ? " " : "│")
						, &node == &m_nodes.back());
		} else
			for (auto &node : m_nodes)
				node->Print("", &node == &m_nodes.back());
	}

	string GetRootVal() { return m_value; }

	std::vector<RTPtr> m_nodes;
private:
	string m_value;
	bool m_is_elem = false;

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
		rt.PrintNickname();
		rt.Print();
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
