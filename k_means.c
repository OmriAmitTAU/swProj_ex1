#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define DEFAULT_EPSILON 0.001
#define DEFAULT_ITERATION 200
#define MAX_ITERATION 1000

double **vectors;
double **centroids;
double **new_centroids;
int *centroid_size;

double distance(double *v, double *u, int d);
int assert_input(int x, char *input);
int init_vector_list(int d, int N);
int init_centroids(int k, int d, int n);
int find_closest_centroid_index(double *curr, int k, int d);
void print_centroids(int k, int d);
void calculate_updated_centroid(int centroid_idx, int x, int d);
void calculate_mean(int k, int i, int d);
void reset_new_centroids(int i, int d);
int k_means(int k, int n, int d, int max_iter);
void free_memory(int K, int d);

double distance(double *v, double *u, int d)
{
    double add = 0.0;
    for (int i = 0; i < d; i++)
    {
        add += pow((v[i] - u[i]), 2);
    }
    return sqrt(add);
}

int assert_input(int x, char *input)
{
    int i;
    for (i = 0; input[i] != '\0'; i++)
    {
        char curr = input[i];
        if (curr > '9' || curr < '0')
        {
            if (x == 'k')
            {
                printf("Invalid number of clusters!\n");
            }
            else if (x == 'n')
            {
                printf("Invalid number of points!\n");
            }
            else if (x == 'd')
            {
                printf("Invalid dimension of point!\n");
            }
            else if (x == 'i')
            {
                printf("Invalid maximum iteration!\n");
            }
            return 0;
        }
    }
    int j = atoi(input);
    if (x == 'k' && j <= 1)
    {
        printf("Invalid number of clusters!\n");
        return 0;
    }
    if (x == 'i' && (j <= 1 || j >= MAX_ITERATION))
    {
        printf("Invalid maximum iteration!\n");
        return 0;
    }
    if (x == 'n' && j <= 1)
    {
        printf("Invalid number of points!\n");
        return 0;
    }
    return j;
}

int init_vector_list(int d, int N)
{
    vectors = (double **)malloc((N * sizeof(double *)));
    if (vectors == NULL)
    {
        printf("An Error Has Occurred\n");
        return 1;
    }

    for (int x = 0; x < N; x++)
    {
        vectors[x] = (double *)malloc((d * sizeof(double *)));
        if (vectors[x] == NULL)
        {
            printf("An Error Has Occurred\n");
            return 1;
        }

        for (int y = 0; y < d; y++)
        {
            if ((scanf("%lf,", &vectors[x][y])) != 1)
            {
                printf(("An Error Has Occurred"));
                return 1;
            }
        }
    }

    return 0;
}

int init_centroids(int k, int d, int n)
{
    centroids = (double **)malloc(k * sizeof(double *));
    new_centroids = (double **)malloc(k * sizeof(double *));
    centroid_size = (int *)malloc(k * sizeof(int));
    if (centroids == NULL || new_centroids == NULL || centroid_size == NULL)
    {
        printf("An Error Has Occurred\n");
        return 1;
    }
    for (int i = 0; i < k; i++)
    {
        centroids[i] = (double *)malloc(d * sizeof(double));
        new_centroids[i] = (double *)malloc(d * sizeof(double));
        centroid_size[i] = 0;
        if (centroids[i] == NULL || new_centroids[i] == NULL)
        {
            printf("An Error Has Occurred\n");
            return 1;
        }
        for (int j = 0; j < d; j++)
        {
            centroids[i][j] = vectors[i][j];
        }
    }

    return 0;
}

/**
 * returns the index of the closest centroid
 */
int find_closest_centroid_index(double *curr, int k, int d)
{
    double closest_d = INFINITY;
    int closest_centroid_index = -1;

    for (int i = 0; i < k; i++)
    {
        double dis = distance(curr, centroids[i], d);
        if (dis < closest_d)
        {
            closest_d = dis;
            closest_centroid_index = i;
        }
    }
    return closest_centroid_index;
}

void print_centroids(int k, int d)
{
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < d; j++)
        {
            printf("%.4f", centroids[i][j]);
            if (j < d - 1)
            {
                printf(",");
            }
        }
        printf("%c", '\n');
    }
}

void calculate_updated_centroid(int centroid_idx, int x, int d)
{
    for (int i = 0; i < d; i++)
    {
        new_centroids[centroid_idx][i] += vectors[x][i];
    }
    centroid_size[centroid_idx]++;
}
void calculate_mean(int k, int i, int d)
{
    for (int j = 0; j < d; j++)
    {

        double mean = (new_centroids[i][j]) / (centroid_size[i]);
        new_centroids[i][j] = mean;
    }
}
void reset_new_centroids(int i, int d)
{
    for (int x = 0; x < d; x++)
    {
        centroids[i][x] = new_centroids[i][x];
        new_centroids[i][x] = 0.0;
    }
    centroid_size[i] = 0;
}

int k_means(int k, int n, int d, int max_iter)
{
    int iter = 0;
    double delta_miu = INFINITY;
    double curr_miu;

    while (delta_miu >= DEFAULT_EPSILON && iter < max_iter)
    {
        delta_miu = distance(centroids[0], new_centroids[0], d);
        for (int i = 0; i < n; i++)
        {
            int curr_idx = find_closest_centroid_index(vectors[i], k, d);
            calculate_updated_centroid(curr_idx, i, d);
        }

        for (int y = 0; y < k; y++)
        {
            calculate_mean(k, y, d);
            curr_miu = distance(centroids[y], new_centroids[y], d);
            delta_miu = (curr_miu > delta_miu) ? curr_miu : delta_miu;
            reset_new_centroids(y, d);
        }
        iter++;
    }

    print_centroids(k, d);
    return 0;
}

void free_memory(int K, int d)
{
    for (int x = 0; x < d; x++)
    {
        free(vectors[x]);
        if (x < K)
        {
            free(centroids[x]);
            free(new_centroids[x]);
        }
    }
    free(vectors);
    free(centroids);
    free(new_centroids);
}

int main(int argc, char *argv[])
{

    if (argc < 4 || argc > 5)
    {
        printf("An Error Has Occurred\n");
        return 1;
    }

    int iter, c;
    int K = assert_input('k', argv[1]);
    if (K == 0)
    {
        return 1;
    }
    int N = assert_input('n', argv[2]);
    if (N == 0)
    {
        return 1;
    }
    int d = assert_input('d', argv[3]);
    if (d == 0)
    {
        return 1;
    }
    iter = DEFAULT_ITERATION;
    if (argc == 6)
    { /* We got an iteration number */
        iter = assert_input('i', argv[4]);
        if (iter == 0)
        {
            return 1;
        }
    }

    if (K >= N)
    {
        printf("Invalid number of clusters!\n");
        return 1;
    }

    if ((init_vector_list(d, N)) == 1)
    {

        return 1;
    }

    if ((init_centroids(K, d, N)) == 1)
    {

        return 1;
    }

    k_means(K, N, d, iter);

    free_memory(K, d);

    return 0;
}