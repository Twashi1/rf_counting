int addmul(int a, int b) {
    int s = a + b;
    int p = a * b;
    if (s > p)
        return s - p;
    else
        return p - s;
}

int main() {
    int sum = 0;
    for (int i = 0; i < 10; ++i)
        sum += addmul(i, i + 1);
    return sum;
}
