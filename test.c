int double(int x) {
    int temp;
    temp = x + x;
    return temp;
}

int triple(int x) {
    int temp;
    temp = x + x + x;
    return temp;
}

int main() {
    int x;
    x = 5;
    print(double(x));
    print(triple(x));
    return 0;
}