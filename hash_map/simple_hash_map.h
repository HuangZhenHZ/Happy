#include <cstring>
#include <cassert>
#include <cstdio>

struct old_hash_map {
  typedef unsigned long long uLL;
  static constexpr int H = (1 << 20) - 1;
	int la[H + 1], top;
	struct E { uLL key; int da,ne; } e[int(1e6+5)];
  old_hash_map() { clear(); }
	void clear(){
		memset(la,0,sizeof(la)); top=0;
	}
	bool count(uLL k) {
		// int i=la[k&H];
		int i = la[(k * 0x9ddfea08eb382d69) >> 44];
		while (i&&e[i].key!=k) i=e[i].ne;
		return i;
	}
	int& operator[] (uLL k) {
		static int h,i;
		// i=la[h=k&H];
		i = la[h = (k * 0x9ddfea08eb382d69) >> 44];
		while (i&&e[i].key!=k) i=e[i].ne;
		if (!i) { e[i=++top]=(E){k,0,la[h]}; la[h]=top; }
		assert(top < 1e6 + 5);
		return e[i].da;
	}
};

class new_hash_map {
	using uLL = unsigned long long;
	static constexpr int H = (1 << 21) - 1;
	struct Data {
		uLL key;
		int value;
		int length;
	} data_[H + 1];

public:
  new_hash_map() { clear(); }
  void clear() {
		memset(data_, 0, sizeof(data_));
	}
  bool count(uLL k) {
		int begin = (k * 0x9ddfea08eb382d69) >> 43;
		int length = data_[begin].length;
		for (int i = 0; i < length; ++i) {
			if (data_[(begin + i) & H].key == k) {
				return true;
			}
		}
		return false;
	}
	int& operator[] (uLL k) {
		int begin = (k * 0x9ddfea08eb382d69) >> 43;
		int length = data_[begin].length;
		int t = 0;
		for (; t < length; ++t) {
			if (data_[(begin + t) & H].key == k) {
				return data_[(begin + t) & H].value;
			}
		}
		while (data_[(begin + t) & H].length) {
			t++;
		}
		int pos = (begin + t) & H;
		data_[pos].key = k;
		data_[pos].length = -1;
		data_[begin].length = t + 1;
		return data_[pos].value = 0;
	}
};
