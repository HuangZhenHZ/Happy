#include <cstring>
#include <cassert>

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
