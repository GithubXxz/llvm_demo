int m;
const int n = 10;
int main () {
	int i = 0;
	m = 1;
	while (i <= n - 1) {
		i = i + 1;
		m + 1;
		m = m + m;
	}
	putint(m);
	return m;
}
