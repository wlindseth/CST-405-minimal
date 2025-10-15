int x;
int y;

x = 10;
y = 20;
print(x + y);

int arr[3];
int i;

i = 0;
arr[i] = 100;

i = 1;
arr[i] = 200;

print(arr[0]);
print(arr[1]);

int matrix[2][3];
int row;
int col;

row = 0;
col = 0;
matrix[row][col] = 5;

row = 0;
col = 1;
matrix[row][col] = 10;

row = 0;
col = 2;
matrix[row][col] = 15;

row = 1;
col = 0;
matrix[row][col] = 20;

row = 1;
col = 1;
matrix[row][col] = 25;

row = 1;
col = 2;
matrix[row][col] = 30;

print(matrix[0][0]);
print(matrix[0][1]);
print(matrix[0][2]);
print(matrix[1][0]);
print(matrix[1][1]);
print(matrix[1][2]);

x = matrix[0][0] + matrix[1][2];
print(x);