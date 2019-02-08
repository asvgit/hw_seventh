#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace utf8 {
class String {
	using string = std::string;
public:
	String() : m_str() {}
	String(const char *str) : m_str(str) {}
	String(const string &str) : m_str(str) {}
	String(string &&str) : m_str(std::move(str)) {}
	class iterator : public std::iterator<std::bidirectional_iterator_tag, int, int, char *, int> {
		private:
			string::const_iterator it;
		public:
			template <typename Type>
			inline iterator(const Type & i): it(i) {}
			template <typename Type>
			inline iterator & operator = (Type & i) { it = i; return *this; }
			template <typename Type>
			inline bool operator == (Type i) const { return i == it; }
			template <typename Type>
			inline bool operator != (Type i) const { return i != it; }
			iterator& operator ++() {
				auto ch = (unsigned char)*it;
				if (ch >= 0x80) {
					while (ch & 0x80) {
						ch <<= 1;
						++it;
					}
				} else
					++it;
				return *this;
			}
			string operator *() const {
				int mask = 0x80, ch = *it, i;
				for (i = 0; ch & mask; ++i)
					mask>>=1;
				return string(&(*it), i ? i : 1);
			}
	};
	iterator begin() const { return iterator(m_str.begin()); }
	iterator end() const { return iterator(m_str.end()); }

	bool empty() const { return m_str.empty(); }
	size_t size() const { return std::distance(begin(), end()); }
	string operator [](const size_t i) const {
		auto ind(i);
		if (m_str.empty())
			throw std::out_of_range("utf8 oper[] empty");
		auto e = begin();
		while (ind) {
			++e;
			if (e == m_str.end())
				throw std::out_of_range("utf8 oper[]");
			--ind;
		}
		return *e;
	}
	string operator +=(const String &str) {
		m_str += str.m_str;
		return m_str;
	}
	string operator +(const String &str) const {
		return m_str + str.m_str;
	}
	operator string() const {
		return m_str;
	}
	string front() const {
		if (m_str.empty())
			throw std::out_of_range("utf8 front");
		return *begin();
	}
	string substr(const size_t i) const {
		string res;
		auto ind(0);
		if (m_str.empty())
			return "";
		auto e = begin();
		// res += *begin();
		while (ind < size()) {
			if (e == m_str.end())
				throw std::out_of_range("utf8 substr");
			if (ind++ < i) {
				++e;
				continue;
			}
			res += *e;
			++e;
		}
		return res;
	}
private:
	string m_str;
};
}

using string = utf8::String;

std::ostream& operator<<(std::ostream& os, const string& obj) {
	os << obj.substr(0);
	return os;
}

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
				if (ptr->GetRootVal().front() == tail.front()) {
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
				<< " " + (std::string)prefix + m_value.front()
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
		for (std::string line; std::getline(std::cin, line);) {
			rt.Insert(line);
		}
		rt.PrintNickname();
		rt.Print();
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
