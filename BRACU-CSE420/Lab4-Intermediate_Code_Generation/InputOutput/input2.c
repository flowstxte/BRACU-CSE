int func(int a, int b) {
    return a + b;
}

int func2(float a, int b) {
    return a - b;
}

int main () {
    int a, b, c, i;
    int e, f[10], g[11];
    a = 1;
    b = 2;
    c = func(a, b);

    float d;

    for (i = 0; i < 10; i++) {
        int a;
        a = c + i;
        a = c;
    }

    if (c > 10) {
        float b;
        b = func2(1.0, 2);
        d = b;
    }

    while (c > 0) {
        int i;
        c--;
        i = c;
    }
    return c;
}