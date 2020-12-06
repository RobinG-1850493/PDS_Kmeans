#include <iostream>
#include <vector>

using namespace std;

__global__ void mult(const int *pA, const int *pB, int *pC, int N)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < N)
		pC[i] = pA[i] * pB[i];
}

int main(void)
{
	const int N = 8192;
	vector<int> a(N), b(N), c(N);

	for (int i = 0 ; i < N ; i++)
	{
		a[i] = i;
		b[i] = -i;
	}

	int *cuA, *cuB, *cuC;
	cudaMalloc(&cuA, N*sizeof(int));
	cudaMalloc(&cuB, N*sizeof(int));
	cudaMalloc(&cuC, N*sizeof(int));

	cudaMemcpy(cuA, a.data(), N*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(cuB, b.data(), N*sizeof(int), cudaMemcpyHostToDevice);

	int blockSize = 256;
	int numBlocks = N/blockSize + ((N%blockSize == 0)?0:1);

	mult<<<numBlocks, blockSize>>>(cuA, cuB, cuC, N);

	cudaMemcpy(c.data(), cuC, N*sizeof(int), cudaMemcpyDeviceToHost);

	cudaFree(cuA);
	cudaFree(cuB);
	cudaFree(cuC);

	for (auto x : c)
		cout << x << " ";
	cout << endl;

	return 0;
}
