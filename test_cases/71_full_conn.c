
int relu_reg(int a)
{
    if (a > 0x7F) return 0x7F;
    if (a < 0) return 0;
    return a;
}

int model(int a[][5])
{
    if (+ relu_reg( + a[0][0] *   85 + a[0][1] *   23 + a[0][2] *  -82 + a[0][3] * -103 + a[0][4] * -123 + a[1][0] *   64 + a[1][1] * -120 + a[1][2] *   50 + a[1][3] *  -59 + a[1][4] *   47 + a[2][0] * -111 + a[2][1] *  -67 + a[2][2] * -106 + a[2][3] *  -75 + a[2][4] * -102 + a[3][0] *   34 + a[3][1] *  -39 + a[3][2] *   65 + a[3][3] *   47 + a[3][4] *  113 + a[4][0] *  110 + a[4][1] *   47 + a[4][2] *   -4 + a[4][3] *   80 + a[4][4] *   46) *   39
        + relu_reg( + a[0][0] * -106 + a[0][1] *  126 + a[0][2] *  -18 + a[0][3] *  -31 + a[0][4] *   -8 + a[1][0] *   47 + a[1][1] *   -4 + a[1][2] *   67 + a[1][3] *  -94 + a[1][4] * -121 + a[2][0] *    7 + a[2][1] *  -21 + a[2][2] *  -60 + a[2][3] *  -43 + a[2][4] *  105 + a[3][0] *  -42 + a[3][1] *   87 + a[3][2] *   29 + a[3][3] * -106 + a[3][4] *  -31 + a[4][0] * -110 + a[4][1] * -100 + a[4][2] *  -22 + a[4][3] *  -75 + a[4][4] * -125) *   77
        + relu_reg( + a[0][0] *   26 + a[0][1] *   76 + a[0][2] *  -70 + a[0][3] *   29 + a[0][4] *  -95 + a[1][0] *   96 + a[1][1] *   52 + a[1][2] *  -68 + a[1][3] *   -5 + a[1][4] *   34 + a[2][0] *  -34 + a[2][1] *  102 + a[2][2] *    6 + a[2][3] *  -38 + a[2][4] *   27 + a[3][0] *  110 + a[3][1] *  116 + a[3][2] *   39 + a[3][3] *  -63 + a[3][4] *  -99 + a[4][0] *   65 + a[4][1] *  120 + a[4][2] *  -39 + a[4][3] *   -6 + a[4][4] *   94) *  127
        + relu_reg( + a[0][0] *  -23 + a[0][1] *  -63 + a[0][2] *   49 + a[0][3] *   50 + a[0][4] *   72 + a[1][0] *   85 + a[1][1] *  -30 + a[1][2] *   12 + a[1][3] *  125 + a[1][4] * -117 + a[2][0] *  -65 + a[2][1] *  -67 + a[2][2] *  125 + a[2][3] *  110 + a[2][4] *  -31 + a[3][0] * -123 + a[3][1] *   83 + a[3][2] *  122 + a[3][3] *   11 + a[3][4] *  -23 + a[4][0] *  -47 + a[4][1] *  -32 + a[4][2] * -117 + a[4][3] *   95 + a[4][4] *  118) * -106
        + relu_reg( + a[0][0] *    8 + a[0][1] *   82 + a[0][2] * -104 + a[0][3] *  101 + a[0][4] * -116 + a[1][0] *  -63 + a[1][1] *  -16 + a[1][2] *  -70 + a[1][3] *  125 + a[1][4] *   75 + a[2][0] *   66 + a[2][1] *  -96 + a[2][2] * -101 + a[2][3] * -114 + a[2][4] *   59 + a[3][0] *   12 + a[3][1] *    5 + a[3][2] *  -95 + a[3][3] *  116 + a[3][4] *  -93 + a[4][0] *   15 + a[4][1] *   79 + a[4][2] *    3 + a[4][3] *   49 + a[4][4] * -124) *   -3
        + relu_reg( + a[0][0] *   81 + a[0][1] *   68 + a[0][2] * -102 + a[0][3] *  -74 + a[0][4] *  121 + a[1][0] *  -15 + a[1][1] *   55 + a[1][2] *  101 + a[1][3] *  -13 + a[1][4] *  -62 + a[2][0] *   64 + a[2][1] *  114 + a[2][2] *   38 + a[2][3] *  -21 + a[2][4] *  112 + a[3][0] *  114 + a[3][1] *  112 + a[3][2] *  -10 + a[3][3] *  -16 + a[3][4] *  -50 + a[4][0] * -112 + a[4][1] * -116 + a[4][2] *  -54 + a[4][3] *   82 + a[4][4] *  -72) *   32
        + relu_reg( + a[0][0] *   15 + a[0][1] *  -77 + a[0][2] *   66 + a[0][3] *  -90 + a[0][4] *   -6 + a[1][0] *  -30 + a[1][1] *   -8 + a[1][2] *   81 + a[1][3] *    2 + a[1][4] * -110 + a[2][0] *  -95 + a[2][1] *   59 + a[2][2] *   52 + a[2][3] *   15 + a[2][4] *   55 + a[3][0] *  -33 + a[3][1] *   14 + a[3][2] *   58 + a[3][3] *   67 + a[3][4] *   86 + a[4][0] *  -79 + a[4][1] *   48 + a[4][2] *  -13 + a[4][3] *  -15 + a[4][4] *   66) *  -95
        + relu_reg( + a[0][0] *   33 + a[0][1] *   82 + a[0][2] *   67 + a[0][3] *   30 + a[0][4] *   -2 + a[1][0] *   65 + a[1][1] *  120 + a[1][2] *  -13 + a[1][3] *   18 + a[1][4] *    5 + a[2][0] *  104 + a[2][1] * -119 + a[2][2] *   -7 + a[2][3] *   71 + a[2][4] *  107 + a[3][0] *   24 + a[3][1] *   82 + a[3][2] *  -96 + a[3][3] * -104 + a[3][4] * -121 + a[4][0] *   65 + a[4][1] *   97 + a[4][2] *   83 + a[4][3] *   46 + a[4][4] *  -84) *  -50
        + relu_reg( + a[0][0] *  -29 + a[0][1] *    7 + a[0][2] *  -70 + a[0][3] *   38 + a[0][4] *  -90 + a[1][0] *  -15 + a[1][1] *  -32 + a[1][2] *   37 + a[1][3] *   36 + a[1][4] *  -62 + a[2][0] * -125 + a[2][1] *  -46 + a[2][2] *  -70 + a[2][3] *   37 + a[2][4] *  -73 + a[3][0] *  -34 + a[3][1] *  -87 + a[3][2] *  -75 + a[3][3] *   71 + a[3][4] *  -77 + a[4][0] *   53 + a[4][1] *   37 + a[4][2] * -103 + a[4][3] *  -13 + a[4][4] * -114) *  -23
        + relu_reg( + a[0][0] *   67 + a[0][1] *   42 + a[0][2] *   41 + a[0][3] * -123 + a[0][4] *  -92 + a[1][0] *   10 + a[1][1] *  -77 + a[1][2] *   75 + a[1][3] *   96 + a[1][4] *  -51 + a[2][0] *  109 + a[2][1] *  -74 + a[2][2] *   -7 + a[2][3] * -122 + a[2][4] *   67 + a[3][0] *   47 + a[3][1] *   22 + a[3][2] *  -68 + a[3][3] *   38 + a[3][4] *   29 + a[4][0] *  115 + a[4][1] * -121 + a[4][2] *   36 + a[4][3] *  -49 + a[4][4] *   85) *   46
        > 0)
        return 1;
    return 0;
}

// int main()
// {
//     int N = getint();
//     int a[5][5];
//     while (N > 0) {
//         int i = 0;
//         while (i < 5) {
//             int j = 0;
//             while (j < 5) {
//                 a[i][j] = getint();
//                 j = j + 1;
//             }
//             i = i + 1;
//         }
//         if (model(a)) {
//             // cat
//             putch(99); putch(97); putch(116); putch(10);
//         } else {
//             // dog
//             putch(100); putch(111); putch(103); putch(10);
//         }
//         N = N - 1;
//     }
//     return 0;
// }
