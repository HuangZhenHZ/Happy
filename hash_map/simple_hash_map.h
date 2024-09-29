#include <cstring>
#include <cassert>
#include <cstdio>
#include <vector>
#include <chrono>
#include <string>

class AutoTimer {
  const std::chrono::time_point<std::chrono::steady_clock> tp_;
  std::string info_;
public:
  AutoTimer(const std::string& str) : tp_(std::chrono::steady_clock::now()), info_(std::move(str)) {}
  ~AutoTimer() {
    auto tp2 = std::chrono::steady_clock::now();
    printf("%s: time = %lld\n", info_.c_str(), std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp_).count());
  }
};

struct old_hash_map {
  typedef unsigned long long uLL;
  static constexpr int H = (1 << 20) - 1;
	int la[H + 1], top;
	struct E { uLL key; int da,ne; } e[int(1e6+5)];
  old_hash_map() { clear(); }
	void clear(){
		for (int i = 0; i <= H; ++i) {
			la[i] = -1;
		}
		top=0;
	}
	bool count(uLL k) {
		// int i=la[k&H];
		int i = la[(k * 0x9ddfea08eb382d69) >> 44];
		while (i >= 0 && e[i].key!=k) i=e[i].ne;
		return i >= 0;
	}
	int& operator[] (uLL k) {
		static int h,i;
		// i=la[h=k&H];
		i = la[h = (k * 0x9ddfea08eb382d69) >> 44];
		while (i >= 0 && e[i].key!=k) i=e[i].ne;
		if (i < 0) { e[i=top++]=(E){k,0,la[h]}; la[h]=top-1; }
		assert(top < 1e6 + 5);
		return e[i].da;
	}
};

class VectorHashMap {
	using uLL = unsigned long long;
	std::vector<int> la_;
	struct E {
		std::pair<uLL, int> kv;
		int ne;
	};
	std::vector<E> e_;
	int num_hash_bits_;
	int num_shift_bits_;
	static constexpr int kMinNumHashBits = 20;

	void Rehash() {
		// AutoTimer timer_("Rehash");
		// assert(false);
		num_hash_bits_++;
		num_shift_bits_--;
		{
			// AutoTimer timer_("assign");
			la_.assign(1 << num_hash_bits_, -1);
		}
		{
			// AutoTimer timer_("rehash main");
			for (int i = 0; i < (int)e_.size(); ++i) {
				int h = (e_[i].kv.first * 0x9ddfea08eb382d69) >> num_shift_bits_;
				e_[i].ne = la_[h];
				la_[h] = i;
			}
		}
	}

public:
  VectorHashMap() : la_(1 << kMinNumHashBits, -1), num_hash_bits_(kMinNumHashBits), num_shift_bits_(64 - num_hash_bits_) {}
	void clear() {
		la_ = std::vector<int>(1 << kMinNumHashBits, -1);
		e_.clear();
		num_hash_bits_ = kMinNumHashBits;
		num_shift_bits_ = 64 - num_hash_bits_;
	}
	bool count(uLL k) {
		int i = la_[(k * 0x9ddfea08eb382d69) >> num_shift_bits_];
		while (i >= 0 && e_[i].kv.first != k) {
			i = e_[i].ne;
		}
		return i >= 0;
	}
	int& operator[] (uLL k) {
		int h = (k * 0x9ddfea08eb382d69) >> num_shift_bits_;
		int i = la_[h];
		while (i >= 0 && e_[i].kv.first != k) {
			i = e_[i].ne;
		}
		if (i >= 0) {
			return e_[i].kv.second;
		}
		e_.emplace_back(E{.kv{k, 0}, .ne{la_[h]}});
		la_[h] = e_.size() - 1;
		if (__builtin_expect(e_.size() > la_.size() * 0.8, 0)) {
			Rehash();
		}
		return e_.back().kv.second;
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
